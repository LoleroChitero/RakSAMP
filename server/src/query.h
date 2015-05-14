/*
	Updated to 0.3.7 by P3ti
*/

extern char serverName[256];

char* getRuleValue(char* szSRule);
void modifyRuleValue(char* szSRule, char* szMValue);

void handleQueries(SOCKET sListen, int iAddrSize, struct sockaddr_in client, char *pBuffer);
