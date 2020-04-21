#pragma once
#include "../utilities/interfaceUtilities.hpp"
#include "vector.hpp"

#define SIGNONSTATE_NONE		0	// no state yet, about to connect
#define SIGNONSTATE_CHALLENGE	1	// client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED	2	// client is connected to server, netchans ready
#define SIGNONSTATE_NEW			3	// just got serverinfo and string tables
#define SIGNONSTATE_PRESPAWN	4	// received signon buffers
#define SIGNONSTATE_SPAWN		5	// ready to receive entity packets
#define SIGNONSTATE_FULL		6	// we are fully connected, first non-delta packet received
#define SIGNONSTATE_CHANGELEVEL	7	// server is changing level, please wait

#define FLOW_OUTGOING 0		
#define FLOW_INCOMING 1
#define MAX_FLOWS 2	// in & out

typedef struct InputContextHandle_t__* InputContextHandle_t;
struct client_textmessage_t;
struct model_t;
class SurfInfo;
class IMaterial;
class CSentence;
class CAudioSource;
class AudioState_t;
class ISpatialQuery;
class IMaterialSystem;
class CPhysCollide;
class IAchievementMgr;
class INetChannelInfo;
class ISPSharedMemory;
class CGamestatsData;
class KeyValues;
class CSteamAPIContext;
struct Frustum_t;
class Color;

struct playerinfo_t
{
	int64_t __pad0;

	union
	{
		int64_t xuid;
		struct
		{
			int xuidlow;
			int xuidhigh;
		};
	};

	char name[128];
	int userId;
	char guid[33];
	unsigned int friendsid;
	char friendsname[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
};

class INetChannelInfo
{
public:
	enum
	{
		GENERIC = 0,    // must be first and is default group
		LOCALPLAYER,    // bytes for local player entity update
		OTHERPLAYERS,   // bytes for other players update
		ENTITIES,       // all other entity bytes
		SOUNDS,         // game sounds
		EVENTS,         // event messages
		USERMESSAGES,   // user messages
		ENTMESSAGES,    // entity messages
		VOICE,          // voice data
		STRINGTABLE,    // a stringtable update
		MOVE,           // client move cmds
		STRINGCMD,      // string command
		SIGNON,         // various signondata
		TOTAL,          // must be last and is not a real group
	};

	virtual const char* GetName(void) const = 0;	            // get channel name
	virtual const char* GetAddress(void) const = 0;            // get channel IP address as string
	virtual float       GetTime(void) const = 0;	            // current net time
	virtual float       GetTimeConnected(void) const = 0;	    // get connection time in seconds
	virtual int         GetBufferSize(void) const = 0;	        // netchannel packet history size
	virtual int         GetDataRate(void) const = 0;            // send data rate in byte/sec

	virtual bool        IsLoopback(void) const = 0;             // true if loopback channel
	virtual bool        IsTimingOut(void) const = 0;            // true if timing out
	virtual bool        IsPlayback(void) const = 0;             // true if demo playback

	virtual float       GetLatency(int flow) const = 0;         // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0;      // average packet latency in seconds
	virtual float       GetAvgLoss(int flow) const = 0;         // avg packet loss[0..1]
	virtual float       GetAvgChoke(int flow) const = 0;        // avg packet choke[0..1]
	virtual float       GetAvgData(int flow) const = 0;         // data flow in bytes/sec
	virtual float       GetAvgPackets(int flow) const = 0;      // avg packets/sec
	virtual int         GetTotalData(int flow) const = 0;       // total flow in/out in bytes
	virtual int         GetSequenceNr(int flow) const = 0;      // last send seq number
	virtual bool        IsValidPacket(int flow, int frame_number) const = 0;                // true if packet was not lost/dropped/chocked/flushed
	virtual float       GetPacketTime(int flow, int frame_number) const = 0;                // time when packet was send
	virtual int         GetPacketBytes(int flow, int frame_number, int group) const = 0;    // group size of this packet
	virtual bool        GetStreamProgress(int flow, int* received, int* total) const = 0;   // TCP progress if transmitting
	virtual float       GetTimeSinceLastReceived(void) const = 0;// get time since last recieved packet in seconds
	virtual	float       GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};

class NetChannel
{
public:

	int sendDataGram( )
	{
		using originalFn = VFUNC_SIGNATURE( int, void* );

		return ( ( originalFn ) g_InterfaceUtils->getVfuncTable( this )[ 46 ] )( this, NULL );
 	}

	uint8_t pad_0x0000[0x17];
	bool m_bShouldDelete;
	int m_nOutSequenceNr;
	int m_nInSequenceNr;
	int m_nOutSequenceNrAck;
	int m_nOutReliableState;
	int m_nInReliableState;
	int m_nChokedPackets;
};

class ClockDriftManager
{
public:
	float ClockOffsets[17];   //0x0000
	uint32_t CurrentClockOffsets; //0x0044
	uint32_t ServerTick;     //0x0048
	uint32_t ClientTick;     //0x004C
}; //Size: 0x0050


class CClientState
{
public:
	void FullUpdate()
	{
		*reinterpret_cast<int*>(uintptr_t(this) + 0x174) = -1;
	};

	char pad_0000[156];             //0x0000
	NetChannel* NetChannel;          //0x009C
	uint32_t ChallengeNumber;        //0x00A0
	char pad_00A4[100];             //0x00A4
	uint32_t SignonState;        //0x0108
	char pad_010C[8];               //0x010C
	float flNextCmdTime;          //0x0114
	uint32_t ServerCount;        //0x0118
	uint32_t CurrentSequence;    //0x011C
	char pad_0120[8];               //0x0120
	ClockDriftManager ClockDriftManager; //0x0128
	uint32_t DeltaTick;          //0x0178
	bool Paused;                 //0x017C
	char pad_017D[3];               //0x017D
	uint32_t ViewEntity;         //0x0180
	uint32_t PlayerSlot;         //0x0184
	char szLevelName[260];        //0x0188
	char szLevelNameShort[40];    //0x028C
	char szGroupName[40];         //0x02B4
	char pad_02DC[52];              //0x02DC
	uint32_t MaxClients;         //0x0310
	char pad_0314[18820];           //0x0314
	float LastServerTickTime;   //0x4C98
	bool InSimulation;              //0x4C9C
	char pad_4C9D[3];               //0x4C9D
	uint32_t OldTickCount;          //0x4CA0
	float TickRemainder;          //0x4CA4
	float FrameTime;              //0x4CA8
	uint32_t LastOutgoingCommand;   //0x4CAC
	uint32_t ChokedCommands;        //0x4CB0
	uint32_t LastCommandAck;      //0x4CB4
	uint32_t CommandAck;           //0x4CB8
	uint32_t SoundSequence;      //0x4CBC
	char pad_4CC0[80];              //0x4CC0
	Vector3 ViewAngles;              //0x4D10
}; //Size: 0x4D1C


class IVEngineClient
{
public:
	virtual int                   GetIntersectingSurfaces(const model_t* model, const Vector3& vCenter, const float radius, const bool bOnlyVisibleSurfaces, SurfInfo* pInfos, const int nMaxInfos) = 0;
	virtual Vector3                GetLightForPoint(const Vector3& pos, bool bClamp) = 0;
	virtual IMaterial* TraceLineMaterialAndLighting(const Vector3& start, const Vector3& end, Vector3& diffuseLightColor, Vector3& baseColor) = 0;
	virtual const char* ParseFile(const char* data, char* token, int maxlen) = 0;
	virtual bool                  CopyFile(const char* source, const char* destination) = 0;
	virtual void                  GetScreenSize(int& width, int& height) = 0;
	virtual void                  ServerCmd(const char* szCmdString, bool bReliable = true) = 0;
	virtual void                  ClientCmd(const char* szCmdString) = 0;
	virtual bool                  GetPlayerInfo(int ent_num, playerinfo_t* pinfo) = 0;
	virtual int                   GetPlayerForUserID(int userID) = 0;
	virtual client_textmessage_t* TextMessageGet(const char* pName) = 0; // 10
	virtual bool                  Con_IsVisible(void) = 0;
	virtual int                   GetLocalPlayer(void) = 0;
	virtual const model_t* LoadModel(const char* pName, bool bProp = false) = 0;
	virtual float                 GetLastTimeStamp(void) = 0;
	virtual CSentence* GetSentence(CAudioSource* pAudioSource) = 0; // 15
	virtual float                 GetSentenceLength(CAudioSource* pAudioSource) = 0;
	virtual bool                  IsStreaming(CAudioSource* pAudioSource) const = 0;
	virtual void                  GetViewAngles(Vector3& va) = 0;
	virtual void                  SetViewAngles(Vector3& va) = 0;
	virtual int                   GetMaxClients(void) = 0; // 20
	virtual const char* Key_LookupBinding(const char* pBinding) = 0;
	virtual const char* Key_BindingForKey(int& code) = 0;
	virtual void                  Key_SetBinding(int, char const*) = 0;
	virtual void                  StartKeyTrapMode(void) = 0;
	virtual bool                  CheckDoneKeyTrapping(int& code) = 0;
	virtual bool                  IsInGame(void) = 0;
	virtual bool                  IsConnected(void) = 0;
	virtual bool                  IsDrawingLoadingImage(void) = 0;
	virtual void                  HideLoadingPlaque(void) = 0;
	virtual void                  Con_NPrintf(int pos, const char* fmt, ...) = 0; // 30
	virtual void                  Con_NXPrintf(const struct con_nprint_s* info, const char* fmt, ...) = 0;
	virtual int                   IsBoxVisible(const Vector3& mins, const Vector3& maxs) = 0;
	virtual int                   IsBoxInViewCluster(const Vector3& mins, const Vector3& maxs) = 0;
	virtual bool                  CullBox(const Vector3& mins, const Vector3& maxs) = 0;
	virtual void                  Sound_ExtraUpdate(void) = 0;
	virtual const char* GetGameDirectory(void) = 0;
	//virtual const VMatrix& WorldToScreenMatrix() = 0;
	//virtual const VMatrix& WorldToViewMatrix() = 0;
	virtual int                   GameLumpVersion(int lumpId) const = 0;
	virtual int                   GameLumpSize(int lumpId) const = 0; // 40
	virtual bool                  LoadGameLump(int lumpId, void* pBuffer, int size) = 0;
	virtual int                   LevelLeafCount() const = 0;
	virtual ISpatialQuery* GetBSPTreeQuery() = 0;
	virtual void                  LinearToGamma(float* linear, float* gamma) = 0;
	virtual float                 LightStyleValue(int style) = 0; // 45
	virtual void                  ComputeDynamicLighting(const Vector3& pt, const Vector3* pNormal, Vector3& color) = 0;
	virtual void                  GetAmbientLightColor(Vector3& color) = 0;
	virtual int                   GetDXSupportLevel() = 0;
	virtual bool                  SupportsHDR() = 0;
	virtual void                  Mat_Stub(IMaterialSystem* pMatSys) = 0; // 50
	virtual void                  GetChapterName(char* pchBuff, int iMaxLength) = 0;
	virtual char const* GetLevelName(void) = 0;
	virtual char const* GetLevelNameShort(void) = 0;
	virtual char const* GetMapGroupName(void) = 0;
	virtual struct IVoiceTweak_s* GetVoiceTweakAPI(void) = 0;
	virtual void                  SetVoiceCasterID(unsigned int someint) = 0; // 56
	virtual void                  EngineStats_BeginFrame(void) = 0;
	virtual void                  EngineStats_EndFrame(void) = 0;
	virtual void                  FireEvents() = 0;
	virtual int                   GetLeavesArea(unsigned short* pLeaves, int nLeaves) = 0;
	virtual bool                  DoesBoxTouchAreaFrustum(const Vector3& mins, const Vector3& maxs, int iArea) = 0; // 60
	virtual int                   GetFrustumList(Frustum_t** pList, int listMax) = 0;
	virtual bool                  ShouldUseAreaFrustum(int i) = 0;
	virtual void                  SetAudioState(const AudioState_t& state) = 0;
	virtual int                   SentenceGroupPick(int groupIndex, char* name, int nameBufLen) = 0;
	virtual int                   SentenceGroupPickSequential(int groupIndex, char* name, int nameBufLen, int sentenceIndex, int reset) = 0;
	virtual int                   SentenceIndexFromName(const char* pSentenceName) = 0;
	virtual const char* SentenceNameFromIndex(int sentenceIndex) = 0;
	virtual int                   SentenceGroupIndexFromName(const char* pGroupName) = 0;
	virtual const char* SentenceGroupNameFromIndex(int groupIndex) = 0;
	virtual float                 SentenceLength(int sentenceIndex) = 0;
	virtual void                  ComputeLighting(const Vector3& pt, const Vector3* pNormal, bool bClamp, Vector3& color, Vector3* pBoxColors = NULL) = 0;
	virtual void                  ActivateOccluder(int nOccluderIndex, bool bActive) = 0;
	virtual bool                  IsOccluded(const Vector3& vecAbsMins, const Vector3& vecAbsMaxs) = 0; // 74
	virtual int                   GetOcclusionViewId(void) = 0;
	virtual void* SaveAllocMemory(size_t num, size_t size) = 0;
	virtual void                  SaveFreeMemory(void* pSaveMem) = 0;
	virtual INetChannelInfo* GetNetChannelInfo(void) = 0;
	//virtual void                  DebugDrawPhysCollide(const CPhysCollide* pCollide, IMaterial* pMaterial, const matrix3x4_t& transform, const uint8_t* color) = 0; //79
	virtual void                  CheckPoint(const char* pName) = 0; // 80
	virtual void                  DrawPortals() = 0;
	virtual bool                  IsPlayingDemo(void) = 0;
	virtual bool                  IsRecordingDemo(void) = 0;
	virtual bool                  IsPlayingTimeDemo(void) = 0;
	virtual int                   GetDemoRecordingTick(void) = 0;
	virtual int                   GetDemoPlaybackTick(void) = 0;
	virtual int                   GetDemoPlaybackStartTick(void) = 0;
	virtual float                 GetDemoPlaybackTimeScale(void) = 0;
	virtual int                   GetDemoPlaybackTotalTicks(void) = 0;
	virtual bool                  IsPaused(void) = 0; // 90
	virtual float                 GetTimescale(void) const = 0;
	virtual bool                  IsTakingScreenshot(void) = 0;
	//virtual void				  Unknown_0(void) = 0;
	virtual bool                  IsHLTV(void) = 0;
	virtual bool                  IsLevelMainMenuBackground(void) = 0;
	virtual void                  GetMainMenuBackgroundName(char* dest, int destlen) = 0;
	virtual void                  SetOcclusionParameters(const int /*OcclusionParams_t*/& params) = 0; // 96
	virtual void                  GetUILanguage(char* dest, int destlen) = 0;
	virtual int                   IsSkyboxVisibleFromPoint(const Vector3& vecPoint) = 0;
	virtual const char* GetMapEntitiesString() = 0;
	virtual bool                  IsInEditMode(void) = 0; // 100
	virtual float                 GetScreenAspectRatio(int viewportWidth, int viewportHeight) = 0;
	virtual bool                  REMOVED_SteamRefreshLogin(const char* password, bool isSecure) = 0;
	virtual bool                  REMOVED_SteamProcessCall(bool& finished) = 0;
	virtual unsigned int          GetEngineBuildNumber() = 0; // engines build
	virtual const char* GetProductVersionString() = 0; // mods version number (steam.inf)
	virtual void                  GrabPreColorCorrectedFrame(int x, int y, int width, int height) = 0;
	virtual bool                  IsHammerRunning() const = 0;
	virtual void                  ExecuteClientCmd(const char* szCmdString) = 0; //108
	virtual bool                  MapHasHDRLighting(void) = 0;
	virtual bool                  MapHasLightMapAlphaData(void) = 0;
	virtual int                   GetAppID() = 0;
	virtual Vector3                GetLightForPointFast(const Vector3& pos, bool bClamp) = 0;
	virtual void                  ClientCmd_Unrestricted(char  const*, int, bool) = 0;
	virtual void                  ClientCmd_Unrestricted(const char* szCmdString) = 0; // 114
	virtual void                  SetRestrictServerCommands(bool bRestrict) = 0;
	virtual void                  SetRestrictClientCommands(bool bRestrict) = 0;
	virtual void                  SetOverlayBindProxy(int iOverlayID, void* pBindProxy) = 0;
	virtual bool                  CopyFrameBufferToMaterial(const char* pMaterialName) = 0;
	virtual void                  ReadConfiguration(const int iController, const bool readDefault) = 0;
	virtual void                  SetAchievementMgr(IAchievementMgr* pAchievementMgr) = 0;
	virtual IAchievementMgr* GetAchievementMgr() = 0;
	virtual bool                  MapLoadFailed(void) = 0;
	virtual void                  SetMapLoadFailed(bool bState) = 0;
	virtual bool                  IsLowViolence() = 0;
	virtual const char* GetMostRecentSaveGame(void) = 0;
	virtual void                  SetMostRecentSaveGame(const char* lpszFilename) = 0;
	virtual void                  StartXboxExitingProcess() = 0;
	virtual bool                  IsSaveInProgress() = 0;
	virtual bool                  IsAutoSaveDangerousInProgress(void) = 0;
	virtual unsigned int          OnStorageDeviceAttached(int iController) = 0;
	virtual void                  OnStorageDeviceDetached(int iController) = 0;
	virtual char* const           GetSaveDirName(void) = 0;
	virtual void                  WriteScreenshot(const char* pFilename) = 0;
	virtual void                  ResetDemoInterpolation(void) = 0;
	virtual int                   GetActiveSplitScreenPlayerSlot() = 0;
	virtual int                   SetActiveSplitScreenPlayerSlot(int slot) = 0;
	virtual bool                  SetLocalPlayerIsResolvable(char const* pchContext, int nLine, bool bResolvable) = 0;
	virtual bool                  IsLocalPlayerResolvable() = 0;
	virtual int                   GetSplitScreenPlayer(int nSlot) = 0;
	virtual bool                  IsSplitScreenActive() = 0;
	virtual bool                  IsValidSplitScreenSlot(int nSlot) = 0;
	virtual int                   FirstValidSplitScreenSlot() = 0; // -1 == invalid
	virtual int                   NextValidSplitScreenSlot(int nPreviousSlot) = 0; // -1 == invalid
	virtual ISPSharedMemory* GetSinglePlayerSharedMemorySpace(const char* szName, int ent_num = (1 << 11)) = 0;
	virtual void                  ComputeLightingCube(const Vector3& pt, bool bClamp, Vector3* pBoxColors) = 0;
	//virtual void                  RegisterDemoCustomDataCallback(const char* szCallbackSaveID, pfnDemoCustomDataCallback pCallback) = 0;
	//virtual void                  RecordDemoCustomData(pfnDemoCustomDataCallback pCallback, const void* pData, size_t iDataLength) = 0;
	virtual void                  SetPitchScale(float flPitchScale) = 0;
	virtual float                 GetPitchScale(void) = 0;
	virtual bool                  LoadFilmmaker() = 0;
	virtual void                  UnloadFilmmaker() = 0;
	virtual void                  SetLeafFlag(int nLeafIndex, int nFlagBits) = 0;
	virtual void                  RecalculateBSPLeafFlags(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomNew(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomChanged(void) = 0;
	virtual bool                  DSPGetCurrentDASRoomSkyAbove(void) = 0;
	virtual float                 DSPGetCurrentDASRoomSkyPercent(void) = 0;
	virtual void                  SetMixGroupOfCurrentMixer(const char* szgroupname, const char* szparam, float val, int setMixerType) = 0;
	virtual int                   GetMixLayerIndex(const char* szmixlayername) = 0;
	virtual void                  SetMixLayerLevel(int index, float level) = 0;
	virtual int                   GetMixGroupIndex(char  const* groupname) = 0;
	virtual void                  SetMixLayerTriggerFactor(int i1, int i2, float fl) = 0;
	virtual void                  SetMixLayerTriggerFactor(char  const* char1, char  const* char2, float fl) = 0;
	virtual bool                  IsCreatingReslist() = 0;
	virtual bool                  IsCreatingXboxReslist() = 0;
	virtual void                  SetTimescale(float flTimescale) = 0;
	virtual void                  SetGamestatsData(CGamestatsData* pGamestatsData) = 0;
	virtual CGamestatsData* GetGamestatsData() = 0;
	virtual void                  GetMouseDelta(int& dx, int& dy, bool b) = 0; // unknown
	virtual   const char* Key_LookupBindingEx(const char* pBinding, int iUserId = -1, int iStartCount = 0, int iAllowJoystick = -1) = 0;
	virtual int                   Key_CodeForBinding(char  const*, int, int, int) = 0; // 186
	virtual void                  UpdateDAndELights(void) = 0;
	virtual int                   GetBugSubmissionCount() const = 0;
	virtual void                  ClearBugSubmissionCount() = 0;
	virtual bool                  DoesLevelContainWater() const = 0;
	virtual float                 GetServerSimulationFrameTime() const = 0;
	virtual void                  SolidMoved(class IClientEntity* pSolidEnt, class ICollideable* pSolidCollide, const Vector3* pPrevAbsOrigin, bool accurateBboxTriggerChecks) = 0;
	virtual void                  TriggerMoved(class IClientEntity* pTriggerEnt, bool accurateBboxTriggerChecks) = 0;
	virtual void                  ComputeLeavesConnected(const Vector3& vecOrigin, int nCount, const int* pLeafIndices, bool* pIsConnected) = 0;
	virtual bool                  IsInCommentaryMode(void) = 0;
	virtual void                  SetBlurFade(float amount) = 0;
	virtual bool                  IsTransitioningToLoad() = 0;
	virtual void                  SearchPathsChangedAfterInstall() = 0;
	virtual void                  ConfigureSystemLevel(int nCPULevel, int nGPULevel) = 0;
	virtual void                  SetConnectionPassword(char const* pchCurrentPW) = 0;
	virtual CSteamAPIContext* GetSteamAPIContext() = 0;
	virtual void                  SubmitStatRecord(char const* szMapName, unsigned int uiBlobVersion, unsigned int uiBlobSize, const void* pvBlob) = 0;
	virtual void                  ServerCmdKeyValues(KeyValues* pKeyValues) = 0; // 203
	virtual void                  SpherePaintSurface(const model_t* model, const Vector3& location, unsigned char chr, float fl1, float fl2) = 0;
	virtual bool                  HasPaintmap(void) = 0;
	virtual void                  EnablePaintmapRender() = 0;
	//virtual void                TracePaintSurface( const model_t *model, const Vector3& position, float radius, CUtlVector3<Color>& surfColors ) = 0;
	virtual void                  SphereTracePaintSurface(const model_t* model, const Vector3& position, const Vector3& vec2, float radius, /*CUtlVector3<unsigned char, CUtlMemory<unsigned char, int>>*/ int& utilVecShit) = 0;
	virtual void                  RemoveAllPaint() = 0;
	virtual void                  PaintAllSurfaces(unsigned char uchr) = 0;
	virtual void                  RemovePaint(const model_t* model) = 0;
	virtual bool                  IsActiveApp() = 0;
	virtual bool                  IsClientLocalToActiveServer() = 0;
	virtual void                  TickProgressBar() = 0;
	virtual InputContextHandle_t  GetInputContext(int /*EngineInputContextId_t*/ id) = 0;
	virtual void                  GetStartupImage(char* filename, int size) = 0;
	virtual bool                  IsUsingLocalNetworkBackdoor(void) = 0;
	virtual void                  SaveGame(const char*, bool, char*, int, char*, int) = 0;
	virtual void                  GetGenericMemoryStats(/* GenericMemoryStat_t */ void**) = 0;
	virtual bool                  GameHasShutdownAndFlushedMemory(void) = 0;
	virtual int                   GetLastAcknowledgedCommand(void) = 0;
	virtual void                  FinishContainerWrites(int i) = 0;
	virtual void                  FinishAsyncSave(void) = 0;
	virtual int                   GetServerTick(void) = 0;
	virtual const char* GetModDirectory(void) = 0;
	virtual bool                  AudioLanguageChanged(void) = 0;
	virtual bool                  IsAutoSaveInProgress(void) = 0;
	virtual void                  StartLoadingScreenForCommand(const char* command) = 0;
	virtual void                  StartLoadingScreenForKeyValues(KeyValues* values) = 0;
	virtual void                  SOSSetOpvarFloat(const char*, float) = 0;
	virtual void                  SOSGetOpvarFloat(const char*, float&) = 0;
	virtual bool                  IsSubscribedMap(const char*, bool) = 0;
	virtual bool                  IsFeaturedMap(const char*, bool) = 0;
	virtual void                  GetDemoPlaybackParameters(void) = 0;
	virtual int                   GetClientVersion(void) = 0;
	virtual bool                  IsDemoSkipping(void) = 0;
	virtual void                  SetDemoImportantEventData(const KeyValues* values) = 0;
	virtual void                  ClearEvents(void) = 0;
	virtual int                   GetSafeZoneXMin(void) = 0;
	virtual bool                  IsVoiceRecording(void) = 0;
	virtual void                  ForceVoiceRecordOn(void) = 0;
	virtual bool                  IsReplay(void) = 0;
};

class CGameEngine
{
	
public:

	virtual int                   unk31() = 0;
	virtual void*                unk32() = 0;
	virtual void*            unk3() = 0;
	//virtual ISpatialQuery* GetBSPTreeQuery() = 0;
	virtual const char*           unk34() = 0;
	virtual bool                  unk33() = 0;
	virtual void                  unk35() = 0;
	virtual void                  unk36() = 0;
	virtual void                  unk37(const char *szCmdString) = 0;
	virtual bool                  getPlayerInfo2(int ent_num, playerinfo_t *pinfo) = 0;

	int getLocalPlayer()
	{
		using OriginalFn = VFUNC_SIGNATURE(int);

		return ((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[12])(this);
	}

	int getPlayerIndex(int userId)
	{
		using OriginalFn = VFUNC_SIGNATURE(int, int);

		return ((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[9])(this, userId);
	}

	void getPlayerInfo(int index, playerinfo_t* playerinfo)
	{
		using OriginalFn = VFUNC_SIGNATURE(void, int, playerinfo_t*);

		((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[8])(this, index, playerinfo);
	}

	void getScreenSize(int &width, int &height)
	{
		using OriginalFn = VFUNC_SIGNATURE(void, int&, int&);

		((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[5])(this, width, height);
	}

	void executeCmd(const char* cmd)
	{
		using OriginalFn = VFUNC_SIGNATURE(void, const char*);

		((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[108])(this, cmd);
	}

	void setViewAngles(Vector3 &angle)
	{
		using OriginalFn = VFUNC_SIGNATURE(void, Vector3&);

		((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[19])(this, angle);
	}

	void getViewAngles(Vector3 &angle)
	{
		using OriginalFn = VFUNC_SIGNATURE(void, Vector3&);

		((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[18])(this, angle);
	}

	Vector3 getViewAngles()
	{
		Vector3 temp;

		using OriginalFn = VFUNC_SIGNATURE(Vector3, Vector3);
		((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[18])(this, temp);

		return temp;
	}

	bool inGame()
	{
		using OriginalFn = VFUNC_SIGNATURE(bool);
		return ((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[26])(this);
	}

	bool connected()
	{
		using OriginalFn = VFUNC_SIGNATURE(bool);
		return ((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[27])(this);
	}

	const char* levelName()
	{
		using OriginalFn = VFUNC_SIGNATURE(const char*);
		return ((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[52])(this);
	}

	INetChannelInfo* netchannelInfo()
	{
		using OriginalFn = VFUNC_SIGNATURE(INetChannelInfo*);

		return ((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[78])(this);
	}

	NetChannel* getNetChannel()
	{
		using OriginalFn = VFUNC_SIGNATURE(NetChannel*);

		return ((OriginalFn)g_InterfaceUtils->getVfuncTable(this)[78])(this);
	}
};