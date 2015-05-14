/*
	Updated to 0.3.7 by P3ti
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

VEHICLEID addStaticVehicle(int iVehModelID, float fPosX, float fPosY, float fPosZ, float fRot,
					  BYTE bColor1, BYTE bColor2, int iRespawnsIfDead, int iTimeUntilRespawn);
VEHICLEID addVehicle(int bVehModelID, float fPosX, float fPosY, float fPosZ, float fRot, BYTE bColor1, BYTE bColor2);
void removeVehicle(VEHICLEID iVehicleID);
bool doesVehicleExist(VEHICLEID iVehicleID);
