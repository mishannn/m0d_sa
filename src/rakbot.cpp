#include "main.h"

unsigned short botCount;
stRakBotSettings botSettings[SAMP_MAX_PLAYERS];

void CRakBot::Thread(void *)
{
	while (true)
	{
		botCount = 0;
		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (botSettings[i].bIsActive == false)
				continue;

			UpdateNetwork(i);
			botCount++;
		}
		Sleep(40);
	}
}

void CRakBot::Connect(char *szNickName, char *szPassword)
{
	unsigned short botID = 0;
	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (botSettings[i].bIsActive == false)
		{
			botID = i;
			break;
		}
	}
	botSettings[botID].pRakClient = RakNetworkFactory::GetRakClientInterface();
	if (botSettings[botID].pRakClient == NULL)
	{
		Log("Ошибка инициализации RakClientInterface <> Бот %d", botID);
		return;
	}
	botSettings[botID].pRakClient->SetMTUSize(576);
	Log("Подключение к %s:%d", g_SAMP->szIP, g_SAMP->ulPort);
	strcpy(botSettings[botID].szNickName, szNickName);
	botSettings[botID].pRakClient->SetPassword(szPassword);
	botSettings[botID].pRakClient->Connect(g_SAMP->szIP, g_SAMP->ulPort, 0, 0, 5);
	botSettings[botID].bIsActive = 1;
}

void CRakBot::Disconnect(unsigned short botID)
{
	botSettings[botID].pRakClient->Disconnect(0);
	botSettings[botID].iIsConnected = 0;
	ZeroMemory(&botSettings[botID],
		sizeof(stRakBotSettings));
	botSettings[botID].bIsActive = 0;
}

void CRakBot::UpdateNetwork(unsigned short botID)
{
	RakClientInterface *pRakClient = botSettings[botID].pRakClient;
	unsigned char packetId;
	Packet *pkt;

	while (pkt = pRakClient->Receive())
	{
		if ((unsigned char)pkt->data[0] == ID_TIMESTAMP)
		{
			if (pkt->length > sizeof(unsigned char)+sizeof(unsigned int))
				packetId = (unsigned char)pkt->data[sizeof(unsigned char)+sizeof(unsigned int)];
			else
				return;
		}
		else packetId = (unsigned char)pkt->data[0];

		if (packetId == ID_DISCONNECTION_NOTIFICATION)
		{
			Log("Соединение было потеряно <> Бот %d", botID);
			Disconnect(botID);
		}
		else if (packetId == ID_CONNECTION_BANNED)
		{
			Log("Вы забанены на этом сервере <> Бот %d", botID);
			Disconnect(botID);
		}
		else if (packetId == ID_CONNECTION_ATTEMPT_FAILED)
		{
			Log("Не удалось подключиться к серверу <> Бот %d", botID);
			Disconnect(botID);
		}
		else if (packetId == ID_NO_FREE_INCOMING_CONNECTIONS)
		{
			Log("Сервер заполнен <> Бот %d", botID);
			Disconnect(botID);
		}
		else if (packetId == ID_INVALID_PASSWORD)
		{
			Log("Неверный пароль сервера <> Бот %d", botID);
			Disconnect(botID);
		}
		else if (packetId == ID_CONNECTION_LOST)
		{
			Log("Соединение было потеряно <> Бот %d", botID);
			Disconnect(botID);
		}
		else if (packetId == ID_AUTH_KEY)
		{
			char *szAuthKey = GenAuthKey((char *)(pkt->data + 2));
			BYTE byteAuthKeyLen = (BYTE)strlen(szAuthKey);
			RakNet::BitStream bsKey;
			bsKey.Write((BYTE)ID_AUTH_KEY);
			bsKey.Write((BYTE)byteAuthKeyLen);
			bsKey.Write(szAuthKey, byteAuthKeyLen);

			pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);
		}
		else if (packetId == ID_CONNECTION_REQUEST_ACCEPTED)
		{
			RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);

			bsSuccAuth.IgnoreBits(8);
			bsSuccAuth.IgnoreBits(32);
			bsSuccAuth.IgnoreBits(16);

			bsSuccAuth.Read(botSettings[botID].usID);
			bsSuccAuth.Read(botSettings[botID].uiChallenge);

			Log("Подключено <> Бот %d", botID);

			int iVersion = NETGAME_VERSION;
			BYTE byteMod = 1;
			BYTE byteAuthBSLen = (BYTE)strlen(AUTH_BS);
			BYTE byteNameLen = (BYTE)strlen(botSettings[botID].szNickName);
			BYTE byteClientVerLen = (BYTE)strlen(CLIENT_VERSION);

			unsigned int uiClientChallengeResponse = botSettings[botID].uiChallenge ^ iVersion;

			RakNet::BitStream bsSend;
			bsSend.Write(iVersion);
			bsSend.Write(byteMod);
			bsSend.Write(byteNameLen);
			bsSend.Write(botSettings[botID].szNickName, byteNameLen);

			bsSend.Write(uiClientChallengeResponse);
			bsSend.Write(byteAuthBSLen);
			bsSend.Write(AUTH_BS, byteAuthBSLen);
			bsSend.Write(byteClientVerLen);
			bsSend.Write(CLIENT_VERSION, byteClientVerLen);
			bsSend.Write(uiClientChallengeResponse);

			pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE);

			botSettings[botID].iIsConnected = true;
		}
		pRakClient->DeallocatePacket(pkt);
	}
}