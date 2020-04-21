#include "baseEntity.hpp"
#include "../utilities/memory.hpp"
#include "../utilities/math.hpp"
#include "modelInfo.hpp"

Vector3 CBaseEntity::hitboxPosition(int Hitbox, matrix3x4_t *Matrix, float *Radius)
{
	studiohdr_t* hdr = g_Interfaces->modelInfo->getStudiomodel(this->GetModel());
	mstudiohitboxset_t* set = hdr->getHitboxSet(0);
	mstudiobbox_t* hitbox = set->getHitbox(Hitbox);

	if (hitbox)
	{
		Vector3 vMin, vMax, vCenter, sCenter;
		g_Math.vectorTransform(hitbox->min, Matrix[hitbox->bone], vMin);
		g_Math.vectorTransform(hitbox->max, Matrix[hitbox->bone], vMax);
		vCenter = (vMin + vMax) * 0.5;

		*Radius = hitbox->radius;

		return vCenter;
	}

	return Vector3(0, 0, 0);
}

Vector3 CBaseEntity::hitboxPosition(int Hitbox, matrix3x4_t *Matrix)
{
	studiohdr_t* hdr = g_Interfaces->modelInfo->getStudiomodel(this->GetModel());
	mstudiohitboxset_t* set = hdr->getHitboxSet(0);
	mstudiobbox_t* hitbox = set->getHitbox(Hitbox);

	if (hitbox)
	{
		Vector3 vMin, vMax, vCenter, sCenter;
		g_Math.vectorTransform(hitbox->min, Matrix[hitbox->bone], vMin);
		g_Math.vectorTransform(hitbox->max, Matrix[hitbox->bone], vMax);
		vCenter = (vMin + vMax) * 0.5;

		return vCenter;
	}

	return Vector3(0, 0, 0);
}

Vector3 CBaseEntity::getHitbox( int hitbox ) {
	matrix3x4_t matrix[ 128 ];

	if ( this->SetupBones( matrix, 128, 256, g_Interfaces->globalVars->curtime ) ) {
		studiohdr_t* hdr = g_Interfaces->modelInfo->getStudiomodel( this->GetModel( ) );
		mstudiohitboxset_t* set = hdr->getHitboxSet( 0 );
		mstudiobbox_t* box = set->getHitbox( hitbox );

		if ( box ) {
			Vector3 min, max, vec_center, screen_center;
			g_Math.vectorTransform( box->min, matrix[ box->bone ], min );
			g_Math.vectorTransform( box->max, matrix[ box->bone ], max );
			vec_center = ( min + max ) * 0.5;

			return vec_center;
		}
	}

	return Vector3( 0, 0, 0 );
}

CAnimationLayer* CBaseEntity::GetAnimOverlays()
{
	// to find offset: use 9/12/17 dll
	// sig: 55 8B EC 51 53 8B 5D 08 33 C0
	return *(CAnimationLayer**)((DWORD)this + 0x2980);
}


void CBaseEntity::UpdateAnimationState(CBasePlayerAnimState* state, Vector3 angle)
{
	if (!state)
		return;

	static auto UpdateAnimState = g_Memory->findPattern("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");//sigchange
	if (!UpdateAnimState)
		return;

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}
void CBaseEntity::CreateAnimationState(CBasePlayerAnimState* state)
{
	using CreateAnimState_t = void(__thiscall*)(CBasePlayerAnimState*, CBaseEntity*);
	static auto CreateAnimState = (CreateAnimState_t)g_Memory->findPattern("client_panorama.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, Globals::localPlayer);
}
void CBaseEntity::ResetAnimationState(CBasePlayerAnimState* state)
{
	if (!state)
		return;

	using reset_animstate_t = void(__thiscall*)(CBasePlayerAnimState*);
	static auto rest_animstate = (reset_animstate_t)g_Memory->findPattern("client_panorama.dll", "56 6A 01 68 ? ? ? ? 8B F1");
	if (!rest_animstate)
		return;

	rest_animstate(state);
}


void CBaseEntity::fixSetupBones(matrix3x4_t* Matrix, float simtime)
{
	int Backup = *(int*)((uintptr_t)this + 0x274);
	*(int*)((uintptr_t)this + 0x274) = 0;
	Vector3 absOriginBackupLocal = this->GetAbsOrigin();
	this->setAbsOrigin(this->vecOrigin());
	this->SetupBones(Matrix, 128, 256, simtime);
	this->setAbsOrigin(absOriginBackupLocal);
	*(int*)((uintptr_t)this + 0x274) = Backup;
}

CBaseCombatWeapon* CBaseEntity::activeWeapon()
{
	static int m_hActiveWeapon = g_NetVarManager->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	const auto weaponData = getValue<CBaseHandle>(m_hActiveWeapon);
	return reinterpret_cast<CBaseCombatWeapon*>(g_Interfaces->clientEntity->GetClientEntityFromHandle(weaponData));
}

void CBaseEntity::setAbsAngles(Vector3 angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const Vector3 & angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)g_Memory->findPattern("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");

	SetAbsAngles(this, angles);
}

void CBaseEntity::setAbsOrigin(Vector3 ArgOrigin)
{
	using Fn = void(__thiscall*)(void*, const Vector3 &ArgOrigin);
	static Fn AbsOrigin = (Fn)g_Memory->findPattern("client_panorama.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");

	AbsOrigin(this, ArgOrigin);
}

bool CBaseEntity::isKnifeorNade()
{
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)this->activeWeapon();

	if (!pWeapon)
		return false;

	std::string WeaponName = pWeapon->name();

	if (WeaponName == "weapon_knife")
		return true;
	else if (WeaponName == "weapon_incgrenade")
		return true;
	else if (WeaponName == "weapon_decoy")
		return true;
	else if (WeaponName == "weapon_flashbang")
		return true;
	else if (WeaponName == "weapon_hegrenade")
		return true;
	else if (WeaponName == "weapon_smokegrenade")
		return true;
	else if (WeaponName == "weapon_molotov")
		return true;

	return false;
}

bool CBaseEntity::isKnife()
{
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)this->activeWeapon();

	if (!pWeapon)
		return false;

	std::string WeaponName = pWeapon->name();

	if (WeaponName == "weapon_knife")
		return true;

	return false;
}

bool CBaseEntity::isNade()
{
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)this->activeWeapon();

	if (!pWeapon)
		return false;

	std::string WeaponName = pWeapon->name();

	if (WeaponName == "weapon_incgrenade")
		return true;
	else if (WeaponName == "weapon_decoy")
		return true;
	else if (WeaponName == "weapon_flashbang")
		return true;
	else if (WeaponName == "weapon_hegrenade")
		return true;
	else if (WeaponName == "weapon_smokegrenade")
		return true;
	else if (WeaponName == "weapon_molotov")
		return true;

	return false;
}

void CBaseEntity::invalidateBoneCache()
{
	*( int* ) ( ( uintptr_t ) this + 0x268C + 0x20 ) = 0;
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2690) = (Globals::g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}


CUtlVectorSimple* CBaseEntity::animOverlayStruct()
{
	return (CUtlVectorSimple*)((unsigned long)this + 0x2980);
}

CAnimationLayer& CBaseEntity::animOverlayStructByIndex(int index)
{
	return (*(CAnimationLayer**)((DWORD)this + 0x2980))[index];
}

CAnimationLayer* CBaseEntity::animOverlay(int i)
{
	if (i >= 0 && i < 15)
	{
		CUtlVectorSimple *m_AnimOverlay = animOverlayStruct();
		return (CAnimationLayer*)m_AnimOverlay->Retrieve(i, sizeof(CAnimationLayer));
	}
}

int CBaseEntity::numAnimOverlays()
{
	CUtlVectorSimple *m_AnimOverlay = animOverlayStruct();
	return m_AnimOverlay->count;
}

int CBaseEntity::sequenceActivity(int sequence)
{
	auto hdr = g_Interfaces->modelInfo->getStudiomodel( this->GetModel( ) );

	if ( !hdr )
		return -1;

	static auto sequenceActivity = reinterpret_cast< int( __fastcall* )( void*, studiohdr_t*, int ) >( g_Memory->findPattern( "client_panorama.dll", "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );

	return sequenceActivity( this, hdr, sequence );
}


bool CBaseCombatWeapon::isReloading()
{
	static auto inReload = *(uint32_t*)(g_Memory->findPattern("client_panorama.dll", "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

bool CBaseCombatWeapon::canFire()
{
	return !isReloading() && ammo() > 0;
}

bool CBaseCombatWeapon::canFirePostpone( ) {
	float rdyTime = postponeFireReadyTime( );

	if ( rdyTime > 0 && rdyTime < g_Interfaces->globalVars->curtime )
		return true;

	return false;
}