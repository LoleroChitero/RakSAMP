/*
	Updated to 0.3.7 by P3ti
*/

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "../../common/common.h"

#define BAN_FILENAME "RakSAMPServer.ban"

#include "PacketEnumerations.h"
#include "RakNetworkFactory.h"
#include "RakServerInterface.h"
#include "NetworkTypes.h"
#include "BitStream.h"
#include "StringCompressor.h"

#include "../../tinyxml/tinyxml.h"
#include "samp_netencr.h"
#include "SAMPRPC.h"
#include "vehicles.h"
#include "players.h"
#include "query.h"
#include "netgame.h"
#include "RPCs.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "misc_funcs.h"
#include "Events.h"
#include "ScrInternal.h"
#include "ScrFunctions.h"


extern TiXmlDocument xmlSettings;

extern int iPort;
extern unsigned short usMaxPlayers;

extern struct stPlayerInfo playerInfo[MAX_PLAYERS];

extern int iMainLoop;
extern RakServerInterface *pRakServer;
extern unsigned int _uiRndSrvChallenge;

void Log ( char *fmt, ... );
void gen_random(char *s, const int len);
