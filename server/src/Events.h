/*
	Updated to 0.3.7 by P3ti
*/

void ScriptEvent_OnScriptStart(lua_State *L);
void ScriptEvent_OnScriptExit(lua_State *L);

void ScriptEvent_OnNewQuery(lua_State *L, char *pszIP, int iSourcePort, bool bBanned);
void ScriptEvent_OnNewConnection(lua_State *L, int iPlayerID, char *pszIP, int iSourcePort);
void ScriptEvent_OnPlayerJoin(lua_State *L, int iPlayerID, char *pszPlayerName, char *pszIP, int iSourcePort);
void ScriptEvent_OnPlayerDisconnect(lua_State *L, int iPlayerID, char *pszPlayerName, char *pszReason);
void ScriptEvent_OnPlayerRequestClass(lua_State *L, int iPlayerID, int iClassID);
void ScriptEvent_OnPlayerSpawn(lua_State *L, int iPlayerID);
void ScriptEvent_OnPlayerDeath(lua_State *L, int iPlayerID, int iKillerID, int iReasonID);
void ScriptEvent_OnPlayerDamageVehicle(lua_State *L, int iPlayerID, int iVehicleID);
void ScriptEvent_OnPlayerWantsEnterVehicle(lua_State *L, int iPlayerID, int iVehicleID, int iAsPassenger);
void ScriptEvent_OnPlayerLeaveVehicle(lua_State *L, int iPlayerID, int iVehicleID);
void ScriptEvent_OnPlayerMessage(lua_State *L, int iPlayerID, char *pszMessage);
void ScriptEvent_OnPlayerCommand(lua_State *L, int iPlayerID, char *pszCommand);
void ScriptEvent_OnPlayerWeaponShot(lua_State *L, int iPlayerID, int iWeaponID, int bHitType, int iHitID, float fX, float fY, float fZ);
void ScriptEvent_OnPlayerEnterCheckpoint(lua_State *L, int iPlayerID);
void ScriptEvent_OnPlayerLeaveCheckpoint(lua_State *L, int iPlayerID);
void ScriptEvent_OnPlayerClickMap(lua_State *L, int iPlayerID, float fX, float fY, float fZ);
void ScriptEvent_OnDialogResponse(lua_State *L, int iPlayerID, int iDialogID, int iButtonID, int iListBoxItem, char* szInputResp);
void ScriptEvent_OnPlayerInteriorChange(lua_State *L, int iPlayerID, BYTE byteInteriorId);
void ScriptEvent_OnScmEvent(lua_State *L, int iPlayerID, int iEventID, DWORD dwParams1, DWORD dwParams2, DWORD dwParams3);