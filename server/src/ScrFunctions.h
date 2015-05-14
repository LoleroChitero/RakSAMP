/*
	Updated to 0.3.7 by P3ti
*/

extern int iScriptsRunning;

struct stScript
{
	char szScriptName[MAX_SCRIPTS][32];
	lua_State *scriptVM[MAX_SCRIPTS];
};
extern struct stScript script;

void RegisterScriptingFunctions(lua_State *L);
bool LoadScript(char *szScriptName, bool bFirstLoad);
bool UnloadScript(char *szScriptName);
bool LoadScripts();
void HandleScriptingConsoleInputCommands(char *szInputBuffer);
void CleanUpScripting();
