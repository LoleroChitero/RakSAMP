/*
	Updated to 0.3.7 by P3ti
*/

struct stPlayerPool
{
	int iIsConnected;
	PlayerID rakPlayerID;
	PLAYERID playerID;
	char szPlayerName[32];
	char szIP[17];
	unsigned short usClientPort;
	int iPlayerScore;
	DWORD dwPlayerPing;
	VEHICLEID currentVehicleID;

	// spawn info
	BYTE bTeam;
	int iSkin;
	float fSpawnPos[3];
	float fRotation;
	DWORD dwColor;
	BYTE fightingStyle;

	// checkpoint info
	bool bCheckpointActive;
	float vecCheckpointPos[3];
	float fCheckpointSize;
	bool bPlayerInCheckpoint;
};

extern PLAYERID playerCount;
extern struct stPlayerPool playerPool[MAX_PLAYERS];

void addPlayerToPool(PlayerID rakPlayerID, PLAYERID playerID, char *szNick);
void removePlayerFromPool(PLAYERID playerID, int iTimeout);
int isPlayerConnected(PLAYERID playerID);
