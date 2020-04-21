#pragma once
#include "../../utilities/includes.h"
#include "..\..\sdk\baseClient.hpp"
#include "..\..\sdk\baseEntity.hpp"
#include <deque>

class Resolver 
{
public:
	bool UseFreestandAngle[65];
	float FreestandAngle[65];

	float flOldGoalFeetYaw[65];
	float flGoalFeetYaw[65];
	Vector3 vOldEyeAng[65];
	bool bUseFreestandAngle[65];
	float flFreestandAngle[65];
	float flLastFreestandAngle[65];
	float flGoalFeetYawB[65];
	float flLbyB[65];
	std::string ResolverMode[65];
	float pitchHit[65];

	void HandleBackUpResolve(CBaseEntity* pEnt);

	void preresolver(CBasePlayerAnimState* AnimState, CBaseEntity* pEntity, bool bShot);

	void PostResolver(CBasePlayerAnimState* AnimState, CBaseEntity* pEntity, bool bShot);

	void HandleHits(CBaseEntity* pEnt);

	void manageFakeMatrix();

	void OnCreateMove();
	void Yaw(CBaseEntity* ent);
	void FrameStage(clientFrameStage_t stage);
	void AnimationFix(CBaseEntity* pEnt);
};
extern Resolver g_Resolver;