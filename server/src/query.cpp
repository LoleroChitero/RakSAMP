/*
	Updated to 0.3z by P3ti
*/

#include "main.h"

char serverName[256];
char gmName[] = "Unknown";

unsigned short numberOfRules = 6;
struct stRules
{
	char szRule[64];
	char szValue[64];
};
struct stRules rules[] =
{
	{ "gravity", "0.008" },
	{ "mapname", "San Andreas" },
	{ "version", "RakSAMP " RAKSAMP_VERSION },
	{ "weather", "10" },
	{ "weburl", "tr7network.com" },
	{ "worldtime", "12:00" }
};

char queryBufferSend[4092];
void handleQueries(SOCKET sListen, int iAddrSize, struct sockaddr_in client, char *buffer)
{
	struct sockaddr_in *addrin = (struct sockaddr_in *)(struct sockaddr *)&client;
	char *ip_address = inet_ntoa(addrin->sin_addr);
	
	if(pRakServer->IsBanned(ip_address))
	{
		if(buffer[10] == 0x70)
		{
			for(int i = 0; i < iScriptsRunning; i++)
			{
				if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
					ScriptEvent_OnNewQuery(script.scriptVM[i], ip_address, addrin->sin_port, true);
			}
		}
		if(buffer[10] == 0x70 || buffer[10] == 0x69 || buffer[10] == 0x63 || buffer[10] == 0x72) return;
	}

	int queryLen = 0;
	if(buffer[10] == 0x70) // Ping query
	{
		for(int i = 0; i < iScriptsRunning; i++)
		{
			if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
				ScriptEvent_OnNewQuery(script.scriptVM[i], ip_address, addrin->sin_port, false);
		}

		memcpy(queryBufferSend, buffer, 10); queryLen += 10;
		*(unsigned char *)&queryBufferSend[10] = 0x70; queryLen += 1;
		*(unsigned int *)&queryBufferSend[11] = *(unsigned int *)&buffer[11]; queryLen += 4;
		sendto(sListen, queryBufferSend, queryLen, 0, (struct sockaddr *)&client, iAddrSize);
	}

	else if(buffer[10] == 0x69) // Server name, player count, game mode name query
	{
		memcpy(queryBufferSend, buffer, 10); queryLen += 10;
		*(unsigned short *)&queryBufferSend[10] = 0x69; queryLen += 2;
		*(unsigned short *)&queryBufferSend[12] = playerCount; queryLen += 2;
		*(unsigned short *)&queryBufferSend[14] = usMaxPlayers; queryLen += 2;
		int serverNameLen = (int)strlen(serverName); *(int *)&queryBufferSend[16] = serverNameLen; queryLen += 4;
		strncpy(&queryBufferSend[20], serverName, serverNameLen); queryLen += serverNameLen;
		int gmNameLen = (int)strlen(gmName); *(int *)&queryBufferSend[20 + serverNameLen] = gmNameLen; queryLen += 4;
		strncpy(&queryBufferSend[20 + serverNameLen + 4], gmName, gmNameLen); queryLen += gmNameLen;

		sendto(sListen, queryBufferSend, queryLen, 0, (struct sockaddr *)&client, iAddrSize);

		return;
	}

	else if(buffer[10] == 0x63) // Player list query
	{
		memcpy(queryBufferSend, buffer, 10); queryLen += 10;
		*(unsigned char *)&queryBufferSend[10] = 0x63; queryLen += 1;
		*(unsigned short *)&queryBufferSend[11] = playerCount; queryLen += 2;
		char *curbufpos = &queryBufferSend[13];
		int bufcount = 0;
		for(unsigned short i = 0; i < playerCount; i++)
		{
			unsigned char pnamelen = (unsigned char)strlen(playerPool[i].szPlayerName);
			curbufpos[0] = pnamelen;
			strncpy(&curbufpos[1], playerPool[i].szPlayerName, pnamelen);
			*(int *)&curbufpos[1 + pnamelen] = playerPool[i].iPlayerScore;
			curbufpos += (1 + pnamelen + 4);
			bufcount += (1 + pnamelen + 4);
		}
		queryLen += bufcount;

		sendto(sListen, queryBufferSend, queryLen, 0, (struct sockaddr *)&client, iAddrSize);

		return;
	}

	else if(buffer[10] == 0x72) // Rules query
	{
		memcpy(queryBufferSend, buffer, 10); queryLen += 10;
		*(unsigned char *)&queryBufferSend[10] = 0x72; queryLen += 1;
		*(unsigned short *)&queryBufferSend[11] = numberOfRules; queryLen += 2;
		char *curbufpos = &queryBufferSend[13];
		for(unsigned short i = 0; i < numberOfRules; i++)
		{
			unsigned char rulelen = (unsigned char)strlen(rules[i].szRule);
			curbufpos[0] = rulelen;
			strncpy(&curbufpos[1], rules[i].szRule, rulelen);
			unsigned char valuelen = (unsigned char)strlen(rules[i].szValue);
			curbufpos[1 + rulelen] = valuelen;
			strncpy(&curbufpos[1 + rulelen + 1], rules[i].szValue, valuelen);
			curbufpos += (1 + rulelen + 1 + valuelen);
		}
		curbufpos = &queryBufferSend[13];
		queryLen += strlen(curbufpos);

		sendto(sListen, queryBufferSend, queryLen, 0, (struct sockaddr *)&client, iAddrSize);

		return;
	}
}
