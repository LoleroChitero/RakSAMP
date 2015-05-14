/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

using namespace RakNet;
int iScriptsRunning = 0;
struct stScript script;

extern char* gmName;
extern float m_fGravity;

int OutputConsole(lua_State *L)
{
	Log("%s", lua_tostring(L, 1));
	return 1;
}

int SetGameModeText(lua_State *L)
{
	gmName = (char*)lua_tostring(L, 1);
	return 1;
}

int SetMapName(lua_State *L)
{
	modifyRuleValue("mapname", (char*)lua_tostring(L, 1));
	return 1;
}

int SetWebURL(lua_State *L)
{
	modifyRuleValue("weburl", (char*)lua_tostring(L, 1));
	return 1;
}

int IsPlayerConnected(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);
	
	int iIsConnected = isPlayerConnected(playerID);
	lua_pushnumber(L, iIsConnected);

	return iIsConnected;
}

int GetPlayerName(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);

	if(isPlayerConnected(playerID))
	{
		lua_pushstring(L, playerPool[playerID].szPlayerName);
		return 1;
	}

	lua_pushstring(L, "nil");

	return 0;
}

int GetPlayerPos(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);

	if(isPlayerConnected(playerID))
	{
		float x, y, z;
		if(playerInfo[playerID].incarData.VehicleID)
		{
			x = playerInfo[playerID].incarData.vecPos[0];
			y = playerInfo[playerID].incarData.vecPos[1];
			z = playerInfo[playerID].incarData.vecPos[2];
		}
		else
		{
			x = playerInfo[playerID].onfootData.vecPos[0];
			y = playerInfo[playerID].onfootData.vecPos[1];
			z = playerInfo[playerID].onfootData.vecPos[2];
		}

		lua_pushnumber(L, (lua_Number)x);
		lua_pushnumber(L, (lua_Number)y);
		lua_pushnumber(L, (lua_Number)z);

		return 3;
	}

	return 0;
}

int GetPlayerInterior(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);

	if(isPlayerConnected(playerID))
	{
		lua_pushnumber(L, playerInfo[playerID].byteInteriorId);
		return 1;
	}

	return 0;
}

int GetPlayerScore(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);

	if(isPlayerConnected(playerID))
	{
		lua_pushnumber(L, playerPool[playerID].iPlayerScore);
		return 1;
	}

	return 0;
}

int SetPlayerScore(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);
	int iScore = lua_tointeger(L, 2);

	if(isPlayerConnected(playerID))
	{
		playerPool[playerID].iPlayerScore = iScore;
		return 1;
	}

	return 0;
}

int GetPlayerIP(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);

	if(isPlayerConnected(playerID))
	{
		const char *ip = pRakServer->GetPlayerIDFromIndex(playerID).ToString(false);
		if(ip)
			lua_pushstring(L, ip);

		return 1;
	}

	return 0;
}

int GetPlayerPing(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);

	if(isPlayerConnected(playerID))
	{
		DWORD dwPing = playerPool[playerID].dwPlayerPing;

		lua_pushnumber(L, (DWORD)dwPing);

		return 1;
	}

	return 0;
}

int GetPlayerVehicleID(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);

	if(isPlayerConnected(playerID))
	{
		lua_pushnumber(L, playerPool[playerID].currentVehicleID);
		return 1;
	}

	return 0;
}

int AddStaticVehicle(lua_State *L)
{
	int iVehModelID = lua_tointeger(L, 1);
	lua_Number fX = lua_tonumber(L, 2);
	lua_Number fY = lua_tonumber(L, 3);
	lua_Number fZ = lua_tonumber(L, 4);
	lua_Number fRot = lua_tonumber(L, 5);
	BYTE bColor1 = lua_tointeger(L, 6);
	BYTE bColor2 = lua_tointeger(L, 7);
	int bRespawnOnDeath = lua_tointeger(L, 8);
	int iTimeUntilRespawn = lua_tointeger(L, 9);

	addStaticVehicle(iVehModelID, (float)fX, (float)fY, (float)fZ, (float)fRot, bColor1, bColor2, bRespawnOnDeath, iTimeUntilRespawn);

	return 1;
}

int CreateVehicle(lua_State *L)
{
	int iVehModelID = lua_tointeger(L, 1);
	lua_Number fX = lua_tonumber(L, 2);
	lua_Number fY = lua_tonumber(L, 3);
	lua_Number fZ = lua_tonumber(L, 4);
	lua_Number fRot = lua_tonumber(L, 5);
	BYTE bColor1 = lua_tointeger(L, 6);
	BYTE bColor2 = lua_tointeger(L, 7);

	addVehicle(iVehModelID, (float)fX, (float)fY, (float)fZ, (float)fRot, bColor1, bColor2);

	return 1;
}

int RemoveVehicle(lua_State *L)
{
	VEHICLEID vehicleID = lua_tointeger(L, 1);
	if(!doesVehicleExist(vehicleID)) return 0;

	BitStream bs;
	bs.Write(vehicleID);
	pRakServer->RPC(&RPC_WorldVehicleRemove, &bs, HIGH_PRIORITY, RELIABLE,
		0, UNASSIGNED_PLAYER_ID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

	removeVehicle(vehicleID);

	return 1;
}

int RepairVehicle(lua_State *L)
{
	VEHICLEID vehicleID = lua_tointeger(L, 1);

	if(!doesVehicleExist(vehicleID)) return 0;

	RakNet::BitStream bs;

	bs.Write(vehicleID);
	bs.Write(1000.0f);

	pRakServer->RPC(&RPC_ScrSetVehicleHealth, &bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

	bs.Reset();

	bs.Write(vehicleID);
	bs.Write((uint32_t)0);
	bs.Write((uint32_t)0);
	bs.Write((uint8_t)0);
	bs.Write((uint8_t)0);

	pRakServer->RPC(&RPC_DamageVehicle, &bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetVehicleHealth(lua_State *L)
{
	VEHICLEID vehicleID = lua_tointeger(L, 1);

	if(!doesVehicleExist(vehicleID)) return 0;

	lua_Number iVehicleHealth = lua_tonumber(L, 2);

	RakNet::BitStream bs;

	bs.Write(vehicleID);
	bs.Write((float)iVehicleHealth);

	pRakServer->RPC(&RPC_ScrSetVehicleHealth, &bs, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SendPlayerMessage(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number dwColor = lua_tonumber(L, 2);
	char *szMessage = (char *)lua_tostring(L, 3);

	BYTE bMessageLen = strlen(szMessage);
	SendClientMessage(playerID, (DWORD)dwColor, szMessage);

	return 1;
}

int SendPlayerChatMessage(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);
	char *szMessage = (char *)lua_tostring(L, 2);

	BYTE bMessageLen = strlen(szMessage);
	SendChatMessage(playerID, szMessage, bMessageLen, 0);

	return 1;
}

int SendPlayerChatMessageToAll(lua_State *L)
{
	PLAYERID playerID = lua_tointeger(L, 1);
	char *szMessage = (char *)lua_tostring(L, 2);

	BYTE bMessageLen = strlen(szMessage);
	SendChatMessage(playerID, szMessage, bMessageLen, 1);

	return 1;
}

int SetPlayerPos(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number fX = lua_tonumber(L, 2);
	lua_Number fY = lua_tonumber(L, 3);
	lua_Number fZ = lua_tonumber(L, 4);

	bs.Write((float)fX);
	bs.Write((float)fY);
	bs.Write((float)fZ);

	pRakServer->RPC(&RPC_ScrSetPlayerPos, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerPosFindZ(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number fX = lua_tonumber(L, 2);
	lua_Number fY = lua_tonumber(L, 3);
	lua_Number fZ = lua_tonumber(L, 4);

	bs.Write((float)fX);
	bs.Write((float)fY);
	bs.Write((float)fZ);

	pRakServer->RPC(&RPC_ScrSetPlayerPosFindZ, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerRotation(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number fRot = lua_tonumber(L, 2);

	bs.Write((float)fRot);

	pRakServer->RPC(&RPC_ScrSetPlayerFacingAngle, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerInterior(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number interiorID = lua_tointeger(L, 2);

	playerInfo[playerID].byteInteriorId = (BYTE)interiorID;

	bs.Write((BYTE)interiorID);

	pRakServer->RPC(&RPC_ScrSetInterior, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerCameraPos(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);

	lua_Number fX = lua_tonumber(L, 2);
	lua_Number fY = lua_tonumber(L, 3);
	lua_Number fZ = lua_tonumber(L, 4);

	bs.Write((float)fX);
	bs.Write((float)fY);
	bs.Write((float)fZ);

	pRakServer->RPC(&RPC_ScrSetCameraPos, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerCameraLookAt(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);

	lua_Number fX = lua_tonumber(L, 2);
	lua_Number fY = lua_tonumber(L, 3);
	lua_Number fZ = lua_tonumber(L, 4);

	lua_Number bCut = lua_tonumber(L, 5);

	bs.Write((float)fX);
	bs.Write((float)fY);
	bs.Write((float)fZ);
	bs.Write((BYTE)bCut);

	pRakServer->RPC(&RPC_ScrSetCameraLookAt, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetCameraBehindPlayer(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);

	pRakServer->RPC(&RPC_ScrSetCameraBehindPlayer, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int GivePlayerMoney(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number iMoney = lua_tonumber(L, 2);

	bs.Write((int)iMoney);

	pRakServer->RPC(&RPC_ScrHaveSomeMoney, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int ResetPlayerMoney(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);

	pRakServer->RPC(&RPC_ScrResetMoney, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerHealth(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number iHealth = lua_tonumber(L, 2);

	bs.Write((float)iHealth);

	pRakServer->RPC(&RPC_ScrSetPlayerHealth, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerArmour(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number iArmour = lua_tonumber(L, 2);

	bs.Write((float)iArmour);

	pRakServer->RPC(&RPC_ScrSetPlayerArmour, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int GiveWeapon(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number iWeapon = lua_tonumber(L, 2);
	lua_Number iAmmo = lua_tonumber(L, 3);

	bs.Write((unsigned long)iWeapon);
	bs.Write((unsigned long)iAmmo);

	pRakServer->RPC(&RPC_ScrGivePlayerWeapon, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetWeaponAmmo(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number iWeapon = lua_tonumber(L, 2);
	lua_Number iAmmo = lua_tonumber(L, 3);

	bs.Write((unsigned char)iWeapon);
	bs.Write((unsigned short)iAmmo);

	pRakServer->RPC(&RPC_ScrSetWeaponAmmo, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int ClearPlayerWeapons(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);

	pRakServer->RPC(&RPC_ScrResetPlayerWeapons, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int PlayAudioStreamForPlayer(lua_State *L)
{
	BitStream bs;

	PLAYERID playerID = lua_tointeger(L, 1);
	
	char szURL[256];
	sprintf_s(szURL, 256, lua_tostring(L, 2));

	lua_Number PosX = lua_tonumber(L, 3);
	lua_Number PosY = lua_tonumber(L, 4);
	lua_Number PosZ = lua_tonumber(L, 5);
	lua_Number Distance = lua_tonumber(L, 6);
	lua_Number UsePos = lua_tointeger(L, 7);

	bs.Write((unsigned char)strlen(szURL));
	bs.Write(szURL, strlen(szURL));
	bs.Write((float)PosX);
	bs.Write((float)PosY);
	bs.Write((float)PosZ);
	bs.Write((float)Distance);
	bs.Write((unsigned char)UsePos);

	pRakServer->RPC(&RPC_PlayAudioStream, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int StopAudioStreamForPlayer(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);

	pRakServer->RPC(&RPC_StopAudioStream, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SendDeathMessage(lua_State *L)
{
	BitStream bs;

	PLAYERID killerID = lua_tointeger(L, 1);
	PLAYERID killeeID = lua_tointeger(L, 2);
	unsigned char reasonID = lua_tointeger(L, 3);

	bs.Write((unsigned short)killerID);
	bs.Write((unsigned short)killeeID);
	bs.Write((unsigned char)reasonID);

	pRakServer->RPC(&RPC_ScrDeathMessage, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(0xFFFF), TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SendDeathMessageForPlayer(lua_State *L)
{
	BitStream bs;

	PLAYERID playerID = lua_tointeger(L, 1);
	PLAYERID killerID = lua_tointeger(L, 2);
	PLAYERID killeeID = lua_tointeger(L, 3);
	unsigned char reasonID = lua_tointeger(L, 4);

	bs.Write((unsigned short)killerID);
	bs.Write((unsigned short)killeeID);
	bs.Write((unsigned char)reasonID);

	pRakServer->RPC(&RPC_ScrDeathMessage, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetGravity(lua_State *L)
{
	BitStream bs;
	lua_Number fGravity = lua_tointeger(L, 1);

	m_fGravity = (float)fGravity;
	bs.Write((float)fGravity);
	
	pRakServer->RPC(&RPC_ScrSetGravity, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(0xFFFF), TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerGravity(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number fGravity = lua_tointeger(L, 2);

	bs.Write((float)fGravity);
	
	pRakServer->RPC(&RPC_ScrSetGravity, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int SetPlayerCheckpoint(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);
	lua_Number fCheckpointX = lua_tointeger(L, 2);
	lua_Number fCheckpointY = lua_tointeger(L, 3);
	lua_Number fCheckpointZ = lua_tointeger(L, 4);
	lua_Number fCheckpointSize = lua_tointeger(L, 5);

	playerPool[playerID].vecCheckpointPos[0] = (float)fCheckpointX;
	playerPool[playerID].vecCheckpointPos[1] = (float)fCheckpointY;
	playerPool[playerID].vecCheckpointPos[2] = (float)fCheckpointZ;
	playerPool[playerID].fCheckpointSize = (float)fCheckpointSize;

	playerPool[playerID].bPlayerInCheckpoint = false;
	playerPool[playerID].bCheckpointActive = true;

	bs.Write((float)fCheckpointX);
	bs.Write((float)fCheckpointY);
	bs.Write((float)fCheckpointZ);
	bs.Write((float)fCheckpointSize);

	pRakServer->RPC(&RPC_SetCheckpoint, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int DisablePlayerCheckpoint(lua_State *L)
{
	BitStream bs;
	PLAYERID playerID = lua_tointeger(L, 1);

	playerPool[playerID].bPlayerInCheckpoint = false;
	playerPool[playerID].bCheckpointActive = false;

	pRakServer->RPC(&RPC_DisableCheckpoint, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(playerID), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int GameTextForAll(lua_State *L)
{
	BitStream bs;
	char *szText = (char *)lua_tostring(L, 1);
	int iTime = lua_tointeger(L, 2);
	int iStyle = lua_tointeger(L, 3);
	int iLength = strlen(szText);

	bs.Write(iStyle);
	bs.Write(iTime);
	bs.Write(iLength);
	bs.Write(szText, iLength);

	pRakServer->RPC(&RPC_ScrDisplayGameText, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(0xFFFF), TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int GameTextForPlayer(lua_State *L)
{
	BitStream bs;
	PLAYERID iPlayerIndex = lua_tointeger(L, 1);
	char *szText = (char *)lua_tostring(L, 2);
	int iTime = lua_tointeger(L, 3);
	int iStyle = lua_tointeger(L, 4);
	int iLength = strlen(szText);

	bs.Write(iStyle);
	bs.Write(iTime);
	bs.Write(iLength);
	bs.Write(szText, iLength);

	pRakServer->RPC(&RPC_ScrDisplayGameText, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(iPlayerIndex), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int ShowPlayerDialog(lua_State *L)
{
	BitStream bs;
	PLAYERID iPlayerIndex = lua_tointeger(L, 1);

	unsigned short wDialogID = lua_tointeger(L, 2);
	unsigned char bDialogStyle = lua_tointeger(L, 3);

	char *szCaption = (char *)lua_tostring(L, 4);
	char *szInfo = (char *)lua_tostring(L, 5);
	char *szButton1 = (char *)lua_tostring(L, 6);
	char *szButton2 = (char *)lua_tostring(L, 7);

	unsigned char iCaptionLength = strlen(szCaption);
	unsigned char iButton1Length = strlen(szButton1);
	unsigned char iButton2Length = strlen(szButton2);

	bs.Write(wDialogID);
	bs.Write(bDialogStyle);

	bs.Write(iCaptionLength);
	bs.Write(szCaption, iCaptionLength);

	bs.Write(iButton1Length);
	bs.Write(szButton1, iButton1Length);

	bs.Write(iButton2Length);
	bs.Write(szButton2, iButton2Length);

	stringCompressor->EncodeString(szInfo, strlen(szInfo)+1, &bs);

	pRakServer->RPC(&RPC_ScrDialogBox, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pRakServer->GetPlayerIDFromIndex(iPlayerIndex), FALSE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

int BroadcastScmEvent(lua_State *L)
{
	BitStream bs;

	PLAYERID iPlayerIndex = lua_tointeger(L, 1);

	int iEvent = lua_tointeger(L, 2);

	DWORD dwParams1 = lua_tointeger(L, 3);
	DWORD dwParams2 = lua_tointeger(L, 4);
	DWORD dwParams3 = lua_tointeger(L, 5);
	
	bs.Write(iPlayerIndex);
	bs.Write(iEvent);
	bs.Write(dwParams1);
	bs.Write(dwParams2);
	bs.Write(dwParams3);

	pRakServer->RPC(&RPC_ScmEvent, &bs, HIGH_PRIORITY, RELIABLE, 0, pRakServer->GetPlayerIDFromIndex(iPlayerIndex), TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	return 1;
}

void RegisterScriptingFunctions(lua_State *L)
{
	generateAndLoadInternalScript(L);

	lua_register(L, "outputConsole", OutputConsole);

	lua_register(L, "setGameModeText", SetGameModeText);
	lua_register(L, "setMapName", SetMapName);
	lua_register(L, "setWebURL", SetWebURL);

	lua_register(L, "isPlayerConnected", IsPlayerConnected);
	lua_register(L, "getPlayerName", GetPlayerName);

	lua_register(L, "getPlayerPos", GetPlayerPos);
	lua_register(L, "getPlayerInterior", GetPlayerInterior);
	lua_register(L, "setPlayerPos", SetPlayerPos);
	lua_register(L, "setPlayerPosFindZ", SetPlayerPosFindZ);

	lua_register(L, "getPlayerScore", GetPlayerScore);
	lua_register(L, "setPlayerScore", SetPlayerScore);

	lua_register(L, "getPlayerIP", GetPlayerIP);
	lua_register(L, "getPlayerPing", GetPlayerPing);

	lua_register(L, "getPlayerVehicleID", GetPlayerVehicleID);

	lua_register(L, "sendPlayerMessage", SendPlayerMessage);
	lua_register(L, "sendPlayerChatMessage", SendPlayerChatMessage);
	lua_register(L, "sendPlayerChatMessageToAll", SendPlayerChatMessageToAll);

	lua_register(L, "setPlayerPos", SetPlayerPos);
	lua_register(L, "setPlayerRotation", SetPlayerRotation);
	lua_register(L, "setPlayerInterior", SetPlayerInterior);

	lua_register(L, "setPlayerCameraPos", SetPlayerCameraPos);
	lua_register(L, "setPlayerCameraLookAt", SetPlayerCameraLookAt);
	lua_register(L, "setCameraBehindPlayer", SetCameraBehindPlayer);

	lua_register(L, "givePlayerMoney", GivePlayerMoney);
	lua_register(L, "resetPlayerMoney", ResetPlayerMoney);

	lua_register(L, "setPlayerHealth", SetPlayerHealth);
	lua_register(L, "setPlayerArmour", SetPlayerArmour);

	lua_register(L, "addStaticVehicle", AddStaticVehicle);
	lua_register(L, "createVehicle", CreateVehicle);
	lua_register(L, "removeVehicle", RemoveVehicle);
	lua_register(L, "repairVehicle", RepairVehicle);
	lua_register(L, "setVehicleHealth", SetVehicleHealth);

	lua_register(L, "giveWeapon", GiveWeapon);
	lua_register(L, "setWeaponAmmo", SetWeaponAmmo);
	lua_register(L, "clearPlayerWeapons", ClearPlayerWeapons);

	lua_register(L, "playAudioStreamForPlayer", PlayAudioStreamForPlayer);
	lua_register(L, "stopAudioStreamForPlayer", StopAudioStreamForPlayer);

	lua_register(L, "sendDeathMessage", SendDeathMessage);
	lua_register(L, "sendDeathMessageForPlayer", SendDeathMessageForPlayer);

	lua_register(L, "setGravity", SetGravity);
	lua_register(L, "setPlayerGravity", SetPlayerGravity);
	
	lua_register(L, "setPlayerCheckpoint", SetPlayerCheckpoint);
	lua_register(L, "disablePlayerCheckpoint", DisablePlayerCheckpoint);
	
	lua_register(L, "gameTextForAll", GameTextForAll);
	lua_register(L, "gameTextForPlayer", GameTextForPlayer);

	lua_register(L, "showPlayerDialog", ShowPlayerDialog);

	lua_register(L, "broadcastScmEvent", BroadcastScmEvent);
}

// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------

extern char szWorkingDirectory[MAX_PATH];
extern char szScripts[512];

bool LoadScript(char *szScriptName, bool bFirstLoad)
{
	char szTemp[256];
	sprintf_s(szTemp, sizeof(szTemp), "%s\\scripts\\%s.lua", szWorkingDirectory, szScriptName);

	for(int i = 0; i < MAX_SCRIPTS; i++)
	{
		if(strcmp(script.szScriptName[i], szScriptName) == 0)
		{
			Log("  %s.lua... FAIL (already loaded)", szScriptName);
			Log("  ");
			return false;
		}
	}

	FILE *fExists = fopen(szTemp, "rb");
	if(!fExists)
	{
		Log("  %s.lua... FAIL (does not exist)", szScriptName);
		Log("  ");
		return false;
	}

	strcpy_s(script.szScriptName[iScriptsRunning], 32, szScriptName);
	script.scriptVM[iScriptsRunning] = lua_open();
	if(script.scriptVM[iScriptsRunning] == NULL)
	{
		Log("  %s.lua... FAIL (LUA virtual machine)", szScriptName);
		Log("  ");
		return false;
	}
	luaL_openlibs(script.scriptVM[iScriptsRunning]);

	RegisterScriptingFunctions(script.scriptVM[iScriptsRunning]);

	int iCurrentScriptStatus = luaL_loadfile(script.scriptVM[iScriptsRunning], szTemp);
	if(bFirstLoad) Log("  %s.lua... %s", szScriptName, iCurrentScriptStatus ? "FAIL" : "OK");
	if(iCurrentScriptStatus == 0)
	{
		iCurrentScriptStatus = lua_pcall(script.scriptVM[iScriptsRunning], 0, LUA_MULTRET, 0);
		if(iCurrentScriptStatus == 0)
		{
			ScriptEvent_OnScriptStart(script.scriptVM[iScriptsRunning]);

			if(!bFirstLoad)
			{
				// respawn all static vehicles if any
				BitStream bsAnnounceStaticVehicle;
				for(VEHICLEID i = 1; i < MAX_VEHICLES; i++)
				{
					if(doesVehicleExist(i))
					{
						NEW_VEHICLE veh;
						bsAnnounceStaticVehicle.Reset();
						veh.VehicleId = (VEHICLEID)i;
						veh.iVehicleType = vehiclePool[i].iModelID;
						veh.vecPos[0] = vehiclePool[i].fVehiclePos[0];
						veh.vecPos[1] = vehiclePool[i].fVehiclePos[1];
						veh.vecPos[2] = vehiclePool[i].fVehiclePos[2];
						veh.fRotation = vehiclePool[i].fRotation;
						veh.aColor1 = vehiclePool[i].bColor1;
						veh.aColor2 = vehiclePool[i].bColor2;
						veh.fHealth = vehiclePool[i].fHealth;
						veh.byteInterior = 0;
						veh.dwDoorDamageStatus = 0;
						veh.dwPanelDamageStatus = 0;
						veh.byteLightDamageStatus = 0;
						bsAnnounceStaticVehicle.Write((const char *)&bsAnnounceStaticVehicle, sizeof(NEW_VEHICLE));
						pRakServer->RPC(&RPC_WorldVehicleAdd, &bsAnnounceStaticVehicle, HIGH_PRIORITY, RELIABLE,
							0, UNASSIGNED_PLAYER_ID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

						Sleep(5); // well, shit.
					}
				}
			}
		}
		if(bFirstLoad)
			Log(" ");
		else
			Log("  Script %s was succesfully loaded.", script.szScriptName[iScriptsRunning]);

		iScriptsRunning++;
	}
	else
	{
		Log("  %s", lua_tostring(script.scriptVM[iScriptsRunning], -1));
		Log("  ");
		return false;
	}

	return true;
}

bool UnloadScript(char *szScriptName)
{
	bool bFoundAndUnloaded = false;
	for(int i = 0; i < MAX_SCRIPTS; i++)
	{
		if(strcmp(script.szScriptName[i], szScriptName) == 0)
		{
			if(script.scriptVM[i] != NULL)
			{
				ScriptEvent_OnScriptExit(script.scriptVM[i]);
				lua_close(script.scriptVM[i]);
				script.scriptVM[i] = NULL;
			}
			memset(script.szScriptName[i], 0, 32);
			iScriptsRunning--;

			Log("  Script %s was succesfully unloaded.", szScriptName);
			bFoundAndUnloaded = true;
		}
	}

	if(bFoundAndUnloaded == false)
	{
		Log("  Could not find %s for unload.", szScriptName);
		return false;
	}

	return true;
}

bool LoadScripts()
{
	Log("  Loading scripts...");
	Log(" ");

	TiXmlElement* serverElement = xmlSettings.FirstChildElement("server");
	if(serverElement)
	{
		char *pLine = (char *)serverElement->Attribute("scripts");
		char *pch = strtok(pLine, " ");
		while(pch != NULL)
		{
			LoadScript(pch, true);
			pch = strtok (NULL, " ");
		}
	}

	return true;
}

void HandleScriptingConsoleInputCommands(char *szInputBuffer)
{
	if(!strncmp(szInputBuffer, "loadscr", 7) || !strncmp(szInputBuffer, "LOADSCR", 7))
	{
		if(szInputBuffer[8] == 0x0D || szInputBuffer[8] == 0x0A || szInputBuffer[8] == 0x00) return;
		memset(szInputBuffer + strlen(szInputBuffer) - 2, 0, 2);
		LoadScript(&szInputBuffer[8], false);
	}
	if(!strncmp(szInputBuffer, "unloadscr", 9) || !strncmp(szInputBuffer, "UNLOADSCR", 9))
	{
		if(szInputBuffer[10] == 0x0D || szInputBuffer[10] == 0x0A || szInputBuffer[10] == 0x00) return;
		memset(szInputBuffer + strlen(szInputBuffer) - 2, 0, 2);
		UnloadScript(&szInputBuffer[10]);
	}
	if(!strncmp(szInputBuffer, "reloadscr", 9) || !strncmp(szInputBuffer, "RELOADSCR", 9)) // !!! CHECK TODO
	{
		if(szInputBuffer[10] == 0x0D || szInputBuffer[10] == 0x0A || szInputBuffer[10] == 0x00) return;
		memset(szInputBuffer + strlen(szInputBuffer) - 2, 0, 2);
		if(UnloadScript(&szInputBuffer[10]))
			LoadScript(&szInputBuffer[10], false);
	}
	if(!strncmp(szInputBuffer, "listscr", 7) || !strncmp(szInputBuffer, "LISTSCR", 7))
	{
		Log(" Scripts running: %d", iScriptsRunning);
		for(int i = 0; i < iScriptsRunning; i++)
		{
			if(script.szScriptName[i][0] == 0x00 && script.scriptVM[i] == NULL) continue;
			Log("[%d] %s", i, script.szScriptName[i]);
		}
	}

	return;
}

void CleanUpScripting()
{
	for(int i = 0; i < iScriptsRunning; i++)
	{
		if(script.scriptVM[iScriptsRunning] != NULL)
			lua_close(script.scriptVM[iScriptsRunning]);
	}

	return;
}
