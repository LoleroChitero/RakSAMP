/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

void ScriptEvent_OnScriptStart(lua_State *L)
{
	lua_getglobal(L, "onScriptStart");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	if(lua_pcall(L, 0, 0, 0) != 0)
		Log("WARNING: Error calling onScriptStart:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnScriptExit(lua_State *L)
{
	lua_getglobal(L, "onScriptExit");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	if(lua_pcall(L, 0, 0, 0) != 0)
		Log("WARNING: Error calling onScriptExit:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnNewQuery(lua_State *L, char *pszIP, int iSourcePort, bool bBanned)
{
	lua_getglobal(L, "onNewQuery");
	if(!lua_isfunction(L, -1)) return;

	lua_pushstring(L, pszIP);
	lua_pushnumber(L, iSourcePort);
	lua_pushboolean(L, bBanned);

	if(lua_pcall(L, 3, 0, 0) != 0)
		Log("WARNING: Error calling onNewQuery:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnNewConnection(lua_State *L, int iPlayerID, char *pszIP, int iSourcePort)
{
	lua_getglobal(L, "onNewConnection");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushnumber(L, iPlayerID);
	lua_pushstring(L, pszIP);
	lua_pushnumber(L, iSourcePort);

	if(lua_pcall(L, 3, 0, 0) != 0)
		Log("WARNING: Error calling onNewConnection:\n%s", lua_tostring(L, -1));
}


void ScriptEvent_OnPlayerJoin(lua_State *L, int iPlayerID, char *pszPlayerName, char *pszIP, int iSourcePort)
{
	lua_getglobal(L, "onPlayerJoin");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushnumber(L, iPlayerID);
	lua_pushlstring(L, pszPlayerName, strlen(pszPlayerName));
	lua_pushstring(L, pszIP);
	lua_pushnumber(L, iSourcePort);

	if(lua_pcall(L, 4, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerJoin:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerDisconnect(lua_State *L, int iPlayerID, char *pszPlayerName, char *pszReason)
{
	lua_getglobal(L, "onPlayerDisconnect");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushnumber(L, iPlayerID);
	lua_pushstring(L, pszPlayerName);
	lua_pushstring(L, pszReason);

	if(lua_pcall(L, 3, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerDisconnect:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerRequestClass(lua_State *L, int iPlayerID, int iClassID)
{
	lua_getglobal(L, "onPlayerRequestClass");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iClassID);

	if(lua_pcall(L, 2, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerRequestClass:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerSpawn(lua_State *L, int iPlayerID)
{
	lua_getglobal(L, "onPlayerSpawn");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);

	if(lua_pcall(L, 1, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerSpawn:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerDeath(lua_State *L, int iPlayerID, int iKillerID, int iReasonID)
{
	lua_getglobal(L, "onPlayerDeath");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iKillerID);
	lua_pushinteger(L, iReasonID);

	if(lua_pcall(L, 3, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerDeath:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerDamageVehicle(lua_State *L, int iPlayerID, int iVehicleID)
{
	lua_getglobal(L, "onPlayerDamageVehicle");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iVehicleID);

	if(lua_pcall(L, 2, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerDamageVehicle:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerWantsEnterVehicle(lua_State *L, int iPlayerID, int iVehicleID, int iAsPassenger)
{
	lua_getglobal(L, "onPlayerWantsEnterVehicle");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iVehicleID);
	lua_pushinteger(L, iAsPassenger);

	if(lua_pcall(L, 3, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerWantsEnterVehicle:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerLeaveVehicle(lua_State *L, int iPlayerID, int iVehicleID)
{
	lua_getglobal(L, "onPlayerLeaveVehicle");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iVehicleID);

	if(lua_pcall(L, 2, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerLeaveVehicle:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerMessage(lua_State *L, int iPlayerID, char *pszMessage)
{
	lua_getglobal(L, "onPlayerMessage");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushstring(L, pszMessage);

	if(lua_pcall(L, 2, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerMessage:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerCommand(lua_State *L, int iPlayerID, char *pszCommand)
{
	lua_getglobal(L, "onPlayerCommand");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushstring(L, pszCommand);

	if(lua_pcall(L, 2, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerCommand:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerWeaponShot(lua_State *L, int iPlayerID, int iWeaponID, int bHitType, int iHitID, float fX, float fY, float fZ)
{
	lua_getglobal(L, "onPlayerWeaponShot");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iWeaponID);
	lua_pushinteger(L, bHitType);
	lua_pushinteger(L, iHitID);
	lua_pushnumber(L, fX);
	lua_pushnumber(L, fY);
	lua_pushnumber(L, fZ);

	if(lua_pcall(L, 7, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerWeaponShot:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerEnterCheckpoint(lua_State *L, int iPlayerID)
{
	lua_getglobal(L, "onPlayerEnterCheckpoint");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);

	if(lua_pcall(L, 1, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerEnterCheckpoint:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerLeaveCheckpoint(lua_State *L, int iPlayerID)
{
	lua_getglobal(L, "onPlayerLeaveCheckpoint");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);

	if(lua_pcall(L, 1, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerLeaveCheckpoint:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerClickMap(lua_State *L, int iPlayerID, float fX, float fY, float fZ)
{
	lua_getglobal(L, "onPlayerClickMap");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushnumber(L, fX);
	lua_pushnumber(L, fY);
	lua_pushnumber(L, fZ);

	if(lua_pcall(L, 4, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerClickMap:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnDialogResponse(lua_State *L, int iPlayerID, int iDialogID, int iButtonID, int iListBoxItem, char* szInputResp)
{
	lua_getglobal(L, "onDialogResponse");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iDialogID);
	lua_pushinteger(L, iButtonID);
	lua_pushinteger(L, iListBoxItem);
	lua_pushstring(L, szInputResp);

	if(lua_pcall(L, 5, 0, 0) != 0)
		Log("WARNING: Error calling onDialogResponse:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnPlayerInteriorChange(lua_State *L, int iPlayerID, BYTE byteInteriorId)
{
	lua_getglobal(L, "onPlayerInteriorChange");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, byteInteriorId);

	if(lua_pcall(L, 2, 0, 0) != 0)
		Log("WARNING: Error calling onPlayerInteriorChange:\n%s", lua_tostring(L, -1));
}

void ScriptEvent_OnScmEvent(lua_State *L, int iPlayerID, int iEventID, DWORD dwParams1, DWORD dwParams2, DWORD dwParams3)
{
	lua_getglobal(L, "onScmEvent");
	if(!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	lua_pushinteger(L, iPlayerID);
	lua_pushinteger(L, iEventID);
	lua_pushinteger(L, dwParams1);
	lua_pushinteger(L, dwParams2);
	lua_pushinteger(L, dwParams3);

	if(lua_pcall(L, 5, 0, 0) != 0)
		Log("WARNING: Error calling onScmEvent:\n%s", lua_tostring(L, -1));
}