/*
	Updated to 0.3z by P3ti
*/

#define MAX_AUTORUN_CMDS 32
#define MAX_FIND_ITEMS	 128

struct stServer
{
	char szAddr[256];
	int iPort;
	char szNickname[20];
	char szPassword[32];
};

struct stAutoRunCMD
{
	int iExists;
	char szCMD[512];
};

struct stFindItem
{
	int iExists;
	char szFind[512];
	char szSay[512];
	unsigned char bBkRed;
	unsigned char bBkGreen;
	unsigned char bBkBlue;
	unsigned char bTextRed;
	unsigned char bTextGreen;
	unsigned char bTextBlue;
};

struct stCheckpointData
{
	bool bActive;
	float fPosition[3];
	float fSize;
};

struct stSettings
{
	struct stServer server;
	SOCKET RCONSocket;
	struct sockaddr_in RCONserver;
	struct sockaddr_in RCONclient;
	int iRCONLoggedIn;
	char szRCONPass[512];
	int iConsole;
	eRunModes runMode;
	int iAutorun;
	int iFind;
	int iClassID;
	int iManualSpawn;
	int iPrintTimestamps;
	
	int iUpdateStats;
	int iMinFPS;
	int iMaxFPS;

	char szClientVersion[20];

	bool bSpam;
	bool bFakeKill;
	bool bLag;

	unsigned int uiSpamInterval;
	unsigned int uiFakeKillInterval;
	unsigned int uiLagInterval;

	unsigned int uiObjectsLogging;
	unsigned int uiPickupsLogging;

	unsigned char bChatColorRed;
	unsigned char bChatColorGreen;
	unsigned char bChatColorBlue;

	unsigned char bCMsgRed;
	unsigned char bCMsgGreen;
	unsigned char bCMsgBlue;

	unsigned char bCPAlertRed;
	unsigned char bCPAlertGreen;
	unsigned char bCPAlertBlue;

	char szFollowingPlayerName[20];
	int iFollowingWithVehicleID;
	float fFollowXOffset;
	float fFollowYOffset;
	float fFollowZOffset;
	char szImitateChatPlayerName[20];

	float fNormalModePos[3];
	float fNormalModeRot;
	int iNormalModePosForce;

	bool pulseHealth;

	float fHealthBeforePulse;
	float fArmourBeforePulse;

	float fPlayerHealth;
	float fPlayerArmour;

	bool bulletFlood;

	float fCurrentPosition[3];

	BYTE bCurrentWeapon;

	struct stAutoRunCMD autoRunCMDs[MAX_AUTORUN_CMDS];
	struct stFindItem findItems[MAX_FIND_ITEMS];

	unsigned int uiChallange;
	
	struct stCheckpointData CurrentCheckpoint;

	bool AutoGotoCP;
};
extern struct stSettings settings;

int LoadSettings();
int UnLoadSettings();
int ReloadSettings();
