/*
	Updated to 0.3z by P3ti
*/

extern char serverName[256];

void handleQueries(SOCKET sListen, int iAddrSize, struct sockaddr_in client, char *pBuffer);
