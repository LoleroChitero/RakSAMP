/*
	Updated to 0.3z by P3ti
*/

extern int iVehicleCount;

struct stVehiclePool
{
	bool bExists;
	int iModelID;
	float fHealth;
	float fVehicleRoll[3];
	float fVehicleDir[3];
	float fVehiclePos[3];
	float fRotation;
	BYTE bColor1;
	BYTE bColor2;
	bool bIsStatic;
	int iRespawnsIfDead;
	int iTimeUntilRespawn;
};
extern struct stVehiclePool vehiclePool[MAX_VEHICLES];

#pragma pack(1)
typedef struct _NEW_VEHICLE {
	VEHICLEID VehicleId;
	int		  iVehicleType;
	float	  vecPos[3];
	float	  fRotation;
	char	  aColor1;
	char	  aColor2;
	float	  fHealth;
	BYTE	  byteInterior;
	BYTE	  byteDoorsLocked;
	DWORD	  dwDoorDamageStatus;
	DWORD	  dwPanelDamageStatus;
	BYTE	  byteLightDamageStatus;
} NEW_VEHICLE;

VEHICLEID addStaticVehicle(int iVehModelID, float fPosX, float fPosY, float fPosZ, float fRot,
					  BYTE bColor1, BYTE bColor2, int iRespawnsIfDead, int iTimeUntilRespawn);
VEHICLEID addVehicle(int bVehModelID, float fPosX, float fPosY, float fPosZ, float fRot, BYTE bColor1, BYTE bColor2);
void removeVehicle(VEHICLEID iVehicleID);
bool doesVehicleExist(VEHICLEID iVehicleID);
