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
#include "main.h"

void cheat_colpuls()
{
	if (cheat_state->_generic.carcolpuls && cheat_state->state == CHEAT_STATE_VEHICLE)
	{
		struct vehicle_info *vinfo = vehicle_info_get(-1, 0x01);
		if (vinfo == NULL)
			return;
		vehicle_setColor0(vinfo, rand() % 255);
		vehicle_setColor1(vinfo, rand() % 255);
	}
}

void cheat_hint()
{
	if (cheat_state->_generic.shpora)
	{
		for (int i = 0; i < 500; i++)
		{
			if (strlen(set.shpora[i]) > 0)
				pD3DFontFixed->PrintShadow(pPresentParam.BackBufferWidth / 50,
					set.shporaoffset + i * 10, D3DCOLOR_XRGB(255, 165, 0), set.shpora[i]);
		}
	}
}

void cheat_hppuls()
{
	if (cheat_state->_generic.hppuls)
	{
		struct actor_info *info = actor_info_get(-1, 0x01);
		if (info == NULL)
			return;
		info->hitpoints = (rand() % 99) + 1.0f;
	}
}

void cheat_killflood()
{
	if (cheat_state->_generic.killflood)
		g_RakClient->SendDeath(rand() % getPlayerCount(), rand() % 46);
}

void cheat_lsamats()
{
	if (cheat_state->_generic.lsamats && cheat_state->state == CHEAT_STATE_ACTOR)
	{
		static bool take = 1;
		static DWORD dwTime = 0;
		if (GetTickCount() - dwTime > 1000)
		{
			if (take == 1)
			{
				float mPos[3] = { 2728.30, -2451.53, 18.95 };
				g_RakClient->SendFakeOnfootSyncData(mPos, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
				say("/materials get");
			}
			else say("/materials put");
			take ^= 1;
			dwTime = GetTickCount();
		}
	}
}

void cheat_skillup()
{
	if (cheat_state->_generic.skill)
	{
		static DWORD dwTime = 0;
		if (GetTickCount() - dwTime > iNetModeFiringSendRate)
		{
			actor_info *self = actor_info_get(-1, 0);
			if (self == NULL)
				return;

			if (self->weapon_slot != 0)
			{
				if (self->weapon[self->weapon_slot].ammo_clip == 0)
				{
					if (self->weapon[self->weapon_slot].ammo > 7)
						self->weapon[self->weapon_slot].ammo_clip += 7;
					else
						self->weapon[self->weapon_slot].ammo_clip = self->weapon[self->weapon_slot].ammo;
				}
				if (self->weapon[self->weapon_slot].ammo_clip > 0)
				{
					self->weapon[self->weapon_slot].ammo_clip -= 1;
					self->weapon[self->weapon_slot].ammo -= 1;
				}
			}
			float vect3_null[3] = { 0.0f, 0.0f, 0.0f };
			g_RakClient->SendFakeBulletSyncData(&self->base.matrix[12], vect3_null, &self->base.matrix[12], 0, -1);
			dwTime = GetTickCount();
		}
	}
}

void cheat_damager()
{
	if (cheat_state->_generic.damag && g_Players->pLocalPlayer->iIsActive)
	{
		static DWORD dwTime = 0;
		if (GetTickCount() - dwTime > iNetModeFiringSendRate)
		{
			float curPos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };
			float bodyPos[3] = { 0.0f, 0.0f, 0.0f };
			int playerId = samp_actor_find_nearest(ACTOR_ALIVE);
			if (playerId > -1 && playerId < SAMP_MAX_PLAYERS)
			{
				cheat_state_text("Атака игрока %d...", playerId);
				if (!set.damagtype && getPlayerSAMPVehicleID(playerId))
				{
					int vehId = getPlayerSAMPVehicleID(playerId);
					g_RakClient->SendFakeBulletSyncData(curPos, bodyPos, &getGTAVehicleFromSAMPVehicleID(vehId)->base.matrix[12], 2, vehId);
				}
				else g_RakClient->SendFakeBulletSyncData(curPos, bodyPos, &getGTAPedFromSAMPPlayerID(playerId)->base.matrix[12], 1, playerId);
			}
			dwTime = GetTickCount();
		}
	}
}

void cheat_farm()
{
	if (cheat_state->_generic.farm)
	{
		static DWORD time = 0;
		if (g_SAMPMisc->iCheckpointEnabled && vect3_dist(g_SAMPMisc->fCheckpointPos, 
			&actor_info_get(-1, 0x01)->base.matrix[12]) > g_SAMPMisc->fCheckpointSize[0])
		{
			float spd[3] = { 0, 0, 0 };
			for (int i = 0; i <= 32; i++)
			{
				float x = *(float *)(0xC7DD58 + 48 + 160 * i);
				float y = *(float *)(0xC7DD58 + 52 + 160 * i);
				float z = *(float *)(0xC7DD58 + 56 + 160 * i);
				if ((x > 2695 && x < 2696 && y > -1705 && y < -1704) || (x > 1099 && x < 1100 && y > 1601 && y < 1602) || (x == 0 && y == 0) || *(WORD *)(0xC7DD58 + 80 + 160 * i) != 1) continue;
				else
				{
					float pos[3] = { x, y, z + 1.0f };
					cheat_teleport(pos, 0);
					break;
				}
			}
		}
		else if (!g_SAMPMisc->iCheckpointEnabled && 
			GetTickCount() - time > 1000)
		{
			float curPos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };
			int iPic = getPickup(curPos, 50, 19197);
			g_RakClient->SendFakeOnfootSyncData(g_SAMP->pPools->pPickup->pickup[iPic].fPosition, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
			RakNet::BitStream bs;
			bs.Write(iPic);
			g_RakClient->RPC(RPC_PickedUpPickup, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, 0);
			time = GetTickCount();
		}
	}
}

void cheat_friends()
{
	if (set.friendchecker)
	{
		if (!gta_menu_active() && g_Players && g_Players->pRemotePlayer)
		{
			int count = 0;
			for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
			{
				if (g_Players->iIsListed[i] != 1)
					continue;
				if (g_Players->pRemotePlayer[i] == NULL)
					continue;

				for (int x = 0; x < 300; x++)
				{
					if (getPlayerName(i) == NULL || set.admin[x] == NULL)
						break;

					if (!strcmp(getPlayerName(i), set.frend[x]))
					{

						D3DCOLOR color;
						char text[64];
						if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[i]) && (g_Players->pRemotePlayer[i]->pPlayerData == NULL || g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == NULL))
						{
							color = D3DCOLOR_XRGB(255, 165, 0);
							sprintf(text, "[ID: %d][LVL: %d] %s", i, g_Players->pRemotePlayer[i]->iScore, getPlayerName(i));
						}
						else
						{
							color = D3DCOLOR_XRGB(255, 0, 0);
							sprintf(text, "[ID: %d][LVL: %d] %s", i, g_Players->pRemotePlayer[i]->iScore, getPlayerName(i));
						}
						pD3DFont->PrintShadow(pPresentParam.BackBufferWidth - pD3DFont->DrawLength("Друзья в сети:") - 3, pPresentParam.BackBufferHeight / 2 - 20, D3DCOLOR_XRGB(255, 165, 0), "Друзья в сети:");
						pD3DFont->PrintShadow(pPresentParam.BackBufferWidth - pD3DFont->DrawLength(text) - 3, pPresentParam.BackBufferHeight / 2 + count * 20, color, text);
						count++;
					}
				}
			}
			if (count == 0)
			{
				pD3DFont->PrintShadow(pPresentParam.BackBufferWidth - pD3DFont->DrawLength("Друзья в сети:") - 3, pPresentParam.BackBufferHeight / 2 - 20, D3DCOLOR_ARGB(100, 255, 165, 0), "Друзья в сети:");
				pD3DFont->PrintShadow(pPresentParam.BackBufferWidth - pD3DFont->DrawLength("Нет друзей в сети") - 3, pPresentParam.BackBufferHeight / 2, D3DCOLOR_ARGB(100, 255, 165, 0), "Нет друзей в сети");
			}
		}
	}
}

void cheat_admins()
{
	if (set.adminchecker)
	{
		if (!gta_menu_active() && g_Players && g_Players->pRemotePlayer)
		{
			int count = 0;
			for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
			{
				if (g_Players->iIsListed[i] != 1)
					continue;
				if (g_Players->pRemotePlayer[i] == NULL)
					continue;

				for (int x = 0; x < 300; x++)
				{
					if (getPlayerName(i) == NULL || set.admin[x] == NULL)
						break;

					if (!strcmp(getPlayerName(i), set.admin[x]))
					{
						D3DCOLOR color;
						char text[64];
						if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[i]) && (g_Players->pRemotePlayer[i]->pPlayerData == NULL || g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == NULL))
						{
							color = D3DCOLOR_XRGB(255, 165, 0);
							sprintf(text, "[ID: %d][LVL: %d] %s", i, g_Players->pRemotePlayer[i]->iScore, getPlayerName(i));
						}
						else
						{
							color = D3DCOLOR_XRGB(255, 0, 0);
							sprintf(text, "[ID: %d][LVL: %d] %s", i, g_Players->pRemotePlayer[i]->iScore, getPlayerName(i));
							if (cheat_state->_generic.exitnearadm && g_Players->pRemotePlayer[i]->iScore > 0)
							{
								addMessageToChatWindow("Выход из игры, админ рядом: %s", getPlayerName(i));
								Log("Выход из игры, админ рядом: %s", getPlayerName(i));
								disconnect(0);
								ExitProcess(0);
							}
						}
						pD3DFont->PrintShadow(1, pPresentParam.BackBufferHeight / 2 - 20, D3DCOLOR_XRGB(255, 165, 0), "Админы в сети:");
						pD3DFont->PrintShadow(1, pPresentParam.BackBufferHeight / 2 + count * 20, color, text);
						if (g_Players->pRemotePlayer[i]->iScore > 0 && cheat_state->_generic.exitadm)
						{
							addMessageToChatWindow("Выход из игры, админ в сети: %s", getPlayerName(i));
							Log("Выход из игры, админ в сети: %s", getPlayerName(i));
							disconnect(0);
							ExitProcess(0);
						}
						count++;
					}
				}
			}
			if (count == 0)
			{
				pD3DFont->PrintShadow(1, pPresentParam.BackBufferHeight / 2 - 20, D3DCOLOR_ARGB(100, 255, 165, 0), "Админы в сети:");
				pD3DFont->PrintShadow(1, pPresentParam.BackBufferHeight / 2, D3DCOLOR_ARGB(100, 255, 165, 0), "Нет админов в сети");
			}
		}
	}
}

void cheat_lockstatus()
{
	if (!cheat_state->_generic.lockstatus)
		return;
	if (gta_menu_active())
		return;
	if (cheat_state->_generic.cheat_panic_enabled)
		return;
	if (!pGameInterface || !pGameInterface->GetPools())
		return;
	if (!pPedSelf)
		return;

	const vehicle_entry *vehicle;
	char				buf[256];
	int					v;
	float				w, h;

	D3DXVECTOR3			poss, screenposs;
	CVector2D			screenPosition;
	CVector				ourPosition, iterPosition, ourPosMinusIter;

	// get our position
	if (!isBadPtr_GTA_pVehicle(pPedSelf->GetVehicle()))
	{
		ourPosition = *(pPedSelf->GetVehicle()->GetPosition());
	}
	else
	{
		ourPosition = pPedSelf->GetInterface()->Placeable.matrix->vPos;
	}

	// setup iterator
	CVehicleSA	*iterVehicle = NULL;
	CPoolsSA	*pPools = reinterpret_cast <CPoolsSA *> (pGameInterface->GetPools());
	CPoolsSA::vehiclePool_t::mapType::iterator iter = pPools->m_vehiclePool.map.begin();

	// iterate
	while (iter.pos < iter.end)
	{
		// map iterator CVehicleSA pointer to our CVehicleSA pointer
		iterVehicle = iter.pos->second;

		// advance to next CVehicleSA for next pass
		iter.pos++;

		// move past null pointers
		if (isBadPtr_GTA_pVehicle(iterVehicle))
			continue;
		//if ( isBadPtr_GTA_pVehicle(iterVehicle->GetVehicleInterface()) )
		//	continue;

		// check if it's farther than set.vehicle_tags_dist
		iterPosition = iterVehicle->GetInterface()->Placeable.matrix->vPos;
		ourPosMinusIter = ourPosition - iterPosition;
		if (ourPosMinusIter.Length() > set.vehicle_tags_dist)
			continue;

		// check if it's our vehicle
		if (iterVehicle == pPedSelf->GetVehicle())
			continue;

		// check if it's near zero
		if (iterVehicle->GetPosition()->IsNearZero())
			continue;

		// CVector to D3DXVECTOR3, function to be converted to CVector later
		poss.x = iterPosition.fX;
		poss.y = iterPosition.fY;
		poss.z = iterPosition.fZ;

		// yup
		CalcScreenCoors(&poss, &screenposs);

		// check if the vehicle is culled or not
		if (screenposs.z < 1.f)
			continue;

		// D3DXVECTOR3 to CVector2D
		screenPosition.fX = screenposs.x;
		screenPosition.fY = screenposs.y;

		// get the vehicle model's name
		vehicle = gta_vehicle_get_by_id(iterVehicle->GetModelIndex());

		// get SAMP's vehicle id
		if (g_Vehicles != NULL)
			v = translateGTASAMP_vehiclePool.iSAMPID[getVehicleGTAIDFromInterface((DWORD *)iterVehicle->GetVehicleInterface())];
		else
			v = (int)iterVehicle->GetArrayID();

		/////////////////
		// render time //
		h = pD3DFont->DrawHeight();
		_snprintf_s(buf, sizeof(buf)-1, "%s", iterVehicle->AreDoorsLocked() ? "Закрыта" : "Открыта");
		w = pD3DFont->DrawLength(buf);

		// different color if car is being driven
		DWORD	color_veh;
		if (iterVehicle->AreDoorsLocked())
			color_veh = D3DCOLOR_XRGB(255, 0, 0); // blueish 100, 150, 235
		else
			color_veh = D3DCOLOR_XRGB(0, 255, 0);

		// render vehicle name
		pD3DFont->PrintShadow(screenPosition.fX, screenPosition.fY,
			color_veh, buf);
	}
	return;
}

void cheat_takecarbyid()
{
	if (cheat_state->_generic.getauto)
	{
		for (int i = 0; i < SAMP_MAX_VEHICLES; i++)
		{
			if (g_Vehicles->iIsListed[i] == NULL)
				continue;
			if (g_Vehicles->pSAMP_Vehicle[i] == NULL)
				continue;
			if (g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle == NULL)
				continue;

			int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[i]) - (DWORD)pool_vehicle->start) / 2584;
			struct vehicle_info *vinfo = vehicle_info_get(car_id, 0);

			int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);
			if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID) continue;

			if (!g_Vehicles->pSAMP_Vehicle[i]->iIsLocked)
			{
				if (g_Vehicles->pGTA_Vehicle[i]->base.model_alt_id == set.find_id)
				{
					g_RakClient->SendEnterPlayerVehicle(i, 0);
					GTAfunc_PutActorInCar(vinfo);
					cheat_state->_generic.getauto = 0;
					break;
				}
			}
		}
	}
}

void cheat_autodriver()
{
	static bool action = true;
	static DWORD dwTime = 0;
	if (cheat_state->_generic.autodriver == 1)
	{
		if (GetTickCount() - 2500 > dwTime && action == false)
		{
			float cpos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };
			g_RakClient->SendFakeOnfootSyncData(cpos, actor_info_get(-1, 0)->hitpoints, actor_info_get(-1, 0)->speed);
			dwTime = GetTickCount();
			action = true;
		}
		else if (GetTickCount() - 250 > dwTime && action == true)
		{
			float cpos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };
			g_RakClient->SendEnterPlayerVehicle(g_Players->pLocalPlayer->sCurrentVehicleID, 0);
			g_RakClient->SendFakeDriverSyncData(g_Players->pLocalPlayer->sCurrentVehicleID, cpos, vehicle_info_get(-1, 0)->hitpoints, vehicle_info_get(-1, 0)->speed);
			dwTime = GetTickCount();
			action = false;
		}
	}
	return;
}

void cheat_getguns()
{
	int step = 0;
	static int time = 0;
	if (GetTickCount() - time > 1250 && cheat_state->_generic.slivobsh)
	{
		if (!step)
		{
			say("/get guns 500");
			step = 1;
		}
		else
		{
			say("/sellguns SDPistol 500 4 %d", g_Players->sLocalPlayerID);
			step = 0;
		}
		time = GetTickCount();
	}
}

void cheat_smsflood()
{
	static uint16_t playerId = 0;
	if (cheat_state->_generic.smsflood)
	{
		static DWORD dwTime = 0;
		if (GetTickCount() - dwTime > set.flood_delay)
		{
			say("/sms %d %s", playerId, set.flood_text);
			playerId++;
			dwTime = GetTickCount();
		}
	}
	else playerId = 0;
	return;
}

void cheat_flood()
{
	static DWORD dwTime = 0;
	if (cheat_state->_generic.flood)
	{
		if (GetTickCount() - dwTime > set.flood_delay)
		{
			say(set.flood_text);
			dwTime = GetTickCount();
		}
	}
	return;
}

void cheat_repflood(void)
{
	if (cheat_state->_generic.repflood)
	{
		static DWORD dwTime = 0;
		sampPatchDisableDialog(1);
		if (GetTickCount() - dwTime > set.flood_delay)
		{
			say("/mm");
			g_RakClient->SendDialogResponse(22, 1, 4, "");
			g_RakClient->SendDialogResponse(24, 1, 0, set.flood_text);
			dwTime = GetTickCount();
		}
		sampPatchDisableDialog(0);
	}
	return;
}

void cheat_drugsuse()
{
	if (cheat_state->_generic.narkouser)
	{
		static int time = 0;
		if (actor_info_get(-1, 0) != NULL && actor_info_get(-1, 0)->hitpoints <= set.narkohits && GetTickCount() - time > (set.narkotime * 1000))
		{
			say("/usedrugs %d", set.narkokolvo);
			time = GetTickCount();
		}
	}
}

void cheat_healme()
{
	static int time = 0;
	if (GetTickCount() - time > 1250 && cheat_state->_generic.healme)
	{
		say("/healme");
		time = GetTickCount();
	}
}

void cheat_boxer()
{
	if (cheat_state->_generic.boxer)
	{
		static int time = 0;
		if (GetTickCount() - time > iNetModeNormalOnfootSendRate)
		{
			struct actor_info *self = actor_info_get(-1, 0x01);
			stOnFootData ofSync;
			ZeroMemory(&ofSync, sizeof(ofSync));
			RakNet::BitStream bsOnfootSync;
			ofSync.sKeys = 144;
			ofSync.byteHealth = actor_info_get(-1, 0)->hitpoints;
			ofSync.byteCurrentWeapon = 0;
			ofSync.fMoveSpeed[0] = 0.01f;
			vect3_copy(&self->base.matrix[12], ofSync.fPosition);
			ofSync.sCurrentAnimationID = 471 + rand() % 3;
			bsOnfootSync.Write((BYTE)ID_PLAYER_SYNC);
			bsOnfootSync.Write((PCHAR)&ofSync, sizeof(stOnFootData));
			g_RakClient->Send(&bsOnfootSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			time = GetTickCount();
		}
	}
}

void cheat_coordmaster()
{
	if (!cheat_state->_generic.coordmaster)
		return;

	if(vehicle_info *info = vehicle_info_get(-1, 1))
		vehicleUnflip(info);

	static DWORD dwCount = 0;

	if (GetTickCount() < dwCount)
		return;

	float fPosition[3] = {
		pPedSelf->GetPosition()->fX,
		pPedSelf->GetPosition()->fY,
		pPedSelf->GetPosition()->fZ
	};

	float fDist = vect3_dist(fPosition, set.coord_target);

	if (fDist > set.coord_dist)
	{
		float fDistXY = vect2_dist(fPosition, set.coord_target);
		float fDistSin = (set.coord_target[0] - fPosition[0]) / fDistXY;
		float fDistCos = (set.coord_target[1] - fPosition[1]) / fDistXY;

		fPosition[0] += fDistSin * set.coord_dist;
		fPosition[1] += fDistCos * set.coord_dist;
		if (set.coord_height != fPosition[2])
		{
			if (set.coord_height - fPosition[2] > set.coord_dist * 0.5f)
			{
				float fK = set.coord_height > fPosition[2] ? 0.5f : -0.5f;
				fPosition[2] += set.coord_dist * fK;
			}
			else
				fPosition[2] = set.coord_height;
		}
		else if (fDistXY < set.coord_dist)
		{
			float fK = set.coord_target[2] > fPosition[2] ? 0.5f : -0.5f;
			fPosition[2] += set.coord_dist * fK;
		}
	}
	else
	{
		vect3_copy(set.coord_target, fPosition);
		Log("[RAKBOT] Вы достигли места назначения!");
		cheat_state->_generic.coordmaster = 0;
	}

	cheat_teleport(fPosition, gta_interior_id_get());

	GTAfunc_LockActor(cheat_state->_generic.coordmaster ? 1 : 0);
	dwCount = GetTickCount() + set.coord_time;
}

void cheat_cpmaster()
{
	if (cheat_state->_generic.cpm)
	{
		float checkPos[3] = { 0, 0, 0 };
		float playerPos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };
		static int iTime = 0;
		if ((int)GetTickCount() - iTime > set.coord_time)
		{
			if (g_SAMPMisc->iRaceCheckpointEnabled)
			{
				vect3_copy(g_SAMPMisc->vecRaceCheckpointPos, checkPos);
				checkPos[2] += set.cpm_offset;
				float lenX = checkPos[0] - playerPos[0];
				float lenY = checkPos[1] - playerPos[1];
				float lenXY = sqrt(pow(lenX, 2) + pow(lenY, 2));
				float sinA = lenX / lenXY, cosA = lenY / lenXY;
				if (vect3_dist(checkPos, playerPos) > set.coord_dist)
				{
					playerPos[0] += sinA * set.coord_dist;
					playerPos[1] += cosA * set.coord_dist;
					playerPos[2] = checkPos[2] - 40.0f;
				}
				else
					vect3_copy(checkPos, playerPos);
				cheat_teleport(playerPos, 0);
			}
			iTime = GetTickCount();
		}
		struct vehicle_info *info = vehicle_info_get(-1, 0x01);
		if (info != NULL)
			vehicleUnflip(info);
	}
}

void buyhouse(void *)
{
	static struct patch_set sampPatchDialog =
	{
		"Data", 0, 0,
		{
			{ 1, (void *)((uint8_t *)g_dwSAMP_Addr + 0xBC70), NULL, hex_to_bin("C390"), NULL }
		}
	};
	patcher_install(&sampPatchDialog);
	Sleep(1000);
	say("/buyhouse");
	Sleep(1000);
	patcher_remove(&sampPatchDialog);
}

void cheat_pickchecker()
{
	if (cheat_state->_generic.pickchecker)
	{
		for (int i = 0; i < SAMP_MAX_PICKUPS; i++)
		{
			if (g_SAMP->pPools->pPickup->pickup[i].iModelID == set.model_pickup)
			{
				struct actor_info *self = actor_info_get(ACTOR_SELF, 0);
				float pPos[3] = { 0.0f, 0.0f, 0.0f };
				vect3_copy(g_SAMP->pPools->pPickup->pickup[i].fPosition, pPos);

				if (vect3_near_zero(pPos))
					continue;

				if (cheat_state->state == CHEAT_STATE_VEHICLE)  
					GTAfunc_RemoveActorFromCarAndPutAt(pPos);
				else
				{
					cheat_actor_teleport(self, pPos, 0);
					cheat_state->_generic.coordmaster = 0;
					cheat_state->_generic.pickchecker = 0;
					if (cheat_state->_generic.buyhouse)
					{
						say("/buyhouse");
					}
					GTAfunc_LockActor(0);
					break;
				}
			}
		}
	}
}

void cheat_stealcar()
{
	if (cheat_state->_generic.stealcar)
	{
		static int time = 0;
		if (GetTickCount() - time > 500)
		{
			struct vehicle_info *info = getGTAVehicleFromSAMPVehicleID(set.stcar);
			if (g_Vehicles->pSAMP_Vehicle[set.stcar]->iIsLocked)
			{
				float cPos[3] = { 0.0f, 0.0f, 0.0f };
				vect3_copy(&info->base.matrix[12], cPos);
				cPos[2] -= 1.8f;
				GTAfunc_LockActor(1);
				if (cheat_state->state == CHEAT_STATE_VEHICLE) 
					GTAfunc_RemoveActorFromCarAndPutAt(cPos);
				else cheat_teleport(cPos, gta_interior_id_get());
			}
			else
			{
				g_RakClient->SendEnterPlayerVehicle(set.stcar, 0);
				GTAfunc_PutActorInCar(info);
				info->m_nVehicleFlags.bEngineOn = 1;
				cheat_state->_generic.stealcar = 0;
				GTAfunc_TogglePlayerControllable(0);
				GTAfunc_LockActor(0);
				pGameInterface->GetCamera()->RestoreWithJumpCut();
			}
			time = GetTickCount();
		}
	}
}

/*void cheat_antipause()
{
	if (!memcmp_safe((uint8_t *)0x00561AF0, hex_to_bin("C60549CBB700C3C3"), 8))
	{
		static struct patch_set patch_DisableAntiPause =
		{
			"Disable anti-pause", 0, 0,
			{
				{ 8, (void *)0x00561AF0, NULL, (uint8_t *)"\xC6\x05\x49\xCB\xB7\x00\xC3\xC3", 0 },
			}
		};
		patcher_install(&patch_DisableAntiPause);
	}
}*/

void cheat_syncfcar()
{
	if(!cheat_state->_generic.sync)
		return;

	if(cheat_state->state != CHEAT_STATE_VEHICLE)
		return;

	static DWORD dwCount = 0;

	if (GetTickCount() < dwCount)
		return;

	float fPosition[3] = {
		pPedSelf->GetPosition()->fX,
		pPedSelf->GetPosition()->fY,
		pPedSelf->GetPosition()->fZ
	};

	float fSpeed[3] = { 0, 0, 0	};
	fPosition[2] -= 40.0f;
	g_RakClient->SendFakeOnfootSyncData(fPosition, actor_info_get(-1, 1)->hitpoints, fSpeed);

	dwCount = GetTickCount() + 500;
}

void cheat_mexbot()
{
	if (cheat_state->_generic.mexbot)
	{
		if (cheat_state->state == CHEAT_STATE_VEHICLE)
		{
			actor_info *self = actor_info_get(ACTOR_SELF, NULL);
			float last_pos[3] = { 0, 0, 0 };
			static uint16_t last_id = 0;
			static uint8_t refill = 0;
			getPlayerPos(last_id, last_pos);
			if (vect3_dist(last_pos, &self->base.matrix[12]) > 15.0f)
			{
				last_id = 0;
				refill = 0;
			}
			actor_info *info = actor_info_get(actor_find_nearest(ACTOR_ALIVE + ACTOR_STATE_DRIVING), NULL);
			if (vect3_dist(&info->base.matrix[12], &self->base.matrix[12]) < 5.0f)
			{
				uint16_t samp_id = getSAMPPlayerIDFromGTAPed(info);
				uint16_t veh_id = getPlayerSAMPVehicleID(samp_id);
				if (veh_id != NULL)
				{
					static uint32_t tick = 0;
					if (GetTickCount() > tick)
					{
						if (samp_id != last_id || refill)
						{
							if (!refill)
							{
								last_id = samp_id;
								say("/repair %d %d", samp_id, set.mex_price);
								refill = 1;
							}
							else
							{
								say("/refill %d", samp_id);
								refill = 0;
							}
						}
						tick = GetTickCount() + 1000;
					}
				}
			}
		}
	}
}

void sampAdditionalCheat()
{
	cheat_hint();
	cheat_colpuls();
	cheat_hppuls();
	cheat_killflood();
	cheat_lsamats();
	cheat_skillup();
	cheat_damager();
	cheat_farm();
	cheat_admins();
	cheat_friends();
	cheat_lockstatus();
	cheat_takecarbyid();
	cheat_autodriver();
	cheat_getguns();
	cheat_smsflood();
	cheat_flood();
	cheat_repflood();
	cheat_drugsuse();
	cheat_healme();
	cheat_boxer();
	cheat_coordmaster();
	cheat_cpmaster();
	cheat_pickchecker();
	cheat_stealcar();
	//cheat_antipause();
	cheat_syncfcar();
	cheat_mexbot();
}