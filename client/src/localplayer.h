/*
	Updated to 0.3.7 by P3ti
*/

void SendOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, int sendDeathNoti, PLAYERID followPlayerID);
void SendInCarFullSyncData(INCAR_SYNC_DATA *picSync, int iUseCarPos, PLAYERID followPlayerID);
void SendPassengerFullSyncData(VEHICLEID vehicleID);
void SendAimSyncData(DWORD dwAmmoInClip, int iReloading, PLAYERID copyFromPlayer);
void SendUnoccupiedSyncData(UNOCCUPIED_SYNC_DATA *punocSync);
void SendSpectatorData(SPECTATOR_SYNC_DATA *pSpecData);
void SendBulletData(BULLET_SYNC_DATA *pBulletData);

void SendEnterVehicleNotification(VEHICLEID VehicleID, BOOL bPassenger);
void SendExitVehicleNotification(VEHICLEID VehicleID);
void SendWastedNotification(BYTE byteDeathReason, PLAYERID WhoWasResponsible);
void NotifyVehicleDeath(VEHICLEID VehicleID);
void SendDamageVehicle(WORD vehicleID, DWORD panel, DWORD door, BYTE lights, BYTE tires);