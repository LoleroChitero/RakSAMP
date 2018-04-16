/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

extern int iFollowingPassenger, iFollowingDriver;
extern int iDrunkLevel, iMoney, iLocalPlayerSkin;
extern BYTE m_bLagCompensation;

DWORD dwTimeReconnect = 10000;

int iPassengerNotificationSent = 0, iDriverNotificationSent = 0;

void Packet_AUTH_KEY(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsAuth((unsigned char *)p->data, p->length, false);

	BYTE byteAuthLen;
	char szAuth[260];

	bsAuth.IgnoreBits(8); // ID_AUTH_KEY
	bsAuth.Read(byteAuthLen);
	bsAuth.Read(szAuth, byteAuthLen);
	szAuth[byteAuthLen] = '\0';

/*
	char* auth_key;
	bool found_key = false;

	for(int x = 0; x < 512; x++)
	{
		if(!strcmp(szAuth, AuthKeyTable[x][0]))
		{
			auth_key = AuthKeyTable[x][1];
			found_key = true;
		}
	}

	if(found_key)
	{
		RakNet::BitStream bsKey;
		BYTE byteAuthKeyLen;

		byteAuthKeyLen = (BYTE)strlen(auth_key);
		
		bsKey.Write((BYTE)ID_AUTH_KEY);
		bsKey.Write((BYTE)byteAuthKeyLen);
		bsKey.Write(auth_key, byteAuthKeyLen);

		pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);

		Log("[AUTH] %s -> %s", szAuth, auth_key);
	}
	else
	{
		Log("Unknown AUTH_IN! (%s)", ((char*)p->data + 2));
	}
*/

	char szAuthKey[260];

	if (settings.iNPC)
		strcpy(szAuthKey, "NPC");	
	else
		gen_auth_key(szAuthKey, szAuth);

	RakNet::BitStream bsKey;
	BYTE byteAuthKeyLen = (BYTE)strlen(szAuthKey);

	bsKey.Write((BYTE)ID_AUTH_KEY);
	bsKey.Write((BYTE)byteAuthKeyLen);
	bsKey.Write(szAuthKey, byteAuthKeyLen);

	pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);

	Log("[AUTH] %s -> %s", szAuth, szAuthKey);
}

void Packet_ConnectionSucceeded(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsSuccAuth((unsigned char *)p->data, p->length, false);
	PLAYERID myPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port

	bsSuccAuth.Read(myPlayerID);

	g_myPlayerID = myPlayerID;
	playerInfo[myPlayerID].iIsConnected = 1;

	if (settings.iNPC)
		playerInfo[myPlayerID].byteIsNPC = 1;

	strcpy(playerInfo[myPlayerID].szPlayerName, g_szNickName);

	bsSuccAuth.Read(uiChallenge);

	settings.uiChallange = uiChallenge;

	Log("Connected. Joining the game...");

	int iVersion = NETGAME_VERSION;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;
	BYTE byteMod = 1;

	char auth_bs[4*16] = {0};
	gen_gpci(auth_bs, 0x3e9);

	BYTE byteAuthBSLen;
	byteAuthBSLen = (BYTE)strlen(auth_bs);
	BYTE byteNameLen = (BYTE)strlen(g_szNickName);
	BYTE iClientVerLen = (BYTE)strlen(settings.szClientVersion);

	RakNet::BitStream bsSend;

	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(g_szNickName, byteNameLen);
	bsSend.Write(uiClientChallengeResponse);

	if (!settings.iNPC)
	{
		bsSend.Write(byteAuthBSLen);
		bsSend.Write(auth_bs, byteAuthBSLen);
		bsSend.Write(iClientVerLen);
		bsSend.Write(settings.szClientVersion, iClientVerLen);
	}

	pRakClient->RPC(settings.iNPC ? &RPC_NPCJoin : &RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);

	iAreWeConnected = 1;
}

void Packet_PlayerSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsPlayerSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	//Log("Packet_PlayerSync: %d \n%s\n", p->length, DumpMem((unsigned char *)p->data, p->length));

	bool bHasLR, bHasUD;
	bool bHasSurfInfo, bAnimation;

	bsPlayerSync.IgnoreBits(8);
	bsPlayerSync.Read(playerId);

	if(playerId < 0 || playerId >= MAX_PLAYERS) return;

	// Followed passenger exit
	if(settings.runMode == RUNMODE_FOLLOWPLAYER && playerId == getPlayerIDFromPlayerName(settings.szFollowingPlayerName))
	{
		if(iPassengerNotificationSent)
		{
			SendExitVehicleNotification(playerInfo[playerId].incarData.VehicleID);
			iPassengerNotificationSent = 0;
		}

		iFollowingPassenger = 0;

		if(iDriverNotificationSent)
		{
			SendExitVehicleNotification(playerInfo[playerId].incarData.VehicleID);
			iDriverNotificationSent = 0;
		}

		iFollowingDriver = 0;
	}
	playerInfo[playerId].incarData.VehicleID = -1;


	// clear last data
	memset(&playerInfo[playerId].onfootData, 0, sizeof(ONFOOT_SYNC_DATA));

	// LEFT/RIGHT KEYS
	bsPlayerSync.Read(bHasLR);
	if(bHasLR) bsPlayerSync.Read(playerInfo[playerId].onfootData.lrAnalog);

	// UP/DOWN KEYS
	bsPlayerSync.Read(bHasUD);
	if(bHasUD) bsPlayerSync.Read(playerInfo[playerId].onfootData.udAnalog);

	// GENERAL KEYS
	bsPlayerSync.Read(playerInfo[playerId].onfootData.wKeys);

	// VECTOR POS
	bsPlayerSync.Read(playerInfo[playerId].onfootData.vecPos[0]);
	bsPlayerSync.Read(playerInfo[playerId].onfootData.vecPos[1]);
	bsPlayerSync.Read(playerInfo[playerId].onfootData.vecPos[2]);

	// ROTATION
	bsPlayerSync.ReadNormQuat(
		playerInfo[playerId].onfootData.fQuaternion[0],
		playerInfo[playerId].onfootData.fQuaternion[1],
		playerInfo[playerId].onfootData.fQuaternion[2],
		playerInfo[playerId].onfootData.fQuaternion[3]);
	

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	BYTE byteHealthArmour;
	BYTE byteHealth, byteArmour;
	BYTE byteArmTemp=0,byteHlTemp=0;

	bsPlayerSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) byteArmour = 100;
	else if(byteArmTemp == 0) byteArmour = 0;
	else byteArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) byteHealth = 100;
	else if(byteHlTemp == 0) byteHealth = 0;
	else byteHealth = byteHlTemp * 7;

	playerInfo[playerId].onfootData.byteHealth = byteHealth;
	playerInfo[playerId].onfootData.byteArmour = byteArmour;

	// CURRENT WEAPON
	bsPlayerSync.Read(playerInfo[playerId].onfootData.byteCurrentWeapon);

	// Special Action
	bsPlayerSync.Read(playerInfo[playerId].onfootData.byteSpecialAction);

	// READ MOVESPEED VECTORS
	bsPlayerSync.ReadVector(
		playerInfo[playerId].onfootData.vecMoveSpeed[0],
		playerInfo[playerId].onfootData.vecMoveSpeed[1],
		playerInfo[playerId].onfootData.vecMoveSpeed[2]);

	bsPlayerSync.Read(bHasSurfInfo);
	if(bHasSurfInfo)
	{
		bsPlayerSync.Read(playerInfo[playerId].onfootData.wSurfInfo);
		bsPlayerSync.Read(playerInfo[playerId].onfootData.vecSurfOffsets[0]);
		bsPlayerSync.Read(playerInfo[playerId].onfootData.vecSurfOffsets[1]);
		bsPlayerSync.Read(playerInfo[playerId].onfootData.vecSurfOffsets[2]);
	}
	else
		playerInfo[playerId].onfootData.wSurfInfo = -1;

	bsPlayerSync.Read(bAnimation);
	if(bAnimation)
		bsPlayerSync.Read(playerInfo[playerId].onfootData.iCurrentAnimationID);
}

//----------------------------------------------------

void Packet_UnoccupiedSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsUnocSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	//Log("\n%s\n", DumpMem((unsigned char *)p->data + bsUnocSync.GetReadOffset() / 8, p->length));

	bsUnocSync.IgnoreBits(8);
	bsUnocSync.Read(playerId);

	if(playerId < 0 || playerId >= MAX_PLAYERS) return;

	memset(&playerInfo[playerId].unocData, 0, sizeof(UNOCCUPIED_SYNC_DATA));

	bsUnocSync.Read((char *)&playerInfo[playerId].unocData, sizeof(UNOCCUPIED_SYNC_DATA));
}

void Packet_AimSync(Packet *p, RakClientInterface *pRakClient)
{  
	RakNet::BitStream bsAimSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	//Log("Packet_AimSync:\n%s\n", DumpMem((unsigned char *)p->data, p->length));

	bsAimSync.IgnoreBits(8);
	bsAimSync.Read(playerId);

	if(playerId < 0 || playerId >= MAX_PLAYERS) return;

	memset(&playerInfo[playerId].aimData, 0, sizeof(AIM_SYNC_DATA));

	bsAimSync.Read((PCHAR)&playerInfo[playerId].aimData, sizeof(AIM_SYNC_DATA));
}

void Packet_VehicleSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId;

	VEHICLEID VehicleID;
	bool bLandingGear;
	bool bHydra,bTrain,bTrailer;
	bool bSiren;

	//Log("Packet_VehicleSync: %d \n%s\n", p->length, DumpMem((unsigned char *)p->data, p->length));

	bsSync.IgnoreBits(8);
	bsSync.Read(playerId);
	bsSync.Read(VehicleID);

	if(playerId < 0 || playerId >= MAX_PLAYERS) return;
	if(VehicleID < 0 || VehicleID >= MAX_VEHICLES) return;

	// Follower passenger enter
	playerInfo[playerId].incarData.VehicleID = VehicleID;
	if(settings.runMode == RUNMODE_FOLLOWPLAYER && playerId == getPlayerIDFromPlayerName(settings.szFollowingPlayerName))
	{
		if(!iPassengerNotificationSent)
		{
			SendEnterVehicleNotification(VehicleID, 1);
			iPassengerNotificationSent = 1;
		}
		
		SendPassengerFullSyncData(VehicleID);
		iFollowingPassenger = 1;
		return;
	}


	// clear last data
	memset(&playerInfo[playerId].incarData, 0, sizeof(INCAR_SYNC_DATA));

	// LEFT/RIGHT KEYS
	bsSync.Read(playerInfo[playerId].incarData.lrAnalog);

	// UP/DOWN KEYS
	bsSync.Read(playerInfo[playerId].incarData.udAnalog);

	// GENERAL KEYS
	bsSync.Read(playerInfo[playerId].incarData.wKeys);

	// ROLL / DIRECTION
	// ROTATION
	bsSync.ReadNormQuat(
		playerInfo[playerId].incarData.fQuaternion[0],
		playerInfo[playerId].incarData.fQuaternion[1],
		playerInfo[playerId].incarData.fQuaternion[2],
		playerInfo[playerId].incarData.fQuaternion[3]);

	// POSITION
	bsSync.Read(playerInfo[playerId].incarData.vecPos[0]);
	bsSync.Read(playerInfo[playerId].incarData.vecPos[1]);
	bsSync.Read(playerInfo[playerId].incarData.vecPos[2]);

	// SPEED
	bsSync.ReadVector(
		playerInfo[playerId].incarData.vecMoveSpeed[0],
		playerInfo[playerId].incarData.vecMoveSpeed[1],
		playerInfo[playerId].incarData.vecMoveSpeed[2]);

	// VEHICLE HEALTH
	WORD wTempVehicleHealth;
	bsSync.Read(wTempVehicleHealth);
	playerInfo[playerId].incarData.fCarHealth = (float)wTempVehicleHealth;

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	BYTE byteHealthArmour;
	BYTE bytePlayerHealth, bytePlayerArmour;
	BYTE byteArmTemp=0,byteHlTemp=0;

	bsSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) bytePlayerArmour = 100;
	else if(byteArmTemp == 0) bytePlayerArmour = 0;
	else bytePlayerArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) bytePlayerHealth = 100;
	else if(byteHlTemp == 0) bytePlayerHealth = 0;
	else bytePlayerHealth = byteHlTemp * 7;

	playerInfo[playerId].incarData.bytePlayerHealth = bytePlayerHealth;
	playerInfo[playerId].incarData.bytePlayerArmour = bytePlayerArmour;

	// CURRENT WEAPON
	bsSync.Read(playerInfo[playerId].incarData.byteCurrentWeapon);

	// SIREN
	bsSync.ReadCompressed(bSiren);
	if(bSiren)
		playerInfo[playerId].incarData.byteSirenOn = 1;

	// LANDING GEAR
	bsSync.ReadCompressed(bLandingGear);
	if(bLandingGear)
		playerInfo[playerId].incarData.byteLandingGearState = 1;

	// HYDRA THRUST ANGLE AND TRAILER ID
	bsSync.ReadCompressed(bHydra);
	bsSync.ReadCompressed(bTrailer);

	DWORD dwTrailerID_or_ThrustAngle;
	bsSync.Read(dwTrailerID_or_ThrustAngle);
	playerInfo[playerId].incarData.TrailerID_or_ThrustAngle = (WORD)dwTrailerID_or_ThrustAngle;

	// TRAIN SPECIAL
	WORD wSpeed;
	bsSync.ReadCompressed(bTrain);
	if(bTrain)
	{
		bsSync.Read(wSpeed);
		playerInfo[playerId].incarData.fTrainSpeed = (float)wSpeed;
	}
}

void Packet_PassengerSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsPassengerSync((unsigned char *)p->data, p->length, false);
	PLAYERID	playerId;
	PASSENGER_SYNC_DATA psSync;

	bsPassengerSync.IgnoreBits(8);
	bsPassengerSync.Read(playerId);

	if(playerId < 0 || playerId >= MAX_PLAYERS) return;

	bsPassengerSync.Read((PCHAR)&psSync,sizeof(PASSENGER_SYNC_DATA));

	// Followed wants to drive the vehicle
	playerInfo[playerId].passengerData.VehicleID = psSync.VehicleID;
	if(settings.runMode == RUNMODE_FOLLOWPLAYER && playerId == getPlayerIDFromPlayerName(settings.szFollowingPlayerName))
	{
		if(!iDriverNotificationSent)
		{
			SendEnterVehicleNotification(psSync.VehicleID, 0);
			iDriverNotificationSent = 1;
		}

		INCAR_SYNC_DATA icSync;
		memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));
		icSync.VehicleID = psSync.VehicleID;
		icSync.fCarHealth = 1000.00f;
		icSync.bytePlayerHealth = (BYTE)settings.fPlayerHealth;
		icSync.bytePlayerArmour = (BYTE)settings.fPlayerArmour;
		SendInCarFullSyncData(&icSync, 1, -1);

		iFollowingDriver = 1;
		return;
	}
}

void Packet_TrailerSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsSpectatorSync((unsigned char *)p->data, p->length, false);

	PLAYERID playerId;
	//TRAILER_SYNC_DATA trSync;

	bsSpectatorSync.IgnoreBits(8);
	bsSpectatorSync.Read(playerId);
	//bsSpectatorSync.Read((PCHAR)&trSync, sizeof(TRAILER_SYNC_DATA));
}

void Packet_MarkersSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsMarkersSync((unsigned char *)p->data, p->length, false);

	int i, iNumberOfPlayers;
	PLAYERID playerID;
	short sPosX, sPosY, sPosZ;
	bool bIsPlayerActive;

	bsMarkersSync.IgnoreBits(8);
	bsMarkersSync.Read(iNumberOfPlayers);

	if(iNumberOfPlayers < 0 || iNumberOfPlayers > MAX_PLAYERS) return;

	for(i = 0; i < iNumberOfPlayers; i++)
	{
		bsMarkersSync.Read(playerID);

		if(playerID < 0 || playerID >= MAX_PLAYERS) return;

		bsMarkersSync.ReadCompressed(bIsPlayerActive);
		if(bIsPlayerActive == 0)
		{
			playerInfo[playerID].iGotMarkersPos = 0;
			continue;
		}

		bsMarkersSync.Read(sPosX);
		bsMarkersSync.Read(sPosY);
		bsMarkersSync.Read(sPosZ);

		playerInfo[playerID].iGotMarkersPos = 1;
		playerInfo[playerID].onfootData.vecPos[0] = (float)sPosX;
		playerInfo[playerID].onfootData.vecPos[1] = (float)sPosY;
		playerInfo[playerID].onfootData.vecPos[2] = (float)sPosZ;

		//Log("Packet_MarkersSync: %d %d %0.2f, %0.2f, %0.2f", playerID, bIsPlayerActive, (float)sPosX, (float)sPosY, (float)sPosZ);
	}
}

void Packet_BulletSync(Packet *p, RakClientInterface *pRakClient)
{
	RakNet::BitStream bsBulletSync((unsigned char *)p->data, p->length, false);

	if(m_bLagCompensation)
	{
		PLAYERID PlayerID;

		bsBulletSync.IgnoreBits(8);
		bsBulletSync.Read(PlayerID);

		if(PlayerID < 0 || PlayerID >= MAX_PLAYERS) return;

		memset(&playerInfo[PlayerID].bulletData, 0, sizeof(BULLET_SYNC_DATA));

		bsBulletSync.Read((PCHAR)&playerInfo[PlayerID].bulletData, sizeof(BULLET_SYNC_DATA));

		PLAYERID copyingID = getPlayerIDFromPlayerName(settings.szFollowingPlayerName);

		if(copyingID != (PLAYERID)-1 && (settings.runMode == RUNMODE_FOLLOWPLAYER || settings.runMode == RUNMODE_FOLLOWPLAYERSVEHICLE))
		{
			if(copyingID == PlayerID)
				SendBulletData(&playerInfo[PlayerID].bulletData);
		}
	}
}

void resetPools(int iRestart, DWORD dwTimeReconnect)
{
	memset(playerInfo, 0, sizeof(stPlayerInfo));
	memset(vehiclePool, 0, sizeof(stVehiclePool));

	if(iRestart)
	{
		iAreWeConnected = 0;
		iConnectionRequested = 0;
		iSpawned = 0;
		iMoney = 0;
		iDrunkLevel = 0;
		iLocalPlayerSkin = 0;

		settings.bPulsator = false;

		settings.fPlayerHealth = 100.0f;
		settings.fPlayerArmour = 0.0f;

		Sleep(dwTimeReconnect);
	}
}

void UpdatePlayerScoresAndPings(int iWait, int iMS, RakClientInterface *pRakClient)
{
	static DWORD dwLastUpdateTick = 0;

	if(iWait)
	{
		if ((GetTickCount() - dwLastUpdateTick) > (DWORD)iMS)
		{
			dwLastUpdateTick = GetTickCount();
			RakNet::BitStream bsParams;
			pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		}
	}
	else
	{
		RakNet::BitStream bsParams;
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void UpdateNetwork(RakClientInterface *pRakClient)
{
	unsigned char packetIdentifier;
	Packet *pkt;

	while(pkt = pRakClient->Receive())
	{
		if ( ( unsigned char ) pkt->data[ 0 ] == ID_TIMESTAMP )
		{
			if ( pkt->length > sizeof( unsigned char ) + sizeof( unsigned int ) )
				packetIdentifier = ( unsigned char ) pkt->data[ sizeof( unsigned char ) + sizeof( unsigned int ) ];
			else
				return;
		}
		else
			packetIdentifier = ( unsigned char ) pkt->data[ 0 ];

		//Log("[RAKSAMP] Packet received. PacketID: %d.", pkt->data[0]);

		switch(packetIdentifier)
		{
			case ID_DISCONNECTION_NOTIFICATION:
				if (pRakClient == ::pRakClient)
				{
					Log("[RAKSAMP] Connection was closed by the server. Reconnecting in %d seconds.", iReconnectTime / 1000);
					resetPools(1, iReconnectTime);
				}
				break;
			case ID_CONNECTION_BANNED:
				if (pRakClient == ::pRakClient)
				{
					Log("[RAKSAMP] You are banned. Reconnecting in %d seconds.", iReconnectTime / 1000);
					resetPools(1, iReconnectTime);
				}
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				if (pRakClient == ::pRakClient)
				{
					Log("[RAKSAMP] Connection attempt failed. Reconnecting in %d seconds.", iReconnectTime / 1000);
					resetPools(1, iReconnectTime);
				}
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				if (pRakClient == ::pRakClient)
				{
					Log("[RAKSAMP] The server is full. Reconnecting in %d seconds.", iReconnectTime / 1000);
					resetPools(1, iReconnectTime);
				}
				break;
			case ID_INVALID_PASSWORD:
				if (pRakClient == ::pRakClient)
				{
					Log("[RAKSAMP] Invalid password. Reconnecting in %d seconds.", iReconnectTime / 1000);
					resetPools(1, iReconnectTime);
				}
				break;
			case ID_CONNECTION_LOST:
				if (pRakClient == ::pRakClient)
				{
					Log("[RAKSAMP] The connection was lost. Reconnecting in %d seconds.", iReconnectTime / 1000);
					resetPools(1, iReconnectTime);
				}
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				Packet_ConnectionSucceeded(pkt, pRakClient);
				break;
			case ID_AUTH_KEY:
				Packet_AUTH_KEY(pkt, pRakClient);
				break;
			case ID_PLAYER_SYNC:
				Packet_PlayerSync(pkt, pRakClient);
				break;
			case ID_VEHICLE_SYNC:
				Packet_VehicleSync(pkt, pRakClient);
				break;
			case ID_PASSENGER_SYNC:
				Packet_PassengerSync(pkt, pRakClient);
				break;
			case ID_AIM_SYNC:
				Packet_AimSync(pkt, pRakClient);
				break;
			case ID_TRAILER_SYNC:
				Packet_TrailerSync(pkt, pRakClient);
				break;
			case ID_UNOCCUPIED_SYNC:
				Packet_UnoccupiedSync(pkt, pRakClient);
				break;
			case ID_MARKERS_SYNC:
				Packet_MarkersSync(pkt, pRakClient);
				break;
			case ID_BULLET_SYNC:
				Packet_BulletSync(pkt, pRakClient);
				break;
		}

		pRakClient->DeallocatePacket(pkt);
	}

	UpdatePlayerScoresAndPings(1, 3000, pRakClient);
}
