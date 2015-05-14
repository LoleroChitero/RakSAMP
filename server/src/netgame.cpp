/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

extern int iLagCompensation;

void resetPools()
{
	memset(playerInfo, 0, sizeof(stPlayerInfo));
	memset(vehiclePool, 0, sizeof(stVehiclePool));
}

void Packet_PlayerSync(Packet *p)
{
	//Log("Packet_PlayerSync: %d  %d", pRakServer->GetIndexFromPlayerID(p->playerId), p->length);

	RakNet::BitStream bsPlayerSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId = pRakServer->GetIndexFromPlayerID(p->playerId);

	// clear last data
	memset(&playerInfo[playerId].onfootData, 0, sizeof(ONFOOT_SYNC_DATA));

	bsPlayerSync.IgnoreBits(8);
	bsPlayerSync.Read((PCHAR)&playerInfo[playerId].onfootData, sizeof(ONFOOT_SYNC_DATA));


	// BROADCAST DATA
	RakNet::BitStream bsOnFootBC;
	bsOnFootBC.Write((BYTE)ID_PLAYER_SYNC);
	bsOnFootBC.Write((PLAYERID)playerId);

	if(playerInfo[playerId].onfootData.lrAnalog)
	{
		bsOnFootBC.Write(true);
		bsOnFootBC.Write(playerInfo[playerId].onfootData.lrAnalog);
	}
	else
		bsOnFootBC.Write(false);

	if(playerInfo[playerId].onfootData.udAnalog)
	{
		bsOnFootBC.Write(true);
		bsOnFootBC.Write(playerInfo[playerId].onfootData.udAnalog);
	}
	else
		bsOnFootBC.Write(false);

	bsOnFootBC.Write(playerInfo[playerId].onfootData.wKeys);
	bsOnFootBC.Write(playerInfo[playerId].onfootData.vecPos[0]);
	bsOnFootBC.Write(playerInfo[playerId].onfootData.vecPos[1]);
	bsOnFootBC.Write(playerInfo[playerId].onfootData.vecPos[2]);
	bsOnFootBC.WriteNormQuat(
		playerInfo[playerId].onfootData.fQuaternion[0],
		playerInfo[playerId].onfootData.fQuaternion[1],
		playerInfo[playerId].onfootData.fQuaternion[2],
		playerInfo[playerId].onfootData.fQuaternion[3]);

	BYTE byteSyncHealthArmour=0;
	BYTE byteHealth = playerInfo[playerId].onfootData.byteHealth;
	BYTE byteArmour = playerInfo[playerId].onfootData.byteArmour;
	if( byteHealth > 0 && byteHealth < 100 ) {
		byteSyncHealthArmour = ((BYTE)(byteHealth / 7)) << 4;
	} 
	else if(byteHealth >= 100) {
		byteSyncHealthArmour = 0xF << 4;
	}

	if( byteArmour > 0 && byteArmour < 100 ) {
		byteSyncHealthArmour |=  (BYTE)(byteArmour / 7);
	}
	else if(byteArmour >= 100) {
		byteSyncHealthArmour |= 0xF;
	}
	bsOnFootBC.Write(byteSyncHealthArmour);
	bsOnFootBC.Write(playerInfo[playerId].onfootData.byteCurrentWeapon);
	bsOnFootBC.Write(playerInfo[playerId].onfootData.byteSpecialAction);
	bsOnFootBC.WriteVector(playerInfo[playerId].onfootData.vecMoveSpeed[0],
		playerInfo[playerId].onfootData.vecMoveSpeed[1], playerInfo[playerId].onfootData.vecMoveSpeed[2]);

	if(playerInfo[playerId].onfootData.wSurfInfo)
	{
		bsOnFootBC.Write(true);
		bsOnFootBC.Write(playerInfo[playerId].onfootData.wSurfInfo);
		bsOnFootBC.Write(playerInfo[playerId].onfootData.vecSurfOffsets[0]);
		bsOnFootBC.Write(playerInfo[playerId].onfootData.vecSurfOffsets[1]);
		bsOnFootBC.Write(playerInfo[playerId].onfootData.vecSurfOffsets[2]);
	}
	else
		bsOnFootBC.Write(false);

	if(playerInfo[playerId].onfootData.iCurrentAnimationID)
	{
		bsOnFootBC.Write(true);
		bsOnFootBC.Write(playerInfo[playerId].onfootData.iCurrentAnimationID);
	}
	else
		bsOnFootBC.Write(false);

	playerPool[playerId].currentVehicleID = 0;

	UpdatePosition(playerId, playerInfo[playerId].onfootData.vecPos[0], playerInfo[playerId].onfootData.vecPos[1], playerInfo[playerId].onfootData.vecPos[2]);

	pRakServer->Send(&bsOnFootBC, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, p->playerId, TRUE);
}

void Packet_VehicleSync(Packet *p)
{
	//Log("Packet_VehicleSync: %d  %d", pRakServer->GetIndexFromPlayerID(p->playerId), p->length);

	RakNet::BitStream bsVehicleSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId = pRakServer->GetIndexFromPlayerID(p->playerId);

	// clear last data
	memset(&playerInfo[playerId].incarData, 0, sizeof(INCAR_SYNC_DATA));

	bsVehicleSync.IgnoreBits(8);
	bsVehicleSync.Read((PCHAR)&playerInfo[playerId].incarData, sizeof(INCAR_SYNC_DATA));

	playerPool[playerId].currentVehicleID = playerInfo[playerId].incarData.VehicleID;

	// BROADCAST DATA
	RakNet::BitStream bsInVehicleBC;
	bsInVehicleBC.Write((BYTE)ID_VEHICLE_SYNC);
	bsInVehicleBC.Write(playerId);
	bsInVehicleBC.Write(playerInfo[playerId].incarData.VehicleID);

	bsInVehicleBC.Write(playerInfo[playerId].incarData.lrAnalog);
	bsInVehicleBC.Write(playerInfo[playerId].incarData.udAnalog);
	bsInVehicleBC.Write(playerInfo[playerId].incarData.wKeys);

	bsInVehicleBC.WriteNormQuat(
		playerInfo[playerId].incarData.fQuaternion[0],
		playerInfo[playerId].incarData.fQuaternion[1],
		playerInfo[playerId].incarData.fQuaternion[2],
		playerInfo[playerId].incarData.fQuaternion[3]);

	bsInVehicleBC.Write(playerInfo[playerId].incarData.vecPos[0]);
	bsInVehicleBC.Write(playerInfo[playerId].incarData.vecPos[1]);
	bsInVehicleBC.Write(playerInfo[playerId].incarData.vecPos[2]);

	bsInVehicleBC.WriteVector(playerInfo[playerId].incarData.vecMoveSpeed[0],
		playerInfo[playerId].incarData.vecMoveSpeed[1], playerInfo[playerId].incarData.vecMoveSpeed[2]);

	WORD wTempVehicleHealh = (WORD)playerInfo[playerId].incarData.fCarHealth;
	bsInVehicleBC.Write(wTempVehicleHealh);
	BYTE byteSyncHealthArmour=0;
	BYTE byteHealth = playerInfo[playerId].incarData.bytePlayerHealth;
	BYTE byteArmour = playerInfo[playerId].incarData.bytePlayerArmour;
	if( byteHealth > 0 && byteHealth < 100 ) {
		byteSyncHealthArmour = ((BYTE)(byteHealth / 7)) << 4;
	} 
	else if(byteHealth >= 100) {
		byteSyncHealthArmour = 0xF << 4;
	}

	if( byteArmour > 0 && byteArmour < 100 ) {
		byteSyncHealthArmour |=  (BYTE)(byteArmour / 7);
	}
	else if(byteArmour >= 100) {
		byteSyncHealthArmour |= 0xF;
	}
	bsInVehicleBC.Write(byteSyncHealthArmour);

	bsInVehicleBC.Write(playerInfo[playerId].incarData.byteCurrentWeapon);

	if(playerInfo[playerId].incarData.byteSirenOn)
		bsInVehicleBC.Write(true);
	else
		bsInVehicleBC.Write(false);

	if(playerInfo[playerId].incarData.byteLandingGearState)
		bsInVehicleBC.Write(true);
	else
		bsInVehicleBC.Write(false);

	// HYDRA THRUST ANGLE AND TRAILER ID
	bsInVehicleBC.Write(false);
	bsInVehicleBC.Write(false);

	DWORD dwTrailerID_or_ThrustAngle = 0;
	bsInVehicleBC.Write(dwTrailerID_or_ThrustAngle);

	// TRAIN SPECIAL
	bsInVehicleBC.Write(false);
	/*WORD wSpeed;
	bsSync.ReadCompressed(bTrain);
	if(bTrain)
	{
		bsSync.Read(wSpeed);
		playerInfo[playerId].incarData.fTrainSpeed = (float)wSpeed;
	}*/
	
	UpdatePosition(playerId, playerInfo[playerId].incarData.vecPos[0], playerInfo[playerId].incarData.vecPos[1], playerInfo[playerId].incarData.vecPos[2]);

	pRakServer->Send(&bsInVehicleBC, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, p->playerId, TRUE);
}

void Packet_PassengerSync(Packet *p)
{
	//Log("Packet_PassengerSync: %d  %d", pRakServer->GetIndexFromPlayerID(p->playerId), p->length);

	RakNet::BitStream bsPassengerSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId = pRakServer->GetIndexFromPlayerID(p->playerId);

	// clear last data
	memset(&playerInfo[playerId].passengerData, 0, sizeof(PASSENGER_SYNC_DATA));

	bsPassengerSync.IgnoreBits(8);
	bsPassengerSync.Read((PCHAR)&playerInfo[playerId].passengerData, sizeof(PASSENGER_SYNC_DATA));

	UpdatePosition(playerId, playerInfo[playerId].passengerData.vecPos[0], playerInfo[playerId].passengerData.vecPos[1], playerInfo[playerId].passengerData.vecPos[2]);
}

void Packet_AimSync(Packet *p)
{
	//Log("Packet_AimSync: %d  %d", pRakServer->GetIndexFromPlayerID(p->playerId), p->length);

	RakNet::BitStream bsAimSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId = pRakServer->GetIndexFromPlayerID(p->playerId);

	// clear last data
	memset(&playerInfo[playerId].aimData, 0, sizeof(AIM_SYNC_DATA));

	bsAimSync.IgnoreBits(8);
	bsAimSync.Read((PCHAR)&playerInfo[playerId].aimData, sizeof(AIM_SYNC_DATA));


	// BROADCAST DATA
	RakNet::BitStream bsAimBC;
	bsAimBC.Write((BYTE)ID_AIM_SYNC);
	bsAimBC.Write((PLAYERID)playerId);
	bsAimBC.Write((PCHAR)&playerInfo[playerId].aimData, sizeof(AIM_SYNC_DATA));
	pRakServer->Send(&bsAimBC, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, p->playerId, TRUE);
}

void Packet_TrailerSync(Packet *p)
{
	//Log("Packet_TrailerSync: %d  %d", pRakServer->GetIndexFromPlayerID(p->playerId), p->length);
}

void Packet_UnoccupiedSync(Packet *p)
{
	//Log("Packet_UnoccupiedSync: %d  %d", pRakServer->GetIndexFromPlayerID(p->playerId), p->length);

	RakNet::BitStream bsUnocSync((unsigned char *)p->data, p->length, false);
	PLAYERID playerId = pRakServer->GetIndexFromPlayerID(p->playerId);

	// clear last data
	memset(&playerInfo[playerId].unocData, 0, sizeof(UNOCCUPIED_SYNC_DATA));

	bsUnocSync.IgnoreBits(8);
	bsUnocSync.Read((PCHAR)&playerInfo[playerId].unocData, sizeof(UNOCCUPIED_SYNC_DATA));
}

void Packet_MarkersSync(Packet *p)
{
	//Log("Packet_MarkersSync: %d  %d", pRakServer->GetIndexFromPlayerID(p->playerId), p->length);
}

void Packet_BulletSync(Packet *p)
{
	if(iLagCompensation)
	{
		RakNet::BitStream bsBulletSync((unsigned char *)p->data, p->length, false);
		PLAYERID playerId = pRakServer->GetIndexFromPlayerID(p->playerId);

		memset(&playerInfo[playerId].bulletData, 0, sizeof(BULLET_SYNC_DATA));

		bsBulletSync.IgnoreBits(8);
		bsBulletSync.Read((PCHAR)&playerInfo[playerId].bulletData, sizeof(BULLET_SYNC_DATA));

		for(int i = 0; i < iScriptsRunning; i++)
		{
			if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
				ScriptEvent_OnPlayerWeaponShot(script.scriptVM[i], playerId, playerInfo[playerId].bulletData.bWeaponID, playerInfo[playerId].bulletData.bHitType, playerInfo[playerId].bulletData.iHitID, playerInfo[playerId].bulletData.fCenterOfHit[0], playerInfo[playerId].bulletData.fCenterOfHit[1], playerInfo[playerId].bulletData.fCenterOfHit[2]);
		}

		bsBulletSync.Reset();

		bsBulletSync.Write((BYTE)ID_BULLET_SYNC);
		bsBulletSync.Write((unsigned short)playerId);
		bsBulletSync.Write((PCHAR)&playerInfo[playerId].bulletData, sizeof(BULLET_SYNC_DATA));

		pRakServer->Send(&bsBulletSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, p->playerId, TRUE);
	}
}

void UpdateNetwork()
{
	unsigned char packetIdentifier;
	Packet *pkt;

	while(pkt = pRakServer->Receive())
	{
		if ( ( unsigned char ) pkt->data[ 0 ] == ID_TIMESTAMP )
		{
			if ( pkt->length > sizeof( unsigned char ) + sizeof( unsigned int ) )
				packetIdentifier = ( unsigned char ) pkt->data[ sizeof( unsigned char ) + sizeof( unsigned int ) ];
			else
				return;
		}
		else
			packetIdentifier = ( unsigned char ) pkt->data[ 0 ];

		//Log("[RAKSAMP] Packet received. PacketID: %d, PlayerID: %d.", pkt->data[0], pkt->playerId);

		PLAYERID playerID = pkt->playerIndex;
		switch(packetIdentifier)
		{
			case ID_DISCONNECTION_NOTIFICATION:
				for(int i = 0; i < iScriptsRunning; i++)
				{
					if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
						ScriptEvent_OnPlayerDisconnect(script.scriptVM[i], playerID, playerPool[playerID].szPlayerName, "Leaving");
				}
				removePlayerFromPool(pkt->playerIndex, 0);
				break;

			case ID_NEW_INCOMING_CONNECTION:
				for(int i = 0; i < iScriptsRunning; i++)
				{
					if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
						ScriptEvent_OnNewConnection(script.scriptVM[i], playerID, 
							pkt->playerId.ToString(false), pkt->playerId.port);
				}
				break;

			case ID_MODIFIED_PACKET:
				Log("Modified packet from: %s", pkt->playerId.ToString(true));
				break;

			case ID_CONNECTION_LOST:
				for(int i = 0; i < iScriptsRunning; i++)
				{
					if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
						ScriptEvent_OnPlayerDisconnect(script.scriptVM[i], playerID, playerPool[playerID].szPlayerName, "Lost connection");
				}
				removePlayerFromPool(pkt->playerIndex, 1);
				break;

			case ID_PLAYER_SYNC:
				Packet_PlayerSync(pkt);
				break;
			case ID_VEHICLE_SYNC:
				Packet_VehicleSync(pkt);
				break;
			case ID_PASSENGER_SYNC:
				Packet_PassengerSync(pkt);
				break;
			case ID_AIM_SYNC:
				Packet_AimSync(pkt);
				break;
			case ID_TRAILER_SYNC:
				Packet_TrailerSync(pkt);
				break;
			case ID_UNOCCUPIED_SYNC:
				Packet_UnoccupiedSync(pkt);
				break;
			case ID_MARKERS_SYNC:
				Packet_MarkersSync(pkt);
				break;
			case ID_BULLET_SYNC:
				Packet_BulletSync(pkt);
		}

		pRakServer->DeallocatePacket(pkt);
	}
}

void UpdatePosition(int iPlayerID, float fX, float fY, float fZ)
{
	if(playerPool[iPlayerID].bCheckpointActive)
	{
		float fSX = (fX - playerPool[iPlayerID].vecCheckpointPos[0]) * (fX - playerPool[iPlayerID].vecCheckpointPos[0]);
		float fSY = (fY - playerPool[iPlayerID].vecCheckpointPos[1]) * (fY - playerPool[iPlayerID].vecCheckpointPos[1]);
		float fSZ = (fZ - playerPool[iPlayerID].vecCheckpointPos[2]) * (fZ - playerPool[iPlayerID].vecCheckpointPos[2]);

		if((float)sqrt(fSX + fSY + fSZ) < playerPool[iPlayerID].fCheckpointSize)
		{
			if(!playerPool[iPlayerID].bPlayerInCheckpoint)
			{
				playerPool[iPlayerID].bPlayerInCheckpoint = true;

				for(int i = 0; i < iScriptsRunning; i++)
				{
					if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
						ScriptEvent_OnPlayerEnterCheckpoint(script.scriptVM[i], iPlayerID);
				}
			}
		} 
		else
		{
			if(playerPool[iPlayerID].bPlayerInCheckpoint)
			{
				playerPool[iPlayerID].bPlayerInCheckpoint = false;

				for(int i = 0; i < iScriptsRunning; i++)
				{
					if(script.scriptVM[i] != NULL && script.szScriptName[i][0] != 0x00)
						ScriptEvent_OnPlayerLeaveCheckpoint(script.scriptVM[i], iPlayerID);
				}
			}
		}
	}
}
