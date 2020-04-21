#include "../managers/hookManager.hpp"
#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -MAX_COORD_FLOAT )
struct RenderableInfo_t {
	IClientRenderable* m_pRenderable;
	void* m_pAlphaProperty;
	int m_EnumCount;
	int m_nRenderFrame;
	unsigned short m_FirstShadow;
	unsigned short m_LeafList;
	short m_Area;
	uint16_t m_Flags;   // 0x0016
	uint16_t m_Flags2; // 0x0018
	Vector3 m_vecBloatedAbsMins;
	Vector3 m_vecBloatedAbsMaxs;
	Vector3 m_vecAbsMins;
	Vector3 m_vecAbsMaxs;
	int pad;
};

int __fastcall CHookManager::hkListLeavesInBox(void* bsp, void* edx, Vector3& mins, Vector3& maxs, unsigned short* pList, int listMax) {

	typedef int(__thiscall* ListLeavesInBox)(void*, const Vector3&, const Vector3&, unsigned short*, int);
	static auto ofunc = g_Hooks->listLeavesHook->getOriginalFunction<ListLeavesInBox>(6);

	// occulusion getting updated on player movement/angle change,
	// in RecomputeRenderableLeaves ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L674 );
	// check for return in CClientLeafSystem::InsertIntoTree
	if (!Globals::localPlayer->alive() || *(uint32_t*)_ReturnAddress() != 0x14244489) // 89 44 24 14 ( 0x14244489 ) - new / 8B 7D 08 8B ( 0x8B087D8B ) - old
		return ofunc(bsp, mins, maxs, pList, listMax);

	//g_Interfaces->gameEngine->GetBSPTreeQuery();

	// get current renderable info from stack ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470 )
	auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);
	if (!info || !info->m_pRenderable)
		return ofunc(bsp, mins, maxs, pList, listMax);

	// check if disabling occulusion for players ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1491 )
	auto base_entity = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity();
	if (!base_entity || !base_entity->IsPlayer())
		return ofunc(bsp, mins, maxs, pList, listMax);

	// fix render order, force translucent group ( https://www.unknowncheats.me/forum/2429206-post15.html )
	// AddRenderablesToRenderLists: https://i.imgur.com/hcg0NB5.png ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473 )
	info->m_Flags &= ~0x100;
	info->m_Flags2 |= 0xC0;

	// extend world space bounds to maximum ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707 )
	static const Vector3 map_min = Vector3(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
	static const Vector3 map_max = Vector3(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
	auto count = ofunc(bsp, map_min, map_max, pList, listMax);
	return count;
}
