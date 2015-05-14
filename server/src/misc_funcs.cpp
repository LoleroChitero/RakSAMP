/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

void AddBan(char * nick, char * ip_mask, char * reason)
{
	const struct tm *tm;
	time_t now;
	now = time(NULL);
	tm = localtime(&now);
	char *s;
	s = new char[256];
	strftime(s, 256, "[%d/%m/%y | %H:%M:%S]", tm);

	pRakServer->AddToBanList(ip_mask);

	FILE * fileBanList = fopen(BAN_FILENAME, "a");
	if(!fileBanList) return;

	fprintf(fileBanList,"%s %s %s - %s\n", ip_mask, s, nick, reason);
	fclose(fileBanList);

	delete [] s;
}

void RemoveBan(char* ip_mask)
{
	pRakServer->RemoveFromBanList(ip_mask);

	FILE* fileBanList = fopen(BAN_FILENAME, "r");
	FILE* fileWriteList = fopen(BAN_FILENAME ".temp", "w");
	if(!fileBanList || !fileWriteList) return;

	char line[256];
	char line2[256];
	char* ip;

	while(!feof(fileBanList))
	{
		if (fgets(line, 256, fileBanList))
		{
			strcpy(line2, line);
			ip = strtok(line, " \t");
			if (strcmp(ip_mask, ip) != 0)
			{
				fprintf(fileWriteList, "%s", line2);
			}
		}
	}

	fclose(fileBanList);
	fclose(fileWriteList);

	remove(BAN_FILENAME);
	rename(BAN_FILENAME ".temp", BAN_FILENAME);
}

void LoadBanList()
{
	pRakServer->ClearBanList();

	FILE * fileBanList = fopen(BAN_FILENAME, "r");

	if(!fileBanList)
		return;

	char tmpban_ip[256];
	while(!feof(fileBanList))
	{
		if (fgets(tmpban_ip,256,fileBanList))
		{
			int len = strlen(tmpban_ip);

			if (len > 0 && tmpban_ip[len - 1] == '\n')
				tmpban_ip[len - 1] = 0;

			len = strlen(tmpban_ip);

			if (len > 0 && tmpban_ip[len - 1] == '\r')
				tmpban_ip[len - 2] = 0;

			if (tmpban_ip[0] != 0 && tmpban_ip[0] != '#')
			{
				char *ban_ip = strtok(tmpban_ip, " \t");
				pRakServer->AddToBanList(ban_ip);
			}
		}
	}

	fclose(fileBanList);
}


void SendClientMessage(PLAYERID playerID, DWORD dwColor, char* szMessage, ...)
{
	va_list va;
	va_start(va, szMessage);
	char szBuffer[512] = { 0 };
	vsprintf(szBuffer, szMessage, va);
	va_end(va);

	RakNet::BitStream bsParams;
	DWORD dwStrLen = strlen(szBuffer);

	bsParams.Write(dwColor);
	bsParams.Write(dwStrLen);
	bsParams.Write(szBuffer, dwStrLen);
	pRakServer->RPC(&RPC_ClientMessage, &bsParams, HIGH_PRIORITY, RELIABLE,
		0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void SendChatMessage(PLAYERID playerID, char *szText, BYTE byteTextLen, int toAll)
{
	RakNet::BitStream bsSend;
	bsSend.Write(playerID);
	bsSend.Write(byteTextLen);
	bsSend.Write((const char *)szText,byteTextLen);
	if(!toAll)
		pRakServer->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE,
			0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	else
		pRakServer->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE,
			0, UNASSIGNED_PLAYER_ID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}
