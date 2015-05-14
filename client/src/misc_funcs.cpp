/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

extern PLAYER_SPAWN_INFO SpawnInfo;
extern int iDrunkLevel, iMoney, iLocalPlayerSkin;
extern BYTE m_bLagCompensation;

extern BOOL bTeleportMenuActive;
extern HFONT hTeleportMenuFont;
extern HANDLE hTeleportMenuThread;
extern HWND hwndTeleportMenu;

DWORD dwLastDisconnection = GetTickCount();
DWORD dwLastFakeKill = GetTickCount();
DWORD dwLastLag = GetTickCount();
DWORD dwLastJoinFlood = GetTickCount();
DWORD dwLastChatFlood = GetTickCount();
DWORD dwLastClassFlood = GetTickCount();
DWORD dwLastBulletFlood = GetTickCount();

int dd = 0;

// false - down | true - up
bool bHealthPulseDirection = false;
bool bArmourPulseDirection = true;

extern BOOL bIsSpectating;

// following functions
void onFootUpdateAtNormalPos()
{
	ONFOOT_SYNC_DATA ofSync;
	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

	ofSync.byteHealth = (BYTE)settings.fPlayerHealth;
	ofSync.byteArmour = (BYTE)settings.fPlayerArmour;
	ofSync.fQuaternion[3] = settings.fNormalModeRot;
	ofSync.vecPos[0] = settings.fNormalModePos[0];
	ofSync.vecPos[1] = settings.fNormalModePos[1];
	ofSync.vecPos[2] = settings.fNormalModePos[2];

	SendOnFootFullSyncData(&ofSync, 0, -1);

	AIM_SYNC_DATA aimSync;
	memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));

	playerInfo[g_myPlayerID].aimData.byteCamMode = 4;
	playerInfo[g_myPlayerID].aimData.vecAimf1[0] = 0.1f;
	playerInfo[g_myPlayerID].aimData.vecAimf1[1] = 0.1f;
	playerInfo[g_myPlayerID].aimData.vecAimf1[2] = 0.1f;
	playerInfo[g_myPlayerID].aimData.vecAimPos[0] = settings.fNormalModePos[0];
	playerInfo[g_myPlayerID].aimData.vecAimPos[1] = settings.fNormalModePos[1];
	playerInfo[g_myPlayerID].aimData.vecAimPos[2] = settings.fNormalModePos[2];

	SendAimSyncData(0, 0, -1);
}

void onFootUpdateFollow(PLAYERID followID)
{
	ONFOOT_SYNC_DATA ofSync;
	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	SendOnFootFullSyncData(&ofSync, 0, followID);
	SendAimSyncData(0, 0, followID);
}

DWORD inCarUpdateTick = GetTickCount();
void inCarUpdateFollow(PLAYERID followID, VEHICLEID withVehicleID)
{
	if(playerInfo[g_myPlayerID].iAreWeInAVehicle)
	{
		INCAR_SYNC_DATA icSync;
		memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));
		icSync.VehicleID = withVehicleID;
		SendInCarFullSyncData(&icSync, 0, followID);
		SendAimSyncData(0, 0, followID);
	}

	if(inCarUpdateTick && inCarUpdateTick < (GetTickCount() - 10000))
	{
		if(!playerInfo[g_myPlayerID].iAreWeInAVehicle)
		{
			if(playerInfo[followID].incarData.VehicleID == (VEHICLEID)-1)
				return;

			SendEnterVehicleNotification((VEHICLEID)settings.iFollowingWithVehicleID, 0);
			playerInfo[g_myPlayerID].iAreWeInAVehicle = 1;
			inCarUpdateTick = 0;
		}

		inCarUpdateTick = GetTickCount();
	}
	else
	{
		if(!playerInfo[g_myPlayerID].iAreWeInAVehicle)
			onFootUpdateFollow(followID);
	}
}

void spectatorUpdate()
{
	SPECTATOR_SYNC_DATA spSync;
	memset(&spSync, 0, sizeof(SPECTATOR_SYNC_DATA));

	spSync.vecPos[0] = settings.fNormalModePos[0];
	spSync.vecPos[1] = settings.fNormalModePos[1];
	spSync.vecPos[2] = settings.fNormalModePos[2];

	SendSpectatorData(&spSync);

	AIM_SYNC_DATA aimSync;
	memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));

	playerInfo[g_myPlayerID].aimData.byteCamMode = 4;
	playerInfo[g_myPlayerID].aimData.vecAimf1[0] = 0.1f;
	playerInfo[g_myPlayerID].aimData.vecAimf1[1] = 0.1f;
	playerInfo[g_myPlayerID].aimData.vecAimf1[2] = 0.1f;
	playerInfo[g_myPlayerID].aimData.vecAimPos[0] = settings.fNormalModePos[0];
	playerInfo[g_myPlayerID].aimData.vecAimPos[1] = settings.fNormalModePos[1];
	playerInfo[g_myPlayerID].aimData.vecAimPos[2] = settings.fNormalModePos[2];

	SendAimSyncData(0, 0, -1);
}

int sampConnect(char *szHostname, int iPort, char *szNickname, char *szPassword, RakClientInterface *pRakClient)
{
	if (!iAreWeConnected) Log("Connecting to %s:%d..", szHostname, iPort);

	strcpy(g_szNickName, szNickname);
	if(pRakClient == NULL) return 0;

	pRakClient->SetPassword(szPassword);
	return (int)pRakClient->Connect(szHostname, iPort, 0, 0, 5);
}

void sampDisconnect(int iTimeout)
{
	if(pRakClient == NULL) return;

	Log("Disconnected.");

	if(iTimeout)
		pRakClient->Disconnect(0);
	else
		pRakClient->Disconnect(500);
}

void sampRequestClass(int iClass)
{
	if(pRakClient == NULL) return;

	RakNet::BitStream bsSpawnRequest;
	bsSpawnRequest.Write(iClass);
	pRakClient->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void sampSpawn()
{
	if(pRakClient == NULL) return;

	if(iSpawned == 0 && settings.iNormalModePosForce == 0)
	{
		iLocalPlayerSkin = SpawnInfo.iSkin;
		settings.fNormalModePos[0] = SpawnInfo.vecPos[0];
		settings.fNormalModePos[1] = SpawnInfo.vecPos[1];
		settings.fNormalModePos[2] = SpawnInfo.vecPos[2];
		settings.fNormalModeRot = SpawnInfo.fRotation;
	}
	
	RakNet::BitStream bsSendRequestSpawn;
	pRakClient->RPC(&RPC_RequestSpawn, &bsSendRequestSpawn, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);

	RakNet::BitStream bsSendSpawn;
	pRakClient->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	
	bIsSpectating = 0;

	Log("You have been spawned!");
}

void sampSpam()
{
	if(GetTickCount() - dwLastDisconnection >= settings.uiSpamInterval)
	{
		gen_random(g_szNickName, rand()%16+3);
		iGettingNewName = true;
		sampDisconnect(0);
		resetPools(1, 1);

		dwLastDisconnection = GetTickCount();
	}
}

void sampFakeKill()
{
	if(GetTickCount() - dwLastFakeKill >= settings.uiFakeKillInterval)
	{
		int randkillerid = 0xFFFF + 1;
		int randreason = rand() % 46;

		while(!(randkillerid >= 0 && randkillerid < MAX_PLAYERS && randkillerid != g_myPlayerID && playerInfo[randkillerid].iIsConnected && !playerInfo[randkillerid].byteIsNPC))
		{
			if(getPlayerCount() < 2)
			{
				randkillerid = 0xFFFF;
				break;
			}

			randkillerid = rand() % MAX_PLAYERS;
		}

		if(randkillerid != 0xFFFF + 1)
			SendWastedNotification(randreason, randkillerid);

		dwLastFakeKill = GetTickCount();
	}
}

void sampLag()
{
	if(GetTickCount() - dwLastLag >= settings.uiLagInterval)
	{
		RakNet::BitStream bsDeath;
		bsDeath.Write(dd++);
		pRakClient->RPC(&RPC_ClickPlayer, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_EnterVehicle, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ExitVehicle, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PickedUpPickup, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_RequestSpawn, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		sendDialogResponse(sampDialog.wDialogID, 1, 1, "");

		dwLastLag = GetTickCount();
	}
}

void sampJoinFlood()
{
	if(GetTickCount() - dwLastJoinFlood >= settings.uiJoinFloodInterval)
	{
		gen_random(g_szNickName, 16);
		strcpy(playerInfo[g_myPlayerID].szPlayerName, g_szNickName);

		int iVersion = NETGAME_VERSION;
		unsigned int uiClientChallengeResponse = settings.uiChallange ^ iVersion;
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
		bsSend.Write(byteAuthBSLen);
		bsSend.Write(auth_bs, byteAuthBSLen);
		bsSend.Write(iClientVerLen);
		bsSend.Write(settings.szClientVersion, iClientVerLen);

		pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);

		dwLastJoinFlood = GetTickCount();
	}
}

void sampChatFlood()
{
	if(GetTickCount() - dwLastChatFlood >= settings.uiChatFloodInterval)
	{
		char szRandomMessage[60 + 1];
		gen_random(szRandomMessage, 60);

		sendChat(szRandomMessage);

		dwLastChatFlood = GetTickCount();
	}
}

void sampClassFlood()
{
	static int iClassID;

	if(iAreWeConnected && iGameInited && GetTickCount() - dwLastClassFlood >= settings.uiClassFloodInterval)
	{
		sampRequestClass(iClassID);
		sampSpawn();

		iClassID++;
	
		if(iClassID >= iSpawnsAvailable)
			iClassID = 0;

		dwLastClassFlood = GetTickCount();
	}
}

void sendServerCommand(char *szCommand)
{
	if (!strnicmp(szCommand+1, "rcon", 4))
	{
		RakNet::BitStream bsSend;
		bsSend.Write((BYTE)ID_RCON_COMMAND);
		DWORD len = strlen(szCommand+4);
		if (len > 0) {	
			bsSend.Write(len);
			bsSend.Write(szCommand+6, len);
		} else {
			bsSend.Write(len);
			bsSend.Write(szCommand+5, len);
		}
		pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
	}
	else
	{
		RakNet::BitStream bsParams;
		int iStrlen = strlen(szCommand);
		bsParams.Write(iStrlen);
		bsParams.Write(szCommand, iStrlen);
		pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void sendChat(char *szMessage)
{
	RakNet::BitStream bsSend;
	BYTE byteTextLen = strlen(szMessage);
	bsSend.Write(byteTextLen);
	bsSend.Write(szMessage, byteTextLen);
	pRakClient->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void sendScmEvent(int iEventType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	RakNet::BitStream bsSend;
	bsSend.Write(iEventType);
	bsSend.Write(dwParam1);
	bsSend.Write(dwParam2);
	bsSend.Write(dwParam3);
	pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void sendDialogResponse(WORD wDialogID, BYTE bButtonID, WORD wListBoxItem, char *szInputResp)
{
	BYTE respLen = (BYTE)strlen(szInputResp);
	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(bButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInputResp, respLen);
	pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void sendPickUp(int iPickupID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(iPickupID);
	pRakClient->RPC(&RPC_PickedUpPickup, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void selectTextDraw(int iTextDrawID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(iTextDrawID);
	pRakClient->RPC(&RPC_ClickTextDraw, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

int isPlayerConnected(PLAYERID iPlayerID)
{
	if(iPlayerID < 0 || iPlayerID >= MAX_PLAYERS)
		return 0;

	if(!playerInfo[iPlayerID].iIsConnected)
		return 0;

	return 1;
}

int getPlayerID(char *szPlayerName)
{
	int i;
	for(i = 0; i < MAX_PLAYERS; i++)
	{
		if(playerInfo[i].iIsConnected == 0) continue;

		if(!strcmp(playerInfo[i].szPlayerName, szPlayerName))
			return i;
	}

	return -1;
}

char *getPlayerName(PLAYERID iPlayerID)
{
	if(playerInfo[iPlayerID].iIsConnected && iPlayerID >= 0 && iPlayerID <= MAX_PLAYERS)
		return playerInfo[iPlayerID].szPlayerName;

	return 0;
}

int getPlayerPos(PLAYERID iPlayerID, float *fPos)
{
	if(!playerInfo[iPlayerID].iIsConnected) return 0;

	if(iPlayerID > 0 && iPlayerID <= MAX_PLAYERS)
	{
		memcpy(fPos, playerInfo[iPlayerID].onfootData.vecPos, sizeof(float) * 3);
		return 1;
	}

	return 0;
}

PLAYERID getPlayerIDFromPlayerName(char *szName)
{
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if(!playerInfo[i].iIsConnected) continue;
		if(!strcmp(playerInfo[i].szPlayerName, szName))
			return (PLAYERID)i;
	}

	return -1;
}

unsigned short getPlayerCount()
{
	unsigned short count = 0;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if(!playerInfo[i].iIsConnected) continue;
		count++;
	}
	return count;
}

const struct vehicle_entry *gta_vehicle_get_by_id ( int id )
{
	id -= VEHICLE_LIST_ID_START;

	if ( id < 0 || id >= VEHICLE_LIST_SIZE )
		return NULL;

	return &vehicle_list[id];
}

int gen_gpci(char buf[64], unsigned long factor) /* by bartekdvd */
{
	unsigned char out[6*4] = {0};

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < 6*4; ++i)
		out[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	out[6*4] = 0;

	BIG_NUM_MUL((unsigned long*)out, (unsigned long*)out, factor);

	unsigned int notzero = 0;
	buf[0] = '0'; buf[1] = '\0';

	if (factor == 0) return 1;

	int pos = 0;
	for (int i = 0; i < 24; i++)
	{
		unsigned char tmp = out[i] >> 4;
		unsigned char tmp2 = out[i]&0x0F;
		
		if (notzero || tmp)
		{
			buf[pos++] = (char)((tmp > 9)?(tmp + 55):(tmp + 48));
			if (!notzero) notzero = 1;
		}

		if (notzero || tmp2)
		{
			buf[pos++] = (char)((tmp2 > 9)?(tmp2 + 55):(tmp2 + 48));
			if (!notzero) notzero = 1;
		}
	}
	buf[pos] = 0;

	return pos;
}

void SetStringFromCommandLine(char *szCmdLine, char *szString)
{
	while(*szCmdLine == ' ') szCmdLine++;
	while(*szCmdLine && *szCmdLine != ' ' && *szCmdLine != '-' && *szCmdLine != '/') 
	{
		*szString = *szCmdLine;
		szString++; szCmdLine++;
	}
	*szString = '\0';
}

void processPulsator()
{
	if (settings.bPulsator)
	{
		if(!bHealthPulseDirection && settings.fPlayerHealth <= 3)
			bHealthPulseDirection = true;

		if(bHealthPulseDirection && settings.fPlayerHealth >= 100)
			bHealthPulseDirection = false;

		if(!bArmourPulseDirection && settings.fPlayerArmour <= 0)
			bArmourPulseDirection = true;

		if(bArmourPulseDirection && settings.fPlayerArmour >= 100)
			bArmourPulseDirection = false;

		if(bHealthPulseDirection)
			settings.fPlayerHealth += 12.5f;
		else
			settings.fPlayerHealth -= 12.5f;

		if(settings.fPlayerHealth < 3.0f)
			settings.fPlayerHealth = 3.0f;

		if(bArmourPulseDirection)
			settings.fPlayerArmour += 12.5f;
		else
			settings.fPlayerArmour -= 12.5f;

		if(settings.fPlayerArmour < 0.0f)
			settings.fPlayerArmour = 0.0f;
	}
}

void processBulletFlood()
{
	static PLAYERID targetid;

	if (settings.bulletFlood && m_bLagCompensation)
	{
		if(GetTickCount() - dwLastBulletFlood >= settings.uiBulletFloodInterval)
		{
			if ( !(targetid >= 0 && targetid < MAX_PLAYERS && targetid != g_myPlayerID && playerInfo[targetid].iIsConnected && (playerInfo[targetid].iIsStreamedIn || playerInfo[targetid].iGotMarkersPos)) )
				goto find_another_target;

			if(settings.runMode == RUNMODE_FOLLOWPLAYER)
			{
				PLAYERID copyingID = getPlayerIDFromPlayerName(settings.szFollowingPlayerName);

				if(copyingID != (PLAYERID)-1 && copyingID == targetid)
				{
					targetid++;
					goto find_another_target;
				}
			}

			BULLET_SYNC_DATA BulletSyncData;

			BulletSyncData.bHitType = (BYTE)BULLET_HIT_TYPE_PLAYER;
			BulletSyncData.iHitID = (unsigned short)targetid;

			BulletSyncData.fHitOrigin[0] = settings.fCurrentPosition[0];
			BulletSyncData.fHitOrigin[1] = settings.fCurrentPosition[1];
			BulletSyncData.fHitOrigin[2] = settings.fCurrentPosition[2];
								
			BulletSyncData.fHitTarget[0] = playerInfo[targetid].onfootData.vecPos[0];
			BulletSyncData.fHitTarget[1] = playerInfo[targetid].onfootData.vecPos[1];
			BulletSyncData.fHitTarget[2] = playerInfo[targetid].onfootData.vecPos[2];

			BulletSyncData.fCenterOfHit[0] = (rand() % 10) / 20.0f;
			BulletSyncData.fCenterOfHit[1] = (rand() % 10) / 20.0f;
			BulletSyncData.fCenterOfHit[2] = (rand() % 10) / 20.0f;

			if(settings.bCurrentWeapon != 0)
				BulletSyncData.bWeaponID = settings.bCurrentWeapon;

			else if(settings.runMode == RUNMODE_FOLLOWPLAYER || settings.runMode == RUNMODE_FOLLOWPLAYERSVEHICLE)
			{
				PLAYERID copyingID = getPlayerIDFromPlayerName(settings.szFollowingPlayerName);
				
				if(copyingID != (PLAYERID)-1)
					BulletSyncData.bWeaponID = playerInfo[copyingID].onfootData.byteCurrentWeapon;
			}

			SendBulletData(&BulletSyncData);

			targetid++;

			dwLastBulletFlood = GetTickCount();
		}
	}

	find_another_target:
	{
		for ( targetid; targetid < MAX_PLAYERS; targetid++ )
		{
			if ( targetid >= 0 && targetid < MAX_PLAYERS && targetid != g_myPlayerID && playerInfo[targetid].iIsConnected && (playerInfo[targetid].iIsStreamedIn || playerInfo[targetid].iGotMarkersPos) )
				break;
		}

		if ( targetid >= MAX_PLAYERS )
			targetid = 0;
	}
}

void showTeleportMenu()
{
	if(bTeleportMenuActive)
	{
		bTeleportMenuActive = 0;
		SendMessage(hwndTeleportMenu, WM_DESTROY, 0, 0);
		DestroyWindow(hwndTeleportMenu);
		UnregisterClass("teleportMenuWndClass", GetModuleHandle(NULL));
		hTeleportMenuFont = NULL;
		TerminateThread(hTeleportMenuThread, 0);
	}

	hTeleportMenuThread = CreateThread(NULL, 0, TeleportMenuThread, NULL, 0, NULL);
}

void useTeleport(int iTeleportID)
{
	if(iTeleportID < 0 || iTeleportID > MAX_TELEPORT_ITEMS - 1)
		return;

	if(settings.TeleportLocations[iTeleportID].bCreated)
	{
		settings.fNormalModePos[0] = settings.TeleportLocations[iTeleportID].fPosition[0];
		settings.fNormalModePos[1] = settings.TeleportLocations[iTeleportID].fPosition[1];
		settings.fNormalModePos[2] = settings.TeleportLocations[iTeleportID].fPosition[2];

		Log("Teleported to %s.", settings.TeleportLocations[iTeleportID].szName);
	}
}
