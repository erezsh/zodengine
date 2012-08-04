#include "zunitrating.h"
#include "constants.h"
#include "zmap.h"

#include <stdio.h>

ZUnitRating::ZUnitRating()
{
	unit_cr = NULL;
}

void ZUnitRating::Init()
{
	InitPopulateUCR();
}

void ZUnitRating::InitMallocUCR()
{
	if(unit_cr) return;

	int i,j,k,l;
	int max_ot, max_oid;

	max_ot = 0;
	max_oid = 0;

	if(CANNON_OBJECT > max_ot) max_ot = CANNON_OBJECT;
	if(VEHICLE_OBJECT > max_ot) max_ot = VEHICLE_OBJECT;
	if(ROBOT_OBJECT > max_ot) max_ot = ROBOT_OBJECT;

	if(MAX_CANNON_TYPES > max_oid) max_oid = MAX_CANNON_TYPES;
	if(MAX_VEHICLE_TYPES > max_oid) max_oid = MAX_VEHICLE_TYPES;
	if(MAX_ROBOT_TYPES > max_oid) max_oid = MAX_ROBOT_TYPES;

	max_ot++;

	//unit_cr = new char[max_ot][max_oid][max_ot][max_oid];
	unit_cr = (char****)malloc(max_ot * sizeof(char***));
	for(i=0;i<max_ot;i++)
	{
		unit_cr[i] = (char***)malloc(max_oid * sizeof(char**));
		for(j=0;j<max_oid;j++)
		{
			unit_cr[i][j] = (char**)malloc(max_ot * sizeof(char*));
			for(k=0;k<max_ot;k++)
			{
				unit_cr[i][j][k] = (char*)malloc(max_oid * sizeof(char));
				for(l=0;l<max_oid;l++)
					unit_cr[i][j][k][l] = UCR_EVEN;
			}
		}
	}
}

void ZUnitRating::InitPopulateUCR()
{
	if(!unit_cr) InitMallocUCR();

	//will die's only

	//Robots vs ...
	InsertCrossReference(ROBOT_OBJECT, GRUNT, ROBOT_OBJECT, PSYCHO, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, ROBOT_OBJECT, SNIPER, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, ROBOT_OBJECT, TOUGH, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, ROBOT_OBJECT, PYRO, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, ROBOT_OBJECT, LASER, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, CANNON_OBJECT, GATLING, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, CANNON_OBJECT, GUN, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, CANNON_OBJECT, HOWITZER, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, VEHICLE_OBJECT, JEEP, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, VEHICLE_OBJECT, LIGHT, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, VEHICLE_OBJECT, MEDIUM, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, VEHICLE_OBJECT, HEAVY, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, GRUNT, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(ROBOT_OBJECT, PSYCHO, ROBOT_OBJECT, TOUGH, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, PSYCHO, ROBOT_OBJECT, PYRO, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, PSYCHO, ROBOT_OBJECT, LASER, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, PSYCHO, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, PSYCHO, VEHICLE_OBJECT, MEDIUM, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, PSYCHO, VEHICLE_OBJECT, HEAVY, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, PSYCHO, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(ROBOT_OBJECT, SNIPER, ROBOT_OBJECT, TOUGH, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, SNIPER, ROBOT_OBJECT, PYRO, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, SNIPER, ROBOT_OBJECT, LASER, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, SNIPER, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, SNIPER, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(ROBOT_OBJECT, TOUGH, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, TOUGH, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(ROBOT_OBJECT, PYRO, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, PYRO, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(ROBOT_OBJECT, LASER, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(ROBOT_OBJECT, LASER, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	//Vehicles vs ...
	InsertCrossReference(VEHICLE_OBJECT, JEEP, ROBOT_OBJECT, TOUGH, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, ROBOT_OBJECT, PYRO, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, ROBOT_OBJECT, LASER, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, CANNON_OBJECT, GUN, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, CANNON_OBJECT, HOWITZER, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, VEHICLE_OBJECT, LIGHT, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, VEHICLE_OBJECT, MEDIUM, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, VEHICLE_OBJECT, HEAVY, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, JEEP, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(VEHICLE_OBJECT, LIGHT, CANNON_OBJECT, MISSILE_CANNON, UCR_WILL_DIE);
	InsertCrossReference(VEHICLE_OBJECT, LIGHT, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(VEHICLE_OBJECT, MEDIUM, VEHICLE_OBJECT, MISSILE_LAUNCHER, UCR_WILL_DIE);

	InsertCrossReference(VEHICLE_OBJECT, MISSILE_LAUNCHER, CANNON_OBJECT, HOWITZER, UCR_WILL_DIE);

}

bool ZUnitRating::IsUnit(unsigned char ot, unsigned char oid)
{
	switch(ot)
	{
	case CANNON_OBJECT:
		if(oid >= 0 && oid < MAX_CANNON_TYPES) return true;
		break;
	case VEHICLE_OBJECT:
		if(oid >= 0 && oid < MAX_VEHICLE_TYPES) return true;
		break;
	case ROBOT_OBJECT:
		if(oid >= 0 && oid < MAX_ROBOT_TYPES) return true;
		break;
	}

	return false;
}

void ZUnitRating::InsertCrossReference(unsigned char a_ot, unsigned char a_oid, unsigned char v_ot, unsigned char v_oid, int ucr)
{
	unit_cr[a_ot][a_oid][v_ot][v_oid] = ucr;

	if(ucr == UCR_WILL_DIE) ucr = UCR_WILL_KILL;
	else if(ucr == UCR_WILL_KILL) ucr = UCR_WILL_DIE;

	unit_cr[v_ot][v_oid][a_ot][a_oid] = ucr;
}

int ZUnitRating::CrossReference(unsigned char a_ot, unsigned char a_oid, unsigned char v_ot, unsigned char v_oid)
{
	if(!unit_cr) return UCR_EVEN;

	if(!IsUnit(a_ot, a_oid)) return UCR_EVEN;
	if(!IsUnit(v_ot, v_oid)) return UCR_EVEN;

	return unit_cr[a_ot][a_oid][v_ot][v_oid];
}
