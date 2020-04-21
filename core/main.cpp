#include <chrono>
#include <thread>
#include <windows.h>

#include "../menu/main/setup.hpp"
#include "../features/Events/hitsounds.hpp"
#include "../utilities/includes.h"
#include "../features/features.hpp"
#include "../features/Misc/Animations.hpp"

//#define _DEV

void chams_material_setup() {
	std::ofstream("csgo/materials/catsense_materialL.vmt") << R"#("VertexLitGeneric" {
            "$basetexture" "vgui/white_additive"
            "$ignorez"      "1"
            "$envmap"       ""
            "$nofog"        "1"
            "$model"        "1"
            "$nocull"       "0"
            "$selfillum"    "1"
            "$halflambert"  "1"
            "$znearer"      "0"
            "$flat"         "0"
			"$phong"		"1"
			"$rimlight"		"1"
        })#";
	std::ofstream("csgo/materials/catsense_materialE.vmt") << R"#("VertexLitGeneric" {
            "$basetexture" "vgui/white_additive"
            "$ignorez"      "1"
            "$envmap"       ""
            "$nofog"        "1"
            "$model"        "1"
            "$nocull"       "0"
            "$selfillum"    "1"
            "$halflambert"  "1"
            "$znearer"      "0"
            "$flat"         "0"
			"$phong"		"1"
			"$rimlight"		"1"
        })#";
	std::ofstream("csgo/materials/catsense_materialVM.vmt") << R"#("VertexLitGeneric" {
            "$basetexture" "vgui/white_additive"
            "$ignorez"      "1"
            "$envmap"       ""
            "$nofog"        "1"
            "$model"        "1"
            "$nocull"       "0"
            "$selfillum"    "1"
            "$halflambert"  "1"
            "$znearer"      "0"
            "$flat"         "0"
			"$phong"		"1"
			"$rimlight"		"1"
        })#";
	std::ofstream("csgo/materials/catsense_reflectiveL.vmt") << R"#("VertexLitGeneric" {
			"$basetexture" "vgui/white_additive"
			"$ignorez" "1"
			"$envmap" "env_cubemap"
			"$normalmapalphaenvmapmask" "1"
			"$envmapcontrast"  "1"
			"$nofog" "1"
			"$model" "1"
			"$nocull" "0"
			"$selfillum" "1"
			"$halflambert" "1"
			"$znearer" "0"
			"$flat" "1"
		})#";
	std::ofstream("csgo/materials/catsense_reflectiveE.vmt") << R"#("VertexLitGeneric" {
			"$basetexture" "vgui/white_additive"
			"$ignorez" "1"
			"$envmap" "env_cubemap"
			"$normalmapalphaenvmapmask" "1"
			"$envmapcontrast"  "1"
			"$nofog" "1"
			"$model" "1"
			"$nocull" "0"
			"$selfillum" "1"
			"$halflambert" "1"
			"$znearer" "0"
			"$flat" "1"
		})#";
	std::ofstream("csgo/materials/catsense_reflectiveVM.vmt") << R"#("VertexLitGeneric" {
			"$basetexture" "vgui/white_additive"
			"$ignorez" "1"
			"$envmap" "env_cubemap"
			"$normalmapalphaenvmapmask" "1"
			"$envmapcontrast"  "1"
			"$nofog" "1"
			"$model" "1"
			"$nocull" "0"
			"$selfillum" "1"
			"$halflambert" "1"
			"$znearer" "0"
			"$flat" "1"
		})#";
	std::ofstream("csgo/materials/catsense_glowOverlayL.vmt") << R"#("VertexLitGeneric" {
 
			"$additive" "1"
			"$ignorez"      "1"
			"$envmap" "models/effects/cube_white"
			"$envmapfresnel" "1"
			"$envmapfresnelminmaxexp" "[0 1 2]"
			"$alpha" "0.8"
		})#";
	std::ofstream("csgo/materials/catsense_glowOverlayE.vmt") << R"#("VertexLitGeneric" {
 
			"$additive" "1"
			"$ignorez"      "1"
			"$envmap" "models/effects/cube_white"
			"$envmapfresnel" "1"
			"$envmapfresnelminmaxexp" "[0 1 2]"
			"$alpha" "0.8"
		})#";
	std::ofstream("csgo/materials/catsense_glowOverlayVM.vmt") << R"#("VertexLitGeneric" {
 
			"$additive" "1"
			"$ignorez"      "1"
			"$envmap" "models/effects/cube_white"
			"$envmapfresnel" "1"
			"$envmapfresnelminmaxexp" "[0 1 2]"
			"$alpha" "0.8"
		})#";
	std::ofstream("csgo/materials/catsense_flatL.vmt") << R"#(UnlitGeneric { })#";
	std::ofstream("csgo/materials/catsense_flatE.vmt") << R"#(UnlitGeneric { })#";
	std::ofstream("csgo/materials/catsense_flatVM.vmt") << R"#(UnlitGeneric { })#";

	std::ofstream("csgo/materials/catsense_AnimatedL.vmt") << R"#(VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1  $alpha 0.8  $envmaptint "[.7 .7 .7]" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } })#";
	std::ofstream("csgo/materials/catsense_AnimatedE.vmt") << R"#(VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1 $envmaptint "[.7 .7 .7]" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } })#";
	std::ofstream("csgo/materials/catsense_AnimatedVM.vmt") << R"#(VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1 $envmaptint "[.7 .7 .7]" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } })#";

	std::ofstream("csgo/materials/catsense_Records.vmt") << R"#("UnlitGeneric" {
			"$basetexture" "vgui/white_additive"
			"$ignorez"      "1"
			"$nofog"        "1"
			"$model"        "1"
			"$nocull"       "0"
			"$selfillum"    "1"
			"$halflambert"  "1"
			"$znearer"      "0"
			"$flat"         "1"
		})#";

	printf("Materials initialized!\n");
}

void initializeCheat(void* modulePtr)
{
	while (!GetModuleHandleA("serverbrowser.dll"))
		std::this_thread::sleep_for(std::chrono::milliseconds(175));

#if defined(_DEV) || defined(_DEBUG)
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
#endif

	g_Interfaces->initialize();

	chams_material_setup();

	PlaySoundA(ps2_wav, NULL, SND_MEMORY | SND_ASYNC);


	g_NetVarManager = new CNetVarManager();

	//this is my cool feature system ;))))))) ~czapek
	g_Features->registerFeature("Visuals", new CVisuals());
	g_Features->registerFeature("Misc", new CMisc());

	g_Renderer->initialize((LPDIRECT3DDEVICE9)g_Interfaces->dx9Device);
	g_Renderer->useSurface();

	setup->setup();
	g_Hooks->initialize();

	//g_Interfaces->gameUI->messageBox("catsense", "Loaded, Make sure Multisampling Anti-Aliasing Mode is OFF or the cheat will not open.");

	while (!config->get_bool("unload"))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	g_Interfaces->inputSystem->enableInput(true);

	g_Features->unregisterFeature("Visuals");
	g_Features->unregisterFeature("Misc");

	g_Hooks->shutdown();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

#if defined(_DEV) || defined(_DEBUG)
	fclose(stdout);
	FreeConsole();
#endif

	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(modulePtr), 0);
}

bool __stdcall DllMain(void* Module, unsigned long Reason, void* Reserved)
{
	DisableThreadLibraryCalls((HMODULE)Module);
	switch (Reason) {
	case DLL_PROCESS_ATTACH:
	{
		CreateDirectoryA(("C:\\catsense"), NULL);
		CreateDirectoryA(("C:\\catsense\\logs"), NULL);
		CreateDirectoryA(("C:\\catsense\\configs"), NULL);
		CreateDirectoryA(("C:\\catsense\\resources"), NULL);
		CreateDirectoryA(("C:\\catsense\\resources\\fonts"), NULL);

		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(initializeCheat), Module, 0, 0);
	}
	break;
	}

	return true;
}