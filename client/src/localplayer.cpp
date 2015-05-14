/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

DWORD dwLastOnFootDataSentTick = GetTickCount();
int iFollowingPassenger = 0, iFollowingDriver = 0;
void SendOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, int sendDeathNoti, PLAYERID followPlayerID)
{
	if(pofSync == NULL)
		return;

	RakNet::BitStream bsPlayerSync;

	if(dwLastOnFootDataSentTick && dwLastOnFootDataSentTick < (GetTickCount() - iNetModeNormalOnfootSendRate))
	{
		if(followPlayerID != (PLAYERID)-1)
		{
			if(!playerInfo[followPlayerID].iIsConnected)
				return;
			if(iFollowingPassenger || iFollowingDriver)
				return;

			pofSync->lrAnalog = playerInfo[followPlayerID].onfootData.lrAnalog;
			pofSync->udAnalog = playerInfo[followPlayerID].onfootData.udAnalog;
			pofSync->wKeys = playerInfo[followPlayerID].onfootData.wKeys;

			pofSync->vecPos[0] = playerInfo[followPlayerID].onfootData.vecPos[0] + settings.fFollowXOffset;
			pofSync->vecPos[1] = playerInfo[followPlayerID].onfootData.vecPos[1] + settings.fFollowYOffset;
			pofSync->vecPos[2] = playerInfo[followPlayerID].onfootData.vecPos[2] + settings.fFollowZOffset;

			pofSync->fQuaternion[0] = playerInfo[followPlayerID].onfootData.fQuaternion[0];
			pofSync->fQuaternion[1] = playerInfo[followPlayerID].onfootData.fQuaternion[1];
			pofSync->fQuaternion[2] = playerInfo[followPlayerID].onfootData.fQuaternion[2];
			pofSync->fQuaternion[3] = playerInfo[followPlayerID].onfootData.fQuaternion[3];

			if(!settings.bPulsator)
			{
				pofSync->byteHealth = playerInfo[followPlayerID].onfootData.byteHealth;
				pofSync->byteArmour = playerInfo[followPlayerID].onfootData.byteArmour;
			}

			if(settings.bCurrentWeapon != 0)  pofSync->byteCurrentWeapon = settings.bCurrentWeapon;
			else pofSync->byteCurrentWeapon = playerInfo[followPlayerID].onfootData.byteCurrentWeapon;

			pofSync->byteSpecialAction = playerInfo[followPlayerID].onfootData.byteSpecialAction;

			pofSync->vecMoveSpeed[0] = playerInfo[followPlayerID].onfootData.vecMoveSpeed[0];
			pofSync->vecMoveSpeed[1] = playerInfo[followPlayerID].onfootData.vecMoveSpeed[1];
			pofSync->vecMoveSpeed[2] = playerInfo[followPlayerID].onfootData.vecMoveSpeed[2];

			pofSync->iCurrentAnimationID = playerInfo[followPlayerID].onfootData.iCurrentAnimationID;

			settings.fCurrentPosition[0] = pofSync->vecPos[0];
			settings.fCurrentPosition[1] = pofSync->vecPos[1];
			settings.fCurrentPosition[2] = pofSync->vecPos[2];

			bsPlayerSync.Write((BYTE)ID_PLAYER_SYNC);
			bsPlayerSync.Write((PCHAR)pofSync, sizeof(ONFOOT_SYNC_DATA));
			pRakClient->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			if(sendDeathNoti && pofSync->byteHealth == 0)
				SendWastedNotification(0, -1);

			dwLastOnFootDataSentTick = GetTickCount();
		}
		else
		{
			if(settings.bCurrentWeapon != 0)
				pofSync->byteCurrentWeapon = settings.bCurrentWeapon;

			settings.fCurrentPosition[0] = pofSync->vecPos[0];
			settings.fCurrentPosition[1] = pofSync->vecPos[1];
			settings.fCurrentPosition[2] = pofSync->vecPos[2];

			bsPlayerSync.Write((BYTE)ID_PLAYER_SYNC);
			bsPlayerSync.Write((PCHAR)pofSync, sizeof(ONFOOT_SYNC_DATA));

			pRakClient->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			if(sendDeathNoti && pofSync->byteHealth == 0)
				SendWastedNotification(0, -1);

			dwLastOnFootDataSentTick = GetTickCount();
		}
	}
}

DWORD dwLastInVehicleDataSentTick = GetTickCount();
void SendInCarFullSyncData(INCAR_SYNC_DATA *picSync, int iUseCarPos, PLAYERID followPlayerID)
{
	if(picSync == NULL)
		return;

	RakNet::BitStream bsVehicleSync;

	if(dwLastInVehicleDataSentTick && dwLastInVehicleDataSentTick < (GetTickCount() - iNetModeNormalIncarSendRate))
	{
		if(followPlayerID != (PLAYERID)-1)
		{
			if(!playerInfo[followPlayerID].iIsConnected)
				return;

			picSync->VehicleID = (VEHICLEID)settings.iFollowingWithVehicleID;

			picSync->lrAnalog = playerInfo[followPlayerID].incarData.lrAnalog;
			picSync->udAnalog = playerInfo[followPlayerID].incarData.udAnalog;
			picSync->wKeys = playerInfo[followPlayerID].incarData.wKeys;

			picSync->fQuaternion[0] = playerInfo[followPlayerID].incarData.fQuaternion[0];
			picSync->fQuaternion[1] = playerInfo[followPlayerID].incarData.fQuaternion[1];
			picSync->fQuaternion[2] = playerInfo[followPlayerID].incarData.fQuaternion[2];
			picSync->fQuaternion[3] = playerInfo[followPlayerID].incarData.fQuaternion[3];

			picSync->vecPos[0] = playerInfo[followPlayerID].incarData.vecPos[0] + settings.fFollowXOffset;
			picSync->vecPos[1] = playerInfo[followPlayerID].incarData.vecPos[1] + settings.fFollowYOffset;
			picSync->vecPos[2] = playerInfo[followPlayerID].incarData.vecPos[2] + settings.fFollowZOffset;

			picSync->vecMoveSpeed[0] = playerInfo[followPlayerID].incarData.vecMoveSpeed[0];
			picSync->vecMoveSpeed[1] = playerInfo[followPlayerID].incarData.vecMoveSpeed[1];
			picSync->vecMoveSpeed[2] = playerInfo[followPlayerID].incarData.vecMoveSpeed[2];

			picSync->fCarHealth = playerInfo[followPlayerID].incarData.fCarHealth;

			if(!settings.bPulsator)
			{
				picSync->bytePlayerHealth = playerInfo[followPlayerID].incarData.bytePlayerHealth;
				picSync->bytePlayerArmour = playerInfo[followPlayerID].incarData.bytePlayerArmour;
			}

			if(settings.bCurrentWeapon != 0) picSync->byteCurrentWeapon = settings.bCurrentWeapon;
			else picSync->byteCurrentWeapon = playerInfo[followPlayerID].incarData.byteCurrentWeapon;

			picSync->byteSirenOn = playerInfo[followPlayerID].incarData.byteSirenOn;
			picSync->byteLandingGearState = playerInfo[followPlayerID].incarData.byteLandingGearState;

			picSync->TrailerID_or_ThrustAngle = playerInfo[followPlayerID].incarData.TrailerID_or_ThrustAngle;
			picSync->fTrainSpeed = playerInfo[followPlayerID].incarData.fTrainSpeed;

			settings.fCurrentPosition[0] = picSync->vecPos[0];
			settings.fCurrentPosition[1] = picSync->vecPos[1];
			settings.fCurrentPosition[2] = picSync->vecPos[2];

			bsVehicleSync.Write((BYTE)ID_VEHICLE_SYNC);
			bsVehicleSync.Write((PCHAR)picSync,sizeof(INCAR_SYNC_DATA));
			pRakClient->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			dwLastInVehicleDataSentTick = GetTickCount();
		}
		else
		{
			VEHICLEID vehicleID = picSync->VehicleID;

			if(!vehiclePool[vehicleID].iDoesExist)
				return;

			if(iUseCarPos)
			{
				picSync->vecPos[0] = vehiclePool[vehicleID].fPos[0];
				picSync->vecPos[1] = vehiclePool[vehicleID].fPos[1];
				picSync->vecPos[2] = vehiclePool[vehicleID].fPos[2];
			}

			settings.fCurrentPosition[0] = picSync->vecPos[0];
			settings.fCurrentPosition[1] = picSync->vecPos[1];
			settings.fCurrentPosition[2] = picSync->vecPos[2];

			bsVehicleSync.Write((BYTE)ID_VEHICLE_SYNC);
			bsVehicleSync.Write((PCHAR)picSync,sizeof(INCAR_SYNC_DATA));
			pRakClient->Send(&bsVehicleSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0);

			dwLastInVehicleDataSentTick = GetTickCount();
		}
	}
}

DWORD dwLastPassengerDataSentTick = GetTickCount();
void SendPassengerFullSyncData(VEHICLEID vehicleID)
{
	if(!vehiclePool[vehicleID].iDoesExist)
		return;

	if(dwLastPassengerDataSentTick && dwLastPassengerDataSentTick < (GetTickCount() - iNetModeNormalIncarSendRate))
	{
		RakNet::BitStream bsPassengerSync;

		PASSENGER_SYNC_DATA psSync;
		memset(&psSync, 0, sizeof(PASSENGER_SYNC_DATA));

		psSync.VehicleID = vehicleID;

		psSync.vecPos[0] = vehiclePool[vehicleID].fPos[0];
		psSync.vecPos[1] = vehiclePool[vehicleID].fPos[1];
		psSync.vecPos[2] = vehiclePool[vehicleID].fPos[2];

		psSync.bytePlayerHealth = (BYTE)settings.fPlayerHealth;
		psSync.bytePlayerArmour = (BYTE)settings.fPlayerArmour;

		bsPassengerSync.Write((BYTE)ID_PASSENGER_SYNC);
		bsPassengerSync.Write((PCHAR)&psSync, sizeof(PASSENGER_SYNC_DATA));
		pRakClient->Send(&bsPassengerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		dwLastPassengerDataSentTick = GetTickCount();
	}
}

DWORD dwLastAimDataSentTick = GetTickCount();
void SendAimSyncData(DWORD dwAmmoInClip, int iReloading, PLAYERID copyFromPlayer)
{
	if(dwLastAimDataSentTick && dwLastAimDataSentTick < (GetTickCount() - iNetModeFiringSendRate))
	{
		RakNet::BitStream bsAimSync;
		AIM_SYNC_DATA aimSync;

		if(copyFromPlayer != (PLAYERID)-1)
		{
			if(!playerInfo[copyFromPlayer].iIsConnected)
				return;

			memcpy((void *)&aimSync, (void *)&playerInfo[copyFromPlayer].aimData, sizeof(AIM_SYNC_DATA));

			if(aimSync.vecAimPos[0] == 0.0f && aimSync.vecAimPos[1] == 0.0f && aimSync.vecAimPos[2] == 0.0f)
			{
				aimSync.vecAimPos[0] = 0.25f;
			}

			bsAimSync.Write((BYTE)ID_AIM_SYNC);
			bsAimSync.Write((PCHAR)&aimSync, sizeof(AIM_SYNC_DATA));

			pRakClient->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			dwLastAimDataSentTick = GetTickCount();
		}
		else
		{
			if(iReloading)
				playerInfo[g_myPlayerID].aimData.byteWeaponState = WS_RELOADING;
			else
				playerInfo[g_myPlayerID].aimData.byteWeaponState = (dwAmmoInClip > 1) ? WS_MORE_BULLETS : dwAmmoInClip;

			playerInfo[g_myPlayerID].aimData.bUnk = 0x55;

			memcpy((void *)&aimSync, (void *)&playerInfo[g_myPlayerID].aimData, sizeof(AIM_SYNC_DATA));

			bsAimSync.Write((BYTE)ID_AIM_SYNC);
			bsAimSync.Write((PCHAR)&aimSync, sizeof(AIM_SYNC_DATA));

			pRakClient->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			dwLastAimDataSentTick = GetTickCount();
		}
	}
}

DWORD dwLastUnocDataSentTick = GetTickCount();
void SendUnoccupiedSyncData(UNOCCUPIED_SYNC_DATA *punocSync)
{
	if(dwLastUnocDataSentTick && dwLastUnocDataSentTick < (GetTickCount() - 30))
	{
		RakNet::BitStream bsUnoccupiedSync;

		bsUnoccupiedSync.Write((BYTE)ID_UNOCCUPIED_SYNC);
		bsUnoccupiedSync.Write((PCHAR)punocSync,sizeof(UNOCCUPIED_SYNC_DATA));
		pRakClient->Send(&bsUnoccupiedSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0);

		dwLastUnocDataSentTick = GetTickCount();
	}
}

void SendSpectatorData(SPECTATOR_SYNC_DATA *pSpecData)
{
	RakNet::BitStream bsSpecSync;

	bsSpecSync.Write((BYTE)ID_SPECTATOR_SYNC);
	bsSpecSync.Write((PCHAR)pSpecData, sizeof(SPECTATOR_SYNC_DATA));
	
	pRakClient->Send(&bsSpecSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0);
}

void SendBulletData(BULLET_SYNC_DATA *pBulletData)
{
	RakNet::BitStream bsBulletSync;

	bsBulletSync.Write((BYTE)ID_BULLET_SYNC);
	bsBulletSync.Write((PCHAR)pBulletData, sizeof(BULLET_SYNC_DATA));

	pRakClient->Send(&bsBulletSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0);
}

void SendEnterVehicleNotification(VEHICLEID VehicleID, BOOL bPassenger)
{
	RakNet::BitStream bsSend;
	BYTE bytePassenger=0;

	if(bPassenger)
		bytePassenger = 1;

	bsSend.Write(VehicleID);
	bsSend.Write(bytePassenger);
	pRakClient->RPC(&RPC_EnterVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void SendExitVehicleNotification(VEHICLEID VehicleID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(VehicleID);
	pRakClient->RPC(&RPC_ExitVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void SendWastedNotification(BYTE byteDeathReason, PLAYERID WhoWasResponsible)
{
	RakNet::BitStream bsPlayerDeath;

	bsPlayerDeath.Write(byteDeathReason);
	bsPlayerDeath.Write(WhoWasResponsible);
	pRakClient->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void NotifyVehicleDeath(VEHICLEID VehicleID)
{
	RakNet::BitStream bsDeath;
	bsDeath.Write(VehicleID);
	pRakClient->RPC(&RPC_VehicleDestroyed, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void SendDamageVehicle(WORD vehicleID, DWORD panel, DWORD door, BYTE lights, BYTE tires)
{
	RakNet::BitStream bsDamageVehicle;

	bsDamageVehicle.Write(vehicleID);
	bsDamageVehicle.Write(panel);
	bsDamageVehicle.Write(door);
	bsDamageVehicle.Write(lights);
	bsDamageVehicle.Write(tires);
	pRakClient->RPC(&RPC_DamageVehicle, &bsDamageVehicle, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}
