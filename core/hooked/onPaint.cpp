#include "../../utilities/includes.h"
#include "../../menu/menu.hpp"
#include "../../sdk/vector.hpp"
#include "../../utilities/renderer_d3d.hpp"
#include "../../features/Events/player_hurt.hpp"
#include "../../features/Aimbot/Autowall.hpp"
#include "../../features//Misc/Misc.hpp"
#include "../../features/Visuals/Visuals.hpp"
#include <chrono>
#include <iomanip>


void SplashScreen() {
	int i = 0;

	static bool once = false;
	static int up = 0, down = false, stop = false;
	static float alpha = 0;  static auto plus_or_minus = false;
	if (!once) {
		if (alpha <= 0.f || alpha >= 255.f)
			plus_or_minus = !plus_or_minus;

		if (alpha == 0.f)
			up++;

		if (alpha == 255.f)
			down = true;

		//if (alpha <= 230.f && alpha >= 255.f)


		alpha += plus_or_minus ? 0.5f : -0.5f;

		//if(alpha > 230.f && alpha < 255.f )
			//alpha += plus_or_minus ? 0.3f : -0.3f;



		once = up == 2 && down;

	}
	else {
		alpha = 0;
	}
	char text[256];
	sprintf_s(text, "Build: %s", __DATE__);

	int w, y;
	g_Interfaces->gameEngine->getScreenSize(w,y);

	if (alpha != 0) {
		g_Renderer->fillRectangle(w / 2 - 200, y / 2 - 130, 400, 230, Color(25, 25, 25, (int)alpha));
		g_Renderer->gradient(w / 2 - 200, y / 2 - 130, 400, 3, Color(59, 81, 245, (int)alpha), Color(212, 108, 252,int(alpha)));
		g_Renderer->fillRectangle(w / 2 - 175, y / 2 - 90, 350, 1, Color(255, 255, 255, (int)alpha));
		g_Renderer->text(w / 2, y / 2 - 110,g_Fonts->segoeS, "Welcome Back", Color(255, 255, 255, (int)alpha), true);
		g_Renderer->text(w / 2 - 80, y / 2 - 50, g_Fonts->segoe, "Exp0s", Color(255, 255, 255, (int)alpha), true);
		g_Renderer->text(w / 2 + 65, y / 2 - 50, g_Fonts->segoe, "ure.xyz", Color(59, 81, 245, (int)alpha), true);
		g_Renderer->text(w / 2, y / 2 + 30, g_Fonts->segoeI, "Raping kids since 2019", Color(255, 255, 255, (int)alpha), true);
		g_Renderer->text(w / 2, y / 2 + 50, g_Fonts->segoeS, "Make sure Multisampling Anti-Aliasing Mode is OFF!", Color(255, 255, 255, (int)alpha), true);
		g_Renderer->text(w / 2, y / 2 + 70, g_Fonts->segoeS, text, Color(255, 255, 255, (int)alpha), true);
	}
}

void __fastcall CHookManager::onPaint( void *ecx, void *edx, unsigned int panel, bool unk0, bool unk1 )
{
	static auto originalFn = g_Hooks->panelHook->getOriginalFunction<decltype( &onPaint )>( 41 );
	static unsigned int targetPanel = -1;
	static unsigned int hudZoomPanel = -1;

	if ( strcmp( g_Interfaces->panel->getPanelName( panel ), "HudZoom" ) == 0 )
		hudZoomPanel = panel;

	if ( panel == hudZoomPanel && config->get_bool("espRemovalsScope") && Globals::localPlayer && Globals::localPlayer->alive( ) && Globals::localPlayer->scoped( ) )
		return;

	originalFn( ecx, edx, panel, unk0, unk1 );

	if ( strcmp( g_Interfaces->panel->getPanelName( panel ), "MatSystemTopPanel" ) == 0 )
		targetPanel = panel;

	if ( panel != targetPanel )
		return;

	if ( !g_Renderer->isUsingSurface( ) )
		return;

	g_Features->onPaint( panel );

	auto getCurrentTime = [ ] ( ) -> std::string
	{
		auto now = std::chrono::system_clock::now( );
		auto in_time_t = std::chrono::system_clock::to_time_t( now );

		std::stringstream ss;
		ss << std::put_time( std::localtime( &in_time_t ), "%X" );
		return ss.str( );
	};

	int x, y;
	g_Interfaces->gameEngine->getScreenSize( x, y );
	x /= 2; y /= 2;


	SplashScreen();

	if (config->get_bool("espWatermark"))
	{
		std::stringstream ss;
		auto net_channel = g_Interfaces->gameEngine->netchannelInfo();

		

		std::string outgoing = Globals::localPlayer ? std::to_string((int)(net_channel->GetLatency(FLOW_OUTGOING) * 1000)) : "0";

		int width, height;
		g_Interfaces->gameEngine->getScreenSize(width, height);

		static int fps, old_tick_count;

		if ((g_Interfaces->globalVars->tickcount - old_tick_count) > 50) {
			fps = static_cast<int>(1.f / g_Interfaces->globalVars->frametime);
			old_tick_count = g_Interfaces->globalVars->tickcount;
		}
		char FPS;
		int radix = 10;
		static char buffer[256];
		static unsigned int last_time;


		/////rainbow sync//////
		last_time = GetTickCount();

		//////////////////////

		auto local_player = Globals::localPlayer;
		std::string incoming = local_player ? std::to_string((int)(net_channel->GetLatency(FLOW_INCOMING) * 1000)) : "0";

		//ss << "[WarHooK BETA] | YOUR NICK" << " | FPS: " << fps << " | ping: " << incoming.c_str();

		//RECT TextSize = g_pSurface->GetTextSizeRect(Globals::CourierNew, ss.str().c_str());

		//g_pSurface->FilledRect(width - (TextSize.right + 20), 25, TextSize.right + 10, 4, rainbow);
		//g_pSurface->FilledRect(width - (TextSize.right + 20), 5, TextSize.right + 10, 20, Color(0, 0, 0, 180));
		//g_pSurface->DrawT(width - (TextSize.right + 15), 8, Color(rainbow), Globals::CourierNew, false, ss.str().c_str());

		//FPS loss
		/*GradientLine(0, 0, width / 2, 4, Color(g, b, r, 255), Color(r, g, b, 255));
		GradientLine(width / 2, 0, width / 2, 4, Color(r, g, b, 255), Color(b, r, g, 255));*/
		int MenuAlpha_Main;

	https://agn.pw/api/exposure/username.php

		std::time_t result = std::time(nullptr);
		std::string time = std::asctime(std::localtime(&result));
		/*std::asctime(std::localtime(&result))*/







		//GMARK
		ss << "Exp0sure | Version: 2.0 [Dev Build]" << " | FPS: " << fps << " | Ping: " << incoming.c_str() << " | Time: " << std::asctime(std::localtime(&result));
		int x, y;
		y = 575;


		g_Renderer->fillRectangle(width - (y + 23), 3, y + 18, 25, Color(40, 40, 40, 225));
		g_Renderer->fillRectangle(width - (y + 18), 8, y + 8, 15, Color(10, 10, 10, 225));
		g_Renderer->rectangle(width - (y + 18), 8, y + 8, 15, Color(200, 200, 200, 50));
		g_Renderer->rectangle(width - (y + 23), 3, y + 18, 25, Color(200, 200, 200, 50));
		/*GradientLine(width - 244, 17, 228, 1, Color(c_config::get().menu_color_r, c_config::get().menu_color_g, c_config::get().menu_color_b, 255), Color(0, 0, 0, 255));*/

		g_Renderer->gradient(width - (y + 18), 8, y + 8, 1, Color(59, 81, 245), Color(212, 108, 252));
		g_Renderer->text(width - (y + 13), 8,g_Fonts->watermark, ss.str().c_str(), Color(255,255,255), false);


		float distance = 40.f;

		
	}



	if (config->get_bool("espAutowallIndicator") && Globals::localPlayer && Globals::localPlayer->alive( ) && g_Interfaces->gameEngine->inGame( ) ) {
		float damage = 0.f;
		g_Renderer->fillRectangle( x - 1, y - 1, 3, 3, g_Autowall.CanWallbang( damage ) ? Color( 0, 255, 0 ) : Color( 255, 0, 0 ) );
		if (g_Autowall.CanWallbang(damage))
			g_Renderer->text(x + 5, y + 5, g_Fonts->visuals, std::to_string(damage), Color(0, 255, 0), true);
	}
	


	/*if (config->get_bool("aaEnable") && config_system.item.config.bind_antiaimflip_key && Globals::localPlayer && Globals::localPlayer->alive() && g_Interfaces->gameEngine->inGame())
	{
		if (GetAsyncKeyState(config_system.item.config.bind_antiaimflip_key))
		{
			g_Renderer->text(x + 40, y, (g_Fonts->indicator), ">", Color(255, 255, 255, 55), true);
			g_Renderer->text(x - 40, y, (g_Fonts->indicator), "<", Color::MenuMain, true);
		}
		else
		{
			g_Renderer->text(x + 40, y, (g_Fonts->indicator), ">", Color::MenuMain, true);
			g_Renderer->text(x - 40, y, (g_Fonts->indicator), "<", Color(255, 255, 255, 55), true);
		}
	}*/
}

static bool initialized = false;

long __stdcall CHookManager::onPaintDX9(LPDIRECT3DDEVICE9 dx9Device, LPRECT sourceRect, LPRECT destRect, HWND destWindowOverride, LPRGNDATA dirtyRegion)
{
	static auto originalFn = g_Hooks->dx9DeviceHook->getOriginalFunction<decltype(&onPaintDX9)>(17);

	if (!initialized)
	{
		g_renderer_d3d->init(dx9Device);
		initialized = true;
	}

	g_renderer_d3d->begin();

	if (initialized)
	{
		if (!g_Interfaces->gameEngine->inGame() && !Globals::eventInfo.empty())
			Globals::eventInfo.clear();

		if (Globals::localPlayer && Globals::localPlayer->alive() && g_Interfaces->gameEngine->inGame()) {
			player_hurt::get().paint();
		}

		// LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOL ~alpha
		g_Misc.drawIndicators();
		g_Visuals.inaccuracyOverlay();

		menu->think();
		menu->draw();
	}

	g_renderer_d3d->end();

	if (!g_Renderer->isUsingDX9())
		return originalFn(dx9Device, sourceRect, destRect, destWindowOverride, dirtyRegion);

	g_Renderer->preRender();
	g_Features->onPaint();
	g_Renderer->postRender();

	return originalFn(dx9Device, sourceRect, destRect, destWindowOverride, dirtyRegion);
}