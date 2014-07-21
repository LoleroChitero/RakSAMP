/*
	Updated to 0.3z by P3ti
*/

void onFootUpdateAtNormalPos();
void onFootUpdateFollow(PLAYERID followID);
void inCarUpdateFollow(PLAYERID followID, VEHICLEID withVehicleID);

int sampConnect(char *szHostname, int iPort, char *szNickname, char *szPassword, RakClientInterface *pRakClient);
void sampDisconnect(int iTimeout);
void sampRequestClass(int iClass);
void sampSpawn();
void sampSpam();
void sampFakeKill();
void sampLag();
void sendServerCommand(char *szCommand);
void sendChat(char *szMessage);
void sendRconCommand(char *szRCONCmd, int iIsLogin);
void SendScmEvent(int iEventType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
void sendDialogResponse(WORD wDialogID, BYTE bButtonID, WORD wListBoxItem, char *szInputResp);
void sendPickUp(int iPickupID);

int isPlayerConnected(PLAYERID iPlayerID);
int getPlayerID(char *szPlayerName);
char *getPlayerName(PLAYERID iPlayerID);
int getPlayerPos(PLAYERID iPlayerID, float *fPos);

PLAYERID getPlayerIDFromPlayerName(char *szName);
unsigned short getPlayerCount();

const struct vehicle_entry *gta_vehicle_get_by_id ( int id );

int gen_gpci(char buf[64], unsigned long factor);

void SetStringFromCommandLine(char *szCmdLine, char *szString);