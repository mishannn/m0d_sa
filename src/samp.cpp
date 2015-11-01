#include "main.h"

#define SAMP_DLL		"samp.dll"
#define SAMP_CMP		"F8036A004050518D4C24"

// randomStuff
//bool							g_bGotoSite = FALSE;
extern int						iViewingInfoPlayer;
int								g_iSpectateEnabled = 0, g_iSpectateLock = 0, g_iSpectatePlayerID = -1;
int								iNetModeNormalOnfootSendRate, iNetModeNormalIncarSendRate, iNetModeFiringSendRate, iNetModeSendMultiplier, iLagCompMode;
int								g_iCursorEnabled = 0;
float							vect3_null[3] = { 0.0f, 0.0f, 0.0f };

// global samp pointers
int								iIsSAMPSupported = 0;
int								g_renderSAMP_initSAMPstructs;
stSAMP							*g_SAMP = NULL;
stSAMPMisc						*g_SAMPMisc = NULL;
stPlayerPool					*g_Players = NULL;
stVehiclePool					*g_Vehicles = NULL;
stChatInfo						*g_Chat = NULL;
stInputInfo						*g_Input = NULL;
stKillInfo						*g_DeathList = NULL;
stDialog						*g_Dialog = NULL;

stTranslateGTASAMP_vehiclePool	translateGTASAMP_vehiclePool;
stTranslateGTASAMP_pedPool		translateGTASAMP_pedPool;

stStreamedOutPlayerInfo			g_stStreamedOutInfo;

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// FUNCTIONS //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

int getPickup(float cpos[3], float range, int modelid)
{
	int iPickupID = -1;
	float fDist = -1.0f;

	for (int i = 0; i < SAMP_MAX_PICKUPS; i++)
	{
		if (g_SAMP->pPools->pPickup->pickup[i].iModelID == modelid)
		{
			float fTempDist = vect3_dist(g_SAMP->pPools->pPickup->pickup[i].fPosition, cpos);
			if ((fTempDist < fDist || fDist < 0.0f) && fTempDist < range)
			{
				iPickupID = i;
				fDist = fTempDist;
			}
		}
	}
	cheat_state_text("%d", iPickupID);
	return iPickupID;
}

void vehicleUnflip(struct vehicle_info *info)
{
	if (cheat_state->state == CHEAT_STATE_VEHICLE)
	{
		struct vehicle_info *info = vehicle_info_get(VEHICLE_SELF, NULL);
		CVehicle *cveh = getSelfCVehicle();
		CVector vZero(0.0f, 0.0f, 0.0f);
		cveh->SetTurnSpeed(&vZero);
		vect3_zero(info->speed);
	}
}

bool gotoPos(float pos[3], float range)
{
	actor_info *self = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
	float cpos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };
	float angle = atan2(pos[0] - cpos[0], cpos[1] - pos[1]);
	angle += M_PI;
	self->fTargetRotation = angle;
	self->fCurrentRotation = angle;
	if (vect3_dist(cpos, pos) > range)
	{
		GTAfunc_PerformAnimation("PED", "RUN_CIVI", -1, 1, 1, 0, 0, 0, 1, 0);
		return 0;
	}
	else return 1;
}

void update_translateGTASAMP_vehiclePool(void)
{
	traceLastFunc("update_translateGTASAMP_vehiclePool()");
	if (!g_Vehicles)
		return;

	int iGTAID;
	for (int i = 0; i <= SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		if (isBadPtr_writeAny(g_Vehicles->pSAMP_Vehicle[i], sizeof(stSAMPVehicle)))
			continue;
		iGTAID = getVehicleGTAIDFromInterface((DWORD *)g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle);
		if (iGTAID <= SAMP_MAX_VEHICLES && iGTAID >= 0)
		{
			translateGTASAMP_vehiclePool.iSAMPID[iGTAID] = i;
		}
	}
}

// update SAMPGTA ped translation structure
void update_translateGTASAMP_pedPool(void)
{
	traceLastFunc("update_translateGTASAMP_pedPool()");
	if (!g_Players)
		return;

	int iGTAID, i;
	for (i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (i == g_Players->sLocalPlayerID)
		{
			translateGTASAMP_pedPool.iSAMPID[0] = i;
			continue;
		}

		if (isBadPtr_writeAny(g_Players->pRemotePlayer[i], sizeof(stRemotePlayer)))
			continue;
		if (isBadPtr_writeAny(g_Players->pRemotePlayer[i]->pPlayerData, sizeof(stRemotePlayerData)))
			continue;
		if (isBadPtr_writeAny(g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor, sizeof(stSAMPPed)))
			continue;

		iGTAID = getPedGTAIDFromInterface((DWORD *)g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped);
		if (iGTAID <= SAMP_MAX_PLAYERS && iGTAID >= 0)
		{
			translateGTASAMP_pedPool.iSAMPID[iGTAID] = i;
		}
	}
}

// ClientCommands
void cmd_sskin(char *params)
{
	int iParam1, iParam2;
	RPCParameters rpcParams;
	int numberOfParams = sscanf(params, "%d%d", &iParam1, &iParam2);
	int inputArray[2] = 
	{
		numberOfParams < 2 ? g_Players->sLocalPlayerID : iParam1,
		numberOfParams < 2 ? iParam1 : iParam2
	};
	rpcParams.input = (unsigned char *)inputArray;
	rpcParams.numberOfBitsOfData = 64;
	((void(__cdecl *)(RPCParameters *))(g_dwSAMP_Addr + 0x15860))(&rpcParams);
}

void cmd_addbot(char *params)
{
	if (strlen(params) < 0)
	{
		char szNick[16];
		GenRandomStr(szNick, 16);
		CRakBot::Connect(szNick, "");
	}
	else CRakBot::Connect(params, "");
}

int spectateId;
void spectate(void *)
{
	float pos[3];
	while (spectateId != -1)
	{
		getPlayerPos(spectateId, pos);
		pos[2] -= 70.0f;
		cheat_teleport(pos, gta_interior_id_get());
		Sleep(500);
	}
	ExitThread(0);
}

void cmd_re(char *param)
{
	static float positionspec[3];
	static bool saveposspec = 1;
	if (saveposspec == 1)
	{
		positionspec[0] = g_Players->pLocalPlayer->onFootData.fPosition[0];
		positionspec[1] = g_Players->pLocalPlayer->onFootData.fPosition[1];
		positionspec[2] = g_Players->pLocalPlayer->onFootData.fPosition[2];
	}
	if (!strcmp(param, ""))
	{
		spectatePlayer(-1);
		cheat_teleport(positionspec, gta_interior_id_get());
		sampPatchDisableOnFoot(0);
		cheat_handle_unfreeze(vehicle_info_get(-1, 0), actor_info_get(-1, 0));
		spectateId = -1;
		saveposspec = 1;
	}
	else
	{
		spectatePlayer(atoi(param));
		sampPatchDisableOnFoot(1);
		spectateId = atoi(param);
		_beginthread(spectate, 0, 0);
		saveposspec = 0;
	}
}

void cmd_ucarwarp(char *param)
{
	int vehid = atoi(param);
	if (*(int *)0xBA6774 != 0)
	{
		float   mapPos[3];
		for (int i = 0; i < (0xAF * 0x28); i += 0x28)
		{
			if (*(short *)(0xBA873D + i) == 4611)
			{
				float   *pos = (float *)(0xBA86F8 + 0x28 + i);
				mapPos[0] = *pos;
				mapPos[1] = *(pos + 1);
				mapPos[2] = pGameInterface->GetWorld()->FindGroundZForPosition(mapPos[0], mapPos[1]) + 2.0f;
				g_RakClient->SendFakeUnoccupiedSyncData(vehid, mapPos);
				cheat_vehicle_teleport(g_Vehicles->pGTA_Vehicle[vehid], mapPos, 0);
			}
		}
	}
	else
	{
		cheat_state_text("Не поставлена метка на карте");
	}
}

void cmd_carwarp(char *param)
{
	int vehid = atoi(param);
	if (*(int *)0xBA6774 != 0)
	{
		float   mapPos[3];
		for (int i = 0; i < (0xAF * 0x28); i += 0x28)
		{
			if (*(short *)(0xBA873D + i) == 4611)
			{
				float   *pos = (float *)(0xBA86F8 + 0x28 + i);
				mapPos[0] = *pos;
				mapPos[1] = *(pos + 1);
				mapPos[2] = pGameInterface->GetWorld()->FindGroundZForPosition(mapPos[0], mapPos[1]) + 2.0f;
				g_RakClient->SendEnterPlayerVehicle(vehid, 0);
				g_RakClient->SendFakeDriverSyncData(vehid, mapPos, g_Vehicles->pGTA_Vehicle[vehid]->hitpoints, g_Vehicles->pGTA_Vehicle[atoi(param)]->speed);
				cheat_vehicle_teleport(g_Vehicles->pGTA_Vehicle[vehid], mapPos, 0);
			}
		}
	}
	else
	{
		cheat_state_text("Не поставлена метка на карте");
	}
}

void cmd_createcar(char *param)
{
	if (atoi(param) < -1 || atoi(param) > -1 && atoi(param) < 400 || atoi(param) > 611)
		return;

	int vModel = atoi(param);
	loadSpecificModel(vModel);
	CVehicle *car = pGame->GetPools()->AddVehicle(eVehicleTypes(vModel));
	car->SetPosition(pPedSelf->GetPosition());
	vehicle_info *vinfo = (vehicle_info *)car->GetInterface();
	vinfo->base.matrix[14] += 0.5;
	GTAfunc_PutActorInCar(vinfo);
	cheat_state->_generic.sync = 1;
}

void cmd_ftazer(char *param)
{
	int playerid = atoi(param);
	float okpos[3];
	getPlayerPos(playerid, okpos);
	okpos[2] -= 2.5f;
	g_RakClient->SendFakeOnfootSyncData(okpos, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
	say("/tazer");
}

void cmd_ghere(char *param)
{
	if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[atoi(param)]) && (
						g_Players->pRemotePlayer[atoi(param)]->pPlayerData == NULL || 
						g_Players->pRemotePlayer[atoi(param)]->pPlayerData->pSAMP_Actor == NULL)) 
		cheat_state_text("Игрок не находится в зоне прорисовки!");
	else if (g_Players->pLocalPlayer->sCurrentVehicleID == 65535) 
		cheat_state_text("Вы должны находиться в машине!");
	else
	{
		float fPos[3];
		getPlayerPos(atoi(param), fPos);
		fPos[2] -= 2.0f;
		g_RakClient->SendFakeDriverSyncData(g_Players->pLocalPlayer->sCurrentVehicleID, fPos, 
											g_Players->pLocalPlayer->inCarData.fVehicleHealth, vect3_null);
	}
}

void cmd_fcuff(char *param)
{
	int playerid = atoi(param);
	float okpos[3];
	getPlayerPos(playerid, okpos);
	okpos[2] -= 2.5f;
	g_RakClient->SendFakeOnfootSyncData(okpos, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
	say("/cuff %d", playerid);
}

void cmd_fpay(char *params)
{
	int playerid, money;
	if (!strlen(params) || sscanf(params, "%d%d", &playerid, &money) < 2)
		return;

	float okpos[3];
	getPlayerPos(playerid, okpos);
	okpos[2] -= 2.5f;
	g_RakClient->SendFakeOnfootSyncData(okpos, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
	say("/pay %d %d", playerid, money);
}

void cmd_gotopickup(char *param)
{
	if (cheat_state->state == CHEAT_STATE_ACTOR) 
		g_RakClient->SendFakeOnfootSyncData(g_SAMP->pPools->pPickup->pickup[atoi(param)].fPosition, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
	else 
		g_RakClient->SendFakeDriverSyncData(g_Players->pLocalPlayer->sCurrentVehicleID, g_SAMP->pPools->pPickup->pickup[atoi(param)].fPosition, getGTAVehicleFromSAMPVehicleID(g_Players->pLocalPlayer->sCurrentVehicleID)->hitpoints, getGTAVehicleFromSAMPVehicleID(g_Players->pLocalPlayer->sCurrentVehicleID)->speed);
	RakNet::BitStream bs;
	bs.Write(atoi(param));
	g_RakClient->RPC(RPC_PickedUpPickup, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, 0);
}

void cmd_setint(char *param)
{
	gta_interior_id_set(atoi(param));
}

void cmd_flood(char *param)
{
	strcpy(set.flood_text, param);
	cheat_state_text("Установлен текст для флуда: \"%s\"", set.flood_text);
}

void cmd_floodtime(char *param)
{
	set.flood_delay = atoi(param);
	cheat_state_text("Установлена задержка флуда: %dмс", set.flood_delay);
}

void cmd_findtext(char *param)
{
	if(param[0] == NULL)
	{
		cheat_state->_generic.findtext = 0;
		cheat_state_text("Поиск текста отключен");
		return;
	}

	strcpy(set.find_text, param);
	cheat_state->_generic.findtext = 1;
	cheat_state_text("Включен поиск текста: \"%s\"", set.find_text);
}

void cmd_vcolor(char *params)
{
	int numcolor, color;
	if (!strlen(params) || sscanf(params, "%d%d", &numcolor, &color) < 2)
	{
		addMessageToChatWindow("USAGE: /m0d_vcolor <num color> <color>");
		return;
	}
	if (color < 0 || color > 255 || numcolor < 1)
		return;

	struct vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
	if (vinfo == NULL)
		return;

	if (numcolor == 1) vehicle_setColor0(vinfo, color);
	else if (numcolor == 2) vehicle_setColor1(vinfo, color);
}

void cmd_paintjob(char *param)
{
	struct vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
	if (vinfo == NULL)
		return;

	if (atoi(param) > 0 && atoi(param) < 5) vehicle_setPaintJob(vinfo, atoi(param));
}

void cmd_addtune(char *param)
{
	struct vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
	if (vinfo == NULL)
		return;

	vehicle_addUpgrade(vinfo, atoi(param));
}

void cmd_scar(char *param)
{
	g_RakClient->SendFakeCarDeath(atoi(param));
}

void cmd_menusel(char *params)
{
	BYTE bRow = (BYTE)atoi(params);
	if (bRow != 0xFF)
	{
		RakNet::BitStream bsSend;
		bsSend.Write(bRow);
		g_RakClient->RPC(RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE);
	}
}

void cmd_sdialog(char *params)
{
	int dialogId, buttonId, listItem;
	char dialogText[128];
	if (sscanf(params, "%d%d%d%[^\0]", &dialogId, &buttonId, &listItem, &dialogText) < 4)
	{
		if (sscanf(params, "%d%d%d", &dialogId, &buttonId, &listItem) < 3)
			return;
		else
			strcpy(dialogText, "");
	}
	g_RakClient->SendDialogResponse(dialogId, buttonId, listItem, dialogText);
	cheat_state_text("Данные диалога отправлены | ID: %d Кнопка: %d Пункт: %d Текст: %s", dialogId, buttonId, listItem, strlen(dialogText) ? dialogText : "пусто");
}

void cmd_fakerejoin(char *params)
{
	cheat_state->_generic.fakerejoin = 1;
	char newNick[16], oldNick[24];
	GenRandomStr(newNick, 16);
	strcpy(oldNick, getPlayerName(g_Players->sLocalPlayerID));
	strcpy(set.fakerejoinpass, params);
	g_RakClient->SendClientJoin(newNick);
	g_RakClient->SendClientJoin(oldNick);
}

void cmd_fakejoin(char *params)
{
	char newNick[24];
	if (strlen(params) < 1) 
		GenRandomStr(newNick, 16);
	else 
		strcpy(newNick, params);
	g_RakClient->SendClientJoin(newNick);
}

void cmd_stealcar(char *params)
{
	if (strlen(params) < 1) return;
	int carId = atoi(params);
	set.stcar = carId;
	cheat_state->_generic.stealcar ^= 1;
	if (cheat_state->_generic.stealcar)
	{
		GTAfunc_CameraOnVehicle(getGTAVehicleFromSAMPVehicleID(carId));
		cheat_state_text("Угон машины с ID: %d", carId);
	}
	else
	{
		GTAfunc_TogglePlayerControllable(0);
		GTAfunc_LockActor(0);
		pGameInterface->GetCamera()->RestoreWithJumpCut();
		cheat_state_text("Угон отключен");
	}
}

void cmd_autorej(char *param)
{
	cheat_state->_generic.autorejoin = true;
	strcpy(set.autorejoin_pass, param);
	addMessageToChatWindow("Автореконнект включен! Пароль: %s", set.autorejoin_pass);
}

void cmd_fire(char *param)
{
	int vehID = atoi(param);
	struct vehicle_info *info = getGTAVehicleFromSAMPVehicleID(vehID);
	g_RakClient->SendEnterPlayerVehicle(vehID, 0);
	g_RakClient->SendFakeDriverSyncData(vehID, &info->base.matrix[12], 0.0f, info->speed);
	info->hitpoints = 0.0f;
}

void cmd_findveh(char *param)
{
	for (int i = 0; i < 212; i++)
	{
		if (!strcmpi(param, vehicle_list[i].name))
		{
			set.find_id = 400 + i;
			cheat_state->_generic.getauto = 1;
			cheat_state_text("Установлена машина для поиска: %s | %d", vehicle_list[i].name, set.find_id);
			break;
		}
	}
}

void cmd_fake(char *params)
{
	char kind[16], text[256];
	sscanf(params, "%s", kind);
	if (!strcmp(kind, "kick"))
	{
		char admin[24], player[24], reason[64];
		if (sscanf(params, "%s%s%s%[^\0]", kind, admin, player, reason) < 4) return;
		sprintf(text, " Администратор: %s кикнул %s. Причина:%s", admin, player, reason);
		addToChatWindow(text, 0xFFFF6347, -1);
		addToChatWindow("Server closed the connection.", 0xFFA9C4E4, -1);
		return;
	}
	if (!strcmp(kind, "ban"))
	{
		char admin[24], player[24], reason[64];
		if (sscanf(params, "%s%s%s%[^\0]", kind, admin, player, reason) < 4) return;
		sprintf(text, " Администратор: %s забанил %s. Причина:%s", admin, player, reason);
		addToChatWindow(text, 0xFFFF6347, -1);
		addToChatWindow("Server closed the connection.", 0xFFA9C4E4, -1);
		return;
	}
	if (!strcmp(kind, "3ban"))
	{
		char admin[24], player[24], reason[64];
		if (sscanf(params, "%s%s%s%[^\0]", kind, admin, player, reason) < 4) return;
		sprintf(text, " Администратор: %s забанил %s [3 Предупреждения]. Причина:%s", admin, player, reason);
		addToChatWindow(text, 0xFFFF6347, -1);
		addToChatWindow("Server closed the connection.", 0xFFA9C4E4, -1);
		return;
	}
	if (!strcmp(kind, "pass"))
	{
		char lic[1];
		if (sscanf(params, "%s%s", kind, lic) < 2) return;
		addToChatWindow("-----------===[PASSPORT]===----------", 0xFFFFFF00, -1);
		char buf[512];
		sprintf(buf, " Имя: %s", set.fakestat[0]);
		addToChatWindow(buf, 0xFFF5DEB3, -1);
		sprintf(buf, " Возраст: %s   Телефон: %s", set.fakestat[1], set.fakestat[4]);
		addToChatWindow(buf, 0xFFF5DEB3, -1);
		sprintf(buf, " Фракция: %s   Должность: %s", set.fakestat[16], set.fakestat[17]);
		addToChatWindow(buf, 0xFFF5DEB3, -1);
		sprintf(buf, " Работа: %s   Разрешение на оружие: %s", set.fakestat[18], !strcmp(lic, "1") ? "Есть" : "Нет");
		addToChatWindow(buf, 0xFFF5DEB3, -1);
		sprintf(buf, " Преступлений: %s", set.fakestat[7]);
		addToChatWindow(buf, 0xFFF5DEB3, -1);
		sprintf(buf, " Законопослушность: %s", set.fakestat[6]);
		addToChatWindow(buf, 0xFFF5DEB3, -1);
		addToChatWindow("=============================", 0xFFFFFF00, -1);
		sprintf(buf, " %s показал(а) свой паспорт", set.fakestat[0]);
		addToChatWindow(buf, 0xFFCFA2DA, -1);
		return;
	}
	if (!strcmp(kind, "stat"))
	{
		char caption[1024];
		sprintf(caption, "Имя:				%s\n\nУровень:			%s\nExp:				%s\nДеньги:			%s\nТелефон:			%s\nВарнов:			%s\nЗаконопослушность:		%s\nПреступлений:		%s\nАрестов:			%s\nСмертей в розыске:		%s\nУровень розыска:		%s\nДата регистрации:		%s\nЗависимость:			%s\nНаркотики:			%s\nМатериалы:			%s\nРыбы поймано			%s\nОрганизация:			%s\nРанг:				%s\nРабота:			%s\nСтатус:				%s\nЖена/Муж:			%s\nПол:				%s", set.fakestat[0], set.fakestat[1], set.fakestat[2], set.fakestat[3], set.fakestat[4], set.fakestat[5], set.fakestat[6], set.fakestat[7], set.fakestat[8], set.fakestat[9], set.fakestat[10], set.fakestat[11], set.fakestat[12], set.fakestat[13], set.fakestat[14], set.fakestat[15], set.fakestat[16], set.fakestat[17], set.fakestat[18], set.fakestat[19], set.fakestat[20], set.fakestat[21]);
		showSampDialog(0, 228, 0, "Статистика персонажа", caption, "Готово", "");
		return;
	}
	if (!strcmp(kind, "news"))
	{
		char buf[512], prislal[24], phone[16], news[2], proveril[24], text[512];
		if (sscanf(params, "%s%s%s%s%s%[^\0]", kind, prislal, phone, news, proveril, text) < 5) return;
		sprintf(buf, " Объявление: %s. Прислал: %s. Тел: %s", text, prislal, phone);
		addToChatWindow(buf, 0xFF00D900, -1);
		sprintf(buf, "        Отредактировал сотрудник %s News: %s", news, proveril);
		addToChatWindow(buf, 0xFF00D900, -1);
		return;
	}
	if (!strcmp(kind, "time"))
	{
		char day[2], decmouth[2], hour[2], minute[2], server[2];
		if (sscanf(params, "%s%s%s%s%s%s", kind, day, decmouth, hour, minute, server) < 6) return;
		char mouth[16];
		switch (atoi(decmouth))
		{
		case 1:
			strcpy(mouth, "January");
			break;
		case 2:
			strcpy(mouth, "February");
			break;
		case 3:
			strcpy(mouth, "March");
			break;
		case 4:
			strcpy(mouth, "April");
			break;
		case 5:
			strcpy(mouth, "May");
			break;
		case 6:
			strcpy(mouth, "June");
			break;
		case 7:
			strcpy(mouth, "July");
			break;
		case 8:
			strcpy(mouth, "August");
			break;
		case 9:
			strcpy(mouth, "September");
			break;
		case 10:
			strcpy(mouth, "October");
			break;
		case 11:
			strcpy(mouth, "November");
			break;
		case 12:
			strcpy(mouth, "December");
			break;
		}
		if (atoi(decmouth) > 12) strcpy(mouth, "Fail");
		char buf[512];
		sprintf(buf, "~y~%d %s~n~~w~%.2d:%.2d~n~~g~Server %.2d", atoi(day), mouth, atoi(hour), atoi(minute), atoi(server));
		showGameText(buf, 5000, 1);
		return;
	}
	if (!strcmp(kind, "skills"))
	{
		int skill[6], i;
		if (sscanf(params, "%s%d%d%d%d%d%d", kind, &skill[0], &skill[1], &skill[2], &skill[3], &skill[4], &skill[5]) < 7) return;
		if (skill[0] > 100 || skill[0] < 0 ||
			skill[1] > 100 || skill[1] < 0 ||
			skill[2] > 100 || skill[2] < 0 ||
			skill[3] > 100 || skill[3] < 0 ||
			skill[4] > 100 || skill[4] < 0 ||
			skill[5] > 100 || skill[5] < 0)
		{
			cheat_state_text("Неверное значение!");
			return;
		}
		char skills[1024], buf[256];
		strcpy(skills, "<< Навыки владения оружием >>\n\nSDPistol 	[");
		for (i = 0; i < skill[0]; i++)	strcat(skills, "|");
		for (i = 0; i < 100 - skill[0]; i++) strcat(skills, "'");
		sprintf(buf, "] %d\nDesert eagle 	[", skill[0]);
		strcat(skills, buf);
		for (i = 0; i < skill[1]; i++)	strcat(skills, "|");
		for (i = 0; i < 100 - skill[1]; i++) strcat(skills, "'");
		sprintf(buf, "] %d\nShotgun	[", skill[1]);
		strcat(skills, buf);
		for (i = 0; i < skill[2]; i++)	strcat(skills, "|");
		for (i = 0; i < 100 - skill[2]; i++) strcat(skills, "'");
		sprintf(buf, "] %d\nMP5 		[", skill[2]);
		strcat(skills, buf);
		for (i = 0; i < skill[3]; i++)	strcat(skills, "|");
		for (i = 0; i < 100 - skill[3]; i++) strcat(skills, "'");
		sprintf(buf, "] %d\nAK47 		[", skill[3]);
		strcat(skills, buf);
		for (i = 0; i < skill[4]; i++)	strcat(skills, "|");
		for (i = 0; i < 100 - skill[4]; i++) strcat(skills, "'");
		sprintf(buf, "] %d\nM4A1 		[", skill[4]);
		strcat(skills, buf);
		for (i = 0; i < skill[5]; i++)	strcat(skills, "|");
		for (i = 0; i < 100 - skill[5]; i++) strcat(skills, "'");
		sprintf(buf, "] %d", skill[5]);
		strcat(skills, buf);
		showSampDialog(0, 228228, 0, " ", skills, "Готово", "");
		return;
	}
	if (!strcmp(kind, "sms"))
	{
		char prislal[24], sms[128], text[256];
		if (sscanf(params, "%s%s%[^\0]", kind, prislal, sms) < 3) return;
		sprintf(text, " SMS: %s. Отправитель: %s", sms, prislal);
		addToChatWindow(text, 0xFFFFFF00, -1);
		return;
	}
}

void cmd_boom(char *param)
{
	struct actor_info    *self = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
	for (int v = 0; v < SAMP_MAX_VEHICLES; v++)
	{
		if (!g_Vehicles->iIsListed[v]) continue;
		if (!g_Vehicles->pSAMP_Vehicle[v]) continue;
		if (!g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle) continue;
		if (g_Vehicles->pSAMP_Vehicle[v]->iIsLocked) continue;

		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info *info = vehicle_info_get(car_id, 0);

		if (info->passengers[0]) continue;

		int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(info);
		if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID) continue;

		float fPos[3], fSpd[3] = { 0.0f, 0.0f, -0.4f };
		vect3_copy(&info->base.matrix[12], fPos);

		int id = atoi(param);
		getPlayerPos(id, fPos);
		fPos[2] += 4.0f;
		g_RakClient->SendEnterPlayerVehicle(iSAMPVehicleID, 0);
		g_RakClient->SendFakeDriverSyncData(iSAMPVehicleID, fPos, info->hitpoints = 0.0f, fSpd);
		cheat_vehicle_teleport(info, fPos, gta_interior_id_get());
		info->speed[2] = -0.4f;
		break;
	}
}

void cmd_vkick(char *param)
{
	struct actor_info    *self = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
	for (int v = 0; v < SAMP_MAX_VEHICLES; v++)
	{
		if (g_Vehicles->iIsListed[v] == NULL)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[v] == NULL)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[v]->pGTA_Vehicle == NULL)
			continue;

		static int time = 0;

		if (GetTickCount() - 1500 > time)
		{

			int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
			struct vehicle_info *vinfo = vehicle_info_get(car_id, 0);

			int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);

			float fPos[3], fSpd[3] = { 0.0f, 0.0f, 3.0f };
			vect3_copy(&vinfo->base.matrix[12], fPos);

			int id = atoi(param);
			getPlayerPos(id, fPos);
			fPos[2] -= 3.0f;
			g_RakClient->SendEnterPlayerVehicle(iSAMPVehicleID, 0);
			g_RakClient->SendFakeDriverSyncData(iSAMPVehicleID, fPos, vinfo->hitpoints, fSpd);
			time = GetTickCount();
		}
	}
}

void cmd_takecar(char *param)
{
	int carId = getPlayerSAMPVehicleID(atoi(param));
	struct vehicle_info *info = getGTAVehicleFromSAMPVehicleID(carId);
	if (info)
	{
		g_RakClient->SendEnterPlayerVehicle(carId, 0);
		GTAfunc_PutActorInCar(info);
	}
}

void cmd_coordcw(char *params)
{
	int carid;
	float pos[3];
	if (!strlen(params) || sscanf(params, "%d%f%f%f", &carid, &pos[0], &pos[1], &pos[2]) < 2) return;

	struct vehicle_info *info = vehicle_info_get(carid, 0);

	g_RakClient->SendEnterPlayerVehicle(carid, 0);
	g_RakClient->SendFakeDriverSyncData(carid, pos, 1000.0f, vect3_null);
	cheat_vehicle_teleport(g_Vehicles->pGTA_Vehicle[carid], pos, gta_interior_id_get());
}

void cmd_nick(char *param)
{
	setLocalPlayerName(param);
	restartGame();
	disconnect(500);
	cheat_state_text("Переподключение со сменой ника | %d секунд...", set.rejoin_delay / 1000);
	cheat_state->_generic.rejoinTick = GetTickCount();
}

void cmd_shot(char *param)
{
	int playerId = atoi(param);
	float playerPos[3];
	float cPos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };
	float bodyPos[3] = { 0.0f, 0.0f, 0.0f };
	getPlayerPos(playerId, playerPos);
	g_RakClient->SendFakeBulletSyncData(cPos, bodyPos, playerPos, 1, playerId);
	cheat_state_text("Фейк выстрел в %s(%d)", getPlayerName(playerId), playerId);
}

uint16_t trailerId = -1;
void cmd_tsave(char *param)
{
	trailerId = atoi(param);
	float fPos[3] = { 49999998976.00, 49999998976.00, 49999998976.00 };
	g_RakClient->SendFakeUnoccupiedSyncData(trailerId, fPos);
	cheat_vehicle_teleport(getGTAVehicleFromSAMPVehicleID(trailerId), fPos, 0);
	cheat_state_text("Прицеп c ID %d сохранен.", trailerId);
}

void cmd_tunload()
{
	g_RakClient->SendFakeTrailerToCar(trailerId);
	say("/tunload");
}

// delete car
void cmd_urc(char *param)
{
	int vehId = atoi(param);
	float fPos[3] = { 49999998976.00f, 49999998976.00f, 49999998976.00f };
	g_RakClient->SendFakeUnoccupiedSyncData(vehId, fPos);
}

void cmd_crc(char *param)
{
	int carid = atoi(param);
	float pos[3] = { 49999998976.00f, 49999998976.00f, 49999998976.00f };
	g_RakClient->SendFakeDriverSyncData(carid, pos, 1000.0f, vect3_null);
}

void cmd_parse(char *param)
{
	int count = 0;
	uint32_t	samp_info = (uint32_t)g_SAMP;
	uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_UPDATESCOREBOARDDATA;
	__asm mov ecx, samp_info
	__asm call func
	int levelplayer = atoi(param);

	FILE    *flStolenObjects = NULL;
	char    filename[512];
	snprintf(filename, sizeof(filename), "%s\\logins.txt", g_szWorkingDirectory);

	flStolenObjects = fopen(filename, "w");
	if (flStolenObjects == NULL)return;

	if (levelplayer == -1)
	{
		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (g_Players->iIsListed[i] == 1)
			{
				fprintf(flStolenObjects, "%s\n", getPlayerName(i));
				count++;
			}
		}
	}
	else
	{
		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (g_Players->iIsListed[i] == 1)
			{
				if (g_Players->pRemotePlayer[i]->iScore >= levelplayer)
				{
					fprintf(flStolenObjects, "%s\n", getPlayerName(i));
					count++;
				}
			}
		}
	}
	fclose(flStolenObjects);
	char text[400];
	sprintf_s(text, "%d ников записаны в файл logins.txt", count);
	showSampDialog(0, 3, 0, "Готово!", text, "Закрыть", "");
}

void cmd_getcheck(char *param)
{
	for (int i = 0; i <= 32; i++)
	{
		float x = *(float *)(0xC7DD58 + 48 + 160 * i);
		float y = *(float *)(0xC7DD58 + 52 + 160 * i);
		float z = *(float *)(0xC7DD58 + 56 + 160 * i);
		if ((x > 2695 && x < 2696 && y > -1705 && y < -1704) || (x > 1099 && x < 1100 && y > 1601 && y < 1602) || (x == 0 && y == 0) || *(WORD *)(0xC7DD58 + 80 + 160 * i) != 1) continue;
		else
		{
			float pos[3] = { x, y, z + 1 };
			cheat_teleport(pos, gta_interior_id_get());
			break;
		}
	}
}

void cmd_rctime(char *param)
{
	set.rejoin_delay = atoi(param) * 1000;
	cheat_state_text("Установлено время переподключения: %d секунд", set.rejoin_delay / 1000);
}

void cmd_fakecheck()
{
	for (int i = 0; i <= 32; i++)
	{
		float x = *(float *)(0xC7DD58 + 48 + 160 * i);
		float y = *(float *)(0xC7DD58 + 52 + 160 * i);
		float z = *(float *)(0xC7DD58 + 56 + 160 * i);
		if ((x > 2695 && x < 2696 && y > -1705 && y < -1704) || (x > 1099 && x < 1100 && y > 1601 && y < 1602) || (x == 0 && y == 0) || *(WORD *)(0xC7DD58 + 80 + 160 * i) != 1) continue;
		else
		{
			float pos[3] = { x, y, z + 1 };
			if (cheat_state->state == CHEAT_STATE_ACTOR) 
				g_RakClient->SendFakeOnfootSyncData(pos, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
			else 
				g_RakClient->SendFakeDriverSyncData(g_Players->pLocalPlayer->sCurrentVehicleID, pos, getGTAVehicleFromSAMPVehicleID(g_Players->pLocalPlayer->sCurrentVehicleID)->hitpoints, getGTAVehicleFromSAMPVehicleID(g_Players->pLocalPlayer->sCurrentVehicleID)->speed);
			break;
		}
	}
}

void cmd_sethp(char *param)
{
	g_RakClient->SendDeath(52, 0);
	actor_info_get(-1, 0x01)->hitpoints = atoi(param);
}

void cmd_gethp(char *param)
{
	actor_info_get(-1, 0x01)->hitpoints = atoi(param);
}

void cmd_fakehp(char *param)
{
	struct actor_info *self = actor_info_get(-1, 0x01);
	g_RakClient->SendFakeOnfootSyncData(&self->base.matrix[12], (float)atof(param), self->speed);
}

void cmd_setvhp(char *param)
{
	struct vehicle_info *vinfo = vehicle_info_get(VEHICLE_SELF, 0);
	if (vinfo == NULL)
		return;

	vinfo->hitpoints = atoi(param);
}

void cmd_dhide()
{
	g_Dialog->iDialogShowed ^= 1;
}

void cmd_warp(char *param)
{
	struct actor_info *actor = NULL;
	int  playerid = atoi(param);
	float pos[3];
	if (g_Players == NULL)  return;
	if (g_Players->iIsListed[playerid] != 1) 
		return cheat_state_text("Игрок не доступен.");
	if (g_Players->pRemotePlayer[playerid]->pPlayerData == NULL || g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == NULL)
	{
		if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[playerid])) 
			return cheat_state_text("Игрок не доступен.");
		g_stStreamedOutInfo.fPlayerPos[playerid][1] += 1.0f;
		return cheat_teleport(g_stStreamedOutInfo.fPlayerPos[playerid], gta_interior_id_get());
	}
	if (!getPlayerPos(playerid, pos)) 
		return cheat_state_text("Игрок недоступен.");
	if (g_Players->pRemotePlayer[playerid]->pPlayerData != NULL &&  g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor != NULL) 
		actor = g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor->pGTA_Ped;
	if (actor != NULL && ACTOR_IS_DEAD(actor)) 
		return cheat_state_text("Игрок мертв.");
	pos[1] += 1.0f;
	return cheat_teleport(pos, actor->base.interior_id);
}

extern int	joining_server;
void cmd_change_server(char *param)
{
	traceLastFunc("cmd_change_server()");

	bool	success = false;

	char	IP[128], Nick[SAMP_MAX_PLAYER_NAME], Password[128] = "", Port[128];
	int		iPort;

	int ipc = sscanf(param, "%s%s%s%s", IP, Port, Nick, Password);
	if (ipc < 2)
	{
		addMessageToChatWindow("USAGE: /m0d_change_server <ip> <port> <Username> <Server Password>");
		addMessageToChatWindow("Variables that are set to \"NULL\" (capitalized) will be ignored.");
		addMessageToChatWindow("If you set the Password to \"NULL\" it is set to <no server password>.");
		addMessageToChatWindow("Username and password can also be left out completely.");
		return;
	}
	if (stricmp(IP, "NULL") == NULL)
		strcpy(IP, g_SAMP->szIP);

	if (stricmp(Port, "NULL") == NULL)
		iPort = g_SAMP->ulPort;
	else
		iPort = atoi(Port);

	if (ipc > 2)
	{
		if (stricmp(Nick, "NULL") != NULL)
		{
			if (strlen(Nick) > SAMP_ALLOWED_PLAYER_NAME_LENGTH)
				Nick[SAMP_ALLOWED_PLAYER_NAME_LENGTH] = '\0';
			setLocalPlayerName(Nick);
		}
	}
	if (ipc > 3)
	{
		if (stricmp(Password, "NULL") == NULL)
			strcpy(Password, "");
	}

	changeServer(IP, iPort, Password);
}

void cmd_change_server_fav(char *param)
{
	traceLastFunc("cmd_change_server_fav()");

	if (strlen(param) == 0)
	{
		addMessageToChatWindow("/m0d_fav_server <server name/part of server name>");
		addMessageToChatWindow("In order to see the favorite server list type: /m0d_fav_server list");
		return;
	}

	if (strncmp(param, "list", 4) == 0)
	{
		int count = 0;
		for (int i = 0; i < INI_SERVERS_MAX; i++)
		{
			if (set.server[i].server_name == NULL)
				continue;

			count++;
			addMessageToChatWindow("%s", set.server[i].server_name);
		}
		if (count == 0)
			addMessageToChatWindow("No servers in favorite server list. Edit the ini file to add some.");
		return;
	}

	for (int i = 0; i < INI_SERVERS_MAX; i++)
	{
		if (set.server[i].server_name == NULL || set.server[i].ip == NULL
			|| strlen(set.server[i].ip) < 7 || set.server[i].port == 0)
			continue;

		if (!findstrinstr((char *)set.server[i].server_name, param))
			continue;

		if (!set.use_current_name)
			setLocalPlayerName(set.server[i].nickname);

		changeServer(set.server[i].ip, set.server[i].port, set.server[i].password);

		return;
	}

	addMessageToChatWindow("/m0d_fav_server <server name/part of server name>");
	return;
}

void cmd_current_server(char *param)
{
	addMessageToChatWindow("Server Name: %s", g_SAMP->szHostname);
	addMessageToChatWindow("Server Address: %s:%i", g_SAMP->szIP, g_SAMP->ulPort);
	addMessageToChatWindow("Username: %s", getPlayerName(g_Players->sLocalPlayerID));
}

// strtokstristr?
bool findstrinstr(char *text, char *find)
{
	char	realtext[256];
	char	subtext[256];
	char	*result;
	char	*next;
	char	temp;
	int		i = 0;

	traceLastFunc("findstrinstr()");

	// can't find stuff that isn't there unless you are high
	if (text == NULL || find == NULL)
		return false;

	// lower case text ( sizeof()-2 = 1 for array + 1 for termination after while() )
	while (text[i] != NULL && i < (sizeof(realtext)-2))
	{
		temp = text[i];
		if (isupper(temp))
			temp = tolower(temp);
		realtext[i] = temp;
		i++;
	}
	realtext[i] = 0;

	// replace unwanted characters/spaces with dots
	i = 0;
	while (find[i] != NULL && i < (sizeof(subtext)-2))
	{
		temp = find[i];
		if (isupper(temp))
			temp = tolower(temp);
		if (!isalpha(temp))
			temp = '.';
		subtext[i] = temp;
		i++;
	}
	subtext[i] = 0;

	// use i to count the successfully found text parts
	i = 0;

	// split and find every part of subtext/find in text
	result = &subtext[0];
	while (*result != NULL)
	{
		next = strstr(result, ".");
		if (next != NULL)
		{
			// more than one non-alphabetic character
			if (next == result)
			{
				do
				next++;
				while (*next == '.');

				if (*next == NULL)
					return (i != 0);
				result = next;
				next = strstr(result, ".");
				if (next != NULL)
					*next = NULL;
			}
			else
				*next = NULL;
		}

		if (strstr(realtext, result) == NULL)
			return false;

		if (next == NULL)
			return true;

		i++;
		result = next + 1;
	}

	return false;
}

void cmd_tele_loc(char *param)
{
	if (strlen(param) == 0)
	{
		addMessageToChatWindow("USAGE: /m0d_tele_loc <location name>");
		addMessageToChatWindow("Use /m0d_tele_locations to show the location names.");
		addMessageToChatWindow("The more specific you are on location name the better the result.");
		return;
	}

	for (int i = 0; i < STATIC_TELEPORT_MAX; i++)
	{
		if (strlen(set.static_teleport_name[i]) == 0 || vect3_near_zero(set.static_teleport[i].pos))
			continue;

		if (!findstrinstr(set.static_teleport_name[i], param))
			continue;

		cheat_state_text("Teleported to: %s.", set.static_teleport_name[i]);
		cheat_teleport(set.static_teleport[i].pos, set.static_teleport[i].interior_id);
		return;
	}

	addMessageToChatWindow("USAGE: /m0d_tele_loc <location name>");
	addMessageToChatWindow("Use /m0d_tele_locations to show the location names.");
	addMessageToChatWindow("The more specific you are on location name the better the result.");
}

void cmd_tele_locations()
{
	for (int i = 0; i < STATIC_TELEPORT_MAX; i++)
	{
		if (strlen(set.static_teleport_name[i]) == 0 || vect3_near_zero(set.static_teleport[i].pos))
			continue;
		addMessageToChatWindow("%s", set.static_teleport_name[i]);
	}

	addMessageToChatWindow("To teleport use the menu or: /m0d_tele_loc <location name>");
}

void cmd_pickup(char *params)
{
	if (!strlen(params))
	{
		addMessageToChatWindow("USAGE: /sendpic <pickup id>");
		return;
	}

	g_RakClient->SendPickedUpPickup(atoi(params));
}

void cmd_setclass(char *params)
{
	if (!strlen(params))
	{
		addMessageToChatWindow("USAGE: /setclass <class id>");
		return;
	}

	g_RakClient->RequestClass(atoi(params));
}

void cmd_fakekill(char *param)
{
	g_RakClient->SendDeath(atoi(param), rand() % 41);
}

// new functions to check for bad pointers
int isBadPtr_SAMP_iVehicleID(int iVehicleID)
{
	if (g_Vehicles == NULL || iVehicleID == (uint16_t)-1)
		return 1;
	return !g_Vehicles->iIsListed[iVehicleID];

	// this hasn't been required yet
	//if (g_Vehicles->pSAMP_Vehicle[iVehicleID] == NULL) continue;
}

int isBadPtr_SAMP_iPlayerID(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return 1;
	return !g_Players->iIsListed[iPlayerID];
}

void getSamp()
{
	if (set.basic_mode)
		return;

	uint32_t        samp_dll = getSampAddress();

	if (samp_dll != NULL)
	{
		g_dwSAMP_Addr = (uint32_t)samp_dll;

		if (g_dwSAMP_Addr != NULL)
		{
			if (memcmp_safe((uint8_t *)g_dwSAMP_Addr + 0xBABE, hex_to_bin(SAMP_CMP), 10))
			{
				Log("SA:MP 0.3.7 was detected. g_dwSAMP_Addr: 0x%p", g_dwSAMP_Addr);
				struct patch_set fuckAC =
				{
					"kyenub patch", 0, 0,
					{
						{ 1, (void *)(g_dwSAMP_Addr + 0x99250), NULL, (uint8_t *)"\xC3", 0 },
						{ 8, (void *)(g_dwSAMP_Addr + 0xB2F70), NULL, (uint8_t *)"\xB8\x45\x00\x00\x00\xC2\x1C\x00", 0 },
						//{ 6, (void *)(g_dwSAMP_Addr + 0xB30F0), NULL, (uint8_t *)"\xB8\x01\x00\x00\x00\xC3", 0 }
					}
				};
				patcher_install(&fuckAC);
				iIsSAMPSupported = 1;
			}
			else
			{
				Log("Unknown SA:MP version. Running in basic mode.");
				iIsSAMPSupported = 0;
				set.basic_mode = 1;
				g_dwSAMP_Addr = NULL;
			}
		}
	}
	else
	{
		iIsSAMPSupported = 0;
		set.basic_mode = true;
		Log("samp.dll not found. Running in basic mode.");
	}

	return;
}

uint32_t getSampAddress()
{
	if (set.run_mode == RUNMODE_SINGLEPLAYER)
		return 0x0;

	uint32_t	samp_dll;

	if (set.run_mode == RUNMODE_SAMP)
	{
		if (set.wine_compatibility)
		{
			HMODULE temp = LoadLibrary(SAMP_DLL);
			__asm mov samp_dll, eax
		}
		else
		{
			void	*temp = dll_baseptr_get(SAMP_DLL);
			__asm mov samp_dll, eax
		}
	}

	if (samp_dll == NULL)
		return 0x0;

	return samp_dll;
}

struct stSAMP *stGetSampInfo(void)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	uint32_t	info_ptr;
	info_ptr = (UINT_PTR)* (uint32_t *)((uint8_t *)(void *)((uint8_t *)g_dwSAMP_Addr + SAMP_INFO_OFFSET));
	if (info_ptr == NULL)
		return NULL;

	return (struct stSAMP *)info_ptr;
}

struct stSAMPMisc *stGetSampMisc(void)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	uint32_t	info_ptr;
	info_ptr = (UINT_PTR)* (uint32_t *)((uint8_t *)(void *)((uint8_t *)g_dwSAMP_Addr + SAMP_MISC_INFO));
	if (info_ptr == NULL)
		return NULL;

	return (struct stSAMPMisc *)info_ptr;
}

class OrigRakClientInterface *stGetRakClient(void)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	uint32_t	info_ptr;
	info_ptr = (UINT_PTR)* (uint32_t *)((uint8_t *)(void *)((uint8_t *)g_SAMP + 0x3D1));
	if (info_ptr == NULL)
		return NULL;

	return (class OrigRakClientInterface *)info_ptr;
}

struct stChatInfo *stGetSampChatInfo(void)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	uint32_t	chat_ptr;
	chat_ptr = (UINT_PTR)* (uint32_t *)((uint8_t *)(void *)((uint8_t *)g_dwSAMP_Addr + SAMP_CHAT_INFO_OFFSET));
	if (chat_ptr == NULL)
		return NULL;

	return (struct stChatInfo *)chat_ptr;
}

struct stInputInfo *stGetInputInfo(void)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	uint32_t	input_ptr;
	input_ptr = (UINT_PTR)* (uint32_t *)((uint8_t *)(void *)((uint8_t *)g_dwSAMP_Addr + SAMP_CHAT_INPUT_INFO_OFFSET));
	if (input_ptr == NULL)
		return NULL;

	return (struct stInputInfo *)input_ptr;
}

struct stKillInfo *stGetKillInfo(void)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	uint32_t	kill_ptr;
	kill_ptr = (UINT_PTR)* (uint32_t *)((uint8_t *)(void *)((uint8_t *)g_dwSAMP_Addr + SAMP_KILL_INFO_OFFSET));
	if (kill_ptr == NULL)
		return NULL;

	return (struct stKillInfo *)kill_ptr;
}

struct stDialog *stGetDialogInfo(void)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	uint32_t    dialog_ptr;
	dialog_ptr = (UINT_PTR)* (uint32_t *)((uint8_t *)(void *)((uint8_t *)g_dwSAMP_Addr + SAMP_DIALOG_INFO_OFFSET));
	if (dialog_ptr == NULL)
		return NULL;

	return (struct stDialog *)dialog_ptr;
}

D3DCOLOR samp_color_get(int id, DWORD trans)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	D3DCOLOR	*color_table;
	if (id < 0 || id >= (SAMP_MAX_PLAYERS + 3))
		return D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);

	switch (id)
	{
	case (SAMP_MAX_PLAYERS) :
		return 0xFF888888;

	case (SAMP_MAX_PLAYERS + 1) :
		return 0xFF0000AA;

	case (SAMP_MAX_PLAYERS + 2) :
		return 0xFF63C0E2;
	}

	color_table = (D3DCOLOR *)((uint8_t *)g_dwSAMP_Addr + SAMP_COLOR_OFFSET);
	return (color_table[id] >> 8) | trans;
}

void spectatePlayer(int iPlayerID)
{
	if (iPlayerID == -1)
	{
		GTAfunc_TogglePlayerControllable(0);
		GTAfunc_LockActor(0);
		pGameInterface->GetCamera()->RestoreWithJumpCut();

		g_iSpectateEnabled = 0;
		g_iSpectateLock = 0;
		g_iSpectatePlayerID = -1;
		return;
	}

	g_iSpectatePlayerID = iPlayerID;
	g_iSpectateLock = 0;
	g_iSpectateEnabled = 1;
	GTAfunc_LockActor(1);
}

void spectateHandle()
{
	if (g_iSpectateEnabled)
	{
		if (g_iSpectateLock) return;

		if (g_iSpectatePlayerID != -1)
		{
			if (g_Players->iIsListed[g_iSpectatePlayerID] != 0)
			{
				if (g_Players->pRemotePlayer[g_iSpectatePlayerID] != NULL)
				{
					int iState = getPlayerState(g_iSpectatePlayerID);

					if (iState == PLAYER_STATE_ONFOOT)
					{
						struct actor_info *pPlayer = getGTAPedFromSAMPPlayerID(g_iSpectatePlayerID);
						if (pPlayer == NULL) return;
						GTAfunc_CameraOnActor(pPlayer);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_DRIVER)
					{
						struct vehicle_info *pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_PASSENGER)
					{
						struct vehicle_info *pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
				}
				else
				{
					cheat_state_text("Player is not streamed in");
					g_iSpectateEnabled = 0;
				}
			}
		}
	}
}

void sampMainCheat()
{
	traceLastFunc("sampMainCheat()");
	const int i = sizeof(stRemotePlayerData);
	// g_Vehicles & g_Players pointers need to be refreshed or nulled

	if (isBadPtr_writeAny(g_SAMP->pPools, sizeof(stSAMPPools)))
	{
		g_Vehicles = NULL;
		g_Players = NULL;
	}
	else if (g_Vehicles != g_SAMP->pPools->pVehicle || g_Players != g_SAMP->pPools->pPlayer)
	{
		if (isBadPtr_writeAny(g_SAMP->pPools->pVehicle, sizeof(stVehiclePool)))
			g_Vehicles = NULL;
		else
			g_Vehicles = g_SAMP->pPools->pVehicle;
		if (isBadPtr_writeAny(g_SAMP->pPools->pPlayer, sizeof(stPlayerPool)))
			g_Players = NULL;
		else
			g_Players = g_SAMP->pPools->pPlayer;
	}

	// update GTA to SAMP translation structures
	update_translateGTASAMP_vehiclePool();
	update_translateGTASAMP_pedPool();

	spectateHandle();

	static DWORD time = 0;
	if (GetTickCount() - time > 1000 &&
		g_SAMP->iGameState == GAMESTATE_CONNECTED)
	{
		uint32_t	samp_info = (uint32_t)g_SAMP;
		uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_UPDATESCOREBOARDDATA;
		__asm mov ecx, samp_info
		__asm call func
		time = GetTickCount();
	}

	/*if(g_bShowAd)
	{
		if (g_Dialog->iDialogID == 1488 && g_Dialog->iDialogShowed == 0)
		{
			memset_safe((void *)(g_dwSAMP_Addr + 0xB3960), 0xE9, 1);
			g_bShowAd = FALSE;
		}
	}*/

	// start chatbox logging
	if (set.chatbox_logging)
	{
		static int	chatbox_init;
		if (!chatbox_init)
		{
			SYSTEMTIME	time;
			GetLocalTime(&time);
			LogChatbox(false, "Session started at %02d/%02d/%02d", time.wDay, time.wMonth, time.wYear);
			chatbox_init = 1;
		}
	}

	if (cheat_state->_generic.shpora)
	{
		if (KEY_DOWN(0x21))
			set.shporaoffset += 10.0f;
		else if (KEY_DOWN(0x22))
			set.shporaoffset -= 10.0f;
	}

	if (cheat_state->_generic.carshot == 1)
	{
		struct actor_info *self = actor_info_get(-1, 0x01);
		struct vehicle_info *info = vehicle_info_get(-1, 0x01);
		if (info != NULL)
		{
			self->fCurrentRotation = -pGame->GetCamera()->GetCameraRotation();
			self->fTargetRotation = self->fCurrentRotation;
			info->base.matrix[4] = sinf(-self->fCurrentRotation);
			info->base.matrix[5] = cosf(-self->fCurrentRotation);
			if (KEY_DOWN('W'))
			{
				info->speed[0] = sinf(-self->fCurrentRotation) * set.carshot_speed / 100;
				info->speed[1] = cosf(-self->fCurrentRotation) * set.carshot_speed / 100;
			}
			if (KEY_DOWN('S'))
			{
				info->speed[0] = sinf(-self->fCurrentRotation) * -0.3f;
				info->speed[1] = cosf(-self->fCurrentRotation) * -0.3f;
			}
			if (KEY_DOWN(0x20))
				info->speed[2] += 0.05f;
			if (KEY_DOWN(0xA2))
				info->speed[2] -= 0.05f;
			if (KEY_DOWN(0xA0))
			{
				vect3_zero(info->speed);
				info->speed[2] = -0.01f;
			}
		}
	}

	if (KEY_RELEASED(VK_LBUTTON) && cheat_state->_generic.supergun)
	{
		if (pPedSelf->GetCurrentWeaponSlot() == 2)
			pPedSelf->SetCurrentWeaponSlot(eWeaponSlot(3));
		else
			pPedSelf->SetCurrentWeaponSlot(eWeaponSlot(2));
	}

	if (KEY_DOWN(0xA2) && KEY_PRESSED(0x01) && cheat_state->_generic.cargun)
	{
		float coord[3], speed[3];
		int VehicleID = vehicle_find_nearest(VEHICLE_EMPTY);
		if (VehicleID == NULL)
			return;
		struct actor_info *self = actor_info_get(-1, 0);
		struct vehicle_info *vinfo = vehicle_info_get(VehicleID, 0);
		vect3_copy(&self->base.matrix[12], coord);
		coord[0] += sinf(-self->fCurrentRotation) * 5.0f;
		coord[1] += cosf(-self->fCurrentRotation) * 5.0f;
		speed[0] = sinf(-self->fCurrentRotation) * 2;
		speed[1] = cosf(-self->fCurrentRotation) * 2;
		g_RakClient->SendFakeDriverSyncData(getSAMPVehicleIDFromGTAVehicle(vinfo), coord, vinfo->hitpoints, speed);
		cheat_vehicle_teleport(vinfo, coord, 0);
		vect3_copy(speed, vinfo->speed);
	}

	if (KEY_DOWN(set.secondary_key))
	{
		if (KEY_PRESSED(set.key_player_info_list))
			cheat_state->player_info_list ^= 1;

		if (KEY_PRESSED(VK_RBUTTON) && *(int *)0xBA6774 != 0)
		{
			float mapPos[3];
			for (int i = 0; i < (0xAF * 0x28); i += 0x28)
			{
				if (*(short *)(0xBA873D + i) == 4611)
				{
					float	*pos = (float *)(0xBA86F8 + 0x28 + i);
					gta_interior_id_set(0);
					mapPos[0] = *pos;
					mapPos[1] = *(pos + 1);
					mapPos[2] = pGameInterface->GetWorld()->FindGroundZForPosition(mapPos[0], mapPos[1]) + 2.0f;
					cheat_teleport(mapPos, 0);
				}
			}
		}

		if (KEY_PRESSED(set.key_rejoin))
		{
			if (cheat_state->_generic.rejoinTick == 0)
			{
				restartGame();
				disconnect(500);
				cheat_state_text("Переподключение | %d секунд...", set.rejoin_delay / 1000);
				cheat_state->_generic.rejoinTick = GetTickCount();
			}
			else
			{
				disconnect(500);
				g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
				cheat_state->_generic.rejoinTick = 0;
			}
		}

		if (KEY_PRESSED(set.key_dhide))
		{
			if (g_Dialog->iDialogShowed)
			{
				g_Dialog->iDialogShowed = 0;
				toggleSAMPCursor(0);
			}
			else
				g_Dialog->iDialogShowed = 1;
		}

		if (KEY_PRESSED(set.key_coord))
			cheat_state->_generic.coordmaster ^= 1;

		if (KEY_PRESSED(set.key_inviz))
		{
			static int stop = 1;
			cheat_state->_generic.invize ^= 1;
			if (cheat_state->_generic.invize == 1)
			{
				patch_set *pspp = &set.patch[18];
				patcher_install(pspp);
				g_Players->pLocalPlayer->iIsSpectating = 1;
				g_Players->pLocalPlayer->iInitiatedSpectating = 1;
				stop = 0;
			}
			else if (cheat_state->_generic.invize == 0 && stop == 0)
			{
				patch_set *pspp = &set.patch[18];
				patcher_remove(pspp);
				g_Players->pLocalPlayer->iIsSpectating = 0;
				g_Players->pLocalPlayer->iInitiatedSpectating = 0;
			}
		}

		if (KEY_PRESSED(set.key_respawn))
			playerSpawn();
	}

	if (KEY_DOWN(set.chat_secondary_key))
	{
		int			i, key, spam;
		const char	*msg;
		for (i = 0; i < INI_CHATMSGS_MAX; i++)
		{
			struct chat_msg *msg_item = &set.chat[i];
			if (msg_item->key == NULL)
				continue;
			if (msg_item->msg == NULL)
				continue;
			if (msg_item->key != key_being_pressed)
				continue;
			key = msg_item->key;
			msg = msg_item->msg;
			spam = msg_item->spam;
			if (spam)
			{
				if (msg)
				if (KEY_DOWN(key))
					say("%s", msg);
			}
			else
			{
				if (msg)
				if (KEY_PRESSED(key))
					say("%s", msg);
			}
		}
	}

	static int	iSAMPHooksInstalled;
	if (!iSAMPHooksInstalled)
	{
		installSAMPHooks();
		iSAMPHooksInstalled = 1;
	}

	if (cheat_state->_generic.rejoinTick && cheat_state->_generic.rejoinTick < (GetTickCount() - set.rejoin_delay))
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		cheat_state->_generic.rejoinTick = 0;
	}

	if (joining_server == 1)
	{
		restartGame();
		disconnect(500);
		cheat_state_text("Joining server in %d seconds...", set.rejoin_delay / 1000);
		cheat_state->_generic.join_serverTick = GetTickCount();
		joining_server = 2;
	}

	if (joining_server == 2
		&& cheat_state->_generic.join_serverTick
		&&	 cheat_state->_generic.join_serverTick < (GetTickCount() - set.rejoin_delay))
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		joining_server = 0;
		cheat_state->_generic.join_serverTick = 0;
	}
}

int getNthPlayerID(int n)
{
	if (g_Players == NULL)
		return -1;

	int thisplayer = 0;
	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		if (g_Players->sLocalPlayerID == i)
			continue;
		if (thisplayer < n)
		{
			thisplayer++;
			continue;
		}

		return i;
	}

	//shouldnt happen
	return -1;
}

int getPlayerCount(void)
{
	if (g_Players == NULL)
		return NULL;

	int iCount = 0;
	int i;

	for (i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		iCount++;
	}

	return iCount + 1;
}

int setLocalPlayerName(const char *name)
{
	if (g_Players == NULL || g_Players->pLocalPlayer == NULL)
		return 0;

	int strlen_name = strlen(name);
	if (strlen_name == 0 || strlen_name > SAMP_ALLOWED_PLAYER_NAME_LENGTH)
		return 0;

	traceLastFunc("setLocalPlayerName()");

	//strcpy(g_Players->szLocalPlayerName, name);
	//g_Players->iStrlen_LocalPlayerName = strlen_name;

	DWORD	vtbl_nameHandler = ((DWORD)&g_Players->pVTBL_txtHandler);
	DWORD	func = g_dwSAMP_Addr + SAMP_FUNC_NAMECHANGE;
	__asm push strlen_name
	__asm push name
	__asm mov ecx, vtbl_nameHandler
	__asm call func
	return 1;
}

int getVehicleCount(void)
{
	if (g_Vehicles == NULL)
		return NULL;

	int iCount = 0;
	int i;

	for (i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		iCount++;
	}

	return iCount;
}

int getPlayerPos(int iPlayerID, float fPos[3])
{
	traceLastFunc("getPlayerPos()");

	struct actor_info	*pActor = NULL;
	struct vehicle_info *pVehicle = NULL;

	struct actor_info	*pSelfActor = actor_info_get(ACTOR_SELF, 0);

	if (g_Players == NULL)
		return 0;
	if (g_Players->iIsListed[iPlayerID] != 1)
		return 0;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return 0;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return 0;

	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor == NULL)
		return 0;	// not streamed
	else
	{
		pActor = g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped;

		if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Vehicle != NULL)
			pVehicle = g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;

		if (pVehicle != NULL && pActor->vehicle == pVehicle && pVehicle->passengers[0] == pActor)
		{
			// driver of a vehicle
			vect3_copy(&pActor->vehicle->base.matrix[4 * 3], fPos);

			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fVehiclePosition, fPos);
		}
		else if (pVehicle != NULL)
		{
			// passenger of a vehicle
			vect3_copy(&pActor->base.matrix[4 * 3], fPos);

			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fActorPosition, fPos);
		}
		else
		{
			// on foot
			vect3_copy(&pActor->base.matrix[4 * 3], fPos);

			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fActorPosition, fPos);
		}
	}

	if (pSelfActor != NULL)
	{
		if (vect3_dist(&pSelfActor->base.matrix[4 * 3], fPos) < 100.0f)
			vect3_copy(&pActor->base.matrix[4 * 3], fPos);
	}

	// detect zombies
	if (vect3_near_zero(fPos))
		vect3_copy(&pActor->base.matrix[4 * 3], fPos);

	return !vect3_near_zero(fPos);
}

const char *getPlayerName(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;

	if (iPlayerID == g_Players->sLocalPlayerID)
	{
		if (g_Players->iLocalPlayerNameLen <= 0xF)
			return g_Players->szLocalPlayerName;
		return g_Players->pszLocalPlayerName;
	}

	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;

	if (g_Players->pRemotePlayer[iPlayerID]->iNameLen <= 0xF)
		return g_Players->pRemotePlayer[iPlayerID]->szPlayerName;

	return g_Players->pRemotePlayer[iPlayerID]->pszPlayerName;
}

int getPlayerState(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;
	if (iPlayerID == g_Players->sLocalPlayerID)
		return NULL;
	if (g_Players->iIsListed[iPlayerID] != 1)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return NULL;

	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->bytePlayerState;
}

int getPlayerVehicleGTAScriptingID(int iPlayerID)
{
	if (g_Players == NULL)
		return 0;

	// fix to always return our own vehicle always if that's what's being asked for
	if (iPlayerID == ACTOR_SELF)
	{
		if (g_Players->pLocalPlayer->sCurrentVehicleID == (uint16_t)-1) return 0;

		stSAMPVehicle	*sampveh = g_Vehicles->pSAMP_Vehicle[g_Players->pLocalPlayer->sCurrentVehicleID];
		if (sampveh)
		{
			return ScriptCarId(sampveh->pGTA_Vehicle);
			//return (int)( ((DWORD) sampveh->pGTA_Vehicle) - (DWORD) pool_vehicle->start ) / 2584;
		}
		else
			return 0;
	}

	// make sure remote player is legit
	if (g_Players->pRemotePlayer[iPlayerID] == NULL || g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL ||
		g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Vehicle == NULL ||
		g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle == NULL)
		return 0;

	// make sure samp knows the vehicle exists
	if (g_Vehicles->pSAMP_Vehicle[g_Players->pRemotePlayer[iPlayerID]->pPlayerData->sVehicleID] == NULL)
		return 0;

	// return the remote player's vehicle
	return ScriptCarId(g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle);
}

int getPlayerSAMPVehicleID(int iPlayerID)
{
	if (g_Players == NULL && g_Vehicles == NULL) return 0;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL) return 0;
	if (g_Vehicles->pSAMP_Vehicle[g_Players->pRemotePlayer[iPlayerID]->pPlayerData->sVehicleID] == NULL) return 0;
	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->sVehicleID;
}

struct actor_info *getGTAPedFromSAMPPlayerID(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;
	if (iPlayerID == g_Players->sLocalPlayerID)
		return actor_info_get(ACTOR_SELF, 0);
	if (g_Players->iIsListed[iPlayerID] != 1)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped == NULL)
		return NULL;

	// return actor_info, null or otherwise
	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped;
}

struct vehicle_info *getGTAVehicleFromSAMPVehicleID(int iVehicleID)
{
	if (g_Vehicles == NULL || iVehicleID < 0 || iVehicleID >= SAMP_MAX_VEHICLES)
		return NULL;
	if (iVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		return vehicle_info_get(VEHICLE_SELF, 0);
	if (g_Vehicles->iIsListed[iVehicleID] != 1)
		return NULL;

	// return vehicle_info, null or otherwise
	return g_Vehicles->pGTA_Vehicle[iVehicleID];
}

int getSAMPPlayerIDFromGTAPed(struct actor_info *pGTAPed)
{
	if (g_Players == NULL)
		return 0;
	if (actor_info_get(ACTOR_SELF, 0) == pGTAPed)
		return g_Players->sLocalPlayerID;

	int i;
	for (i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		if (g_Players->pRemotePlayer[i] == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped == pGTAPed)
			return i;
	}

	return ACTOR_SELF;
}

int getSAMPVehicleIDFromGTAVehicle(struct vehicle_info *pVehicle)
{
	if (g_Vehicles == NULL)
		return NULL;
	if (vehicle_info_get(VEHICLE_SELF, 0) == pVehicle && g_Players != NULL)
		return g_Players->pLocalPlayer->sCurrentVehicleID;

	int i, iReturn = 0;
	for (i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		if (g_Vehicles->pGTA_Vehicle[i] == pVehicle)
			return i;
	}

	return VEHICLE_SELF;
}

uint32_t getPedGTAScriptingIDFromPlayerID(int iPlayerID)
{
	if (g_Players == NULL)
		return NULL;

	if (g_Players->iIsListed[iPlayerID] != 1)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor == NULL)
		return NULL;

	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle;
}

uint32_t getVehicleGTAScriptingIDFromVehicleID(int iVehicleID)
{
	if (g_Vehicles == NULL)
		return NULL;

	if (g_Vehicles->iIsListed[iVehicleID] != 1)
		return NULL;
	if (g_Vehicles->pSAMP_Vehicle[iVehicleID] == NULL)
		return NULL;

	return g_Vehicles->pSAMP_Vehicle[iVehicleID]->ulGTAEntityHandle;
}

struct m0dClientCMD
{
#pragma pack( 1 )
	char	cmd_name[30];

	//char cmd_description[128];
} m0d_cmd_list[(SAMP_MAX_CLIENTCMDS - 22)];
int m0d_cmd_num = 0;

void cmd_showCMDS()
{
	int i = 0;
	for (; i < m0d_cmd_num; i++)
	{
		addMessageToChatWindow("%s", m0d_cmd_list[i].cmd_name);
	}
}

void addClientCommand(char *name, int function)
{
	if (name == NULL || function == NULL || g_Input == NULL)
		return;

	if (g_Input->iCMDCount == (SAMP_MAX_CLIENTCMDS - 1))
	{
		Log("Error: couldn't initialize '%s'. Maximum command amount reached.", name);
		return;
	}

	if (strlen(name) > 30)
	{
		Log("Error: command name '%s' was too long.", name);
		return;
	}

	if (m0d_cmd_num < (SAMP_MAX_CLIENTCMDS - 22))
	{
		strncpy_s(m0d_cmd_list[m0d_cmd_num].cmd_name, name, sizeof(m0d_cmd_list[m0d_cmd_num].cmd_name) - 1);
		m0d_cmd_num++;
	}
	else
		Log("m0d_cmd_list[] too short.");

	uint32_t	data = g_dwSAMP_Addr + SAMP_CHAT_INPUT_INFO_OFFSET;
	uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_ADDCLIENTCMD;
	__asm mov eax, data
	__asm mov ecx, [eax]
		__asm push function
	__asm push name
	__asm call func
}

bool	modcommands = false;
bool get_isModCommandsActive()
{
	return modcommands;
}

void init_samp_chat_cmds()
{
	if (modcommands == true)
	{
		return;
	}
	else
	{
		cheat_state_text("initiated modcommands");
		modcommands = true;
	}
	addClientCommand("m0d_change_server", (int)cmd_change_server);
	addClientCommand("m0d_fav_server", (int)cmd_change_server_fav);
	addClientCommand("m0d_current_server", (int)cmd_current_server);
	addClientCommand("m0d_tele_loc", (int)cmd_tele_loc);
	addClientCommand("m0d_teleport_location", (int)cmd_tele_loc);
	addClientCommand("m0d_tele_locations", (int)cmd_tele_locations);
	addClientCommand("m0d_teleport_locations", (int)cmd_tele_locations);
	addClientCommand("showcmds", (int)cmd_showCMDS);
	addClientCommand("sendpic", (int)cmd_pickup);
	addClientCommand("setclass", (int)cmd_setclass);
	addClientCommand("setint", (int)cmd_setint);
	addClientCommand("fakekill", (int)cmd_fakekill);
	addClientCommand("warp", (int)cmd_warp);
	addClientCommand("nick", (int)cmd_nick);
	addClientCommand("scar", (int)cmd_scar);
	addClientCommand("shp", (int)cmd_sethp);
	addClientCommand("ghp", (int)cmd_gethp);
	addClientCommand("fhp", (int)cmd_fakehp);
	addClientCommand("vhp", (int)cmd_setvhp);
	addClientCommand("hdialog", (int)cmd_dhide);
	addClientCommand("vcolor", (int)cmd_vcolor);
	addClientCommand("flood", (int)cmd_flood);
	addClientCommand("floodtime", (int)cmd_floodtime);
	addClientCommand("findtext", (int)cmd_findtext);
	addClientCommand("paintjob", (int)cmd_paintjob);
	addClientCommand("fpick", (int)cmd_gotopickup);
	addClientCommand("gcheck", (int)cmd_getcheck);
	addClientCommand("fcheck", (int)cmd_fakecheck);
	addClientCommand("stazer", (int)cmd_ftazer);
	addClientCommand("fcuff", (int)cmd_fcuff);
	addClientCommand("fpay", (int)cmd_fpay);
	addClientCommand("shot", (int)cmd_shot);
	addClientCommand("addtune", (int)cmd_addtune);
	addClientCommand("ghere", (int)cmd_ghere);
	addClientCommand("spectate", (int)cmd_re);
	addClientCommand("vehadd", (int)cmd_createcar);
	addClientCommand("ccw", (int)cmd_carwarp);
	addClientCommand("ucw", (int)cmd_ucarwarp);
	addClientCommand("boom", (int)cmd_boom);
	addClientCommand("tun", (int)cmd_tunload);
	addClientCommand("tid", (int)cmd_tsave);
	addClientCommand("recontime", (int)cmd_rctime);
	addClientCommand("parse", (int)cmd_parse);
	addClientCommand("vkick", (int)cmd_vkick);
	addClientCommand("urc", (int)cmd_urc);
	addClientCommand("crc", (int)cmd_crc);
	addClientCommand("coordcw", (int)cmd_coordcw);
	addClientCommand("takecar", (int)cmd_takecar);
	addClientCommand("fake", (int)cmd_fake);
	addClientCommand("findveh", (int)cmd_findveh);
	addClientCommand("fire", (int)cmd_fire);
	addClientCommand("autorej", (int)cmd_autorej);
	addClientCommand("stcar", (int)cmd_stealcar);
	//addClientCommand("fjoin", (int)cmd_fakejoin);
	//addClientCommand("frejoin", (int)cmd_fakerejoin);
	addClientCommand("sdialog", (int)cmd_sdialog);
	addClientCommand("menusel", (int)cmd_menusel);
	addClientCommand("addbot", (int)cmd_addbot);
	addClientCommand("sskin", (int)cmd_sskin);
}

void AddRecallBufer(char* text)
{
	uint32_t func = g_dwSAMP_Addr + SAMP_FUNC_ADDRECALL;
	_asm
	{
		mov ecx, g_Input;
		push text
			call func;
	}
}

struct gui	*gui_samp_cheat_state_text = &set.guiset[1];
void addMessageToChatWindow(const char *text, ...)
{
	if (g_SAMP != NULL)
	{
		va_list ap;
		if (text == NULL)
			return;

		char	tmp[512];
		memset(tmp, 0, 512);

		va_start(ap, text);
		vsnprintf(tmp, sizeof(tmp)-1, text, ap);
		va_end(ap);

		addToChatWindow(tmp, D3DCOLOR_XRGB(gui_samp_cheat_state_text->red, gui_samp_cheat_state_text->green,
			gui_samp_cheat_state_text->blue));
	}
	else
	{
		va_list ap;
		if (text == NULL)
			return;

		char	tmp[512];
		memset(tmp, 0, 512);

		va_start(ap, text);
		vsnprintf(tmp, sizeof(tmp)-1, text, ap);
		va_end(ap);

		cheat_state_text(tmp, D3DCOLOR_ARGB(255, 0, 200, 200));
	}
}

void addMessageToChatWindowSS(const char *text, ...)
{
	if (g_SAMP != NULL)
	{
		va_list ap;
		if (text == NULL)
			return;

		char	tmp[512];
		memset(tmp, 0, 512);

		va_start(ap, text);
		vsprintf(tmp, text, ap);
		va_end(ap);

		addMessageToChatWindow(tmp, D3DCOLOR_ARGB(255, 0, 200, 200));
	}
	else
	{
		va_list ap;
		if (text == NULL)
			return;

		char	tmp[512];
		memset(tmp, 0, 512);

		va_start(ap, text);
		vsprintf(tmp, text, ap);
		va_end(ap);

		cheat_state_text(tmp, D3DCOLOR_ARGB(255, 0, 200, 200));
	}
}

void addToChatWindow(char *text, D3DCOLOR textColor, int playerID)
{
	if (g_SAMP == NULL || g_Chat == NULL)
		return;

	if (text == NULL)
		return;

	if (playerID < -1)
		playerID = -1;

	uint32_t	chatinfo = g_dwSAMP_Addr + SAMP_CHAT_INFO_OFFSET;
	uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_ADDTOCHATWND;

	if (playerID != -1)
	{
		// getPlayerName does the needed validity checks, no need for doubles
		char *playerName = (char*)getPlayerName(playerID);
		if (playerName == NULL)
			return;

		D3DCOLOR playerColor = samp_color_get(playerID);

		__asm mov eax, dword ptr[chatinfo]
			__asm mov ecx, dword ptr[eax]
			__asm push playerColor
		__asm push textColor
		__asm push playerName
		__asm push text
		__asm push 2
		__asm call func
		return;
	}

	__asm mov eax, dword ptr[chatinfo]
		__asm mov ecx, dword ptr[eax]
		__asm push 0
	__asm push textColor
	__asm push 0
	__asm push text
	__asm push 8
	__asm call func
	return;
}

void restartGame()
{
	if (g_SAMP == NULL)
		return;

	uint32_t	samp_info = g_dwSAMP_Addr + SAMP_INFO_OFFSET;
	uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_RESTARTGAME;
	__asm mov eax, dword ptr[samp_info]
		__asm mov ecx, dword ptr[eax]
		__asm call func
	__asm pop eax
	__asm pop ecx
}

void say(char *text, ...)
{
	if (g_SAMP == NULL)
		return;

	if (text == NULL)
		return;
	if (isBadPtr_readAny(text, 128))
		return;
	traceLastFunc("say()");

	va_list ap;
	char	tmp[128];
	memset(tmp, 0, 128);

	va_start(ap, text);
	vsprintf(tmp, text, ap);
	va_end(ap);

	addSayToChatWindow(tmp);
}

void addSayToChatWindow(char *msg)
{
	if (g_SAMP == NULL)
		return;

	if (msg == NULL)
		return;
	if (isBadPtr_readAny(msg, 128))
		return;
	traceLastFunc("addSayToChatWindow()");

	if (msg[0] == '/')
	{
		uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_SENDCMD;
		__asm push msg
		__asm call func
	}
	else
	{
		uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_SAY;
		void		*lpPtr = g_Players->pLocalPlayer;
		__asm mov ebx, dword ptr[lpPtr]
			__asm push msg
		__asm call func
		__asm pop ebx
	}
}

void showSampDialog(int send, int dialogID, int typedialog, char *caption, char *text, char *button1, char *button2)
{
	uint32_t func = g_dwSAMP_Addr + SAMP_FUNC_DIALOG_SHOW;
	uint32_t data = g_dwSAMP_Addr + SAMP_DIALOG_INFO_OFFSET;

	__asm mov eax, dword ptr[data]
		__asm mov ecx, dword ptr[eax]
		__asm push send
	__asm push button2
	__asm push button1
	__asm push text
	__asm push caption
	__asm push typedialog
	__asm push dialogID
	__asm call func
	return;
}

void showGameText(char *text, int time, int textsize)
{
	if (g_SAMP == NULL)
		return;

	uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_GAMETEXT;
	__asm push textsize
	__asm push time
	__asm push text
	__asm call func
}

void playerSpawn(void)
{
	if (g_SAMP == NULL)
		return;

	uint32_t	func_request = g_dwSAMP_Addr + SAMP_FUNC_REQUEST_SPAWN;
	uint32_t	func_spawn = g_dwSAMP_Addr + SAMP_FUNC_SPAWN;
	void		*lpPtr = g_Players->pLocalPlayer;

	__asm mov ecx, dword ptr[lpPtr]
		__asm push ecx
	__asm call func_request
	__asm pop ecx

	__asm mov ecx, dword ptr[lpPtr]
		__asm push ecx
	__asm call func_spawn
	__asm pop ecx
}

void disconnect(int reason)
{
	if (g_SAMP == NULL)
		return;

	g_RakClient->GetRakClientInterface()->Disconnect(reason);
}

void setPassword(char *password)
{
	if (g_SAMP == NULL)
		return;

	g_RakClient->GetRakClientInterface()->SetPassword(password);
}

void sendSetInterior(uint8_t interiorID)
{
	if (g_SAMP == NULL)
		return;

	uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_SENDINTERIOR;
	void		*lpPtr = g_Players->pLocalPlayer;
	__asm mov ecx, dword ptr[interiorID]
		__asm push ecx
	__asm mov ecx, dword ptr[lpPtr]
		__asm call func
	__asm pop ecx
}

void setSpecialAction(uint8_t byteSpecialAction)
{
	if (g_SAMP == NULL)
		return;

	if (g_Players->pLocalPlayer == NULL)
		return;

	g_Players->pLocalPlayer->onFootData.byteSpecialAction = byteSpecialAction;

	uint32_t	func = g_dwSAMP_Addr + SAMP_FUNC_SETSPECIALACTION;
	void		*lpPtr = g_Players->pLocalPlayer;
	__asm mov ecx, dword ptr[byteSpecialAction]
		__asm push ecx
	__asm mov ecx, dword ptr[lpPtr]
		__asm call func
	__asm pop ecx
}

void sendSCMEvent(int iEvent, int iVehicleID, int iParam1, int iParam2)
{
	g_RakClient->SendSCMEvent(iVehicleID, iEvent, iParam1, iParam2);
}

/*
// this doesn't work when wrapped around the toggle below, samp sux
CMatrix toggleSAMPCursor_Camera = CMatrix();
void _cdecl toggleSAMPCursor_SaveCamera ( void )
{
pGame->GetCamera()->GetMatrix(&toggleSAMPCursor_Camera);
}

void _cdecl toggleSAMPCursor_RestoreCamera ( void )
{
pGame->GetCamera()->SetMatrix(&toggleSAMPCursor_Camera);
}
*/

void toggleSAMPCursor(int iToggle)
{
	if (g_Input->iInputEnabled)
		return;

	uint32_t func = g_dwSAMP_Addr + SAMP_FUNC_TOGGLECURSOR;
	uint32_t obj = g_dwSAMP_Addr + SAMP_MISC_INFO;

	if (iToggle)
	{
		_asm
		{
			//call toggleSAMPCursor_SaveCamera;
			mov ecx, obj;
			push 0;
			push 0;
			push 2;
			call func;
			//call toggleSAMPCursor_RestoreCamera;
		}
		g_iCursorEnabled = 1;
	}
	else
	{
		_asm
		{
			mov ecx, obj;
			push 0;
			push 1;
			push 0;
			call func;
		}
		uint32_t funcunlock = g_dwSAMP_Addr + SAMP_FUNC_CURSORUNLOCKACTORCAM;
		_asm
		{
			mov ecx, obj;
			call funcunlock;
		}
		g_iCursorEnabled = 0;
	}
}

int		g_iNumPlayersMuted = 0;
bool	g_bPlayerMuted[SAMP_MAX_PLAYERS];
uint8_t _declspec (naked) server_message_hook(void)
{
	int		thismsg;
	DWORD	thiscolor;

	__asm mov thismsg, esi
	__asm mov thiscolor, eax
	thiscolor = (thiscolor >> 8) | 0xFF000000;

	static char		last_servermsg[256];
	static DWORD	allow_show_again;

	if (!set.anti_spam || cheat_state->_generic.cheat_panic_enabled
		|| (strcmp(last_servermsg, (char *)thismsg) != NULL || GetTickCount() > allow_show_again))
	{
		// might be a personal message by muted player - look for name in server message
		// ignore message, if name was found
		if (set.anti_spam && g_iNumPlayersMuted > 0)
		{
			int i, j;
			char *playerName = NULL;
			for (i = 0, j = 0; i < SAMP_MAX_PLAYERS && j < g_iNumPlayersMuted; i++)
			{
				if (g_bPlayerMuted[i])
				{
					playerName = (char*)getPlayerName(i);

					if (playerName == NULL)
					{
						// Player not connected anymore - remove player from muted list
						g_bPlayerMuted[i] = false;
						g_iNumPlayersMuted--;
						continue;
					}
					else if (strstr((char*)thismsg, playerName) != NULL)
						goto ignoreThisServChatMsg;
					j++;
				}
			}
		}
		strncpy_s(last_servermsg, (char *)thismsg, sizeof(last_servermsg)-1);

		allow_show_again = GetTickCount() + 5000;

		if (cheat_state->_generic.msgcolor)
		{
			char color[16];
			sprintf(color, " // 0x%08X", thiscolor);
			strcat((char *)last_servermsg, color);
		}

		addToChatWindow((char *)last_servermsg, thiscolor);
		if (set.chatbox_logging)
			LogChatbox(false, "%s", thismsg);
	}

ignoreThisServChatMsg:
	__asm mov ebx, g_dwSAMP_Addr
	__asm add ebx, HOOK_EXIT_SERVERMESSAGE_HOOK
	__asm jmp ebx
}

uint8_t _declspec (naked) client_message_hook(void)
{
	static char last_clientmsg[SAMP_MAX_PLAYERS][256];
	int			thismsg;
	uint16_t	id;

	__asm mov id, dx
	__asm lea edx, [esp + 0x128]
		__asm mov thismsg, edx

	if (id >= 0 && id <= SAMP_MAX_PLAYERS)
	{
		if (id == g_Players->sLocalPlayerID)
		{
			addToChatWindow((char*)thismsg, g_Chat->clTextColor, id);

			if (set.chatbox_logging)
				LogChatbox(false, "%s: %s", getPlayerName(id), thismsg);
			goto client_message_hook_jump_out;
		}

		static DWORD	allow_show_again = GetTickCount();
		if (!set.anti_spam
			|| (strcmp(last_clientmsg[id], (char *)thismsg) != NULL || GetTickCount() > allow_show_again)
			|| cheat_state->_generic.cheat_panic_enabled)
		{
			// ignore chat from muted players
			if (set.anti_spam && g_iNumPlayersMuted > 0 && g_bPlayerMuted[id])
				goto client_message_hook_jump_out;

			// nothing to copy anymore, after chatbox_logging, so copy this before
			strncpy_s(last_clientmsg[id], (char *)thismsg, sizeof(last_clientmsg[id]) - 1);

			if (set.chatbox_logging)
				LogChatbox(false, "%s: %s", getPlayerName(id), thismsg);

			addToChatWindow((char*)thismsg, g_Chat->clTextColor, id);
			allow_show_again = GetTickCount() + 5000;
		}
	}

client_message_hook_jump_out:;
	__asm mov ebx, g_dwSAMP_Addr
	__asm add ebx, HOOK_EXIT_CLIENTMESSAGE_HOOK
	__asm jmp ebx
}

DWORD dwStreamedOutInfoOrigFunc;
float fStreamedOutInfoPosX, fStreamedOutInfoPosY, fStreamedOutInfoPosZ;
uint16_t wStreamedOutInfoPlayerID;
uint8_t _declspec (naked) StreamedOutInfo(void)
{
	_asm
	{
		push eax
			mov eax, dword ptr[esp + 12]
			mov fStreamedOutInfoPosX, eax
			mov eax, dword ptr[esp + 16]
			mov fStreamedOutInfoPosY, eax
			mov eax, dword ptr[esp + 20]
			mov fStreamedOutInfoPosZ, eax
			mov ax, word ptr[esp + 24]
			mov wStreamedOutInfoPlayerID, ax
			pop eax
	}

	_asm pushad
	g_stStreamedOutInfo.iPlayerID[wStreamedOutInfoPlayerID] = (int)wStreamedOutInfoPlayerID;
	g_stStreamedOutInfo.fPlayerPos[wStreamedOutInfoPlayerID][0] = fStreamedOutInfoPosX;
	g_stStreamedOutInfo.fPlayerPos[wStreamedOutInfoPlayerID][1] = fStreamedOutInfoPosY;
	g_stStreamedOutInfo.fPlayerPos[wStreamedOutInfoPlayerID][2] = fStreamedOutInfoPosZ;
	_asm popad

	_asm
	{
		push eax
			mov eax, g_dwSAMP_Addr
			add eax, HOOK_CALL_STREAMEDOUTINFO
			mov dwStreamedOutInfoOrigFunc, eax
			pop eax

			jmp dwStreamedOutInfoOrigFunc
	}
}

void HandleRPCPacketFunc(unsigned char byteRPCID, RPCParameters *rpcParams, void(*functionPointer) (RPCParameters *))
{
	if (set.netPatchAssoc[byteRPCID][INCOMING_RPC] != nullptr && set.netPatchAssoc[byteRPCID][INCOMING_RPC]->enabled)
		return;

	/*if (rpcParams->input != NULL)
	{
		int numberOfBytesData = rpcParams->numberOfBitsOfData / 8;
		Log("> [RPC Recv] id: %d len: %d\n%s\n", byteRPCID, numberOfBytesData, DumpMem(rpcParams->input, numberOfBytesData));
	}*/

	if (byteRPCID == RPC_ScrServerJoin)
	{
		uint16_t playerId;
		BYTE byteNameLen;
		char szPlayerName[256];

		RakNet::BitStream bsData((unsigned char*)rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

		bsData.Read(playerId);
		int iUnk = 0;
		bsData.Read(iUnk);
		BYTE bUnk = 0;
		bsData.Read(bUnk);
		bsData.Read(byteNameLen);
		if (byteNameLen > 20) return;
		bsData.Read(szPlayerName, byteNameLen);
		szPlayerName[byteNameLen] = '\0';

		if (playerId < 0 || playerId > SAMP_MAX_PLAYERS) return;

		if (set.show_connecteds && !gta_menu_active())
		{
			sprintf(cheat_state->context, "Подключился %s[%d]", szPlayerName, playerId);
			cheat_state->context_time = time_get();
		}

		if (set.adminsound)
		{
			for (int x = 0; x < 300; x++)
			{
				if (szPlayerName == NULL || set.admin[x] == NULL)
					break;

				if (!strcmp(szPlayerName, set.admin[x]))
				{
					char buf[256];
					snprintf(buf, sizeof(buf), "Внимание! Админ в сети: %s", szPlayerName);
					addToChatWindow(buf, 0xFFFF0000);
					MessageBeep(MB_ICONEXCLAMATION);
				}
			}
		}
	}

	if (byteRPCID == RPC_ScrChatBubble)
	{
		RakNet::BitStream bsData((unsigned char*)rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

		uint16_t playerId;
		BYTE len;
		DWORD color, time;
		float dist;
		char msg[144];

		bsData.Read(playerId);
		bsData.Read(color);
		bsData.Read(dist);
		bsData.Read(time);
		bsData.Read(len);
		bsData.Read(msg, len);
		msg[len] = '\0';
		if (time > 1500 && cheat_state->_generic.farchat)
			addMessageToChatWindow(" {FF0000}%s[%d]: {FFFFFF}%s", getPlayerName(playerId), playerId, msg);
	}

	if (byteRPCID == RPC_ScrApplyAnimation)
	{
		RakNet::BitStream bsData((unsigned char*)rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

		BYTE len;
		char name[16];

		bsData.IgnoreBits(16);
		bsData.Read(len);
		bsData.Read(name, len);

		if (len == 5 && !strncmp(name, "CRACK", len) && cheat_state->_generic.antilomka) return;
	}

	if (byteRPCID == RPC_ScrClientMessage)
	{
		DWORD color, strLen;
		char msg[257];

		RakNet::BitStream bsData((unsigned char*)rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
		bsData.Read(color);
		bsData.Read(strLen);
		bsData.Read(msg, strLen);

		if(cheat_state->_generic.findtext)
		{
			if (strstr(msg, set.find_text))
				showSampDialog(0, 1231, 0, "Поиск текста", "Текст найден!", "ОК", "");
		}

		if (cheat_state->_generic.capt)
		{
			if (strstr(msg, "Сейчас времени:") && strstr(msg, "часов"))
				say("/capture");
		}

		if (cheat_state->_generic.buynarko)
		{
			if (strstr(msg, "\"/accept drugs\" чтобы купить"))
				say("/accept drugs");
		}

		if (cheat_state->_generic.antislap)
		{
			for (int i = 0; i < 1000; i++)
			{
				char slapmsg[128];
				sprintf(slapmsg, "Админ: %s дал поджопник %s", getPlayerName(i), getPlayerName(g_Players->sLocalPlayerID));
				if (strstr(msg, slapmsg))
				{
					actor_info_get(-1, ACTOR_ALIVE)->hitpoints -= 10.0f;
					break;
				}
				else continue;
			}
		}
	}

	if (byteRPCID == RPC_ScrSetPlayerArmedWeapon)
	{
		if (cheat_state->_generic.skill)
			return;
	}

	if (byteRPCID == RPC_ScrDisplayGameText)
	{
		RakNet::BitStream bsData((unsigned char *)rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
		char szMessage[400];
		int iType, iTime, iLength;

		bsData.Read(iType);
		bsData.Read(iTime);
		bsData.Read(iLength);

		if (iLength > 400) return;

		bsData.Read(szMessage, iLength);
		szMessage[iLength] = '\0';
	}

	if (byteRPCID == RPC_ScrInitGame)
	{
		RakNet::BitStream bsInitGame((unsigned char *)rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

		uint16_t MyPlayerID;
		BYTE byteVehicleModels[212], m_byteWorldTime, m_byteWeather;
		bool m_bZoneNames, m_bUseCJWalk, m_bAllowWeapons, m_bLimitGlobalChatRadius, bLanMode, bStuntBonus;
		bool m_bDisableEnterExits, m_bNameTagLOS, m_bTirePopping, m_bShowPlayerTags, m_bInstagib;
		int m_iSpawnsAvailable, m_iShowPlayerMarkers, m_iDeathDropMoney;
		float m_fGravity, m_fGlobalChatRadius, m_fNameTagDrawDistance;
		char g_szHostName[258];
		uint16_t g_myPlayerID;

		bsInitGame.ReadCompressed(m_bZoneNames);
		bsInitGame.ReadCompressed(m_bUseCJWalk);
		bsInitGame.ReadCompressed(m_bAllowWeapons);
		bsInitGame.ReadCompressed(m_bLimitGlobalChatRadius);
		bsInitGame.Read(m_fGlobalChatRadius);
		bsInitGame.ReadCompressed(bStuntBonus);
		bsInitGame.Read(m_fNameTagDrawDistance);
		bsInitGame.ReadCompressed(m_bDisableEnterExits);
		bsInitGame.ReadCompressed(m_bNameTagLOS);
		bsInitGame.ReadCompressed(m_bTirePopping);
		bsInitGame.Read(m_iSpawnsAvailable);
		bsInitGame.Read(MyPlayerID);
		bsInitGame.ReadCompressed(m_bShowPlayerTags);
		bsInitGame.Read(m_iShowPlayerMarkers);
		bsInitGame.Read(m_byteWorldTime);
		bsInitGame.Read(m_byteWeather);
		bsInitGame.Read(m_fGravity);
		bsInitGame.ReadCompressed(bLanMode);
		bsInitGame.Read(m_iDeathDropMoney);
		bsInitGame.ReadCompressed(m_bInstagib);

		bsInitGame.Read(iNetModeNormalOnfootSendRate);
		bsInitGame.Read(iNetModeNormalIncarSendRate);
		bsInitGame.Read(iNetModeFiringSendRate);
		bsInitGame.Read(iNetModeSendMultiplier);

		bsInitGame.Read(iLagCompMode);

		BYTE byteStrLen;
		bsInitGame.Read(byteStrLen);
		if (byteStrLen)
		{
			memset(g_szHostName, 0, sizeof(g_szHostName));
			bsInitGame.Read(g_szHostName, byteStrLen);
		}
		g_szHostName[byteStrLen] = '\0';

		bsInitGame.Read((char *)&byteVehicleModels[0], 212);

		g_myPlayerID = MyPlayerID;
		
	}

	if (byteRPCID == RPC_ScrShowDialog)
	{
		RakNet::BitStream bsData((unsigned char*)rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
		stSAMPDialog sampDialog;

		bsData.Read(sampDialog.wDialogID);
		bsData.Read(sampDialog.bDialogStyle);

		if (sampDialog.wDialogID != 65535)
			Log("Dialog ID: %d Style: %d", sampDialog.wDialogID, sampDialog.bDialogStyle);
		else return;

		if (cheat_state->_generic.fakerejoin)
		{
			if (sampDialog.bDialogStyle == 1 || sampDialog.bDialogStyle == 3)
			{
				g_RakClient->SendDialogResponse(sampDialog.wDialogID, 1, 0, set.fakerejoinpass);
				cheat_state->_generic.fakerejoin = false;
			}
			else 
				g_RakClient->SendDialogResponse(sampDialog.wDialogID, 1, 0, "");
			return;
		}

		if (cheat_state->_generic.autorejoin_on)
		{
			g_RakClient->SendDialogResponse(sampDialog.wDialogID, 1, 0, set.autorejoin_pass);
			return;
		}

		/*bsData.Read(sampDialog.bTitleLength);
		bsData.Read(sampDialog.szTitle, sampDialog.bTitleLength);
		sampDialog.szTitle[sampDialog.bTitleLength] = 0;

		bsData.Read(sampDialog.bButton1Len);
		bsData.Read(sampDialog.szButton1, sampDialog.bButton1Len);
		sampDialog.szButton1[sampDialog.bButton1Len] = 0;

		bsData.Read(sampDialog.bButton2Len);
		bsData.Read(sampDialog.szButton2, sampDialog.bButton2Len);

		stringCompressor->DecodeString(sampDialog.szInfo, 4097, &bsData);*/
	}

	if (byteRPCID == RPC_ScrSetPlayerHealth)
	{
		RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
		float fHealth;
		bsData.Read(fHealth);
		cheat_state_text("Сервер установил %0.1f здоровья", fHealth);
		if (cheat_state->_generic.newgodmode) return;

		if (set.enable_extra_godmode && cheat_state->_generic.hp_cheat)
		{
			actor_info *self = actor_info_get(ACTOR_SELF, NULL);
			if (self)
			{
				if (fHealth < self->hitpoints)
				{
					cheat_state_text("Warning: Server tried change your health to %0.1f", fHealth);
					return;
				}
			}
		}

		if (set.sat)
		{
			if (byteRPCID == RPC_ScrSetPlayerHealth)
			{
				actor_info *self = actor_info_get(ACTOR_SELF, NULL);
				if (self)
				{
					if (self->hitpoints - fHealth <= 4.0f && fHealth < self->hitpoints  && set.spawnsethp) return;
					else if (!set.spawnsethp)
					{
						self->hitpoints = fHealth;
						set.spawnsethp = 1;
					}
				}
			}
		}
	}

	if (byteRPCID == RPC_ScrSetVehicleHealth)
	{
		vehicle_info *vself = vehicle_info_get(VEHICLE_SELF, NULL);
		if (vself)
		{
			RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
			short sId;
			float fHealth;
			bsData.Read(sId);
			bsData.Read(fHealth);
			if (sId == g_Players->pLocalPlayer->sCurrentVehicleID && fHealth < vself->hitpoints)
			{
				cheat_state_text("Warning: Server tried change your vehicle health to %0.1f", fHealth);
				return;
			}
		}
	}

	if (byteRPCID == RPC_ScrCreateExplosion)
	{
		if (cheat_state->_generic.hp_cheat)
		{
			float pos[3], radius;
			int type;
			float dist[3];
			float *mypos = (cheat_state->state == CHEAT_STATE_VEHICLE) ? cheat_state->vehicle.coords : cheat_state->actor.coords;
			RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

			bsData.Read(pos[0]);
			bsData.Read(pos[1]);
			bsData.Read(pos[2]);
			bsData.Read(type);
			bsData.Read(radius);
			vect3_vect3_sub(pos, mypos, dist);

			cheat_state_text(" Проверка на неуязвимость | Расстояние: %0.2f | %0.2f %0.2f %0.2f ", pos[0], pos[1], pos[2], vect3_length(dist));

			if (vect3_length(dist) < 9)
			{
				if (cheat_state->state == CHEAT_STATE_VEHICLE)
				{
					struct vehicle_info *vInfo = vehicle_info_get(VEHICLE_SELF, 0);
					vInfo->hitpoints -= 40 * (9.0 - vect3_length(dist));
				}
				else
				{
					struct actor_info    *pInfo = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
					pInfo->hitpoints -= 20 * (9.0 - vect3_length(dist));
				}
			}
		}
	}

	if (set.sat)
	{
		if (byteRPCID == RPC_ScrSetPlayerHealth)
		{
			actor_info *self = actor_info_get(ACTOR_SELF, NULL);
			if (self)
			{
				RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
				float fHealth;
				bsData.Read(fHealth);
				if (self->hitpoints - fHealth <= 4.0f && fHealth < self->hitpoints  && set.spawnsethp) return;
				else if (!set.spawnsethp)
				{
					self->hitpoints = fHealth;
					set.spawnsethp = 1;
				}
			}
		}
	}

	if (set.enable_extra_godmode && cheat_state->_generic.hp_cheat)
	{
		if (byteRPCID == RPC_ScrSetPlayerHealth)
		{
			actor_info *self = actor_info_get(ACTOR_SELF, NULL);
			if (self)
			{
				RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
				float fHealth;
				bsData.Read(fHealth);
				if (fHealth < self->hitpoints)
				{
					cheat_state_text("Warning: Server tried change your health to %0.1f", fHealth);
					return;
				}
			}
		}
		else if (byteRPCID == RPC_ScrSetVehicleHealth)
		{
			vehicle_info *vself = vehicle_info_get(VEHICLE_SELF, NULL);
			if (vself)
			{
				RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);
				short sId;
				float fHealth;
				bsData.Read(sId);
				bsData.Read(fHealth);
				if (sId == g_Players->pLocalPlayer->sCurrentVehicleID && fHealth < vself->hitpoints)
				{
					cheat_state_text("Warning: Server tried change your vehicle health to %0.1f", fHealth);
					return;
				}
			}
		}
	}
	functionPointer(rpcParams);
}

uint8_t _declspec (naked) hook_handle_rpc_packet(void)
{
	static DWORD dwTemp1, dwTemp2;
	__asm pushad;
	__asm mov dwTemp1, eax; // RPCParameters rpcParms
	__asm mov dwTemp2, edi; // RPCNode *node

	HandleRPCPacketFunc(*(unsigned char *)dwTemp2, (RPCParameters *)dwTemp1, *(void(**) (RPCParameters *rpcParams))(dwTemp2 + 1));
	dwTemp1 = g_dwSAMP_Addr + SAMP_HOOKEXIT_HANDLE_RPC;

	__asm popad;
	// execute overwritten code
	__asm add esp, 4
	// exit from the custom code
	__asm jmp dwTemp1;
}

uint8_t _declspec (naked) hook_handle_rpc_packet2(void)
{
	static DWORD dwTemp1, dwTemp2;
	__asm pushad;
	__asm mov dwTemp1, ecx; // RPCParameters rpcParms
	__asm mov dwTemp2, edi; // RPCNode *node

	HandleRPCPacketFunc(*(unsigned char *)dwTemp2, (RPCParameters *)dwTemp1, *(void(**) (RPCParameters *rpcParams))(dwTemp2 + 1));
	dwTemp1 = g_dwSAMP_Addr + SAMP_HOOKEXIT_HANDLE_RPC2;

	__asm popad;
	// exit from the custom code
	__asm jmp dwTemp1;
}

void __stdcall CNetGame__destructor(void)
{
	/*if (g_bGotoSite)
	{
		char path[MAX_PATH];
		sprintf(path, "%s\\" M0D_FOLDER "mod_sa.ini", g_szWorkingDirectory);
		FILE *f = fopen(path, "a");
		fwrite("\r\n#offad", 8, 1, f);
		fclose(f);
		ShellExecute(0, "open", "http://rakbot.ru/index.php?from=mod_sa", 0, 0, SW_SHOWNORMAL);
	}*/

	// release hooked rakclientinterface, restore original rakclientinterface address and call CNetGame destructor
	if (g_SAMP->pRakClientInterface != NULL)
		delete g_SAMP->pRakClientInterface;
	g_SAMP->pRakClientInterface = g_RakClient->GetRakClientInterface();

	return ((void(__thiscall *)(void *))(g_dwSAMP_Addr + SAMP_FUNC_CNETGAMEDESTRUCTOR))(g_SAMP);
}

void HandleDialogResponse(WORD wDialogID, BYTE byteButtonID, WORD wItemID, char *szResponseText)
{
	/*if (g_bShowAd && wDialogID == 1488)
	{
		if (byteButtonID == 1)
		{
			g_bGotoSite = TRUE;
			addMessageToChatWindow("{FFFFFF}Спасибо за интерес к проекту! После выхода из игры Вы будете перенаправлены на сайт проекта");
		}

		return;
	}*/
}

uint8_t _declspec (naked) hook_handle_dialog_response(void)
{
	static DWORD dwJumpAddr, dwButtonID;
	__asm pushad;
	__asm mov eax, dword ptr[esp + 0x24];
	__asm mov dwButtonID, eax;
	HandleDialogResponse(g_Dialog->iDialogID, dwButtonID,
		((int(__thiscall *)(void *, int))(g_dwSAMP_Addr + SAMP_FUNC_DIALOG_GETITEM))(g_Dialog->pListBox, -1),
		((char *(__thiscall *)(void *))(g_dwSAMP_Addr + SAMP_FUNC_DIALOG_GETTEXT))(g_Dialog->pEditBox));
	dwJumpAddr = g_dwSAMP_Addr + SAMP_HOOKEXIT_DIALOGRESPONSE;
	__asm popad;
	__asm
	{
		push ebp;
		mov ebp, esp;
		and esp, 0xFFFFFFF8;
		jmp dwJumpAddr;
	}
}

/*void __stdcall HandleSAMPWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Log("HWND: %X, MSG: %d(%X) WPARAM: %d(%X), LPARAM: %d(%X)", hwnd, msg, msg, wParam, wParam, lParam, lParam);
}*/

uint8_t _declspec (naked) hook_handle_wndproc(void)
{
	__asm pushad;
	/*__asm push[esp + 0x30];
	__asm push[esp + 0x30];
	__asm push[esp + 0x30];
	__asm push[esp + 0x30];
	__asm call HandleSAMPWndProc;*/
	static DWORD dwTemp1 = g_dwSAMP_Addr + SAMP_HOOKEXIT_WNDPROC;
	static DWORD dwTemp2 = *(DWORD *)(g_dwSAMP_Addr + SAMP_UNKNOWN_OFFSET);
	__asm popad;
	__asm mov eax, dwTemp2;
	__asm jmp dwTemp1;
}

void SetupSAMPHook(char *szName, DWORD dwFuncOffset, void *Func, int iType, int iSize, char *szCompareBytes)
{
	CDetour api;
	int strl = strlen(szCompareBytes);
	uint8_t *bytes = hex_to_bin(szCompareBytes);

	if (!strl || !bytes || memcmp_safe((uint8_t *)g_dwSAMP_Addr + dwFuncOffset, bytes, strl / 2))
	{
		if (api.Create((uint8_t *)((uint32_t)g_dwSAMP_Addr) + dwFuncOffset, (uint8_t *)Func, iType, iSize) == 0)
			Log("Failed to hook %s.", szName);
	}
	else
	{
		Log("Failed to hook %s (memcmp)", szName);
	}

	if (bytes)
		free(bytes);
}

void installSAMPHooks()
{
	if (g_SAMP == NULL)
		return;

	if (set.anti_spam || set.chatbox_logging)
	{
		SetupSAMPHook("ServerMessage", SAMP_HOOKPOS_ServerMessage, server_message_hook, DETOUR_TYPE_JMP, 5, "6A00C1E808");
		SetupSAMPHook("ClientMessage", SAMP_HOOKPOS_ClientMessage, client_message_hook, DETOUR_TYPE_JMP, 5, "663BD1752D");
	}

	SetupSAMPHook("HandleRPCPacket", SAMP_HOOKENTER_HANDLE_RPC, hook_handle_rpc_packet, DETOUR_TYPE_JMP, 6, "FF5701");
	SetupSAMPHook("HandleRPCPacket2", SAMP_HOOKENTER_HANDLE_RPC2, hook_handle_rpc_packet2, DETOUR_TYPE_JMP, 8, "FF5701");
	SetupSAMPHook("CNETGAMEDESTR1", SAMP_HOOKENTER_CNETGAME_DESTR, CNetGame__destructor, DETOUR_TYPE_CALL_FUNC, 5, "E8");
	SetupSAMPHook("CNETGAMEDESTR2", SAMP_HOOKENTER_CNETGAME_DESTR2, CNetGame__destructor, DETOUR_TYPE_CALL_FUNC, 5, "E8");
	SetupSAMPHook("DialogResponse", SAMP_HOOKENTER_DIALOGRESPONSE, hook_handle_dialog_response, DETOUR_TYPE_JMP, 6, "558BEC83E4F8");
	SetupSAMPHook("WndProc", SAMP_HOOKENTER_WNDPROC, hook_handle_wndproc, DETOUR_TYPE_JMP, 5, "");
}

void setSAMPCustomSendRates(int iOnFoot, int iInCar, int iAim, int iHeadSync)
{
	if (!set.samp_custom_sendrates_enable)
		return;
	if (g_dwSAMP_Addr == NULL)
		return;
	if (g_SAMP == NULL)
		return;

	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_ONFOOTSENDRATE), &iOnFoot, sizeof(int));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_INCARSENDRATE), &iInCar, sizeof(int));
	memcpy_safe((void *)(g_dwSAMP_Addr + SAMP_AIMSENDRATE), &iAim, sizeof(int));
}

// SAMP Patches
int sampPatchDisableDialog(int iEnabled)
{
	static struct patch_set sampPatchDisableShowDialog =
	{
		"Data", 0, 0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_DIALOG), NULL, hex_to_bin("C3"), NULL }
		}
	};

	if (iEnabled && !sampPatchDisableShowDialog.installed)
		return patcher_install(&sampPatchDisableShowDialog);
	else if (!iEnabled && sampPatchDisableShowDialog.installed)
		return patcher_remove(&sampPatchDisableShowDialog);
	return NULL;
}

int sampPatchDisableOnFoot(int iEnabled)
{
	static struct patch_set sampPatchDisableOnFootData =
	{
		"Data", 0, 0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_ONFOOT_DATA), NULL, hex_to_bin("C3"), NULL }
		}
	};

	if (iEnabled && !sampPatchDisableOnFootData.installed)
		return patcher_install(&sampPatchDisableOnFootData);
	else if (!iEnabled && sampPatchDisableOnFootData.installed)
		return patcher_remove(&sampPatchDisableOnFootData);
	return NULL;
}

int sampPatchDisableInCar(int iEnabled)
{
	static struct patch_set sampPatchDisableInCarData =
	{
		"Data",
		0,
		0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_INCAR_DATA), NULL, hex_to_bin("C3"), NULL }
		}
	};

	if (iEnabled && !sampPatchDisableInCarData.installed)
		return patcher_install(&sampPatchDisableInCarData);
	else if (!iEnabled && sampPatchDisableInCarData.installed)
		return patcher_remove(&sampPatchDisableInCarData);
	return NULL;
}

int sampPatchDisableChat(int iEnabled)
{
	static struct patch_set sampPatchDisableChat_patch =
	{
		"Disable chat", 0, 0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_CHAT), NULL, hex_to_bin("C3"), NULL }
		}
	};

	if (iEnabled && !sampPatchDisableChat_patch.installed)
		return patcher_install(&sampPatchDisableChat_patch);
	else if (!iEnabled && sampPatchDisableChat_patch.installed)
		return patcher_remove(&sampPatchDisableChat_patch);
	return NULL;
}

int sampPatchDisableNameTags(int iEnabled)
{
	static struct patch_set sampPatchEnableNameTags_patch =
	{
		"Remove player status", 0, 0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_NAMETAGS), NULL, (uint8_t *)"\xC3", NULL },
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_NAMETAGS_HP), NULL, (uint8_t *)"\xC3", NULL }
		}
	};
	if (iEnabled && !sampPatchEnableNameTags_patch.installed)
		return patcher_install(&sampPatchEnableNameTags_patch);
	else if (!iEnabled && sampPatchEnableNameTags_patch.installed)
		return patcher_remove(&sampPatchEnableNameTags_patch);
	return NULL;
}

int sampPatchDisableInteriorUpdate(int iEnabled)
{
	static struct patch_set sampPatchDisableInteriorUpdate_patch =
	{
		"NOP sendinterior", 0, 0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_SKIPSENDINTERIOR), NULL, (uint8_t *)"\xEB", NULL }
		}
	};

	if (iEnabled && !sampPatchDisableInteriorUpdate_patch.installed)
		return patcher_install(&sampPatchDisableInteriorUpdate_patch);
	else if (!iEnabled && sampPatchDisableInteriorUpdate_patch.installed)
		return patcher_remove(&sampPatchDisableInteriorUpdate_patch);
	return NULL;
}

int sampPatchDisableScoreboardToggleOn(int iEnabled)
{
	static struct patch_set sampPatchDisableScoreboard_patch =
	{
		"NOP Scoreboard Functions", 0, 0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_SCOREBOARDTOGGLEON), NULL, (uint8_t *)"\xC3", NULL },
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_SCOREBOARDTOGGLEONKEYLOCK), NULL, (uint8_t *)"\xC3", NULL }
		}
	};
	if (iEnabled && !sampPatchDisableScoreboard_patch.installed)
		return patcher_install(&sampPatchDisableScoreboard_patch);
	else if (!iEnabled && sampPatchDisableScoreboard_patch.installed)
		return patcher_remove(&sampPatchDisableScoreboard_patch);
	return NULL;
}

int sampPatchDisableChatInputAdjust(int iEnabled)
{
	static struct patch_set sampPatchDisableChatInputAdj_patch =
	{
		"NOP Adjust Chat input box", 0, 0,
		{
			{ 6, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_CHATINPUTADJUST_Y), NULL, (uint8_t *)"\x90\x90\x90\x90\x90\x90", NULL },
			{ 7, (void *)((uint8_t *)g_dwSAMP_Addr + SAMP_PATCH_CHATINPUTADJUST_X), NULL, (uint8_t *)"\x90\x90\x90\x90\x90\x90\x90", NULL }
		}
	};
	if (iEnabled && !sampPatchDisableChatInputAdj_patch.installed)
		return patcher_install(&sampPatchDisableChatInputAdj_patch);
	else if (!iEnabled && sampPatchDisableChatInputAdj_patch.installed)
		return patcher_remove(&sampPatchDisableChatInputAdj_patch);
	return NULL;
}

void sendDeath(void)
{
	if (g_SAMP == NULL)
		return;

	uint32_t func = g_dwSAMP_Addr + SAMP_FUNC_DEATH;
	void  *lpPtr = g_Players->pLocalPlayer;
	__asm mov ecx, dword ptr[lpPtr]
		__asm push ecx
	__asm call func
	__asm pop ecx
}

void changeServer(const char *pszIp, unsigned ulPort, const char *pszPassword)
{
	if (!g_SAMP)
		return;

	((void(__cdecl *)(unsigned))(g_dwSAMP_Addr + SAMP_FUNC_ENCRYPT_PORT))(ulPort);

	disconnect(500);
	strcpy(g_SAMP->szIP, pszIp);
	g_SAMP->ulPort = ulPort;
	setPassword((char *)pszPassword);
	joining_server = 1;
}