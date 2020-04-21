#include "../../utilities/includes.h"

#include "../../features/Events/bullet_impact.hpp"



void __fastcall CHookManager::beginFrame( void* thisptr, void* ptr, float ft )
{
	auto originalFn = g_Hooks->materialHook->getOriginalFunction<decltype( &beginFrame )>( 42 );

	bullet_impact::get( ).paint( );

	if ( !g_Interfaces->gameEngine->connected( ) || !g_Interfaces->gameEngine->inGame( ) )
	{
		Globals::localPlayer = nullptr;
	}
	originalFn( thisptr, ptr, ft );
}