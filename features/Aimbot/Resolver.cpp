#include "Resolver.hpp"
#include "Autowall.hpp"
//#include "..\..\sdk\vector.hpp"
Resolver g_Resolver;
#define	MAX_NETWORKID_LENGTH		65


/*
my attempt at fixing desync and i was pretty successful
it can resolve static desync pretty perfectly
and can resolve some jitter desync but
it still gets rekt by other things
*/
float clamp_yaw(float yaw)
{
	while (yaw > 180.f)
		yaw -= 360.f;
	while (yaw < -180.f)
		yaw += 360.f;
	return yaw;
}
float ClampYaw(float yaw)
{
	while (yaw > 180.f)
		yaw -= 360.f;
	while (yaw < -180.f)
		yaw += 360.f;
	return yaw;
}
float NormalizeYaw180(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);

	return yaw;
}
float angle_difference(float a, float b) {
	auto diff = NormalizeYaw180(a - b);

	if (diff < 180)
		return diff;
	return diff - 360;
}
void Resolver::AnimationFix(CBaseEntity* pEnt)
{
	

		pEnt->ClientAnimations(true);

		auto old_curtime = g_Interfaces->globalVars->curtime;
		auto old_frametime = g_Interfaces->globalVars->frametime;

		g_Interfaces->globalVars->curtime = pEnt->simulationTime();
		g_Interfaces->globalVars->frametime = g_Interfaces->globalVars->intervalPerTick;

		auto player_animation_state = pEnt->animState();
		auto player_model_time = reinterpret_cast<int*>(player_animation_state + 112);
		if (player_animation_state != nullptr && player_model_time != nullptr)
			if (*player_model_time == g_Interfaces->globalVars->framecount)
				* player_model_time = g_Interfaces->globalVars->framecount - 1;


		pEnt->updateClientAnimation();

		g_Interfaces->globalVars->curtime = old_curtime;
		g_Interfaces->globalVars->frametime = old_frametime;

		//pEnt->SetAbsAngles(Vector3(0, player_animation_state->m_flGoalFeetYaw, 0));

		pEnt->ClientAnimations(false);
	

}
float flAngleMod(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}
float ApproachAngle(float target, float value, float speed)
{
	target = flAngleMod(target);
	value = flAngleMod(value);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

/*void update_state(CAnimState * state, Vector3 angles) {
	using Fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
	static auto fn = reinterpret_cast<Fn>(Utils::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
	fn(state, nullptr, 0.0f, angles[1], angles[0], nullptr);
}*/

void Resolver::HandleBackUpResolve(CBaseEntity * pEnt) {

	if (!config->get_bool("rageAntiAimCorrection"))
		return;

	if (pEnt->team() == Globals::localPlayer->team())
		return;

	
	
	auto player_animation_state = pEnt->animState();

	float m_flLastClientSideAnimationUpdateTimeDelta = fabs(player_animation_state->m_iLastClientSideAnimationUpdateFramecount - player_animation_state->m_flLastClientSideAnimationUpdateTime);

	auto v48 = 0.f;

	if (player_animation_state->m_flFeetSpeedForwardsOrSideWays >= 0.0f)
	{
		v48 = fminf(player_animation_state->m_flFeetSpeedForwardsOrSideWays, 1.0f);
	}
	else
	{
		v48 = 0.0f;
	}

	float v49 = ((player_animation_state->m_flStopToFullRunningFraction * -0.30000001) - 0.19999999) * v48;

	float flYawModifier = v49 + 1.0;

	if (player_animation_state->m_fDuckAmount > 0.0)
	{
		float v53 = 0.0f;

		if (player_animation_state->m_flFeetSpeedUnknownForwardOrSideways >= 0.0)
		{
			v53 = fminf(player_animation_state->m_flFeetSpeedUnknownForwardOrSideways, 1.0);
		}
		else
		{
			v53 = 0.0f;
		}
	}

	float flMaxYawModifier = player_animation_state->pad10[516] * flYawModifier;
	float flMinYawModifier = player_animation_state->pad10[512] * flYawModifier;

	float newFeetYaw = 0.f;

	auto eyeYaw = player_animation_state->m_flEyeYaw;

	auto lbyYaw = player_animation_state->m_flGoalFeetYaw;

	float eye_feet_delta = fabs(eyeYaw - lbyYaw);

	if (eye_feet_delta <= flMaxYawModifier)
	{
		if (flMinYawModifier > eye_feet_delta)
		{
			newFeetYaw = fabs(flMinYawModifier) + eyeYaw;
		}
	}
	else
	{
		newFeetYaw = eyeYaw - fabs(flMaxYawModifier);
	}

	float v136 = fmod(newFeetYaw, 360.0);

	if (v136 > 180.0)
	{
		v136 = v136 - 360.0;
	}

	if (v136 < 180.0)
	{
		v136 = v136 + 360.0;
	}

	player_animation_state->m_flGoalFeetYaw = v136;
	
	
}

void Resolver::preresolver(CBasePlayerAnimState* AnimState, CBaseEntity* pEntity, bool bShot)
{
	if (!config->get_bool("rageAntiAimCorrection"))
		return;

	if (!AnimState)
		return;

	if (!pEntity)
		return;

	auto RemapVal = [](float val, float A, float B, float C, float D) -> float
	{
		if (A == B)
			return val >= B ? D : C;
		return C + (D - C) * (val - A) / (B - A);
	};

	int iIndex = pEntity->index();
	auto pLocalEnt = Globals::localPlayer;

	ResolverMode[iIndex] = "Unresolved";
	static int ResolveInt = 0;
	static int LastResolveMode[65];

	static int iMode[MAX_NETWORKID_LENGTH];

	float flLastResolveYaw[MAX_NETWORKID_LENGTH];

	flGoalFeetYawB[iIndex] = 0.f;
	flLbyB[iIndex] = pEntity->lowerBodyYawTarget();

	int ShotDelta = Globals::MissedShots[iIndex];

	if (pLocalEnt && pLocalEnt->alive() && pLocalEnt != pEntity)
	{
		float angToLocal = g_Math.normalizeYaw2(g_Math.calcAngle(pLocalEnt->vecOrigin(), pEntity->vecOrigin()).y);

		float Back = g_Math.normalizeYaw2(angToLocal);

		float Brute = AnimState->m_flGoalFeetYaw;

		if (!isnan(angToLocal) && !isinf(angToLocal) && pEntity != pLocalEnt && pEntity->team() != pLocalEnt->team())
		{
			float AntiSide = 0.f;

			float EyeDelta = fabs(g_Math.normalizeYaw2(vOldEyeAng[iIndex].y - pEntity->eyeAngles().y));

			if (bUseFreestandAngle[iIndex])
			{
				ResolveInt = 1;

				if (LastResolveMode[iIndex] != ResolveInt)
				{
					Globals::MissedShots[iIndex] = 0;
					ShotDelta = 0;
					LastResolveMode[iIndex] = ResolveInt;
				}

				ResolverMode[iIndex] = "Smart";
				Brute = g_Math.normalizeYaw2(Back + flLastFreestandAngle[iIndex]);

				if ((ShotDelta % 3) == 2)
				{
					ResolverMode[iIndex] += " BF";
					Brute = g_Math.normalizeYaw2(g_Math.normalizeYaw2(Back + flLastFreestandAngle[iIndex]) + 180.f);
				}
				else
				{
					if (flLastFreestandAngle[iIndex] > 0)
						ResolverMode[iIndex] += " <";
					else
						ResolverMode[iIndex] += " >";
				}

				if (fabs(g_Math.normalizeYaw2(g_Math.normalizeYaw2(Back + flLastFreestandAngle[iIndex]) - pEntity->eyeAngles().y)) <= 90.f && fabs(g_Math.normalizeYaw2(g_Math.normalizeYaw2(Back) - pEntity->eyeAngles().y)) <= 30.f)
					iMode[iIndex] = 1;
				else
					iMode[iIndex] = 0;
			}
			else if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - Back)) >= 60.f && fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - g_Math.normalizeYaw2(Back + 180))) >= 60.f && EyeDelta > 50)
			{
				ResolveInt = 2;

				if (LastResolveMode[iIndex] != ResolveInt)
				{
					Globals::MissedShots[iIndex] = 0;
					ShotDelta = 0;
					LastResolveMode[iIndex] = ResolveInt;
				}

				ResolverMode[iIndex] = "Jitter";

				if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - g_Math.normalizeYaw2(Back))) <= 105.f)
					AntiSide = 0.f;
				else if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - g_Math.normalizeYaw2(Back + 180))) <= 75.f)
					AntiSide = 180.f;

				Brute = g_Math.normalizeYaw2(Back + AntiSide);

				iMode[iIndex] = 0;
			}
			else
			{
				ResolveInt = 3;

				if (LastResolveMode[iIndex] != ResolveInt)
				{
					Globals::MissedShots[iIndex] = 0;
					ShotDelta = 0;
					LastResolveMode[iIndex] = ResolveInt;
				}

				if (iMode[iIndex] == 0)
				{
					ResolverMode[iIndex] = "Inverse";
					switch (ShotDelta % 2)
					{
					case 0:
						if (g_Math.normalizeYaw2(pEntity->eyeAngles().y - Back) > 0.f)
						{
							ResolverMode[iIndex] += " >";
							AntiSide = -90.f;
						}
						else if (g_Math.normalizeYaw2(pEntity->eyeAngles().y - Back) < 0.f)
						{
							ResolverMode[iIndex] += " <";
							AntiSide = 90.f;
						}
						break;

					case 1:
						if (g_Math.normalizeYaw2(pEntity->eyeAngles().y - Back) > 0.f)
						{
							ResolverMode[iIndex] += " < BF";
							AntiSide = 90.f;
						}
						else if (g_Math.normalizeYaw2(pEntity->eyeAngles().y - Back) < 0.f)
						{
							ResolverMode[iIndex] += " > BF";
							AntiSide = -90.f;
						}

						break;
					}
				}
				else if (iMode[iIndex] == 1)
				{
					ResolverMode[iIndex] = "Center";

					switch (ShotDelta % 2)
					{
					case 0:
						if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - g_Math.normalizeYaw2(Back))) <= 105.f)
						{
							ResolverMode[iIndex] += " ^";
							AntiSide = 0.f;
						}
						else if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - g_Math.normalizeYaw2(Back + 180))) <= 75.f)
						{
							ResolverMode[iIndex] += " v";
							AntiSide = 180.f;
						}

						break;
					case 1:
						if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - g_Math.normalizeYaw2(Back))) <= 105.f)
						{
							ResolverMode[iIndex] += " v BF";
							AntiSide = 180.f;
						}
						else if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - g_Math.normalizeYaw2(Back + 180))) <= 75.f)
						{
							ResolverMode[iIndex] += " ^ BF";
							AntiSide = 0.f;
						}
						break;
					}
				}

				Brute = g_Math.normalizeYaw2(Back + AntiSide);
			}

			//--------------------------------------------------------------------------------------------------------------------

			if (pEntity != pLocalEnt && pEntity->team() != pLocalEnt->team() && (pEntity->flags() & FL_ONGROUND))
			{
				if (bShot)
				{
					ResolveInt = 4;
					ResolverMode[iIndex] = "Shot";

					Brute = vOldEyeAng[iIndex].y;
				}
				else
					flLastResolveYaw[iIndex] = Brute;

				if (fabs(g_Math.normalizeYaw2(pEntity->eyeAngles().y - Brute)) > 58.f)
				{
					if (g_Math.normalizeYaw2(Brute - pEntity->eyeAngles().y) > 0)
					{
						Brute = g_Math.normalizeYaw2(pEntity->eyeAngles().y + 58.f);
					}
					else
					{
						Brute = g_Math.normalizeYaw2(pEntity->eyeAngles().y - 58.f);
					}
				}

				AnimState->m_flGoalFeetYaw = Brute;
				flGoalFeetYawB[iIndex] = AnimState->m_flGoalFeetYaw;
				flLbyB[iIndex] = pEntity->lowerBodyYawTarget();
				pEntity->setLowerBodyYawTarget(Brute);
			}
		}
	}

	if (!bShot)
		vOldEyeAng[iIndex] = pEntity->eyeAngles();
}

void Resolver::PostResolver(CBasePlayerAnimState* AnimState, CBaseEntity* pEntity, bool bShot)
{
	if (!config->get_bool("rageAntiAimCorrection"))
		return;

	if (!AnimState)
		return;

	if (!pEntity)
		return;

	flGoalFeetYaw[pEntity->index()] = AnimState->m_flGoalFeetYaw;

	

	int iIndex = pEntity->index();

	auto pLocalEnt = Globals::localPlayer;

	if (!pLocalEnt)
		return;

	if (pEntity != pLocalEnt)
	{
		pEntity->setLowerBodyYawTarget(flLbyB[iIndex]);
		if (flGoalFeetYawB[iIndex] != 0.f)
			AnimState->m_flGoalFeetYaw = flGoalFeetYawB[iIndex];
	}
}

void Resolver::HandleHits(CBaseEntity * pEnt)
{
	auto NetChannel = g_Interfaces->gameEngine->netchannelInfo();

	if (!NetChannel)
		return;

	static float predTime[65];
	static bool init[65];


	if (Globals::Shot[pEnt->EntIndex()])
	{
		if (init[pEnt->EntIndex()])
		{
			g_Resolver.pitchHit[pEnt->EntIndex()] = pEnt->eyeAngles().x;
			predTime[pEnt->EntIndex()] = g_Interfaces->globalVars->curtime + NetChannel->GetAvgLatency(FLOW_INCOMING) + NetChannel->GetAvgLatency(FLOW_OUTGOING) + TICKS_TO_TIME(1) + TICKS_TO_TIME(g_Interfaces->gameEngine->getNetChannel()->m_nChokedPackets);
			init[pEnt->EntIndex()] = false;
		}

		if (g_Interfaces->globalVars->curtime > predTime[pEnt->EntIndex()] && !Globals::Hit[pEnt->EntIndex()])
		{
			Globals::MissedShots[pEnt->EntIndex()] += 1;
			Globals::Shot[pEnt->EntIndex()] = false;
		}
		else if (g_Interfaces->globalVars->curtime <= predTime[pEnt->EntIndex()] && Globals::Hit[pEnt->EntIndex()])
			Globals::Shot[pEnt->EntIndex()] = false;

	}
	else
		init[pEnt->EntIndex()] = true;

	Globals::Hit[pEnt->EntIndex()] = false;
}

void Resolver::OnCreateMove() // cancer v2
{
	if (!config->get_bool("rageAntiAimCorrection"))
		return;

	if (!Globals::localPlayer->alive())
		return;

	if (!Globals::localPlayer->activeWeapon() || Globals::localPlayer->isKnifeorNade())
		return;


	for (int i = 1; i < g_Interfaces->globalVars->maxClients; ++i)
	{
		CBaseEntity* pPlayerEntity = g_Interfaces->clientEntity->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->alive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == Globals::localPlayer
			|| pPlayerEntity->team() == Globals::localPlayer->team())
		{
			UseFreestandAngle[i] = false;
			continue;
		}

		if (abs(pPlayerEntity->velocity().Length2D()) > 29.f)
			UseFreestandAngle[pPlayerEntity->EntIndex()] = false;

		if (abs(pPlayerEntity->velocity().Length2D()) <= 29.f && !UseFreestandAngle[pPlayerEntity->EntIndex()])
		{
			bool Autowalled = false, HitSide1 = false, HitSide2 = false;

			float angToLocal = g_Math.calcAngle(Globals::localPlayer->GetOrigin(), pPlayerEntity->GetOrigin()).y;
			Vector3 ViewPoint = Globals::localPlayer->GetOrigin() + Vector3(0, 0, 90);

			Vector2 Side1 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal))),(45 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2 Side2 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector2 Side3 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal))),(50 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2 Side4 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector3 Origin = pPlayerEntity->GetOrigin();

			Vector2 OriginLeftRight[] = { Vector2(Side1.x, Side1.y), Vector2(Side2.x, Side2.y) };

			Vector2 OriginLeftRightLocal[] = { Vector2(Side3.x, Side3.y), Vector2(Side4.x, Side4.y) };

			for (int side = 0; side < 2; side++)
			{
				Vector3 OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
				Vector3 OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

				if (g_Autowall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
				{
					if (side == 0)
					{
						HitSide1 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = 90;
					}
					else if (side == 1)
					{
						HitSide2 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = -90;
					}

					Autowalled = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector3 OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 90 };

						if (g_Autowall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
						{
							if (side222 == 0)
							{
								HitSide1 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = 90;
							}
							else if (side222 == 1)
							{
								HitSide2 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = -90;
							}

							Autowalled = true;
						}
					}
				}
			}

			if (Autowalled)
			{
				if (HitSide1 && HitSide2)
					UseFreestandAngle[pPlayerEntity->EntIndex()] = false;
				else
					UseFreestandAngle[pPlayerEntity->EntIndex()] = true;
			}
		}
	}
}

void Resolver::FrameStage(clientFrameStage_t stage)
{
	if (!Globals::localPlayer || !g_Interfaces->gameEngine->inGame())
		return;

	

	for (int i = 1; i < g_Interfaces->globalVars->maxClients; ++i)
	{
		CBaseEntity* pPlayerEntity = g_Interfaces->clientEntity->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->alive())
			continue;
		if (pPlayerEntity->IsDormant())
		{
			continue;
		}

		if (stage == FRAME_RENDER_START)
		{
			HandleHits(pPlayerEntity);
			AnimationFix(pPlayerEntity);


		}

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
			HandleBackUpResolve(pPlayerEntity);
		}

		if (stage == FRAME_NET_UPDATE_END && pPlayerEntity != Globals::localPlayer)
		{
			auto VarMap = reinterpret_cast<uintptr_t>(pPlayerEntity) + 36;
			auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);

			for (auto index = 0; index < VarMapSize; index++)
				* reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = 0;
		}

		
	}
}