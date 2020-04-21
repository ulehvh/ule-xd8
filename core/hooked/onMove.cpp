#include "../managers/hookManager.hpp"
#include "../../features/featureManager.hpp"
#include "../../features/Misc/Misc.hpp"
#include "../../features/Aimbot/Aimbot.hpp"
#include "../../features/Anti-Aim/Anti-aim.hpp"
#include "../../features/Aimbot/Resolver.hpp"
#include "../../features/Anti-aim/Fakelag.hpp"
#include "../../features/Misc/enginePrediction.hpp"
#include "../../features/Misc/grenadePrediction.hpp"
#include "../../features/Misc/Animations.hpp"

void manageFakeMatrix() {
	static float m_fake_spawntime = 0;
	static bool  m_should_update_fake = true;
	static bool init_fake_anim = false;
	CBasePlayerAnimState* m_fake_state = nullptr;
	static bool m_got_fake_matrix = false;
	if (m_fake_spawntime != Globals::localPlayer->m_flSpawnTime() || m_should_update_fake)
	{
		init_fake_anim = false;
		m_fake_spawntime = Globals::localPlayer->m_flSpawnTime();
		m_should_update_fake = false;
	}

	if (!init_fake_anim)
	{
		m_fake_state = static_cast<CBasePlayerAnimState*> (malloc(sizeof(CBasePlayerAnimState)));

		if (m_fake_state != nullptr)
			Globals::localPlayer->CreateAnimationState(m_fake_state);

		init_fake_anim = true;
	}
	float frametime = g_Interfaces->globalVars->frametime;
	if (Globals::bSendPacket)
	{
		std::array<CAnimationLayer, 13> networked_layers;
		std::memcpy(&networked_layers, Globals::localPlayer->GetAnimOverlays(), sizeof(CAnimationLayer) * 13);

		auto backup_abs_angles = Globals::localPlayer->GetAbsAngles();
		auto backup_poses = Globals::localPlayer->m_flPoseParameter();
		if (Globals::localPlayer->flags() & FL_ONGROUND)
			Globals::localPlayer->setFlags(1);
		else
		{
			if (Globals::localPlayer->GetAnimOverlays()[4].m_flWeight != 1.f && Globals::localPlayer->GetAnimOverlays()[5].m_flWeight != 0.f)
				Globals::localPlayer->setFlags(1);

			if (Globals::localPlayer->flags() & FL_ONGROUND)
				Globals::localPlayer->setFlags(-FL_ONGROUND);
		}




		*reinterpret_cast<int*>(Globals::localPlayer + 0xA68) = g_Interfaces->globalVars->frametime;
		g_Interfaces->globalVars->frametime = 23.91753135f; // :^)
		Globals::localPlayer->UpdateAnimationState(m_fake_state, Vector3(Globals::oCmd->viewAngles.x, Globals::oCmd->viewAngles.y + 30.f, Globals::oCmd->viewAngles.z));
		Globals::got_fake_matrix = Globals::localPlayer->SetupBones(Globals::desyncMatrix, 128, 0x7FF00/*g_Menu.Config.nightmodeval*/, g_Interfaces->globalVars->curtime);
		const auto org_tmp = Globals::localPlayer->GetRenderOrigin();
		if (Globals::got_fake_matrix)
		{
			for (auto& i : Globals::desyncMatrix)
			{
				i[0][3] -= org_tmp.x;
				i[1][3] -= org_tmp.y;
				i[2][3] -= org_tmp.z;
			}
		}

		std::memcpy(Globals::localPlayer->GetAnimOverlays(), &networked_layers, sizeof(CAnimationLayer) * 13);

		Globals::localPlayer->m_flPoseParameter() = backup_poses;
		Globals::localPlayer->setAbsAngles(backup_abs_angles);
	}
	g_Interfaces->globalVars->frametime = frametime;
}

bool __fastcall CHookManager::onMove( void *ecx, void *edx, float sampleTime, CUserCmd *userCmd )
{
	static auto originalFn = g_Hooks->clientModeHook->getOriginalFunction<decltype( &onMove )>( 24 );

	originalFn( ecx, edx, sampleTime, userCmd );

	if ( !userCmd || !userCmd->commandNumber )
		return originalFn( ecx, edx, sampleTime, userCmd );

	uintptr_t* pebp;
	__asm mov pebp, ebp;
	auto sendPacket = ( bool* ) ( *pebp - 0x1C );

	Globals::bSendPacket = true;
	Globals::oCmd = userCmd;
	Globals::localPlayer = g_Interfaces->clientEntity->GetClientEntity( g_Interfaces->gameEngine->getLocalPlayer( ) );

	if ( !Globals::localPlayer )
		return originalFn;

	Globals::originalViewAngle = Globals::oCmd->viewAngles;

	if ( g_Interfaces->gameEngine->inGame( ) )
	{

		//thanks chance... :^)
		if ( Globals::localPlayer->lifeState( ) || Globals::localPlayer->moveType( ) == movetype_ladder )
			Globals::localPlayer->clientSideAnimation( ) = true;
		else
		{
			const float oldCurtime = g_Interfaces->globalVars->curtime;
			const float oldFrametime = g_Interfaces->globalVars->frametime;
			const float intervalPerTick = g_Interfaces->globalVars->intervalPerTick;
			const int tickbase = Globals::localPlayer->tickBase( );

			g_Interfaces->globalVars->frametime = intervalPerTick;
			g_Interfaces->globalVars->curtime = tickbase * intervalPerTick;

			if (Globals::InFreecam) {
				userCmd->forwardmove = 0;
				userCmd->sidemove = 0;
			}


			if (config->get_bool("espGrenadePred"))
				g_GrenadePred.think( userCmd );

			if (config->get_bool("rageChokeEnable"))
				g_FakeLag->createMove( );

			if (config->get_bool("miscClanTag"))
				g_Misc.clanTag( ); // TODO: make it so doesnt fuck with fakelag

			g_Features->onMove( sampleTime, userCmd );

			if (config->get_bool("rageAntiAimCorrection"))
				g_Resolver.OnCreateMove();

			if (config->get_bool("aaEnable"))
				g_AntiAim.onMove(userCmd);

			if (config->get_bool("chamsLocalDesync"))
				manageFakeMatrix();
			

			g_EnginePrediction.run( userCmd );
			{
				

				if (config->get_bool("rageEnable"))
					g_Aimbot.creatMove( );

				if(config->get_bool("legitEnable"))
					g_Aimbot.legitCreateMove();
			}

			Globals::oCmd->buttons &= ~( IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT );

			
			
			g_Misc.movementFix( Globals::originalViewAngle );

			if (config->get_bool("miscAntiUT"));
				g_Math.clamp( Globals::oCmd->viewAngles );

			static Vector3 last_choked_angles;

			if ( !Globals::bSendPacket && g_Interfaces->clientState->ChokedCommands == 0 ) // we are choking a command, and we have not already choked a command
				last_choked_angles = userCmd->viewAngles; // save our first choked angles for later use
			else if ( Globals::bSendPacket && g_Interfaces->clientState->ChokedCommands > 0 ) // if we are sending a command, and we have previously choked commands
				Globals::realAngle = last_choked_angles; // apply the first choked angles
			else if ( Globals::bSendPacket && g_Interfaces->clientState->ChokedCommands == 0 ) // if we are sending a command, and have not choked any commands
				Globals::realAngle = userCmd->viewAngles; // apply the sent angles
			else
				Globals::fakeAngle = userCmd->viewAngles;

			if (Globals::bSendPacket)
			{
				Globals::chokedPackets = 0;
				Globals::fakePosition = Globals::localPlayer->GetAbsOrigin();
			}
			else
			{
				++Globals::chokedPackets;
			}

			g_Interfaces->globalVars->frametime = oldFrametime;
			g_Interfaces->globalVars->curtime = oldCurtime;
		}
	}
	*sendPacket = Globals::bSendPacket;

	return false;
}