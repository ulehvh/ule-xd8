#include "../../utilities/includes.h"
#include "../../menu/menu.hpp"

void __fastcall CHookManager::overrideView( void* ecx, void* edx, CViewSetup* pSetup )
{
	static auto original = g_Hooks->clientModeHook->getOriginalFunction<decltype( &overrideView )>( 18 );

	if ( !g_Interfaces->gameEngine->connected( ) && !g_Interfaces->gameEngine->inGame( ) )
		return;

	if ( !Globals::localPlayer )
		return;

	if ( !Globals::localPlayer->alive( ) )
		return;

	static Vector3 newOrigin;

	if (config->get_bool("miscFreecam"))
	{

		float freecamspeed = config->get_int("freecamSpeed");

		if (!menu->get_hotkey("freecamKey"))
		{
			Globals::InFreecam = false;
			newOrigin = pSetup->origin;
		}
		if (menu->get_hotkey("freecamKey"))
		{
			Globals::InFreecam = true;

			unsigned int fSpeed = freecamspeed; //5.f;
			if (GetAsyncKeyState(VK_CONTROL))
				fSpeed = fSpeed * 0.45;
			if (GetAsyncKeyState(VK_SHIFT))
				fSpeed = fSpeed * 1.65;
			if (GetAsyncKeyState(0x57)) // W		
			{
				newOrigin += pSetup->angles.Forward() * fSpeed;
			}
			if (GetAsyncKeyState(0x41)) // A		
			{
				newOrigin += pSetup->angles.Right() * fSpeed;
			}
			if (GetAsyncKeyState(0x44)) // D		
			{
				newOrigin -= pSetup->angles.Right() * fSpeed;
			}
			if (GetAsyncKeyState(0x53)) // S		
			{
				newOrigin -= pSetup->angles.Forward() * fSpeed;

			}
			if (GetAsyncKeyState(VK_SPACE)) // Space		
			{
				newOrigin += pSetup->angles.Up() * fSpeed;
			}
			pSetup->origin = newOrigin;
		}
	}
	

	if (config->get_bool("espRemovalsRecoil"))
	{
		Vector3 viewPunch = Globals::localPlayer->viewPunchAngle( );
		Vector3 aimPunch = Globals::localPlayer->aimPunchAngle( );

		pSetup->angles[ 0 ] -= ( viewPunch[ 0 ] + ( aimPunch[ 0 ] * 2 * 0.4499999f ) );
		pSetup->angles[ 1 ] -= ( viewPunch[ 1 ] + ( aimPunch[ 1 ] * 2 * 0.4499999f ) );
		pSetup->angles[ 2 ] -= ( viewPunch[ 2 ] + ( aimPunch[ 2 ] * 2 * 0.4499999f ) );
	}

	if ( !Globals::localPlayer->scoped( ) )
		pSetup->fov = config->get_float("miscFoV");
		//pSetup->fov = config->get_float( "miscFoV", 90.f );
	
	if (config->get_bool("espRemovalsZoom") && Globals::localPlayer->activeWeapon( ) && Globals::localPlayer->activeWeapon( )->zoomLevel( ) )
		pSetup->fov = config->get_float("miscFoV");
		//pSetup->fov = config->get_float( "miscFoV", 90.f );

	original( ecx, edx, pSetup );
}