#include "../../utilities/includes.h"

#include "../../features/Visuals/Visuals.hpp"
#include "../../features/Events/bullet_impact.hpp"

#include <fstream>




void __fastcall CHookManager::sceneEnd(void* thisptr, void* edx)
{
	static auto originalFn = g_Hooks->renderViewHook->getOriginalFunction<decltype(&sceneEnd)>(9);

	originalFn(thisptr, edx);

	const char* chamsmatLocal[]{
		"Exp0sure_materialL",
		"Exp0sure_reflectiveL",
		"Exp0sure_flatL",
		"Exp0sure_glowOverlayL",
		"Exp0sure_AnimatedL"
	};

	const char* chamsmatEnemy[]{
		"Exp0sure_materialE",
		"Exp0sure_reflectiveE",
		"Exp0sure_flatE",
		"Exp0sure_glowOverlayE",
		"Exp0sure_AnimatedE"
	};




	static IMaterial* material = nullptr;
	static IMaterial* localmaterial = nullptr;


	localmaterial = g_Interfaces->materialSystem->FindMaterial(chamsmatLocal[config->get_int("localchamsMaterial")], TEXTURE_GROUP_OTHER);
	material = g_Interfaces->materialSystem->FindMaterial(chamsmatEnemy[config->get_int("chamsMaterial")], TEXTURE_GROUP_OTHER);



	bool bFoundlocalF = false;
	auto pVarlocalF = localmaterial->FindVar("$envmaptint", &bFoundlocalF);
	localmaterial->IncrementReferenceCount();
	bool bFoundlocalE = false;
	auto pVarlocalE = material->FindVar("$envmaptint", &bFoundlocalE);
	material->IncrementReferenceCount();
	if (config->get_bool("espEnable"))
	{

		for (int i = 1; i < g_Interfaces->globalVars->maxClients; ++i)
		{
			CBaseEntity* pPlayerEntity = g_Interfaces->clientEntity->GetClientEntity(i);

			if (!pPlayerEntity || !pPlayerEntity->alive() || pPlayerEntity->IsDormant() || !Globals::localPlayer)
				continue;
			
			//--------------------------------------------------PEARLESCENCE------------------------------------------------------
			if (config->get_bool("localPearl")) {
				
				if (bFoundlocalF)
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVarlocalF + 44))((uintptr_t)pVarlocalF, config->get_color("localPearlColor").r() / 255.f, config->get_color("localPearlColor").g() / 255.f, config->get_color("localPearlColor").b() / 255.f);
			}
			else {
				if (bFoundlocalF)
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVarlocalF + 44))((uintptr_t)pVarlocalF, 1.f, 1.f, 1.f);
			}

			if (config->get_bool("enemyPearl")) {
				
				if (bFoundlocalE)
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVarlocalE + 44))((uintptr_t)pVarlocalE, config->get_color("enemyPearlColor").r() / 255.f, config->get_color("enemyPearlColor").g() / 255.f, config->get_color("enemyPearlColor").b() / 255.f);
			}
			else {
				if (bFoundlocalE)
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVarlocalE + 44))((uintptr_t)pVarlocalE, 1.f, 1.f, 1.f);
			}
			//--------------------------------------------------PEARLESCENCE------------------------------------------------------


			//--------------------------------------------------LOCALCHAMS--------------------------------------------------------
			if (pPlayerEntity == Globals::localPlayer && config->get_bool("chamsLocal")) {
				localmaterial->AlphaModulate(config->get_color("colorChamsLocal").a() / 255.f);
				g_Interfaces->renderView->SetColorModulation(config->get_color("colorChamsLocal").r() / 255.f,config->get_color("colorChamsLocal").g() / 255.f,config->get_color("colorChamsLocal").b() / 255.f);
				localmaterial->IncrementReferenceCount();
				localmaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				
				g_Interfaces->modelRender->ForcedMaterialOverride(localmaterial);
				pPlayerEntity->DrawModel(0x1, 255);
				localmaterial->IncrementReferenceCount();
				g_Interfaces->modelRender->ForcedMaterialOverride(nullptr);

			}
			//--------------------------------------------------LOCALCHAMS--------------------------------------------------------
			//--------------------------------------------------ENEMYCHAMS--------------------------------------------------------
			else if (pPlayerEntity != Globals::localPlayer && config->get_bool("chamsEnemyXQZ") && pPlayerEntity->team() != Globals::localPlayer->team()) {
				material->AlphaModulate(config->get_color("colorChamsEnemyXQZ").a() / 255.f);
				g_Interfaces->renderView->SetColorModulation(config->get_color("colorChamsEnemyXQZ").r() / 255.f,config->get_color("colorChamsEnemyXQZ").g() / 255.f,config->get_color("colorChamsEnemyXQZ").b() / 255.f);
				
				material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				g_Interfaces->modelRender->ForcedMaterialOverride(material);
				pPlayerEntity->DrawModel(0x1, 255);
				material->IncrementReferenceCount();
				g_Interfaces->modelRender->ForcedMaterialOverride(nullptr);
			}
			//--------------------------------------------------ENEMYCHAMS--------------------------------------------------------



		}
	}

	g_Visuals.glow();
}