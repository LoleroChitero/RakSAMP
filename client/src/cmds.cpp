/*
	Updated to 0.3z by P3ti
*/

#include "main.h"

int RunCommand(char *szCMD, int iFromAutorun)
{
	// return 0: should run server sided command.
	// return 1: found local command.
	// return 2: local command not found.
	// return 3: exit process.

	if(szCMD[0] == 0x00)
		return 2;

	if(settings.iConsole)
		memset(&szCMD[(strlen(szCMD) - 2)], 0, 2);

	if(settings.runMode == RUNMODE_RCON)
	{
		if(!strncmp(szCMD, "login", 5) || !strncmp(szCMD, "LOGIN", 5))
		{
			char *pszPass = &szCMD[6];
			strcpy(settings.szRCONPass, pszPass);
			sendRconCommand(pszPass, 1);
			settings.iRCONLoggedIn = 1;
			Log("RCON password set.");
		}
		else
		{
			if(settings.iRCONLoggedIn)
				sendRconCommand(szCMD, 0);
			else
				Log("RCON password was not set. Type login [password]");
		}

		return 1;
	}

	if(szCMD[0] != '!')
	{
		// SERVER CHAT OR COMMAND
		if(szCMD[0] == '/')
			sendServerCommand(szCMD);
		else
			sendChat(szCMD);

		return 0;
	}

	szCMD++;

	// EXIT
	if(!strncmp(szCMD, "exit", 4) || !strncmp(szCMD, "EXIT", 4) ||
		!strncmp(szCMD, "quit", 4) || !strncmp(szCMD, "QUIT", 4))
	{
		sampDisconnect(0);
		ExitProcess(0);

		return 3;
	}

	// RECONNECT
	if(!strncmp(szCMD, "reconnect", 9) || !strncmp(szCMD, "RECONNECT", 9))
	{
		sampDisconnect(0);
		resetPools(1, 2000);

		return 1;
	}

	// RELOAD SETTINGS
	if(!strncmp(szCMD, "reload", 6) || !strncmp(szCMD, "RELOAD", 6))
	{
		ReloadSettings();

		return 1;
	}

	// PLAYER LIST
	if(!strncmp(szCMD, "players", 7) || !strncmp(szCMD, "PLAYERS", 7))
	{
		int iPlayerCount = 0;
		Log(" ");
		Log("=== PLAYER LIST ===");
		Log("ID Name Ping Score");
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			if(!playerInfo[i].iIsConnected)
				continue;

			Log("(%d) %s %d %d", i, playerInfo[i].szPlayerName, playerInfo[i].dwPing, playerInfo[i].iScore);
			iPlayerCount++;
		}
		Log(" ");
		Log("=== [COUNT: %d] ===", iPlayerCount);
		Log(" ");

		return 1;
	}

	// GOTO
	if(!strncmp(szCMD, "goto", 4) || !strncmp(szCMD, "GOTO", 4))
	{
		int iPlayerID = atoi(&szCMD[5]);
		if(iPlayerID == -1)
		{
			normalMode_goto = -1;
			return 1;
		}

		if(iPlayerID < 0 || iPlayerID > MAX_PLAYERS)
			return 1;

		if(playerInfo[iPlayerID].iIsConnected)
		{
			normalMode_goto = (PLAYERID)iPlayerID;

			ONFOOT_SYNC_DATA ofSync;
			memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
			ofSync.byteHealth = 100;
			ofSync.vecPos[0] = playerInfo[iPlayerID].onfootData.vecPos[0];
			ofSync.vecPos[1] = playerInfo[iPlayerID].onfootData.vecPos[1];
			ofSync.vecPos[2] = playerInfo[iPlayerID].onfootData.vecPos[2];

			SendOnFootFullSyncData(&ofSync, 0, -1);
		}
		else
			Log("[GOTO] Player %d is not connected.", iPlayerID);

		return 1;
	}

	// IMITATE
	if(!strncmp(szCMD, "imitate", 7) || !strncmp(szCMD, "IMITATE", 7))
	{
		char *szPlayerName = &szCMD[8];
		if(!strcmp(szPlayerName,"-1"))
		{
			imitateID = -1;
			Log("[IMITATE] Imitate was disabled.");
			return 1;
		}

		PLAYERID playerID = getPlayerIDFromPlayerName(szPlayerName);

		if(playerID < 0 || playerID > MAX_PLAYERS)
			return 1;

		if(playerInfo[playerID].iIsConnected)
		{
			imitateID = (PLAYERID)playerID;
			Log("[IMITATE] Imitate ID set to %d (%s)", imitateID, szPlayerName);
		}
		else
			Log("[IMITATE] Player %s is not connected.", szPlayerName);

		return 1;
	}

	// VEHICLE LIST
	if(!strncmp(szCMD, "vlist", 5) || !strncmp(szCMD, "VLIST", 5))
	{
		for(VEHICLEID i = 0; i < MAX_VEHICLES; i++)
		{
			if(!vehiclePool[i].iDoesExist)
				continue;

			const struct vehicle_entry *vehicle = gta_vehicle_get_by_id( vehiclePool[i].iModelID );
			if(vehicle)
				Log("[VLIST] %d (%s)", i, vehicle->name);
		}

		return 1;
	}

	// SEND VEHICLE DEATH NOTIFICATION
	if(!strncmp(szCMD, "vdeath", 6) || !strncmp(szCMD, "VDEATH", 6))
	{
		int iSelectedVeh = atoi(&szCMD[7]);
		NotifyVehicleDeath((VEHICLEID)iSelectedVeh);
		Log("[VDEATH] Sent to vehicle ID %d", iSelectedVeh);

		return 1;
	}

	if(!strncmp(szCMD, "fu", 2) || !strncmp(szCMD, "fu", 2))
	{
		RakNet::BitStream bs;
		bs.Write((BYTE)ID_CONNECTION_LOST);
		pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		return 1;
	}

	// SELECT AN ITEM FROM THE GTA MENU
	if(!strncmp(szCMD, "menusel", 7) || !strncmp(szCMD, "MENUSEL", 7))
	{
		BYTE bRow = (BYTE)atoi(&szCMD[8]);

		if(bRow != 0xFF)
		{
			RakNet::BitStream bsSend;
			bsSend.Write(bRow);
			pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);			
		}

		return 1;
	}

	//FAKE KILL :-)
	if(!strncmp(szCMD, "kill", 4) || !strncmp(szCMD, "KILL", 4))
	{
		if(!settings.fakeKill) {
			Log("Started flooding :-)");
			settings.fakeKill=true;
		}else{
			settings.fakeKill=false;
			Log("Stopped flooding...");
		}
		return 1;
	}

	//LAG :-)
	if(!strncmp(szCMD, "lag", 4) || !strncmp(szCMD, "LAG", 4))
	{
		if(!settings.lag) {
			Log("Started lagging :-)");
			settings.lag=true;
		}else{
			settings.lag=false;
			Log("Stopped lagging...");
		}
		return 1;
	}

	if(!strncmp(szCMD, "spam", 4) || !strncmp(szCMD, "SPAM", 4))
	{
		if (settings.ispam) {
			Log("Stopped spamming...");
			settings.ispam = false;
		}else{
			Log("Started spamming...");
			settings.ispam = true;
		}
		return 1;
	}

	if(!strncmp(szCMD, "spawn", 5) || !strncmp(szCMD, "SPAWN", 5))
	{
		sampRequestClass(settings.iClassID);
		sampSpawn();
		return 1;
	}

	Log("Command %s was not found.", szCMD);

	return 2;
}
