/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

PLAYERID playerCount = 0;
struct stPlayerPool playerPool[MAX_PLAYERS];

void addPlayerToPool(PlayerID rakPlayerID, PLAYERID playerID, char *szNick)
{
	BYTE byteNameLen = (BYTE)strlen(szNick);

	playerPool[playerID].iIsConnected = 1;
	playerPool[playerID].rakPlayerID.binaryAddress = rakPlayerID.binaryAddress;
	playerPool[playerID].rakPlayerID.port = rakPlayerID.port;
	playerPool[playerID].playerID = playerCount;
	strcpy(playerPool[playerID].szIP, rakPlayerID.ToString(false));
	strcpy(playerPool[playerID].szPlayerName, szNick);
	playerPool[playerID].usClientPort = rakPlayerID.port;
	playerPool[playerID].iPlayerScore = 0;
	playerPool[playerID].dwPlayerPing = 0;
	playerPool[playerID].bTeam = 0xFF;
	playerPool[playerID].bCheckpointActive = false;
	playerPool[playerID].bPlayerInCheckpoint = false;
	playerPool[playerID].vecCheckpointPos[0] = 0.0f;
	playerPool[playerID].vecCheckpointPos[1] = 0.0f;
	playerPool[playerID].vecCheckpointPos[2] = 0.0f;
	playerPool[playerID].fCheckpointSize = 0.0f;

	RakNet::BitStream bsJoinAnn;
	bsJoinAnn.Write(playerID);
	int iUnk = 1;
	bsJoinAnn.Write(iUnk);
	BYTE bUnk = 0;
	bsJoinAnn.Write(bUnk);
	bsJoinAnn.Write(byteNameLen);
	bsJoinAnn.Write(szNick, byteNameLen);
	pRakServer->RPC(&RPC_ServerJoin, &bsJoinAnn, HIGH_PRIORITY, RELIABLE,
		0, rakPlayerID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

	playerCount++;
}

void removePlayerFromPool(PLAYERID playerID, int iTimeout)
{
	if(!playerPool[playerID].iIsConnected) return;

	playerPool[playerID].iIsConnected = 0;
	playerPool[playerID].playerID = -1;
	memset(playerPool[playerID].szIP, 0, 17);
	memset(playerPool[playerID].szPlayerName, 0, 32);
	playerPool[playerID].iPlayerScore = 0;
	playerPool[playerID].dwPlayerPing = 0;
	playerPool[playerID].bTeam = 0;
	playerPool[playerID].bCheckpointActive = false;
	playerPool[playerID].bPlayerInCheckpoint = false;

	RakNet::BitStream bsSend;
	BYTE byteReason;
	if(iTimeout)
		byteReason = 0;
	else
		byteReason = 1;

	bsSend.Write(playerID);
	bsSend.Write(byteReason);
	pRakServer->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE,
		0, pRakServer->GetPlayerIDFromIndex(playerID), TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

	playerCount--;
}

int isPlayerConnected(PLAYERID playerID)
{
	return playerPool[playerID].iIsConnected;
}
