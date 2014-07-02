/*
	Updated to 0.3z by P3ti
*/

#include "main.h"

void InitGameForPlayer(PLAYERID playerID)
{
	RakNet::BitStream bsInitGame;
	bsInitGame.WriteCompressed((bool)1); // m_bZoneNames
	bsInitGame.WriteCompressed((bool)1); // m_bUseCJWalk
	bsInitGame.WriteCompressed((bool)1); // m_bAllowWeapons
	bsInitGame.WriteCompressed((bool)0); // m_bLimitGlobalChatRadius
	bsInitGame.Write((float)200.00f); // m_fGlobalChatRadius
	bsInitGame.WriteCompressed((bool)0); // bStuntBonus
	bsInitGame.Write((float)70.0f); // m_fNameTagDrawDistance
	bsInitGame.WriteCompressed((bool)0); // m_bDisableEnterExits
	bsInitGame.WriteCompressed((bool)1); // m_bNameTagLOS
	bsInitGame.WriteCompressed((bool)0); // m_bManualVehicleEngineAndLight
	bsInitGame.Write((int)1); // m_iSpawnsAvailable
	bsInitGame.Write(playerID); // MyPlayerID
	bsInitGame.WriteCompressed((bool)1); // m_bShowPlayerTags
	bsInitGame.Write((int)1); // m_iShowPlayerMarkers
	bsInitGame.Write((BYTE)12); // m_byteWorldTime
	bsInitGame.Write((BYTE)10); // m_byteWeather
	bsInitGame.Write((float)0.008); // m_fGravity
	bsInitGame.WriteCompressed((bool)0); // bLanMode
	bsInitGame.Write((int)0); // m_iDeathDropMoney
	bsInitGame.WriteCompressed((bool)0); // m_bInstagib

	bsInitGame.Write((int)40); // iNetModeNormalOnfootSendRate
	bsInitGame.Write((int)40); // iNetModeNormalIncarSendRate
	bsInitGame.Write((int)40); // iNetModeFiringSendRate
	bsInitGame.Write((int)10); // iNetModeSendMultiplier

	bsInitGame.Write((int)0); // m_bLagCompensation
	
	bsInitGame.Write((int)0); // unknown
	bsInitGame.Write((int)0); // unknown
	bsInitGame.Write((int)0); // unknown

	BYTE bServerNameLen = (BYTE)strlen(serverName);
	bsInitGame.Write(bServerNameLen);
	bsInitGame.Write(serverName, bServerNameLen);

	BYTE vehModels[212];
	memset(vehModels, 1, 212);
	bsInitGame.Write((char *)&vehModels, 212);

	pRakServer->RPC(&RPC_InitGame, &bsInitGame, HIGH_PRIORITY, RELIABLE,
		0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void SendPlayerPoolToPlayer(PLAYERID playerID)
{
	// let the player know about all the players in the server
	for(PLAYERID p = 0; p < MAX_PLAYERS; p++)
	{
		if(!playerPool[p].iIsConnected)
			continue;

		if(p == playerID)
			continue;

		BYTE byteNameLen = (BYTE)strlen(playerPool[p].szPlayerName);
		RakNet::BitStream bs;
		bs.Reset();
		bs.Write(p);
		bs.Write((int)1);
		bs.Write((BYTE)0);
		bs.Write(byteNameLen);
		bs.Write(playerPool[p].szPlayerName, byteNameLen);
		pRakServer->RPC(&RPC_ServerJoin, &bs, HIGH_PRIORITY, RELIABLE,
			0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

		Sleep(5); // well, shit.
	}
}

void SpawnAllVehiclesForPlayer(PLAYERID playerID)
{
	// spawn all vehicles for this player
	for(VEHICLEID v = 0; v < MAX_VEHICLES; v++)
	{
		if(!vehiclePool[v].bExists)
			continue;

		NEW_VEHICLE newVeh;
		memset(&newVeh, 0, sizeof(NEW_VEHICLE));
		newVeh.VehicleId = v;
		newVeh.iVehicleType = vehiclePool[v].iModelID;
		newVeh.vecPos[0] = vehiclePool[v].fVehiclePos[0];
		newVeh.vecPos[1] = vehiclePool[v].fVehiclePos[1];
		newVeh.vecPos[2] = vehiclePool[v].fVehiclePos[2];
		newVeh.fRotation = vehiclePool[v].fRotation;
		newVeh.fHealth = 1000.00f;
		newVeh.byteInterior = 0;
		newVeh.byteDoorsLocked = 0;
		newVeh.dwDoorDamageStatus = 0;
		newVeh.dwPanelDamageStatus = 0;
		newVeh.byteLightDamageStatus = 0;

		RakNet::BitStream bs;
		bs.Write((const char *)&newVeh, sizeof(NEW_VEHICLE));
		pRakServer->RPC(&RPC_WorldVehicleAdd, &bs, HIGH_PRIORITY, RELIABLE,
			0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

		Sleep(5); // well, shit.
	}
}

void RPC_ClientJoins(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData((unsigned char *)Data,(iBitLength/8)+1,false);
	RakNet::BitStream bsReject;

	char szNickName[32], pszAuthBullshit[44];
	int iVersion;
	unsigned int uiChallengeResponse;
	BYTE byteMod, byteNameLen, byteAuthBSLen;
	PLAYERID playerID = pRakServer->GetIndexFromPlayerID(sender);
	BYTE byteRejectReason;

	bsData.Read(iVersion);
	bsData.Read(byteMod);
	bsData.Read(byteNameLen);
	bsData.Read(szNickName, byteNameLen);
	szNickName[byteNameLen] = 0;
	bsData.Read(uiChallengeResponse);
	bsData.Read(byteAuthBSLen);
	bsData.Read(pszAuthBullshit, byteAuthBSLen);

	PlayerID MyPlayerID = pRakServer->GetPlayerIDFromIndex(playerID);
	in_addr in;
	if(UNASSIGNED_PLAYER_ID == MyPlayerID)
	{
		in.s_addr = sender.binaryAddress;
		Log("Detected possible bot from (%s)", inet_ntoa(in));
		pRakServer->Kick(MyPlayerID);
		return;
	}

	if(!pRakServer->IsActivePlayerID(sender) || playerID > MAX_PLAYERS)
	{
		byteRejectReason = REJECT_REASON_BAD_PLAYERID;
		bsReject.Write(byteRejectReason);
		pRakServer->RPC(&RPC_ConnectionRejected, &bsReject, HIGH_PRIORITY, RELIABLE,
			0, sender, FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakServer->Kick(sender);
		return;
	}

	if(iVersion != NETGAME_VERSION || _uiRndSrvChallenge != (uiChallengeResponse ^ NETGAME_VERSION))
	{
		byteRejectReason = REJECT_REASON_BAD_VERSION;
		bsReject.Write(byteRejectReason);
		pRakServer->RPC(&RPC_ConnectionRejected, &bsReject, HIGH_PRIORITY, RELIABLE,
			0, sender, FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakServer->Kick(sender);
		return;
	}

	for(int i = 0; i < iScriptsRunning; i++)
	{
		if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
			ScriptEvent_OnPlayerJoin(script.scriptVM[i], playerID, szNickName,
			rpcParams->sender.ToString(false), rpcParams->sender.port);
	}

	addPlayerToPool(rpcParams->sender, playerID, szNickName);

	InitGameForPlayer(playerID);
	SendPlayerPoolToPlayer(playerID);
	SpawnAllVehiclesForPlayer(playerID);
}

#pragma pack(1)
typedef struct _PLAYER_SPAWN_INFO
{
	BYTE byteTeam;
	int iSkin;
	BYTE byteUnknown; // TODO dis wat be
	float vecPos[3];
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO;
void RPC_ClientRequestsClass(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char *)Data,(iBitLength/8)+1,false);

	int iClass;
	bsData.Read(iClass);

	PLAYER_SPAWN_INFO psInfo;
	memset(&psInfo, 0, sizeof(psInfo));
	psInfo.byteTeam = 0xFF;
	psInfo.iSkin = 33;
	psInfo.byteUnknown = 0;
	psInfo.vecPos[0] = 389.8672f;
	psInfo.vecPos[1] = 2543.0046f;
	psInfo.vecPos[2] = 16.5391f;
	psInfo.fRotation = 90.0f;
	psInfo.iSpawnWeapons[0] = 38;
	psInfo.iSpawnWeaponsAmmo[0] = 69;

	RakNet::BitStream bsReply;
	bsReply.Write((BYTE)1);
	bsReply.Write((char *)&psInfo, sizeof(psInfo));
	pRakServer->RPC(&RPC_RequestClass, &bsReply, HIGH_PRIORITY, RELIABLE,
		0, rpcParams->sender, FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void RPC_ClientRequestsSpawn(RPCParameters *rpcParams)
{
	RakNet::BitStream bsReply;

	bsReply.Write((BYTE)2);
	pRakServer->RPC(&RPC_RequestSpawn, &bsReply, HIGH_PRIORITY, RELIABLE,
		0, rpcParams->sender, FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void RPC_ClientSpawns(RPCParameters *rpcParams)
{
	PLAYERID playerId = (PLAYERID)pRakServer->GetIndexFromPlayerID(rpcParams->sender);
	BYTE byteFightingStyle=4;
	BYTE byteTeam=-1;
	int iSkin=0;
	float vecPos[3] = { 389.8672f, 2543.0046f, 16.5391f };
	float fRotation=90.0f;
	DWORD dwColor=-1;

	for(int i = 0; i < iScriptsRunning; i++)
	{
		if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
			ScriptEvent_OnPlayerSpawn(script.scriptVM[i], playerId);
	}

	RakNet::BitStream bsData;
	bsData.Write(playerId);
	bsData.Write(byteTeam);
	bsData.Write(iSkin);
	bsData.Write(vecPos[0]);
	bsData.Write(vecPos[1]);
	bsData.Write(vecPos[2]);
	bsData.Write(fRotation);
	bsData.Write(dwColor);
	bsData.Write(byteFightingStyle);
	pRakServer->RPC(&RPC_WorldPlayerAdd, &bsData, HIGH_PRIORITY, RELIABLE_ORDERED,
		0, pRakServer->GetPlayerIDFromIndex(playerId), TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void RPC_ClientChat(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char *)Data,(iBitLength/8)+1,false);

	PLAYERID playerId = (PLAYERID)pRakServer->GetIndexFromPlayerID(rpcParams->sender);
	char szText[256];
	BYTE byteTextLen;

	memset(szText,0,256);

	bsData.Read(byteTextLen);
	bsData.Read((char *)szText,byteTextLen);
	szText[byteTextLen] = '\0';

	for(int i = 0; i < iScriptsRunning; i++)
	{
		if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
			ScriptEvent_OnPlayerMessage(script.scriptVM[i], playerId, szText);
	}
}

void RPC_ClientUpdateScoresPingsIPs(RPCParameters *rpcParams)
{
	RakNet::BitStream bsUpdate;

	for(PLAYERID i = 0; i < MAX_PLAYERS; i++)
	{
		if(isPlayerConnected(i))
		{
			playerPool[i].iPlayerScore = 0;
			playerPool[i].dwPlayerPing = pRakServer->GetLastPing(playerPool[i].rakPlayerID);

			bsUpdate.Write(i);
			bsUpdate.Write(playerPool[i].iPlayerScore);
			bsUpdate.Write(playerPool[i].dwPlayerPing);
		}
	}

	pRakServer->RPC(&RPC_UpdateScoresPingsIPs, &bsUpdate, HIGH_PRIORITY, RELIABLE, 0,
		rpcParams->sender, FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void RPC_ClientEnterVehicle(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData((unsigned char *)Data,(iBitLength/8)+1,false);
	PLAYERID playerID = pRakServer->GetIndexFromPlayerID(sender);

	if(!playerPool[playerID].iIsConnected)
		return;

	VEHICLEID VehicleID=0;
	BYTE bytePassenger=0;

	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	if(VehicleID == (VEHICLEID)-1)
	{
		SendClientMessage(playerID, -1, "You are sending an invalid vehicle ID. Unlike kye, we wont kick you :)");
		return;
	}

	for(int i = 0; i < iScriptsRunning; i++)
	{
		if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
			ScriptEvent_OnPlayerWantsEnterVehicle(script.scriptVM[i], playerID, VehicleID, bytePassenger);
	}

	playerInfo[playerID].incarData.VehicleID = VehicleID;

	RakNet::BitStream bsVehicle;
	bsVehicle.Write(playerID);
	bsVehicle.Write(VehicleID);
	bsVehicle.Write(bytePassenger);
	pRakServer->RPC(&RPC_EnterVehicle, &bsVehicle, HIGH_PRIORITY, RELIABLE_ORDERED,
		0, sender, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}


void RPC_ClientExitVehicle(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData((unsigned char *)Data,(iBitLength/8)+1,false);
	PLAYERID playerID = pRakServer->GetIndexFromPlayerID(sender);

	if(!playerPool[playerID].iIsConnected)
		return;

	VEHICLEID VehicleID;
	bsData.Read(VehicleID);

	if(VehicleID == (VEHICLEID)-1)
	{
		SendClientMessage(playerID, -1, "You are sending an invalid vehicle ID. Unlike kye, we wont kick you :)");
		return;
	}

	for(int i = 0; i < iScriptsRunning; i++)
	{
		if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
			ScriptEvent_OnPlayerLeaveVehicle(script.scriptVM[i], playerID, VehicleID);
	}

	playerInfo[playerID].incarData.VehicleID = -1;

	RakNet::BitStream bsVehicle;
	bsVehicle.Write(playerID);
	bsVehicle.Write(VehicleID);
	pRakServer->RPC(&RPC_ExitVehicle, &bsVehicle, HIGH_PRIORITY, RELIABLE_ORDERED,
		0, sender, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}


void RegisterServerRPCs(RakServerInterface *pRakServer)
{
	// Core RPCs
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_ClientJoin, RPC_ClientJoins);
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_RequestClass, RPC_ClientRequestsClass);
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, RPC_ClientRequestsSpawn);
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_Spawn, RPC_ClientSpawns);
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_Chat, RPC_ClientChat);
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, RPC_ClientUpdateScoresPingsIPs);
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, RPC_ClientEnterVehicle);
	pRakServer->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, RPC_ClientExitVehicle);
}

void UnRegisterServerRPCs(RakServerInterface * pRakServer)
{
	// Core RPCs
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_ClientJoin);
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_RequestClass);
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_RequestSpawn);
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_Spawn);
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_Chat);
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs);
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_EnterVehicle);
	pRakServer->UnregisterAsRemoteProcedureCall(&RPC_ExitVehicle);
}
