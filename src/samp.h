/*

PROJECT:		mod_sa
LICENSE:		See LICENSE in the top level directory
COPYRIGHT:		Copyright we_sux, FYP

mod_sa is available from http://code.google.com/p/m0d-s0beit-sa/

mod_sa is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

mod_sa is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with mod_sa.  If not, see <http://www.gnu.org/licenses/>.

*/
#define SAMP_DIALOG_INFO_OFFSET						0x21A0B8
#define SAMP_CHAT_INFO_OFFSET						0x21A0E4
#define SAMP_CHAT_INPUT_INFO_OFFSET					0x21A0E8
#define SAMP_KILL_INFO_OFFSET						0x21A0EC
#define SAMP_INFO_OFFSET							0x21A0F8
#define SAMP_MISC_INFO								0x21A10C
#define SAMP_SCOREBOARD_INFO						0x21A0B4
#define SAMP_COLOR_OFFSET							0x216378
#define SAMP_UNKNOWN_OFFSET							0x21A130
#define SAMP_DRAWING_DISTANCE						0xD3DCC

#define SAMP_DRAWTEXTLABELS							0x71471
#define SAMP_ONFOOTSENDRATE							0xEC0A8
#define SAMP_INCARSENDRATE							0xEC0AC
#define SAMP_AIMSENDRATE							0xEC0B0

#define SAMP_FUNC_ADDRECALL							0x65930
#define SAMP_FUNC_ADDTOCHATWND						0x64010
#define SAMP_FUNC_SPAWN								0x3AD0
#define SAMP_FUNC_REQUEST_SPAWN						0x3A20
#define SAMP_FUNC_REQUEST_CLASS						0x56A0
#define SAMP_FUNC_DEATH								0x55E0
#define SAMP_FUNC_SETSPECIALACTION					0x30C0
#define SAMP_FUNC_SAY								0x57F0
#define SAMP_FUNC_SENDCMD							0x65C60
#define SAMP_FUNC_ADDCLIENTCMD						0x65AD0
#define SAMP_FUNC_NAMECHANGE						0xB290
#define SAMP_FUNC_RESTARTGAME						0xA060
#define SAMP_FUNC_ENCRYPT_PORT						0x1BC20
#define SAMP_FUNC_TOGGLECURSOR						0x9BD30
#define SAMP_FUNC_CURSORUNLOCKACTORCAM				0x9BC10
#define SAMP_FUNC_UPDATESCOREBOARDDATA				0x8A10
#define SAMP_FUNC_CNETGAMEDESTRUCTOR				0x9380
#define SAMP_FUNC_SENDINTERIOR						0x5740
#define SAMP_FUNC_DIALOG_SHOW						0x6B9C0
#define SAMP_FUNC_GAMETEXT							0x9C2C0

#define SAMP_HOOKENTER_WNDPROC						0x5DB40
#define SAMP_HOOKENTER_HANDLE_RPC					0x3743D
#define SAMP_HOOKENTER_HANDLE_RPC2					0x373C9
#define SAMP_HOOKENTER_CNETGAME_DESTR				0xB2893
#define SAMP_HOOKENTER_CNETGAME_DESTR2				0xB3A32
#define SAMP_HOOKEXIT_WNDPROC						0x5DB45
#define SAMP_HOOKEXIT_HANDLE_RPC					0x37443
#define SAMP_HOOKEXIT_HANDLE_RPC2					0x37451
#define SAMP_HOOKPOS_ServerMessage					0x645DA
#define SAMP_HOOKPOS_ClientMessage					0xEF6E
#define SAMP_HOOKEXIT_DIALOGRESPONSE				0x6C046
#define SAMP_FUNC_DIALOG_GETTEXT					0x81030
#define SAMP_FUNC_DIALOG_GETITEM					0x84850
#define SAMP_HOOKENTER_DIALOGRESPONSE				0x6C040
#define HOOK_EXIT_SERVERMESSAGE_HOOK				0x645F1
#define HOOK_EXIT_CLIENTMESSAGE_HOOK				0xEFC7
#define HOOK_CALL_STREAMEDOUTINFO					0x9C340

#define SAMP_PATCH_DISABLE_DIALOG					0xCBB0
#define SAMP_PATCH_DISABLE_ONFOOT_DATA				0x4D10
#define SAMP_PATCH_DISABLE_INCAR_DATA				0x6E30
#define SAMP_PATCH_DISABLE_CHAT						0xEEA0
#define SAMP_PATCH_DISABLE_NAMETAGS					0x70D40
#define SAMP_PATCH_DISABLE_NAMETAGS_HP				0x6FC30
#define SAMP_PATCH_SKIPSENDINTERIOR					0x7668
#define SAMP_PATCH_SCOREBOARDTOGGLEON				0x6AA10
#define SAMP_PATCH_SCOREBOARDTOGGLEONKEYLOCK		0x6AD30
#define SAMP_PATCH_CHATINPUTADJUST_Y				0x63FE6
#define SAMP_PATCH_CHATINPUTADJUST_X				0x65855
#define SAMP_PATCH_NOCARCOLORRESETTING				0xB0DE0

enum DialogStyle
{
	DIALOG_STYLE_MSGBOX,
	DIALOG_STYLE_INPUT,
	DIALOG_STYLE_LIST,
	DIALOG_STYLE_PASSWORD,
};

enum Limits
{
	SAMP_MAX_ACTORS = 1000,
	SAMP_MAX_PLAYERS = 1004,
	SAMP_MAX_VEHICLES = 2000,
	SAMP_MAX_PICKUPS = 4096,
	SAMP_MAX_OBJECTS = 1000,
	SAMP_MAX_GANGZONES = 1024,
	SAMP_MAX_3DTEXTS = 2048,
	SAMP_MAX_TEXTDRAWS = 2048,
	SAMP_MAX_PLAYERTEXTDRAWS = 256,
	SAMP_MAX_CLIENTCMDS = 144,
	SAMP_MAX_MENUS = 128,
	SAMP_MAX_PLAYER_NAME = 24,
	SAMP_ALLOWED_PLAYER_NAME_LENGTH = 20,
};

enum ChatMessageType
{
	CHAT_TYPE_NONE = 0,
	CHAT_TYPE_CHAT = 2,
	CHAT_TYPE_INFO = 4,
	CHAT_TYPE_DEBUG = 8
};

enum ChatDisplayMode
{
	CHAT_WINDOW_MODE_OFF = 0,
	CHAT_WINDOW_MODE_LIGHT = 1,
	CHAT_WINDOW_MODE_FULL = 2
};

enum Gamestate
{
	GAMESTATE_WAIT_CONNECT = 9,
	GAMESTATE_CONNECTING = 13,
	GAMESTATE_AWAIT_JOIN = 15,
	GAMESTATE_CONNECTED = 14,
	GAMESTATE_RESTARTING = 18
};

enum PlayerState
{
	PLAYER_STATE_NONE = 0,
	PLAYER_STATE_ONFOOT = 17,
	PLAYER_STATE_PASSENGER,
	PLAYER_STATE_DRIVER,
	PLAYER_STATE_WASTED = 32,
	PLAYER_STATE_SPAWNED
};

enum MarkersMode
{
	PLAYER_MARKERS_MODE_OFF,
	PLAYER_MARKERS_MODE_GLOBAL,
	PLAYER_MARKERS_MODE_STREAMED,
};

enum SpecialAction
{
	SPECIAL_ACTION_NONE,
	SPECIAL_ACTION_DUCK,
	SPECIAL_ACTION_USEJETPACK,
	SPECIAL_ACTION_ENTER_VEHICLE,
	SPECIAL_ACTION_EXIT_VEHICLE,
	SPECIAL_ACTION_DANCE1,
	SPECIAL_ACTION_DANCE2,
	SPECIAL_ACTION_DANCE3,
	SPECIAL_ACTION_DANCE4,
	SPECIAL_ACTION_HANDSUP,
	SPECIAL_ACTION_USECELLPHONE,
	SPECIAL_ACTION_SITTING,
	SPECIAL_ACTION_STOPUSECELLPHONE,
	SPECIAL_ACTION_DRINK_BEER = 20,
	SPECIAL_ACTION_SMOKE_CIGGY,
	SPECIAL_ACTION_DRINK_WINE,
	SPECIAL_ACTION_DRINK_SPRUNK,
	SPECIAL_ACTION_CUFFED,
	SPECIAL_ACTION_CARRY,
};


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// SAMP STRUCTURES ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)
struct stSAMPPools
{
	struct stActorPool		*pActor;
	struct stObjectPool		*pObject;
	struct stGangzonePool	*pGangzone;
	struct stTextLabelPool	*pText3D;
	struct stTextdrawPool	*pTextdraw;
	void					*pPlayerLabels;
	struct stPlayerPool		*pPlayer;
	struct stVehiclePool	*pVehicle;
	struct stPickupPool		*pPickup;
};

struct stSAMP
{
	void					*pUnk0[2];
	uint8_t					byteSpace[24]; // +8
	char					szIP[257]; // +32
	char					szHostname[259]; // +289
	uint8_t					byteUnk1; // +548
	uint32_t				ulPort; // +549
	uint32_t				ulMapIcons[100]; // +553
	int						iLanMode; // +953
	int						iGameState; // +957
	uint32_t				ulConnectTick;
	struct stServerPresets	*pSettings;
	void					*pRakClientInterface;
	struct stSAMPPools		*pPools;
};

struct stServerPresets
{
	uint8_t byteCJWalk;
	uint8_t byteUnk0[4];
	float	fWorldBoundaries[4];
	uint8_t byteUnk1;
	float	fGravity;
	uint8_t byteDisableInteriorEnterExits;
	uint32_t ulVehicleFriendlyFire;
	uint8_t byteUnk2[4];
	int		iClassesAvailable;
	float	fNameTagsDistance;
	uint8_t byteUnk3;
	uint8_t byteWorldTime_Hour;
	uint8_t byteWorldTime_Minute;
	uint8_t byteWeather;
	uint8_t byteNoNametagsBehindWalls;
	uint8_t bytePlayerMarkersMode;
	uint8_t byteUnk4[3];
	float	fGlobalChatRadiusLimit;
	uint8_t byteShowNameTags;
};

struct stDialog
{
	int  unk1; // +0x0
	int  iTextOffsetX; // +0x4
	int  iTextOffsetY; // +0x8
	int  unk2[2]; // +0x10
	int  iButtonOffsetX; // +0x14
	int  iButtonOffsetY; // +0x18
	void *pDialogBox; // +0x1C
	void *pListBox; // +0x20
	void *pEditBox; // +0x24
	int  iDialogShowed; // +0x28
	int  iDialogType; // +0x2C
	int  iDialogID; // +0x30
	int  unk3[3]; // +0x34
	char szDialogHeader[65]; // +0x40
	int  iSendResponse; // +0x81
};

struct stTextDrawTransmit
{
	union
	{
		BYTE byteFlags;
		struct
		{
			BYTE byteBox : 1;
			BYTE byteLeft : 1;
			BYTE byteRight : 1;
			BYTE byteCenter : 1;
			BYTE byteProportional : 1;
			BYTE bytePadding : 3;
		};
	};
	float fLetterWidth;
	float fLetterHeight;
	DWORD dwLetterColor;
	float fBoxWidth;
	float fBoxHeight;
	DWORD dwBoxColor;
	BYTE byteShadow;
	BYTE byteOutline;
	DWORD dwBackgroundColor;
	BYTE byteStyle;
	BYTE byteUNK;
	float fX;
	float fY;
	uint16_t sModel;
	float fRot[3];
	float fZoom;
	WORD sColor[2];
};

struct stSAMPDialog
{
	int		iIsActive;
	BYTE	bDialogStyle;
	WORD	wDialogID;
	BYTE	bTitleLength;
	char	szTitle[257];
	BYTE	bButton1Len;
	char	szButton1[257];
	BYTE	bButton2Len;
	char	szButton2[257];
	char	szInfo[8192];
};

struct stTextdraw
{
	char		szText[800 + 1];
	char		szString[1600 + 2];
	float		fLetterWidth;
	float		fLetterHeight;
	DWORD		dwLetterColor;
	uint8_t		byte_unk;	// always = 01 (?)
	BYTE		byteCenter;
	BYTE		byteBox;
	float		fBoxSizeX;
	float		fBoxSizeY;
	DWORD		dwBoxColor;
	BYTE		byteProportional;
	DWORD		dwShadowColor;
	BYTE		byteShadowSize;
	BYTE		byteOutline;
	BYTE		byteLeft;
	BYTE		byteRight;
	int			iStyle;		// font style/texture/model
	float		fX;
	float		fY;
	byte		unk[8];
	DWORD		dword99B;	// -1 by default
	DWORD		dword99F;	// -1 by default
	DWORD		index;		// -1 if bad
	BYTE		byte9A7;	// = 1; 0 by default
	uint16_t	sModel;
	float		fRot[3];
	float		fZoom;
	WORD		sColor[2];
	BYTE		f9BE;
	BYTE		byte9BF;
	BYTE		byte9C0;
	DWORD		dword9C1;
	DWORD		dword9C5;
	DWORD		dword9C9;
	DWORD		dword9CD;
	BYTE		byte9D1;
	DWORD		dword9D2;
};

struct stTextdrawPool
{
	int					iIsListed[SAMP_MAX_TEXTDRAWS];
	int					iPlayerTextDraw[SAMP_MAX_PLAYERTEXTDRAWS];
	struct stTextdraw	*textdraw[SAMP_MAX_TEXTDRAWS];
	struct stTextdraw	*playerTextdraw[SAMP_MAX_PLAYERTEXTDRAWS];
};

struct stPickup
{
	int		iModelID;
	int		iType;
	float	fPosition[3];
};

struct stPickupPool
{
	int				iPickupsCount;
	uint32_t		ul_GTA_PickupID[SAMP_MAX_PICKUPS];
	int				iPickupID[SAMP_MAX_PICKUPS];
	int				iTimePickup[SAMP_MAX_PICKUPS];
	uint8_t			unk[SAMP_MAX_PICKUPS * 3];
	struct stPickup pickup[SAMP_MAX_PICKUPS];
};

struct stPlayerPool
{
	uint32_t				ulUnk0;
	uint16_t				sLocalPlayerID;
	void					*pVTBL_txtHandler;
	union
	{
		char	szLocalPlayerName[16];
		char	*pszLocalPlayerName;
	};
	int						iLocalPlayerNameLen;
	int						iLocalPlayerNameAllocated;
	struct stLocalPlayer	*pLocalPlayer;
	int						iLocalPlayerPing;
	int						iLocalPlayerScore;
	struct stRemotePlayer	*pRemotePlayer[SAMP_MAX_PLAYERS];
	int						iIsListed[SAMP_MAX_PLAYERS];
	uint32_t				ulUnk1[SAMP_MAX_PLAYERS];
};

struct stSAMPKeys
{
	uint8_t keys_primaryFire : 1;
	uint8_t keys_horn__crouch : 1;
	uint8_t keys_secondaryFire__shoot : 1;
	uint8_t keys_accel__zoomOut : 1;
	uint8_t keys_enterExitCar : 1;
	uint8_t keys_decel__jump : 1;			// on foot: jump or zoom in
	uint8_t keys_circleRight : 1;
	uint8_t keys_aim : 1;					// hydra auto aim or on foot aim
	uint8_t keys_circleLeft : 1;
	uint8_t keys_landingGear__lookback : 1;
	uint8_t keys_unknown__walkSlow : 1;
	uint8_t keys_specialCtrlUp : 1;
	uint8_t keys_specialCtrlDown : 1;
	uint8_t keys_specialCtrlLeft : 1;
	uint8_t keys_specialCtrlRight : 1;
	uint8_t keys__unused : 1;
};

struct stOnFootData
{
	uint16_t	sLeftRightKeys;
	uint16_t	sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float		fPosition[3];
	float		fQuaternion[4];
	uint8_t		byteHealth;
	uint8_t		byteArmor;
	uint8_t		byteCurrentWeapon;
	uint8_t		byteSpecialAction;
	float		fMoveSpeed[3];
	float		fSurfingOffsets[3];
	uint16_t	sSurfingVehicleID;
	short		sCurrentAnimationID;
	short		sAnimFlags;
};

struct stInCarData
{
	uint16_t	sVehicleID;
	uint16_t	sLeftRightKeys;
	uint16_t	sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float		fQuaternion[4];
	float		fPosition[3];
	float		fMoveSpeed[3];
	float		fVehicleHealth;
	uint8_t		bytePlayerHealth;
	uint8_t		byteArmor;
	uint8_t		byteCurrentWeapon;
	uint8_t		byteSiren;
	uint8_t		byteLandingGearState;
	uint16_t	sTrailerID;
	union
	{
		uint16_t	HydraThrustAngle[2];	//nearly same value
		float		fTrainSpeed;
	};
};

struct stAimData
{
	BYTE	byteCamMode;
	float	vecAimf1[3];
	float	vecAimPos[3];
	float	fAimZ;
	BYTE	byteCamExtZoom : 6;		// 0-63 normalized
	BYTE	byteWeaponState : 2;	// see eWeaponState
	BYTE	bUnk;
};

struct stTrailerData
{
	uint16_t	sTrailerID;
	float		fPosition[3];
	//float		fRoll[3];
	//float		fDirection[3];
	float		fQuaternion[4]; // not tested
	float		fSpeed[3];
	float		fUnk[2];
	uint32_t	pad;
};

struct stPassengerData
{
	uint16_t	sVehicleID;
	uint8_t		byteSeatID;
	uint8_t		byteCurrentWeapon;
	uint8_t		byteHealth;
	uint8_t		byteArmor;
	uint16_t	sLeftRightKeys;
	uint16_t	sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float	fPosition[3];
};

struct stDamageData
{
	uint16_t	sVehicleID_lastDamageProcessed;
	int			iBumperDamage;
	int			iDoorDamage;
	uint8_t		byteLightDamage;
	uint8_t		byteWheelDamage;
};

struct stSurfData
{
	int			iIsSurfing;
	float		fSurfPosition[3];
	int			iUnk0;
	uint16_t	sSurfingVehicleID;
	uint32_t	ulSurfTick;
	struct stSAMPVehicle *pSurfingVehicle;
	int			iUnk1;
	int			iSurfMode;	//0 = not surfing, 1 = moving (unstable surf), 2 = fixed on vehicle
};

struct stUnoccupiedData
{
	int16_t sVehicleID;
	uint8_t byteSeatID;
	float	fRoll[3];
	float	fDirection[3];
	float	fPosition[3];
	float	fMoveSpeed[3];
	float	fTurnSpeed[3];
	float	fHealth;
};

struct stBulletData
{
	uint8_t		byteType;
	uint16_t	sTargetID;
	float		fOrigin[3];
	float		fTarget[3];
	float		fCenter[3];
};

struct stSpectatorData
{
	uint16_t	sLeftRightKeys;
	uint16_t	sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float	fPosition[3];
};

struct stStatsData
{
	int iMoney;
	int iAmmo;	// ?
};

struct stHeadSync
{
	float	fHeadSync[3];
	int		iHeadSyncUpdateTick;
	int		iHeadSyncLookTick;
};

struct stLocalPlayer
{
	struct stSAMPPed		*pSAMP_Actor;
	uint16_t				sCurrentAnimID;
	uint16_t				sAnimFlags;
	uint32_t				ulUnk0;
	int						iIsActive;
	int						iIsWasted;
	uint16_t				sCurrentVehicleID;
	uint16_t				sLastVehicleID;
	struct stOnFootData		onFootData;
	struct stPassengerData	passengerData;
	struct stTrailerData	trailerData;
	struct stInCarData		inCarData;
	struct stAimData		aimData;
	uint8_t					byteTeamID;
	int						iSpawnSkin;
	uint8_t					byteUnk1;
	float					fSpawnPos[3];
	float					fSpawnRot;
	int						iSpawnWeapon[3];
	int						iSpawnAmmo[3];
	int						iIsActorAlive;
	int						iSpawnClassLoaded;
	uint32_t				ulSpawnSelectionTick;
	uint32_t				ulSpawnSelectionStart;
	int						iIsSpectating;
	uint8_t					byteTeamID2;
	uint16_t				usUnk2;
	uint32_t				ulSendTick;
	uint32_t				ulSpectateTick;
	uint32_t				ulAimTick;
	uint32_t				ulStatsUpdateTick;
	uint32_t				ulWeapUpdateTick;
	uint16_t				sAimingAtPid;
	uint16_t				usUnk3;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteWeaponInventory[13];
	int						iWeaponAmmo[13];
	int						iPassengerDriveBy;
	uint8_t					byteCurrentInterior;
	int						iIsInRCVehicle;
	uint16_t				sTargetObjectID;
	uint16_t				sTargetVehicleID;
	uint16_t				sTargetPlayerID;
	struct stHeadSync		headSyncData;
	uint32_t				ulHeadSyncTick;
	BYTE					byteSpace3[260];
	struct stSurfData		surfData;
	int						iClassSelectionOnDeath;
	int						iSpawnClassID;
	int						iRequestToSpawn;
	int						iIsInSpawnScreen;
	uint32_t				ulUnk4;
	uint8_t					byteSpectateMode;		// 3 = vehicle, 4 = player, side = 14, fixed = 15
	uint8_t					byteSpectateType;		// 0 = none, 1 = player, 2 = vehicle
	int						iSpectateID;
	int						iInitiatedSpectating;
	struct stDamageData		vehicleDamageData;
};

struct stRemotePlayerData
{
	struct stSAMPPed		*pSAMP_Actor;
	struct stSAMPVehicle	*pSAMP_Vehicle;
	uint8_t					byteTeamID;
	uint8_t					bytePlayerState;
	uint8_t					byteSeatID;
	uint32_t				ulUnk3;
	int						iPassengerDriveBy;
	void					*pUnk0;
	uint8_t					byteUnk1[60];
	float					fSomething[3];
	float					fVehicleRoll[4];
	uint32_t				ulUnk2[3];
	float					fOnFootPos[3];
	float					fOnFootMoveSpeed[3];
	float					fVehiclePosition[3];
	float					fVehicleMoveSpeed[3];
	uint16_t				sPlayerID;
	uint16_t				sVehicleID;
	uint32_t				ulUnk5;
	uint8_t					byteUnk6[2];
	short					sShowNameTag;
	int						iHasJetPack;
	uint8_t					byteSpecialAction;
	uint32_t				ulUnk4[3];
	struct stOnFootData		onFootData;
	struct stInCarData		inCarData;
	struct stTrailerData	trailerData;
	struct stPassengerData	passengerData;
	struct stAimData		aimData;
	float					fActorArmor;
	float					fActorHealth;
	uint32_t				ulUnk10;
	uint8_t					byteUnk9;
	uint32_t				dwTick;
	uint32_t				dwLastStreamedInTick;	// is 0 when currently streamed in
	uint32_t				ulUnk7;
	int						iAFKState;
	struct stHeadSync		headSyncData;
	int						iGlobalMarkerLoaded;
	int						iGlobalMarkerLocation[3];
	uint32_t				ulGlobalMarker_GTAID;
};

struct stRemotePlayer
{
	stRemotePlayerData	*pPlayerData;
	int					iIsNPC;
	void				*pVTBL_txtHandler;
	union
	{
		char			szPlayerName[16];
		char			*pszPlayerName;
	};
	int					iNameLen;
	int					iNameAllocated;
	int					iScore;
	int					iPing;
};

template <typename T>
struct stSAMPEntity
{
	void		*pVTBL;
	uint8_t		byteUnk0[60]; // game CEntity object maybe. always empty.
	T			*pGTAEntity;
	uint32_t	ulGTAEntityHandle;
};

struct stSAMPPed : public stSAMPEntity < actor_info >
{
	int					usingCellPhone;
	uint8_t				byteUnk0[600];
	struct actor_info	*pGTA_Ped;
	uint8_t				byteUnk1[22];
	uint8_t				byteKeysId;
	uint16_t			ulGTA_UrinateParticle_ID;
	int					DrinkingOrSmoking;
	int					object_in_hand;
	int					drunkLevel;
	uint8_t				byteUnk2[5];
	int					isDancing;
	int					danceStyle;
	int					danceMove;
	uint8_t				byteUnk3[20];
	int					isUrinating;
};

struct stVehiclePool
{
	int						iVehicleCount;
	void					*pUnk0;
	uint8_t					byteSpace1[0x112C];
	struct stSAMPVehicle	*pSAMP_Vehicle[SAMP_MAX_VEHICLES];
	int						iIsListed[SAMP_MAX_VEHICLES];
	struct vehicle_info		*pGTA_Vehicle[SAMP_MAX_VEHICLES];
	uint8_t					byteSpace2[SAMP_MAX_VEHICLES * 6];
	uint32_t				ulShit[SAMP_MAX_VEHICLES];
	int						iIsListed2[SAMP_MAX_VEHICLES];
	uint32_t				byteSpace3[SAMP_MAX_VEHICLES * 2];
	float					fSpawnPos[SAMP_MAX_VEHICLES][3];
	int						iInitiated;
};

struct stSAMPVehicle : public stSAMPEntity < vehicle_info >
{
	uint32_t			bUnk0;
	struct vehicle_info *pGTA_Vehicle;
	uint8_t				byteUnk1[8];
	int					bIsMotorOn;
	int					iIsLightsOn;
	int					iIsLocked;
	uint8_t				byteIsObjective;
	int					iObjectiveBlipCreated;
	uint8_t				byteUnk2[20];
	uint8_t				byteColor[2];
	int					iColorSync;
	int					iColor_something;
};

struct stObject : public stSAMPEntity < object_info >
{
	uint8_t				byteUnk0[2];
	uint32_t			ulUnk1;
	int					iModel;
	uint8_t				byteUnk2;
	float				fDrawDistance;
	float				fUnk;
	float				fPos[3];
	// ...
};

struct stObjectPool
{
	int				iObjectCount;
	int				iIsListed[SAMP_MAX_OBJECTS];
	struct stObject *object[SAMP_MAX_OBJECTS];
};

struct stGangzone
{
	float	fPosition[4];
	DWORD	dwColor;
	DWORD	dwAltColor;
};

struct stGangzonePool
{
	struct stGangzone	*pGangzone[SAMP_MAX_GANGZONES];
	int					iIsListed[SAMP_MAX_GANGZONES];
};

struct stTextLabel
{
	char		*pText;
	DWORD		color;
	float		fPosition[3];
	float		fMaxViewDistance;
	uint8_t		byteShowBehindWalls;
	uint16_t	sAttachedToPlayerID;
	uint16_t	sAttachedToVehicleID;
};

struct stTextLabelPool
{
	struct stTextLabel	textLabel[SAMP_MAX_3DTEXTS];
	int					iIsListed[SAMP_MAX_3DTEXTS];
};

struct stChatEntry
{
	uint32_t	SystemTime;
	char		szPrefix[28];
	char		szText[144];
	uint8_t		unknown[64];
	int			iType;			// 2 - text + prefix, 4 - text (server msg), 8 - text (debug)
	D3DCOLOR	clTextColor;
	D3DCOLOR	clPrefixColor;	// or textOnly colour
};

struct stFontRenderer
{
	ID3DXFont	*m_pChatFont;
	ID3DXFont	*m_pLittleFont;
	ID3DXFont	*m_pChatShadowFont;
	ID3DXFont	*m_pLittleShadowFont;
	ID3DXFont	*m_pCarNumberFont;
	ID3DXSprite *m_pTempSprite;
	int			field_18;
	char		*m_pszTextBuffer;
};

struct stChatInfo
{
	int					pagesize;
	char				*pLastMsgText;
	int					iChatWindowMode;
	uint8_t				bTimestamps;
	uint32_t			ulUnk0;
	char				logFilePathChatLog[MAX_PATH + 1];
	void				*pChatbox_unknown[3];	// probably classes that handle fonts/sprites
	D3DCOLOR			clTextColor;
	D3DCOLOR			clInfoColor;
	D3DCOLOR			clDebugColor;
	DWORD				dwChatboxOffset;
	struct stChatEntry	chatEntry[100];
	stFontRenderer		*m_pFontRenderer;
	ID3DXSprite			*m_pChatTextSprite;
	ID3DXSprite			*m_pSprite;
	IDirect3DDevice9	*m_pD3DDevice;
	uint32_t			ulUnk1[2];
	IDirect3DTexture9	*m_pTexture;
	uint32_t			ulUnk2[7];
	int					m_iRedraw;
	int					m_nPrevScrollBarPosition;
	int					m_iDistanceAfterTimestamp;
	uint32_t			ulUnk3;
};

struct stWeaponUpdate
{
	BYTE	slot;
	BYTE	weaponid;
	WORD	ammo;
};

struct stInputBox
{
	void	*pUnknown;
	uint8_t	bIsChatboxOpen;
	uint8_t	bIsMouseInChatbox;
	uint8_t	bMouseClick_related;
	uint8_t	unk;
	DWORD	dwPosChatInput[2];
	uint8_t	unk2[263];
	int		iCursorPosition;
	uint8_t	unk3;
	int		iMarkedText_startPos; // Highlighted text between this and iCursorPosition
	uint8_t	unk4[20];
	int		iMouseLeftButton;
};

typedef void(__cdecl *CMDPROC) (PCHAR);
struct stInputInfo
{
	void				*pD3DDevice;
	void				*pDXUTDialog;
	stInputBox			*pDXUTEditBox;
	CMDPROC				pCMDs[SAMP_MAX_CLIENTCMDS];
	char				szCMDNames[SAMP_MAX_CLIENTCMDS][33];
	int					iCMDCount;
	int					iInputEnabled;
	char				szInputBuffer[129];
	char				szRecallBufffer[10][129];
	char				szCurrentBuffer[129];
	int					iCurrentRecall;
	int					iTotalRecalls;
	CMDPROC				pszDefaultCMD;
};

struct stKillEntry
{
	char		szKiller[25];
	char		szVictim[25];
	D3DCOLOR	clKillerColor;
	D3DCOLOR	clVictimColor;
	uint8_t		byteType;
};

struct stKillInfo
{
	int					iEnabled;
	stKillEntry			killEntry[5];
	int					iXOffset[2];
	int					iYSpacing;
	// not tested
	ID3DXFont		    *pD3DFont;
	ID3DXFont			*pWeaponFont;
	ID3DXSprite			*pSprite;
	IDirect3DDevice9	*pD3DDevice;
	// ...
};

struct stChatPlayer
{
	int		iCreated;
	char	probablyTheText[256];
	DWORD	dwTickCreated;
	DWORD	dwLiveLength;
	DWORD	dwColor;
	float	fDrawDistance;
	DWORD	dwUnknown;
};

struct stSAMPMisc
{
	void		*pUnk_Ptr[3]; // 0
	float		fCheckpointPos[3]; // 12
	float		fCheckpointSize[3]; // 24
	uint32_t	iCheckpointEnabled; // 36
	uint16_t	iCheckpoint_someting[2]; // 40
	float		vecRaceCheckpointPos[3]; // 44
	float		vecRaceCheckpointNextPos[3]; // 56
	uint8_t		unk1[5]; // 68
	uint32_t	iRaceCheckpointEnabled;
	uint32_t	unk2[2];
	BYTE		byteCursor_someting;
	uint32_t	unk3;
};

struct stScoreboardInfo
{
	int					iIsEnabled;
	int					iPlayersCount;
	float				fTextOffset[2];
	float				fScalar;
	float				fSize[2];
	float				fUnk0[5];
	IDirect3DDevice9	*pDirectDevice;
	class _CDXUTDialog	*pDialog;
	class _CDXUTListBox *pList;
	int					iOffset;		// ?
	int					iIsSorted;		// ?
};

struct stActorPool
{
	int					iLastActorID;
	stSAMPEntity<void>	*pActor[SAMP_MAX_ACTORS]; // ?
	int					iIsListed[SAMP_MAX_ACTORS];
	struct actor_info	*pGTAPed[SAMP_MAX_ACTORS];
	uint32_t			ulUnk0[SAMP_MAX_ACTORS];
	uint32_t			ulUnk1[SAMP_MAX_ACTORS];
};

struct stChatBubbleInfo
{
	struct stChatPlayer	chatBubble[SAMP_MAX_PLAYERS];
};

struct stStreamedOutPlayerInfo
{
	int		iPlayerID[SAMP_MAX_PLAYERS];
	float	fPlayerPos[SAMP_MAX_PLAYERS][3];
};
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// SUPPORT VARIABLES //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
struct stTranslateGTASAMP_vehiclePool
{
	// the index is the position in the vehicle pool
	//
	// translation data
	int iSAMPID[SAMP_MAX_VEHICLES];
};

struct stTranslateGTASAMP_pedPool
{
	// the index is the position in the ped pool
	//
	// translation data
	int iSAMPID[SAMP_MAX_PLAYERS];
};

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// FUNCTIONS //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
int												getPickup(float cpos[3], float range, int modelid);
void											vehicleUnflip(struct vehicle_info *info);
bool											gotoPos(float pos[3], float range);
void											update_translateGTASAMP_vehiclePool(void);
void											update_translateGTASAMP_pedPool(void);

bool											findstrinstr(char *text, char *find);

int												isBadPtr_SAMP_iVehicleID(int iVehicleID);
int												isBadPtr_SAMP_iPlayerID(int iPlayerID);

void											getSamp();
uint32_t										getSampAddress();

struct stSAMP									*stGetSampInfo(void);
struct stSAMPMisc								*stGetSampMisc(void);
class OrigRakClientInterface					*stGetRakClient(void);
struct stChatInfo								*stGetSampChatInfo(void);
struct stInputInfo								*stGetInputInfo(void);
struct stKillInfo								*stGetKillInfo(void);
D3DCOLOR										samp_color_get(int id, DWORD trans = 0xFF000000);
struct stDialog									*stGetDialogInfo(void);
extern struct stDialog							*g_Dialog;

void											sampMainCheat();

int												getNthPlayerID(int n);
int												getPlayerCount(void);
int												getVehicleCount(void);

int												setLocalPlayerName(const char *name);
int												getPlayerPos(int iPlayerID, float fPos[3]);
const char										*getPlayerName(int iPlayerID);
int												getPlayerState(int iPlayerID);
int												getPlayerVehicleGTAScriptingID(int iPlayerID);
int												getPlayerSAMPVehicleID(int iPlayerID);
void											spectatePlayer(int iPlayerID);
void											spectateHandle();

struct actor_info								*getGTAPedFromSAMPPlayerID(int iPlayerID);
struct vehicle_info								*getGTAVehicleFromSAMPVehicleID(int iPlayerID);
int												getSAMPPlayerIDFromGTAPed(struct actor_info *info);
int												getSAMPVehicleIDFromGTAVehicle(struct vehicle_info *pVehicle);
uint32_t										getPedGTAScriptingIDFromPlayerID(int iPlayerID);
uint32_t										getVehicleGTAScriptingIDFromVehicleID(int iVehicleID);

bool											get_isModCommandsActive();
void											init_samp_chat_cmds();
void											addClientCommand(char *text, int function);
void											addToChatWindow(char *text, D3DCOLOR textColor, int playerID = -1);
void											addMessageToChatWindow(const char *text, ...);
void											addMessageToChatWindowSS(const char *text, ...);
void											restartGame();
void											addSayToChatWindow(char *msg);
void											say(char *text, ...);
void											showSampDialog(int send, int dialogID, int typedialog, char *caption, char *text, char *button1, char *button2);
void											showGameText(char *text, int time, int textsize);
void											playerSpawn(void);
void											disconnect(int reason);
void											setPassword(char *password);
void											sendSetInterior(uint8_t interiorID);
void											setSpecialAction(uint8_t byteSpecialAction);
void											sendSCMEvent(int iEvent, int iVehicleID, int iParam1, int iParam2);
void											toggleSAMPCursor(int iToggle);
void											installSAMPHooks();
void											setSAMPCustomSendRates(int iOnFoot, int iInCar, int iAim, int iHeadSync);
int												sampPatchDisableDialog(int iEnabled);
int												sampPatchDisableOnFoot(int iEnabled);
int												sampPatchDisableInCar(int iEnabled);
int												sampPatchDisableNameTags(int iEnabled);
int												sampPatchDisableInteriorUpdate(int iEnabled);
int												sampPatchDisableScoreboardToggleOn(int iEnabled);
int												sampPatchDisableChatInputAdjust(int iEnabled);
void											sendDeath(void);
void											changeServer(const char *pszIp, unsigned ulPort, const char *pszPassword);

// global pointer externals
extern int										iIsSAMPSupported;
extern int										g_renderSAMP_initSAMPstructs;
extern struct stSAMP							*g_SAMP;
extern struct stSAMPMisc						*g_SAMPMisc;
extern struct stPlayerPool						*g_Players;
extern struct stVehiclePool						*g_Vehicles;
extern struct stChatInfo						*g_Chat;
extern struct stInputInfo						*g_Input;
extern struct stKillInfo						*g_DeathList;
extern struct stStreamedOutPlayerInfo			g_stStreamedOutInfo;

// global support variables externals
extern struct stTranslateGTASAMP_vehiclePool	translateGTASAMP_vehiclePool;
extern struct stTranslateGTASAMP_pedPool		translateGTASAMP_pedPool;

extern int										g_iSpectateEnabled;
extern int										g_iCursorEnabled;
extern int										g_iNumPlayersMuted;
extern bool										g_bPlayerMuted[SAMP_MAX_PLAYERS];
extern int										iNetModeNormalOnfootSendRate;
extern int										iNetModeNormalIncarSendRate;
extern int										iNetModeFiringSendRate;
extern int										iNetModeSendMultiplier;
extern int										iLagCompMode;