/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

int iVehicleCount = 0;
struct stVehiclePool vehiclePool[MAX_VEHICLES];

VEHICLEID addStaticVehicle(int iVehModelID, float fPosX, float fPosY, float fPosZ, float fRot,
					  BYTE bColor1, BYTE bColor2, int iRespawnsIfDead, int iTimeUntilRespawn)
{
	if(iVehicleCount == MAX_VEHICLES)
		return -1;

	VEHICLEID vehicleIDRet = (VEHICLEID)iVehicleCount;

	vehiclePool[iVehicleCount].bExists = true;
	vehiclePool[iVehicleCount].iModelID = iVehModelID;
	vehiclePool[iVehicleCount].fVehiclePos[0] = fPosX;
	vehiclePool[iVehicleCount].fVehiclePos[1] = fPosY;
	vehiclePool[iVehicleCount].fVehiclePos[2] = fPosZ;
	vehiclePool[iVehicleCount].fRotation = fRot;
	vehiclePool[iVehicleCount].bColor1 = bColor1;
	vehiclePool[iVehicleCount].bColor2 = bColor2;
	vehiclePool[iVehicleCount].bIsStatic = true;
	vehiclePool[iVehicleCount].iRespawnsIfDead = iRespawnsIfDead;
	vehiclePool[iVehicleCount].iTimeUntilRespawn = iTimeUntilRespawn;

	iVehicleCount++;

	return vehicleIDRet;
}

VEHICLEID addVehicle(int iVehModelID, float fPosX, float fPosY, float fPosZ, float fRot, BYTE bColor1, BYTE bColor2)
{
	if(iVehicleCount == MAX_VEHICLES)
		return -1;

	VEHICLEID vehicleIDRet = (VEHICLEID)iVehicleCount;

	vehiclePool[iVehicleCount].bExists = true;
	vehiclePool[iVehicleCount].iModelID = iVehModelID;
	vehiclePool[iVehicleCount].fVehiclePos[0] = fPosX;
	vehiclePool[iVehicleCount].fVehiclePos[1] = fPosY;
	vehiclePool[iVehicleCount].fVehiclePos[2] = fPosZ;
	vehiclePool[iVehicleCount].fRotation = fRot;
	vehiclePool[iVehicleCount].bColor1 = bColor1;
	vehiclePool[iVehicleCount].bColor2 = bColor2;
	vehiclePool[iVehicleCount].bIsStatic = false;

	NEW_VEHICLE newVeh;
	newVeh.VehicleId = vehicleIDRet;
	newVeh.iVehicleType = iVehModelID;
	newVeh.vecPos[0] = fPosX;
	newVeh.vecPos[1] = fPosY;
	newVeh.vecPos[2] = fPosZ;
	newVeh.fRotation = fRot;
	newVeh.fHealth = 1000.00f;
	newVeh.byteInterior = 0;
	newVeh.dwDoorDamageStatus = 0;
	newVeh.dwPanelDamageStatus = 0;
	newVeh.byteLightDamageStatus = 0;

	RakNet::BitStream bs;
	bs.Write((const char *)&newVeh, sizeof(NEW_VEHICLE));
	pRakServer->RPC(&RPC_WorldVehicleAdd, &bs, HIGH_PRIORITY, RELIABLE,
		0, UNASSIGNED_PLAYER_ID, TRUE, FALSE, UNASSIGNED_NETWORK_ID, NULL);

	iVehicleCount++;

	return vehicleIDRet;
}

void removeVehicle(VEHICLEID vehicleID)
{
	vehiclePool[vehicleID].bExists = false;
	vehiclePool[vehicleID].fVehiclePos[0] = 0.0f;
	vehiclePool[vehicleID].fVehiclePos[1] = 0.0f;
	vehiclePool[vehicleID].fVehiclePos[2] = 0.0f;
	vehiclePool[vehicleID].fRotation = 0.0f;
	vehiclePool[vehicleID].bColor1 = 0;
	vehiclePool[vehicleID].bColor2 = 0;
	vehiclePool[vehicleID].bIsStatic = false;
	vehiclePool[vehicleID].iRespawnsIfDead = 0;
	vehiclePool[vehicleID].iTimeUntilRespawn = 0;

	iVehicleCount--;
}

bool doesVehicleExist(VEHICLEID vehicleID)
{
	return vehiclePool[vehicleID].bExists;
}
