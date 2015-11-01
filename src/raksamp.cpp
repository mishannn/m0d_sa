#include "main.h"

OrigRakClient *g_RakClient = NULL;

OrigRakClient::OrigRakClient(void *pOrigRakClientInterface)
{
	pOrigRakClient = (OrigRakClientInterface *)pOrigRakClientInterface;
}

bool OrigRakClient::RPC(int rpcId, RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (!pOrigRakClient)
		return false;

	return pOrigRakClient->RPC(&rpcId, bitStream, priority, reliability, orderingChannel, shiftTimestamp);
}

bool OrigRakClient::Send(RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (!pOrigRakClient)
		return false;

	return pOrigRakClient->Send(bitStream, priority, reliability, orderingChannel);
}

void OrigRakClient::SendDeath(uint16_t killerId, uint8_t reason)
{
	RakNet::BitStream bsDeath;

	bsDeath.Write(reason);
	bsDeath.Write(killerId);
	g_RakClient->RPC(RPC_Death, &bsDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendFakeUnoccupiedSyncData(uint16_t VehicleID, float fPos[3])
{
	stUnoccupiedData sync;
	ZeroMemory(&sync, sizeof(sync));
	RakNet::BitStream bs;
	sync.fPosition[0] = fPos[0];
	sync.fPosition[1] = fPos[1];
	sync.fPosition[2] = fPos[2];
	sync.sVehicleID = VehicleID;
	sync.byteSeatID = 1;
	bs.Write((BYTE)ID_UNOCCUPIED_SYNC);
	bs.Write((PCHAR)&sync, sizeof(stUnoccupiedData));
	g_RakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendWeaponUpdateSync(BYTE weaponslot, BYTE weaponid, WORD ammo)
{
	stWeaponUpdate picSync;
	ZeroMemory(&picSync, sizeof(stWeaponUpdate));
	RakNet::BitStream bsWeaponSync;
	picSync.slot = weaponslot;
	picSync.weaponid = weaponid;
	picSync.ammo = ammo;
	bsWeaponSync.Write((BYTE)ID_WEAPONS_UPDATE);
	bsWeaponSync.Write((PCHAR)&picSync, sizeof(stWeaponUpdate));
	g_RakClient->Send(&bsWeaponSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendFakeDriverSyncData(uint16_t VehicleID, float fPos[3], float HealthCar, float Speed[3])
{
	stInCarData picSync;
	ZeroMemory(&picSync, sizeof(picSync));
	RakNet::BitStream bsVehicleSync;
	picSync.sVehicleID = VehicleID;
	picSync.fPosition[0] = fPos[0];
	picSync.fPosition[1] = fPos[1];
	picSync.fPosition[2] = fPos[2];
	picSync.fVehicleHealth = HealthCar;
	picSync.fMoveSpeed[0] = Speed[0];
	picSync.fMoveSpeed[1] = Speed[1];
	picSync.fMoveSpeed[2] = Speed[2];
	picSync.bytePlayerHealth = actor_info_get(-1, 0x01)->hitpoints;
	picSync.byteArmor = actor_info_get(-1, 0x01)->armor;
	bsVehicleSync.Write((BYTE)ID_VEHICLE_SYNC);
	bsVehicleSync.Write((PCHAR)&picSync, sizeof(stInCarData));
	g_RakClient->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendFakeTrailerToCar(uint16_t TrailerID)
{
	stInCarData picSync;
	ZeroMemory(&picSync, sizeof(picSync));
	RakNet::BitStream bsVehicleSync;
	picSync.sVehicleID = g_Players->pLocalPlayer->inCarData.sVehicleID;
	picSync.fPosition[0] = g_Players->pLocalPlayer->inCarData.fPosition[0];
	picSync.fPosition[1] = g_Players->pLocalPlayer->inCarData.fPosition[1];
	picSync.fPosition[2] = g_Players->pLocalPlayer->inCarData.fPosition[2];
	picSync.fVehicleHealth = g_Players->pLocalPlayer->inCarData.fVehicleHealth;
	picSync.fMoveSpeed[0] = g_Players->pLocalPlayer->inCarData.fMoveSpeed[0];
	picSync.fMoveSpeed[1] = g_Players->pLocalPlayer->inCarData.fMoveSpeed[1];
	picSync.fMoveSpeed[2] = g_Players->pLocalPlayer->inCarData.fMoveSpeed[2];
	picSync.bytePlayerHealth = g_Players->pLocalPlayer->inCarData.bytePlayerHealth;
	picSync.byteArmor = g_Players->pLocalPlayer->inCarData.byteArmor;
	picSync.sTrailerID = TrailerID;
	bsVehicleSync.Write((BYTE)ID_VEHICLE_SYNC);
	bsVehicleSync.Write((PCHAR)&picSync, sizeof(stInCarData));
	g_RakClient->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendFakePassSyncData(uint16_t VehicleID, float fPos[3], uint8_t Seat)
{
	stPassengerData picSync;
	ZeroMemory(&picSync, sizeof(picSync));
	RakNet::BitStream bsVehicleSync;
	picSync.sVehicleID = VehicleID;
	picSync.fPosition[0] = fPos[0];
	picSync.fPosition[1] = fPos[1];
	picSync.fPosition[2] = fPos[2];
	picSync.byteSeatID = Seat;
	picSync.byteHealth = g_Players->pLocalPlayer->passengerData.byteHealth;
	picSync.byteArmor = g_Players->pLocalPlayer->passengerData.byteArmor;
	bsVehicleSync.Write((BYTE)ID_PASSENGER_SYNC);
	bsVehicleSync.Write((PCHAR)&picSync, sizeof(stPassengerData));
	g_RakClient->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendFakeBulletSyncData(float aimHit[3], float bodyPos[3], float hitPos[3], BYTE hitType, uint16_t id)
{
	stBulletData bSync;
	ZeroMemory(&bSync, sizeof(bSync));
	RakNet::BitStream bsBulletSync;
	bSync.fOrigin[0] = aimHit[0];
	bSync.fOrigin[1] = aimHit[1];
	bSync.fOrigin[2] = aimHit[2];
	bSync.fCenter[0] = bodyPos[0];
	bSync.fCenter[1] = bodyPos[1];
	bSync.fCenter[2] = bodyPos[2];
	bSync.fTarget[0] = hitPos[0];
	bSync.fTarget[1] = hitPos[1];
	bSync.fTarget[2] = hitPos[2];
	bSync.byteType = hitType;
	bSync.sTargetID = id;
	bsBulletSync.Write((BYTE)ID_BULLET_SYNC);
	bsBulletSync.Write((PCHAR)&bSync, sizeof(stBulletData));
	g_RakClient->Send(&bsBulletSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendFakeOnfootSyncData(float fPos[3], float Health, float speed[3])
{
	stOnFootData picSync;
	ZeroMemory(&picSync, sizeof(picSync));
	RakNet::BitStream bsOnfootSync;
	picSync.byteHealth = Health;
	picSync.fPosition[0] = fPos[0];
	picSync.fPosition[1] = fPos[1];
	picSync.fPosition[2] = fPos[2];
	picSync.fMoveSpeed[0] = speed[0];
	picSync.fMoveSpeed[1] = speed[1];
	picSync.fMoveSpeed[2] = speed[2];
	picSync.sCurrentAnimationID = 1224;
	bsOnfootSync.Write((BYTE)ID_PLAYER_SYNC);
	bsOnfootSync.Write((PCHAR)&picSync, sizeof(stOnFootData));
	g_RakClient->Send(&bsOnfootSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendFakeSurfing(uint16_t VehicleID)
{
	stOnFootData picSync;
	ZeroMemory(&picSync, sizeof(picSync));
	RakNet::BitStream bsOnfootSync;
	picSync.byteHealth = g_Players->pLocalPlayer->onFootData.byteHealth;
	picSync.byteCurrentWeapon = g_Players->pLocalPlayer->onFootData.byteCurrentWeapon;
	picSync.fPosition[0] = g_Players->pLocalPlayer->onFootData.fPosition[0];
	picSync.fPosition[1] = g_Players->pLocalPlayer->onFootData.fPosition[1];
	picSync.fPosition[2] = g_Players->pLocalPlayer->onFootData.fPosition[2];
	picSync.fMoveSpeed[0] = 0;
	picSync.fMoveSpeed[1] = 0;
	picSync.fMoveSpeed[2] = 0;
	picSync.fSurfingOffsets[0] = 1.0;
	picSync.fSurfingOffsets[1] = 1.0;
	picSync.fSurfingOffsets[2] = 1.0;
	picSync.sSurfingVehicleID = VehicleID;
	bsOnfootSync.Write((BYTE)ID_PLAYER_SYNC);
	bsOnfootSync.Write((PCHAR)&picSync, sizeof(stOnFootData));
	g_RakClient->Send(&bsOnfootSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendFakeAimSyncData(float vecAimPos[3], float vecAimf1[3], float fAimZ, BYTE byteCamMode, BYTE byteCamExtZoom, BYTE byteWeaponState, BYTE bUnk)
{
	stAimData aimSync;
	ZeroMemory(&aimSync, sizeof(aimSync));
	RakNet::BitStream bsAimSync;
	aimSync.fAimZ = fAimZ;
	aimSync.vecAimf1[0] = vecAimf1[0];
	aimSync.vecAimf1[1] = vecAimf1[1];
	aimSync.vecAimf1[2] = vecAimf1[2];
	aimSync.vecAimPos[0] = vecAimPos[0];
	aimSync.vecAimPos[1] = vecAimPos[1];
	aimSync.vecAimPos[2] = vecAimPos[2];
	aimSync.byteCamMode = byteCamMode;
	aimSync.byteCamExtZoom = byteCamExtZoom;
	aimSync.byteWeaponState = byteWeaponState;
	aimSync.bUnk = bUnk;
	bsAimSync.Write((BYTE)ID_AIM_SYNC);
	bsAimSync.Write((PCHAR)&aimSync, sizeof(stAimData));
	g_RakClient->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void OrigRakClient::SendEnterPlayerVehicle(uint16_t VehicleID, BOOL seat)
{
	RakNet::BitStream VehicleEnter;
	VehicleEnter.Write(VehicleID);
	VehicleEnter.Write(seat);
	g_RakClient->RPC(RPC_EnterVehicle, &VehicleEnter, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendFakeCarDeath(uint16_t VehicleID)
{
	RakNet::BitStream VehicleBoom;
	VehicleBoom.Write(VehicleID);
	g_RakClient->RPC(RPC_VehicleDestroyed, &VehicleBoom, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendFakeMenuSelectRow(BYTE RowID)
{
	RakNet::BitStream menu;
	menu.Write(RowID);
	g_RakClient->RPC(RPC_VehicleDestroyed, &menu, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendClickPlayer(BYTE playerID, int source)
{
	RakNet::BitStream ClickPlayer;
	ClickPlayer.Write(playerID);
	ClickPlayer.Write(source);
	g_RakClient->RPC(RPC_ClickPlayer, &ClickPlayer, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendDialogResponse(WORD wDialogID, BYTE bButtonID, WORD wListBoxItem, char *szInputResp)
{
	BYTE respLen = (BYTE)strlen(szInputResp);
	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(bButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInputResp, respLen);
	g_RakClient->RPC(RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendExitPlayerVehicle(uint16_t VehicleID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(VehicleID);
	g_RakClient->RPC(RPC_ExitVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendPickedUpPickup(int PickupID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(PickupID);
	g_RakClient->RPC(RPC_PickedUpPickup, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendPlayerDamage(uint16_t playerid, float amountdam, int weaponid, bool takedam)
{
	RakNet::BitStream damage;
	damage.Write(takedam);
	damage.Write(playerid);
	damage.Write(amountdam);
	damage.Write(weaponid);
	g_RakClient->RPC(RPC_PlayerGiveTakeDamage, &damage, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::RequestClass(int classId)
{
	RakNet::BitStream bsClass;

	bsClass.Write(classId);
	g_RakClient->RPC(RPC_RequestClass, &bsClass, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendSCMEvent(int vehicleID, int eventId, int param1, int param2)
{
	RakNet::BitStream bsScmEvent;

	bsScmEvent.Write(vehicleID);
	bsScmEvent.Write(param1);
	bsScmEvent.Write(param2);
	bsScmEvent.Write(eventId);

	g_RakClient->RPC(RPC_ScmEvent, &bsScmEvent, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendSpawn(void)
{
	RakNet::BitStream bsSpawn;

	g_RakClient->RPC(RPC_RequestSpawn, &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
	g_RakClient->RPC(RPC_Spawn, &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE);
}

void OrigRakClient::SendClientJoin(char *playerNick)
{
	restartGame();
	BYTE byteNameLen = (BYTE)strlen(playerNick);

	char *szVersion = "0.3.7";
	BYTE byteVerLen = (BYTE)strlen(szVersion);

	BYTE byteAuthBSLen = (BYTE)strlen(AUTH_BS);

	RakNet::BitStream bsSend;
	bsSend.Write(NETGAME_VERSION);
	bsSend.Write((BYTE)'\001');
	bsSend.Write(byteNameLen);
	bsSend.Write(playerNick, byteNameLen);

	bsSend.Write(set.challenge ^ NETGAME_VERSION);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(AUTH_BS, byteAuthBSLen);
	bsSend.Write(byteVerLen);
	bsSend.Write(szVersion, byteVerLen);
	bsSend.Write(set.challenge ^ NETGAME_VERSION);

	g_RakClient->RPC(RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE);
}