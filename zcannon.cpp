#include "zcannon.h"

ZSDL_Surface ZCannon::init_place[3];

ZCannon::ZCannon(ZTime *ztime_, ZSettings *zsettings_) : ZObject(ztime_, zsettings_)
{
	object_type = CANNON_OBJECT;
	selectable = true;
	can_be_sniped = true;
	ejectable = true;

	width = 2;
	height = 2;
	width_pix = 32;
	height_pix = 32;
}

void ZCannon::Init()
{
	int i;
	char filename_c[500];
	
	//load colors
	for(i=0;i<3;i++)
	{
		sprintf(filename_c, "assets/units/cannons/init-place_n%02d.png", i);
		init_place[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
}

void ZCannon::SetAttackObject(ZObject *obj)
{
	attack_object = obj;

	if(attack_object)
	{
		int tx, ty;
		int new_dir;

		attack_object->GetCenterCords(tx, ty);
		new_dir = DirectionFromLoc((float)(tx - loc.x), (float)(ty - loc.y));
		if(new_dir != -1) direction = new_dir;
	}
}

bool ZCannon::CanBeSniped()
{
	return can_be_sniped && driver_info.size() && ejectable;
}

bool ZCannon::CanEjectDrivers()
{
	//return true;
	return ejectable;
}

void ZCannon::SetInitialDrivers()
{
	if(owner != NULL_TEAM)
	{
		driver_type = GRUNT;
		//AddDriver(GRUNT_MAX_HEALTH);
		AddDriver((int)(zsettings->GetUnitSettings(ROBOT_OBJECT, GRUNT).health * MAX_UNIT_HEALTH));
	}
	else
	{
		driver_type = GRUNT;
		ClearDrivers();
	}
}

void ZCannon::SetEjectableCannon(bool ejectable_)
{
	ejectable = ejectable_;
}
