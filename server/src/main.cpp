/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

TiXmlDocument xmlSettings;
char szWorkingDirectory[MAX_PATH];
int iMainLoop = 1;
RakServerInterface *pRakServer = NULL;
unsigned int _uiRndSrvChallenge;

int iPort;
unsigned short usMaxPlayers;

int iLagCompensation;

char szLogFile[MAX_PATH];
FILE *flLog = NULL;

struct stPlayerInfo playerInfo[MAX_PLAYERS];

int main(int argc, char *argv[])
{
	if(GetModuleFileName(NULL, szWorkingDirectory, sizeof(szWorkingDirectory) - 32) != 0)
	{
		if(strrchr(szWorkingDirectory, '\\') != NULL)
			*strrchr(szWorkingDirectory, '\\') = 0;
		else
			strcpy_s(szWorkingDirectory, 1, ".");
	}
	else
		strcpy_s(szWorkingDirectory, 1, ".");

	SetConsoleTitle("RakSAMP server");

	if(!xmlSettings.LoadFile("RakSAMPServer.xml"))
	{
		MessageBox(NULL, "Failed to load the config file", "ERROR ERROR ERROR ERROR", MB_ICONERROR);
		ExitProcess(0);
	}

	TiXmlElement* serverElement = xmlSettings.FirstChildElement("server");
	if(serverElement)
	{
		usMaxPlayers = (unsigned short)atoi(serverElement->Attribute("max_players"));
		iPort = (int)atoi(serverElement->Attribute("port"));
		strcpy(serverName, serverElement->Attribute("name"));
		iLagCompensation = (int)atoi(serverElement->Attribute("lagcomp"));

		if(iLagCompensation)
			modifyRuleValue("lagcomp", "On");
		else
			modifyRuleValue("lagcomp", "Off");
	}

	Log(" ");
	Log("  * ============================== *");
	Log("            RakSAMP server          ");
	Log("                                    ");
	Log("    Version: " RAKSAMP_VERSION "    ");
	Log("    Authors: " AUTHOR "             ");
	Log("  * ============================== *");
	Log(" ");

	LoadScripts();

	// Create a challenge number for the clients to be able to connect
	srand((unsigned int)time(NULL));
	_uiRndSrvChallenge = (unsigned int)rand();

	pRakServer = RakNetworkFactory::GetRakServerInterface();
	LoadBanList();

	pRakServer->Start(usMaxPlayers, 0, 5, iPort);
	pRakServer->StartOccasionalPing();
	RegisterServerRPCs(pRakServer);

	// Main loop
	while(iMainLoop)
	{
		UpdateNetwork();

		Sleep(5);
	}

	if(flLog != NULL)
	{
		fclose(flLog);
		flLog = NULL;
	}

	pRakServer->Disconnect(300);
	RakNetworkFactory::DestroyRakServerInterface(pRakServer);

	return 0;
}

void Log ( char *fmt, ... )
{
	SYSTEMTIME	time;
	va_list		ap;

	if ( flLog == NULL )
	{
		flLog = fopen( "RakSAMPServer.log", "a" );
		if ( flLog == NULL )
			return;
	}

	GetLocalTime( &time );
	fprintf( flLog, "[%02d:%02d:%02d.%03d] ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds );

	va_start( ap, fmt );
	vprintf( fmt, ap );
	vfprintf( flLog, fmt, ap );
	va_end( ap );

	fprintf( flLog, "\n" );
	printf( "\n" );
	fflush( flLog );
}

unsigned char rand_byteRange(unsigned char a, unsigned char b) { return ((b-a)*((unsigned char)rand()/RAND_MAX))+a; }
unsigned short rand_shortRange(unsigned short a, unsigned short b) { return ((b-a)*((unsigned short)rand()/RAND_MAX))+a; }
unsigned int rand_intRange(unsigned int a, unsigned int b) { return ((b-a)*((unsigned int)rand()/RAND_MAX))+a; }
float rand_floatRange(float a, float b) { return ((b-a)*((float)rand()/RAND_MAX))+a; }
void gen_random(char *s, const int len)
{
	static const char alphanum[] =
		"0123456789~"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}
