/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

void sendRconCommand(char *szRCONCmd, int iIsLogin)
{
	int iAddrSize = sizeof(struct sockaddr_in);
	int queryLen = 0;
	char queryBufferSend[1024];
	BYTE passLen = (BYTE)strlen(settings.szRCONPass);
	BYTE cmdLen = (BYTE)strlen(szRCONCmd);
	
	memset(queryBufferSend, 0, sizeof(queryBufferSend));

	*(unsigned int *)&queryBufferSend[0] = 0x504D4153; queryLen += 4;
	*(unsigned int *)&queryBufferSend[queryLen] = settings.RCONserver.sin_addr.s_addr; queryLen += 4;
	*(unsigned short *)&queryBufferSend[queryLen] = settings.RCONserver.sin_port; queryLen += 2;
	*(unsigned short *)&queryBufferSend[queryLen] = 0x78; queryLen += 1;

	*(unsigned short *)&queryBufferSend[queryLen] = passLen; queryLen += 2;
	strncpy(&queryBufferSend[queryLen], settings.szRCONPass, passLen); queryLen += passLen;

	if(!iIsLogin)
	{
		*(unsigned short *)&queryBufferSend[queryLen] = cmdLen; queryLen += 2;
		strncpy(&queryBufferSend[queryLen], szRCONCmd, cmdLen); queryLen += cmdLen;
	}

	sendto(settings.RCONSocket, queryBufferSend, queryLen, 0, (struct sockaddr *)&settings.RCONserver, iAddrSize);
}

int RCONReceiveLoop()
{
	WSADATA w;
	if (WSAStartup(0x0101, &w) != 0)
		return 0;

	settings.RCONSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (settings.RCONSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 0;
	}

	memset((void *)&settings.RCONserver, 0, sizeof(struct sockaddr_in));
	settings.RCONserver.sin_family = AF_INET;
	settings.RCONserver.sin_port = htons(settings.server.iPort);
	struct hostent *pAddr = gethostbyname(settings.server.szAddr);
	if(pAddr)
		memcpy(&(settings.RCONserver.sin_addr.s_addr), pAddr->h_addr, pAddr->h_length);
	else
		settings.RCONserver.sin_addr.s_addr = inet_addr(settings.server.szAddr);
	
	settings.RCONclient.sin_family = AF_INET;
	settings.RCONclient.sin_port = htons(0);

	Log("RCON mode initialized (%s:%d). Type login [password] to set the password.",
		settings.server.szAddr, settings.server.iPort);

	int iFromLen = sizeof(struct sockaddr_in);
	int recvlen;
	struct sockaddr_in client;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	while(1)
	{
		recvlen = recvfrom(settings.RCONSocket, buf, 1024, 0, (struct sockaddr *)&client, &iFromLen);
		if(recvlen > 0)
		{
			if(buf[10] == 0x78)
			{
				char szResp[1024];
				memset(szResp, 0, sizeof(szResp));
				unsigned short usRespLen = (unsigned short)buf[11];
				char *pszResponse = &buf[13];
				char *pszResponseBackup = pszResponse;
				if(pszResponse && usRespLen <= 1024)
				{
					while(*pszResponseBackup)
					{
						if(*pszResponseBackup >= 0x01 && *pszResponseBackup <= 0x1F)
							*pszResponseBackup = 0x20;
						pszResponseBackup++;
					}

					strncpy(szResp, pszResponse, usRespLen);
					szResp[usRespLen] = 0;
					Log(szResp);
				}
			}
		}

		Sleep(5);
	}

	return 1;
}