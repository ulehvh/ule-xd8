#include "../managers/hookManager.hpp"
#include "../../features/Aimbot/LagCompensation.hpp"


void PearlModulate(IMaterial* mat,float r,float g,float b) {
	bool bFoundlocalP = false;
	auto pVarlocalP = mat->FindVar("$envmaptint", &bFoundlocalP);
	if (bFoundlocalP)
		(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVarlocalP + 44))((uintptr_t)pVarlocalP, r / 255.f, g / 255.f, b / 255.f);
}


using drawModelExecuteFn = void( __thiscall* )( IVModelRender*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t* );
void __stdcall  CHookManager::drawModelExecute( void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* matrix ) {

	auto originalFn = g_Hooks->modelRenderHook->getOriginalFunction<drawModelExecuteFn>( 21 );

	if ( g_Interfaces->modelRender->IsForcedMaterialOverride( ) ) {
		return originalFn( g_Interfaces->modelRender, context, state, info, matrix );
	}
	//very smart fix yes? -Sithis
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

	const char* chamsmatViewmodel[]{
		"Exp0sure_materialVM",
		"Exp0sure_reflectiveVM",
		"Exp0sure_flatVM",
		"Exp0sure_glowOverlayVM",
		"Exp0sure_AnimatedVM"
	};

	static IMaterial* material = nullptr;
	static IMaterial* localmaterial = nullptr;
	static IMaterial* vmMaterial = nullptr;
	static IMaterial* lcMat = g_Interfaces->materialSystem->FindMaterial("Exp0sure_Records", TEXTURE_GROUP_OTHER);
	static IMaterial* glow = g_Interfaces->materialSystem->FindMaterial("glowOverlay", TEXTURE_GROUP_OTHER);
	
	localmaterial = g_Interfaces->materialSystem->FindMaterial(chamsmatLocal[config->get_int("localchamsMaterial")], TEXTURE_GROUP_OTHER);
	material = g_Interfaces->materialSystem->FindMaterial(chamsmatEnemy[config->get_int("chamsMaterial")], TEXTURE_GROUP_OTHER);
	vmMaterial = g_Interfaces->materialSystem->FindMaterial(chamsmatViewmodel[config->get_int("vmchamsMaterial")], TEXTURE_GROUP_OTHER);
	

	



	CBaseEntity* entity = reinterpret_cast< CBaseEntity* >( g_Interfaces->clientEntity->GetClientEntity( info.entity_index ) );
	std::string model_name = g_Interfaces->modelInfo->getModelName( ( model_t* )info.pModel );

	if ( g_Interfaces->gameEngine->inGame( ) ) {
		if ( Globals::localPlayer )
		{
			if (config->get_bool("espEnable"))
			{

				if (model_name.find("models/weapons/v_models/arms") != std::string::npos)
				{
					// arms
					if (config->get_bool("chamsHands"))
					{
						vmMaterial->AlphaModulate(config->get_color("colorChamsHands").a() / 255.f);
						vmMaterial->ColorModulate(config->get_color("colorChamsHands").r() / 255.f,
							config->get_color("colorChamsHands").g() / 255.f,
							config->get_color("colorChamsHands").b() / 255.f);

						PearlModulate(vmMaterial, config->get_color("colorChamsHands").r(), config->get_color("colorChamsHands").g(), config->get_color("colorChamsHands").b());

						vmMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						
						g_Interfaces->modelRender->ForcedMaterialOverride(vmMaterial);
						originalFn(g_Interfaces->modelRender, context, state, info, matrix);
						vmMaterial->IncrementReferenceCount();
					
					}
				}
				else if (model_name.find("models/weapons") != std::string::npos)
				{
					if (!(model_name.find("models/weapons/v_models/arms") != std::string::npos))
					{
						// arms
						if (config->get_bool("chamsWeapon"))
						{
							vmMaterial->AlphaModulate(config->get_color("colorChamsWeapon").a() / 255.f);
							vmMaterial->ColorModulate(config->get_color("colorChamsWeapon").r() / 255.f,
								config->get_color("colorChamsWeapon").g() / 255.f,
								config->get_color("colorChamsWeapon").b() / 255.f);

							PearlModulate(vmMaterial, config->get_color("colorChamsWeapon").r(), config->get_color("colorChamsWeapon").g(), config->get_color("colorChamsWeapon").b());

							vmMaterial->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							g_Interfaces->modelRender->ForcedMaterialOverride(vmMaterial);
							originalFn(g_Interfaces->modelRender, context, state, info, matrix);
							vmMaterial->IncrementReferenceCount();
							
						}
					}
				}

				if ( model_name.find( "models/player" ) != std::string::npos )
				{
					//if ( material && materialIgnoreZ && materialDefault && materialDefaultIgnoreZ && materialFlat && materialFlatIgnoreZ && materialShaded && materialShadedIgnoreZ && materialMetallic && materialMetallicIgnoreZ && materialGlow && materialGlowIgnoreZ && materialBubble && materialBubbleIgnoreZ && lcMat )
					
						if ( entity )
						{
							
							if ( entity->team( ) != Globals::localPlayer->team( ) ) // enemy
							{
								
								if (config->get_bool("hitChams") && !Globals::m_vecMatrices.empty()) {
									for (size_t i = 0; i < Globals::m_vecMatrices.size(); i++)
									{

										

										if (Globals::m_vecMatrices[i].flDisplayTime + 5.f < g_Interfaces->globalVars->curtime )
											Globals::m_vecMatrices.erase(Globals::m_vecMatrices.begin() + i);

										


										PearlModulate(glow, config->get_color("colorHitmarkerChams").r(), config->get_color("colorHitmarkerChams").g(), config->get_color("colorHitmarkerChams").b());


										glow->AlphaModulate(Globals::m_vecMatrices[i].flDisplayTime + 5.f - g_Interfaces->globalVars->curtime);
										g_Interfaces->modelRender->ForcedMaterialOverride(glow);

										originalFn(g_Interfaces->modelRender, context, state, info, Globals::m_vecMatrices[i].matrix);
										glow->IncrementReferenceCount();
										//g_Interfaces->modelRender->ForcedMaterialOverride(glow);
									}
									//"Un"-force the material after the loop

								}
								// man we're so fucking smart, we better hook drawmodel instead of drawmodelexecute, because POLAK DOES IT. right? ~alpha
								if ( entity->alive() )
								{

									



									auto& records = g_LagCompensation.PlayerRecord[ entity->EntIndex( ) ].records;
									if ( !records.empty( ) ) {
										matrix3x4_t matrix[ 128 ];
										std::memcpy( matrix, records.front( ).boneMatrix, sizeof( records.front( ).boneMatrix ) );

										// lag comp 
										if ( Globals::localPlayer->alive( ) && lcMat && config->get_bool("chamsShadow") && !entity->immunity( ) )
										{
											lcMat->AlphaModulate(config->get_color("colorChamsShadow").a() / 255.f);
											lcMat->ColorModulate(config->get_color("colorChamsShadow").r() / 255.f,
												config->get_color("colorChamsShadow").g() / 255.f,
												config->get_color("colorChamsShadow").b() / 255.f);
											//lcMat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
										
											g_Interfaces->modelRender->ForcedMaterialOverride( lcMat );
											originalFn( g_Interfaces->modelRender, context, state, info, matrix );
											lcMat->IncrementReferenceCount();
											//g_Interfaces->modelRender->ForcedMaterialOverride(lcMat);
										}

									}

									
								

									
									if (config->get_bool("chamsEnemyXQZ")) 
									{
										

										material->AlphaModulate(config->get_color("colorChamsEnemyXQZ").a() / 255.f);
										material->ColorModulate(config->get_color("colorChamsEnemyXQZ").r() / 255.f,
											config->get_color("colorChamsEnemyXQZ").g() / 255.f,
											config->get_color("colorChamsEnemyXQZ").b() / 255.f);
										//material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
										
										if (config->get_bool("enemyPearl")) {
											PearlModulate(material, config->get_color("enemyPearlColor").r(), config->get_color("enemyPearlColor").g(), config->get_color("enemyPearlColor").b());
											
										}
										
										g_Interfaces->modelRender->ForcedMaterialOverride(material);
										originalFn(g_Interfaces->modelRender, context, state, info, matrix);
										material->IncrementReferenceCount();
										//g_Interfaces->modelRender->ForcedMaterialOverride(material);
									}
								}
							}
						
							if ( entity == Globals::localPlayer && Globals::inThirdperson && Globals::localPlayer->alive( ) )
							{
								// i'm sorry 
								bool shouldBlend = Globals::localPlayer->scoped( );
								float blendAmt = config->get_color("colorChamsLocal").a() / 255.f;

								if ( Globals::localPlayer->activeWeapon( ) )
								{
									auto alpha = config->get_color("colorChamsLocal").a() / 255.f;
									if ( Globals::localPlayer->activeWeapon( )->zoomLevel( ) == 1 )
										blendAmt =  0.25f;
									else if ( Globals::localPlayer->activeWeapon( )->zoomLevel( ) == 2 )
										blendAmt =  0.5f;
								
								}

								// Fake angle chams
						
								if (config->get_bool("chamsLocalDesync") && config->get_bool("aaDesync") && g_Interfaces->globalVars->curtime - Globals::localPlayer->m_flSpawnTime() > 3.f && Globals::got_fake_matrix)
								{
									for (auto& i :Globals::desyncMatrix)
									{
										i[0][3] += info.origin.x;
										i[1][3] += info.origin.y;
										i[2][3] += info.origin.z;
									}
									
									
									material->AlphaModulate(config->get_color("colorDesyncModel").a() / 255.f);
									material->ColorModulate(config->get_color("colorDesyncModel").r() / 255.f,
										config->get_color("colorDesyncModel").g() / 255.f,
										config->get_color("colorDesyncModel").b() / 255.f);

									g_Interfaces->modelRender->ForcedMaterialOverride( material );
									originalFn( g_Interfaces->modelRender, context, state, info, Globals::desyncMatrix );
									g_Interfaces->modelRender->ForcedMaterialOverride(nullptr);


									for (auto& i : Globals::desyncMatrix)
									{
										i[0][3] -= info.origin.x;
										i[1][3] -= info.origin.y;
										i[2][3] -= info.origin.z;
									}
								
								}

								// Regular chams
								if (config->get_bool("chamsLocal")) {

									

									localmaterial->AlphaModulate( shouldBlend ? blendAmt : config->get_color("colorChamsLocal").a() / 255.f);
									


									
									localmaterial->ColorModulate(config->get_color("colorChamsLocal").r() / 255.f,
											config->get_color("colorChamsLocal").g() / 255.f,
											config->get_color("colorChamsLocal").b() / 255.f);
									
										if (config->get_bool("localPearl")) {
											PearlModulate(localmaterial, config->get_color("localPearl").r(), config->get_color("localPearl").g(), config->get_color("localPearl").b());											
										}
									localmaterial->IncrementReferenceCount();
									localmaterial->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false);
									g_Interfaces->modelRender->ForcedMaterialOverride(localmaterial);
									originalFn( g_Interfaces->modelRender, context, state, info, matrix );
									localmaterial->IncrementReferenceCount();
									//g_Interfaces->modelRender->ForcedMaterialOverride(localmaterial);
								}
							}
						}
						
					
				}
			}
		}
	}

	originalFn( g_Interfaces->modelRender, context, state, info, matrix );
	g_Interfaces->modelRender->ForcedMaterialOverride( nullptr );
}