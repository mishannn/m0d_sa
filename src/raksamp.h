extern class OrigRakClient *g_RakClient;

class OrigRakClient
{
public:
	OrigRakClient(void *pOrigRakClientInterface);
	bool RPC(int rpcId, RakNet::BitStream *bitStream, PacketPriority priority = HIGH_PRIORITY, PacketReliability reliability = RELIABLE_ORDERED, char orderingChannel = 0, bool shiftTimestamp = false);
	bool Send(RakNet::BitStream *bitStream, PacketPriority priority = HIGH_PRIORITY, PacketReliability reliability = UNRELIABLE_SEQUENCED, char orderingChannel = 0);
	void SendDeath(uint16_t killerId, uint8_t reason);
	void SendFakeUnoccupiedSyncData(uint16_t VehicleID, float fPos[3]);
	void SendWeaponUpdateSync(BYTE weaponslot, BYTE weaponid, WORD ammo);
	void SendFakeDriverSyncData(uint16_t VehicleID, float fPos[3], float HealthCar, float Speed[3]);
	void SendFakeTrailerToCar(uint16_t TrailerID);
	void SendFakePassSyncData(uint16_t VehicleID, float fPos[3], uint8_t Seat);
	void SendFakeBulletSyncData(float aimHit[3], float bodyPos[3], float hitPos[3], BYTE hitType, uint16_t id);
	void SendFakeOnfootSyncData(float fPos[3], float Health, float speed[3]);
	void SendFakeAimSyncData(float vecAimPos[3], float vecAimf1[3], float fAimZ, BYTE byteCamMode, BYTE byteCamExtZoom, BYTE byteWeaponState, BYTE bUnk);
	void SendFakeSurfing(uint16_t VehicleID);
	void SendEnterPlayerVehicle(uint16_t VehicleID, BOOL seat);
	void SendFakeCarDeath(uint16_t VehicleID);
	void SendFakeMenuSelectRow(BYTE RowID);
	void SendClickPlayer(BYTE playerID, int source);
	void SendDialogResponse(WORD wDialogID, BYTE bButtonID, WORD wListBoxItem, char *szInputResp);
	void SendExitPlayerVehicle(uint16_t VehicleID);
	void SendPickedUpPickup(int PickupID);
	void SendPlayerDamage(uint16_t playerid, float amountdam, int weaponid, bool takedam);
	void RequestClass(int classId);
	void SendSCMEvent(int vehicleID, int eventId, int param1, int param2);
	void SendSpawn(void);
	void SendClientJoin(char *playerNick);
	OrigRakClientInterface *GetRakClientInterface(void) { return pOrigRakClient; };

private:
	OrigRakClientInterface *pOrigRakClient;
};