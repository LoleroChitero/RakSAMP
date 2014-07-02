/*
	Updated to 0.3z by P3ti
*/

#include "main.h"

PLAYERID playerCount = 0;
struct stPlayerPool playerPool[MAX_PLAYERS];

void addPlayerToPool(PlayerID rakPlayerID, PLAYERID playerID, char *szNick)
{
	BYTE byteNameLen = (BYTE)strlen(szNick);

	playerPool[playerCount].iIsConnected = 1;
	playerPool[playerCount].rakPlayerID.binaryAddress = rakPlayerID.binaryAddress;
	playerPool[playerCount].rakPlayerID.port = rakPlayerID.port;
	playerPool[playerCount].playerID = playerCount;
	strcpy(playerPool[playerCount].szIP, rakPlayerID.ToString(false));
	strcpy(playerPool[playerCount].szPlayerName, szNick);
	playerPool[playerCount].usClientPort = rakPlayerID.port;
	playerPool[playerCount].iPlayerScore = 0;
	playerPool[playerCount].dwPlayerPing = 0;
	playerPool[playerCount].bTeam = 0xFF;

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
	playerPool[playerCount].bTeam = 0;

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