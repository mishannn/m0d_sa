#include "main.h"

bool HookedRakClientInterface::Connect( const char* host, unsigned short serverPort, unsigned short clientPort, unsigned int depreciated, int threadSleepTimer )
{
	traceLastFunc( "HookedRakClientInterface::Connect" );

	return g_RakClient->GetRakClientInterface()->Connect( host, serverPort, clientPort, depreciated, threadSleepTimer );
}

void HookedRakClientInterface::Disconnect( unsigned int blockDuration, unsigned char orderingChannel )
{
	traceLastFunc( "HookedRakClientInterface::Disconnect" );

	g_RakClient->GetRakClientInterface()->Disconnect( blockDuration, orderingChannel );
}

void HookedRakClientInterface::InitializeSecurity( const char *privKeyP, const char *privKeyQ )
{
	traceLastFunc( "HookedRakClientInterface::InitializeSecurity" );

	g_RakClient->GetRakClientInterface()->InitializeSecurity( privKeyP, privKeyQ );
}

void HookedRakClientInterface::SetPassword( const char *_password )
{
	traceLastFunc( "HookedRakClientInterface::SetPassword" );

	g_RakClient->GetRakClientInterface()->SetPassword( _password );
}

bool HookedRakClientInterface::HasPassword( void ) const
{
	traceLastFunc( "HookedRakClientInterface::HasPassword" );

	return g_RakClient->GetRakClientInterface()->HasPassword();
}

bool HookedRakClientInterface::Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	traceLastFunc("HookedRakClientInterface::Send");

	return g_RakClient->GetRakClientInterface()->Send(data, length, priority, reliability, orderingChannel);
}

bool HookedRakClientInterface::Send( RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel )
{
	traceLastFunc( "HookedRakClientInterface::Send(RakNet::BitStream)" );

	BYTE packetId;
	bitStream->Read(packetId);

	if (set.netPatchAssoc[packetId][OUTCOMING_PACKET] != nullptr && set.netPatchAssoc[packetId][OUTCOMING_PACKET]->enabled)
		return false;

	//Log("< [Packet Send] id: %d, len: %d\n%s\n", packetId, bitStream->GetNumberOfBytesUsed(), DumpMem(bitStream->GetData(), bitStream->GetNumberOfBytesUsed()));
	switch (packetId)
	{
		case ID_PLAYER_SYNC:
		{
			stOnFootData *ofSync = (stOnFootData *)(bitStream->GetData() + 1);
			if (cheat_state->actor.fly_on || cheat_state->actor.air_brake)
			{
				static DWORD dwTime = 0;
				if (GetTickCount() - dwTime > 500)
				{
					vect3_zero(ofSync->fMoveSpeed);
					ofSync->sCurrentAnimationID = 1189;
					dwTime = GetTickCount();
				}
				else return 0;
			}
			if (cheat_state->_generic.noobinv)
			{
				vect3_zero(ofSync->fMoveSpeed);
				ofSync->fPosition[2] -= 50.0f;
			}
			if (cheat_state->_generic.piz)
			{
				struct actor_info *self = actor_info_get(-1, 0x01);
				ofSync->fMoveSpeed[0] = sinf(-self->fCurrentRotation) * set.piz_speed;
				ofSync->fMoveSpeed[1] = cosf(-self->fCurrentRotation) * set.piz_speed;
			}
			if (cheat_state->_generic.stealcar)
			{
				vect3_zero(ofSync->fMoveSpeed);
				ofSync->sKeys = 0x8;
			}
			if (cheat_state->_generic.newgodmode)
			{
				ofSync->byteHealth = 0;
			}
			if (cheat_state->_generic.coordmaster)
			{
				vect3_zero(ofSync->fMoveSpeed);
			}
		}
		break;

		case ID_VEHICLE_SYNC:
		{
			stInCarData *icSync = (stInCarData *)(bitStream->GetData() + 1);
			if (cheat_state->_generic.piz)
			{
				struct actor_info *self = actor_info_get(-1, 0x01);
				icSync->fMoveSpeed[0] = sinf(-self->fCurrentRotation) * set.piz_speed;
				icSync->fMoveSpeed[1] = cosf(-self->fCurrentRotation) * set.piz_speed;
			}
			if (cheat_state->_generic.newgodmode)
				icSync->bytePlayerHealth = 0;
			if (cheat_state->_generic.coordmaster)
				vect3_zero(icSync->fMoveSpeed);
		}
		break;

		case ID_BULLET_SYNC:
		{
			stBulletData *bSync = (stBulletData *)(bitStream->GetData() + 1);
			if (cheat_state->_generic.bullet)
			{
				if (bSync->byteType == 0)
				{
					uint16_t playerId = screen_actor_find_nearest();
					if (playerId > 0 && playerId < SAMP_MAX_PLAYERS)
					{
						struct actor_info *self = getGTAPedFromSAMPPlayerID(playerId);
						bSync->sTargetID = playerId;
						vect3_zero(bSync->fCenter);
						vect3_copy(&getGTAPedFromSAMPPlayerID(playerId)->base.matrix[12], bSync->fTarget);
						bSync->byteType = 1;
					}
				}
				else
					vect3_zero(bSync->fCenter);
			}
		}
		break;

		case ID_PASSENGER_SYNC:
		{
			stPassengerData *pSync = (stPassengerData *)(bitStream->GetData() + 1);
			if (cheat_state->_generic.newgodmode)
				pSync->byteHealth = 0;
		}
		break;

		case ID_UNOCCUPIED_SYNC:
		{
			stUnoccupiedData *uoSync = (stUnoccupiedData *)(bitStream->GetData() + 1);
			if (cheat_state->_generic.coordmaster)
				vect3_zero(uoSync->fMoveSpeed);
		}
		break;
	}

	return g_RakClient->GetRakClientInterface()->Send(bitStream, priority, reliability, orderingChannel);
}

Packet* HookedRakClientInterface::Receive(void)
{
	traceLastFunc("HookedRakClientInterface::Receive");

	Packet *packet = g_RakClient->GetRakClientInterface()->Receive();

	if (packet != NULL && packet->data != NULL)
	{
		BYTE packetId = (BYTE)packet->data[0];

		//Log("> [Packet Recv] id: %d len: %d\r\n%s\n", packetId, packet->length, DumpMem((unsigned char *)packet->data, packet->length));

		if (set.netPatchAssoc[packetId][INCOMING_PACKET] != nullptr && set.netPatchAssoc[packetId][INCOMING_PACKET]->enabled)
			return false;

		switch (packetId)
		{
			case ID_CONNECTION_LOST:
			case ID_DISCONNECTION_NOTIFICATION:
			{
												  if (cheat_state->_generic.autorejoin)
												  {
													  g_Dialog->iDialogShowed = 0;
													  cheat_state->_generic.autorejoin_on = true;
													  restartGame();
													  disconnect(500);
													  cheat_state_text("Переподключение через %d секунд...", set.rejoin_delay / 1000);
													  cheat_state->_generic.rejoinTick = GetTickCount();
												  }
												  break;
			}

			case ID_CONNECTION_BANNED:
			{
										 disconnect(0);
										 g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
			}
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
												   RakNet::BitStream bsData((unsigned char *)packet->data, packet->length, false);

												   bsData.IgnoreBits(8);
												   bsData.IgnoreBits(32);
												   bsData.IgnoreBits(16);
												   bsData.IgnoreBits(16);
												   bsData.Read(set.challenge);
			}
				break;
		}
	}

	return packet;
}

void HookedRakClientInterface::DeallocatePacket(Packet *packet)
{
	traceLastFunc("HookedRakClientInterface::DeallocatePacket");

	return g_RakClient->GetRakClientInterface()->DeallocatePacket(packet);
}

void HookedRakClientInterface::PingServer( void )
{
	traceLastFunc( "HookedRakClientInterface::PingServer" );

	g_RakClient->GetRakClientInterface()->PingServer();
}

void HookedRakClientInterface::PingServer( const char* host, unsigned short serverPort, unsigned short clientPort, bool onlyReplyOnAcceptingConnections )
{
	traceLastFunc( "HookedRakClientInterface::PingServer" );

	g_RakClient->GetRakClientInterface()->PingServer( host, serverPort, clientPort, onlyReplyOnAcceptingConnections );
}

int HookedRakClientInterface::GetAveragePing( void )
{
	traceLastFunc( "HookedRakClientInterface::GetAveragePing" );

	return g_RakClient->GetRakClientInterface()->GetAveragePing();
}

int HookedRakClientInterface::GetLastPing( void ) const
{
	traceLastFunc( "HookedRakClientInterface::GetLastPing" );

	return g_RakClient->GetRakClientInterface()->GetLastPing();
}

int HookedRakClientInterface::GetLowestPing( void ) const
{		
	traceLastFunc( "HookedRakClientInterface::GetLowestPing" );

	return g_RakClient->GetRakClientInterface()->GetLowestPing();
}

int HookedRakClientInterface::GetPlayerPing( const PlayerID playerId )
{
	traceLastFunc( "HookedRakClientInterface::GetPlayerPing" );

	return g_RakClient->GetRakClientInterface()->GetPlayerPing( playerId );
}

void HookedRakClientInterface::StartOccasionalPing( void )
{
	traceLastFunc( "HookedRakClientInterface::StartOccasionalPing" );

	g_RakClient->GetRakClientInterface()->StartOccasionalPing();
}

void HookedRakClientInterface::StopOccasionalPing( void )
{
	traceLastFunc( "HookedRakClientInterface::StopOccasionalPing" );

	g_RakClient->GetRakClientInterface()->StopOccasionalPing();
}

bool HookedRakClientInterface::IsConnected( void ) const
{
	traceLastFunc( "HookedRakClientInterface::IsConnected" );

	return g_RakClient->GetRakClientInterface()->IsConnected();
}

unsigned int HookedRakClientInterface::GetSynchronizedRandomInteger( void ) const
{
	traceLastFunc( "HookedRakClientInterface::GetSynchronizedRandomInteger" );

	return g_RakClient->GetRakClientInterface()->GetSynchronizedRandomInteger();
}

bool HookedRakClientInterface::GenerateCompressionLayer( unsigned int inputFrequencyTable[ 256 ], bool inputLayer )
{
	traceLastFunc( "HookedRakClientInterface::GenerateCompressionLayer" );

	return g_RakClient->GetRakClientInterface()->GenerateCompressionLayer( inputFrequencyTable, inputLayer );
}

bool HookedRakClientInterface::DeleteCompressionLayer( bool inputLayer )
{
	traceLastFunc( "HookedRakClientInterface::DeleteCompressionLayer" );

	return g_RakClient->GetRakClientInterface()->DeleteCompressionLayer( inputLayer );
}

void HookedRakClientInterface::RegisterAsRemoteProcedureCall( int* uniqueID, void ( *functionPointer ) ( RPCParameters *rpcParms ) )
{
	traceLastFunc( "HookedRakClientInterface::RegisterAsRemoteProcedureCall" );

	g_RakClient->GetRakClientInterface()->RegisterAsRemoteProcedureCall( uniqueID, functionPointer );
}

void HookedRakClientInterface::RegisterClassMemberRPC( int* uniqueID, void *functionPointer )
{
	traceLastFunc( "HookedRakClientInterface::RegisterClassMemberRPC" );

	g_RakClient->GetRakClientInterface()->RegisterClassMemberRPC( uniqueID, functionPointer );
}

void HookedRakClientInterface::UnregisterAsRemoteProcedureCall( int* uniqueID )
{
	traceLastFunc( "HookedRakClientInterface::UnregisterAsRemoteProcedureCall" );

	g_RakClient->GetRakClientInterface()->UnregisterAsRemoteProcedureCall( uniqueID );
}

bool HookedRakClientInterface::RPC( int* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp )
{
	traceLastFunc( "HookedRakClientInterface::RPC" );

	return g_RakClient->GetRakClientInterface()->RPC( uniqueID, data, bitLength, priority, reliability, orderingChannel, shiftTimestamp );
}

bool HookedRakClientInterface::RPC(int* uniqueID, RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	traceLastFunc("HookedRakClientInterface::RPC(RakNet::BitStream)");

	if (set.netPatchAssoc[*uniqueID][OUTCOMING_RPC] != nullptr && set.netPatchAssoc[*uniqueID][OUTCOMING_RPC]->enabled)
		return false;

	//Log("< [RPC Send] id: %d len: %d\n%s\n", *uniqueID, parameters->GetNumberOfBytesUsed(), DumpMem(parameters->GetData(), parameters->GetNumberOfBytesUsed()));

	if (*uniqueID == RPC_Spawn)
	{
		if (set.spawnsethp)
			set.spawnsethp = false;

		if (cheat_state->_generic.autorejoin_on)
			cheat_state->_generic.autorejoin_on = false;
	}
	
	return g_RakClient->GetRakClientInterface()->RPC(uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp);
}

void HookedRakClientInterface::SetTrackFrequencyTable( bool b )
{
	traceLastFunc( "HookedRakClientInterface::SetTrackFrequencyTable" );

	g_RakClient->GetRakClientInterface()->SetTrackFrequencyTable( b );
}

bool HookedRakClientInterface::GetSendFrequencyTable( unsigned int outputFrequencyTable[ 256 ] )
{
	traceLastFunc( "HookedRakClientInterface::GetSendFrequencyTable" );

	return g_RakClient->GetRakClientInterface()->GetSendFrequencyTable( outputFrequencyTable );
}

float HookedRakClientInterface::GetCompressionRatio( void ) const 
{
	traceLastFunc( "HookedRakClientInterface::GetCompressionRatio" );

	return g_RakClient->GetRakClientInterface()->GetCompressionRatio();
}

float HookedRakClientInterface::GetDecompressionRatio( void ) const
{
	traceLastFunc( "HookedRakClientInterface::GetDecompressionRatio" );

	return g_RakClient->GetRakClientInterface()->GetDecompressionRatio();
}

void HookedRakClientInterface::AttachPlugin(PluginInterface *messageHandler)
{
	traceLastFunc( "HookedRakClientInterface::AttachPlugin" );

	g_RakClient->GetRakClientInterface()->AttachPlugin(messageHandler);
}

void HookedRakClientInterface::DetachPlugin( PluginInterface *messageHandler )
{
	traceLastFunc( "HookedRakClientInterface::DetachPlugin" );

	g_RakClient->GetRakClientInterface()->DetachPlugin(messageHandler);
}

RakNet::BitStream * HookedRakClientInterface::GetStaticServerData( void )
{
	traceLastFunc( "HookedRakClientInterface::GetStaticServerData" );

	return g_RakClient->GetRakClientInterface()->GetStaticServerData();
}

void HookedRakClientInterface::SetStaticServerData( const char *data, const int length )
{
	traceLastFunc( "HookedRakClientInterface::SetStaticServerData" );

	g_RakClient->GetRakClientInterface()->SetStaticServerData( data, length );
}

RakNet::BitStream * HookedRakClientInterface::GetStaticClientData( const PlayerID playerId )
{	
	traceLastFunc( "HookedRakClientInterface::GetStaticClientData" );

	return g_RakClient->GetRakClientInterface()->GetStaticClientData( playerId );
}

void HookedRakClientInterface::SetStaticClientData( const PlayerID playerId, const char *data, const int length )
{
	traceLastFunc( "HookedRakClientInterface::SetStaticClientData" );

	g_RakClient->GetRakClientInterface()->SetStaticClientData( playerId, data, length );
}

void HookedRakClientInterface::SendStaticClientDataToServer( void )
{
	traceLastFunc( "HookedRakClientInterface::SendStaticClientDataToServer" );

	g_RakClient->GetRakClientInterface()->SendStaticClientDataToServer();
}

PlayerID HookedRakClientInterface::GetServerID( void ) const
{
	traceLastFunc( "HookedRakClientInterface::GetServerID" );

	return g_RakClient->GetRakClientInterface()->GetServerID();
}

PlayerID HookedRakClientInterface::GetPlayerID( void ) const
{
	traceLastFunc( "HookedRakClientInterface::GetPlayerID" );

	return g_RakClient->GetRakClientInterface()->GetPlayerID();
}

PlayerID HookedRakClientInterface::GetInternalID( void ) const
{
	traceLastFunc( "HookedRakClientInterface::GetInternalID" );

	return g_RakClient->GetRakClientInterface()->GetInternalID();
}

const char* HookedRakClientInterface::PlayerIDToDottedIP( const PlayerID playerId ) const
{
	traceLastFunc( "HookedRakClientInterface::PlayerIDToDottedIP" );

	return g_RakClient->GetRakClientInterface()->PlayerIDToDottedIP( playerId );
}

void HookedRakClientInterface::PushBackPacket( Packet *packet, bool pushAtHead )
{
	traceLastFunc( "HookedRakClientInterface::PushBackPacket" );

	g_RakClient->GetRakClientInterface()->PushBackPacket(packet, pushAtHead);
}

void HookedRakClientInterface::SetRouterInterface( RouterInterface *routerInterface )
{
	traceLastFunc( "HookedRakClientInterface::SetRouterInterface" );

	g_RakClient->GetRakClientInterface()->SetRouterInterface(routerInterface);
}
void HookedRakClientInterface::RemoveRouterInterface(RouterInterface *routerInterface)
{
	traceLastFunc( "HookedRakClientInterface::RemoveRouterInterface" );

	g_RakClient->GetRakClientInterface()->RemoveRouterInterface(routerInterface);
}

void HookedRakClientInterface::SetTimeoutTime( RakNetTime timeMS )
{
	traceLastFunc( "HookedRakClientInterface::SetTimeoutTime" );

	g_RakClient->GetRakClientInterface()->SetTimeoutTime( timeMS );
}

bool HookedRakClientInterface::SetMTUSize( int size )
{
	traceLastFunc( "HookedRakClientInterface::SetMTUSize" );

	return g_RakClient->GetRakClientInterface()->SetMTUSize( size );
}

int HookedRakClientInterface::GetMTUSize( void ) const
{
	traceLastFunc( "HookedRakClientInterface::GetMTUSize" );

	return g_RakClient->GetRakClientInterface()->GetMTUSize();
}

void HookedRakClientInterface::AllowConnectionResponseIPMigration( bool allow )
{
	traceLastFunc( "HookedRakClientInterface::AllowConnectionResponseIPMigration" );

	g_RakClient->GetRakClientInterface()->AllowConnectionResponseIPMigration( allow );
}

void HookedRakClientInterface::AdvertiseSystem( const char *host, unsigned short remotePort, const char *data, int dataLength )
{
	traceLastFunc( "HookedRakClientInterface::AdvertiseSystem" );

	g_RakClient->GetRakClientInterface()->AdvertiseSystem( host, remotePort, data, dataLength );
}

RakNetStatisticsStruct* const HookedRakClientInterface::GetStatistics( void )
{
	traceLastFunc( "HookedRakClientInterface::GetStatistics" );

	return g_RakClient->GetRakClientInterface()->GetStatistics();
}

void HookedRakClientInterface::ApplyNetworkSimulator( double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance)
{
	traceLastFunc( "HookedRakClientInterface::ApplyNetworkSimulator" );

	g_RakClient->GetRakClientInterface()->ApplyNetworkSimulator( maxSendBPS, minExtraPing, extraPingVariance );
}

bool HookedRakClientInterface::IsNetworkSimulatorActive( void )
{
	traceLastFunc( "HookedRakClientInterface::IsNetworkSimulatorActive" );

	return g_RakClient->GetRakClientInterface()->IsNetworkSimulatorActive();
}

PlayerIndex HookedRakClientInterface::GetPlayerIndex( void )
{
	traceLastFunc( "HookedRakClientInterface::GetPlayerIndex" );

	return g_RakClient->GetRakClientInterface()->GetPlayerIndex();
}

void HookedRakClientInterface::Pad( void )
{
	DWORD at = (DWORD)_ReturnAddress();
	Log( "HookedRakClientInterface::Pad has been called. Return address: %p", at );
}