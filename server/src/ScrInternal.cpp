/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

#define Define(name, value) strcat(szScript, name); strcat(szScript, " = "); strcat(szScript, value); strcat(szScript, "\n");

void generateAndLoadInternalScript(lua_State *L)
{
	char *szScript = new char[10334];

	sprintf(szScript, "MAX_PLAYERS = %u\nMAX_VEHICLES = %u\n", MAX_PLAYERS, MAX_VEHICLES);

	Define("KEY_ACTION", "1");
	Define("KEY_CROUCH", "2");
	Define("KEY_FIRE", "4");
	Define("KEY_SPRINT", "8");
	Define("KEY_SECONDARY_ATTACK", "16");
	Define("KEY_JUMP", "32");
	Define("KEY_LOOK_RIGHT", "64");
	Define("KEY_HANDBRAKE", "128");
	Define("KEY_LOOK_LEFT", "256");
	Define("KEY_SUBMISSION", "512");
	Define("KEY_LOOK_BEHIND", "512");
	Define("KEY_WALK", "1024");
	Define("KEY_ANALOG_UP", "2048");
	Define("KEY_ANALOG_DOWN", "4096");
	Define("KEY_ANALOG_LEFT", "8192");
	Define("KEY_ANALOG_RIGHT", "16384");
	Define("KEY_YES", "65536");
	Define("KEY_NO", "131072");
	Define("KEY_CTRL_BACK", "262144");

	Define("KEY_UP", "-128");
	Define("KEY_DOWN", "128");
	Define("KEY_LEFT", "-128");
	Define("KEY_RIGHT", "128");

	Define("DIALOG_STYLE_MSGBOX", "0");
	Define("DIALOG_STYLE_INPUT", "1");
	Define("DIALOG_STYLE_LIST", "2");
	Define("DIALOG_STYLE_PASSWORD", "3");

	Define("EVENT_TYPE_PAINTJOB", "1");
	Define("EVENT_TYPE_CARCOMPONENT", "2");
	Define("EVENT_TYPE_CARCOLOR", "3");
	Define("EVENT_ENTEREXIT_MODSHOP", "4");

	Define("BULLET_HIT_TYPE_NONE", "0");
	Define("BULLET_HIT_TYPE_PLAYER", "1");
	Define("BULLET_HIT_TYPE_VEHICLE", "2");
	Define("BULLET_HIT_TYPE_OBJECT", "3");
	Define("BULLET_HIT_TYPE_PLAYER_OBJECT", "4");

	char buf[256];
	for(int n = 0; n < VEHICLE_LIST_SIZE; n++)
	{
		sprintf(buf, "%s = %u\n", vehicle_list[n].name, vehicle_list[n].id);
		strcat(szScript, buf);
	}

	int error = luaL_loadbuffer(L, szScript, strlen(szScript), "line");
	if(!error)
		lua_pcall(L, 0, 0, 0);
	else
	{
		Log("Error loading memory script: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	free(szScript);
}
