#include "zbuildlist.h"

ZBuildList::ZBuildList()
{
	zsettings = NULL;
}

void ZBuildList::SetZSettings(ZSettings *zsettings_)
{
	zsettings = zsettings_;
}

void ZBuildList::ClearData()
{
	int i, j;

	for(i=0;i<MAX_BUILDING_TYPES;i++)
		for(j=0;j<MAX_BUILDING_LEVELS;j++)
			buildlist_data[i][j].clear();
}

void ZBuildList::LoadDefaults()
{
	int level;

	//FORT_BACK will default to FORT_FRONT
	level = 0;
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, CRANE));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));

	level = 1;
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, CRANE));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GUN));

	level = 2;
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, CRANE));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));

	level = 3;
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PYRO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, APC));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, CRANE));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));

	level = 4;
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PYRO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, LASER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, HEAVY));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, APC));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, CRANE));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, MISSILE_CANNON));

	level = 5;
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, PYRO));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(ROBOT_OBJECT, LASER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, HEAVY));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, APC));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, MISSILE_LAUNCHER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(VEHICLE_OBJECT, CRANE));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));
	buildlist_data[FORT_FRONT][level].push_back(buildlist_object(CANNON_OBJECT, MISSILE_CANNON));

	level = 0;
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));

	level = 1;
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));

	level = 2;
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));

	level = 3;
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PYRO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));

	level = 4;
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PYRO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, LASER));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));

	level = 5;
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, GRUNT));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PSYCHO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, SNIPER));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, TOUGH));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, PYRO));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(ROBOT_OBJECT, LASER));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));
	buildlist_data[ROBOT_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, MISSILE_CANNON));

	level = 0;
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));

	level = 1;
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));

	level = 2;
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));

	level = 3;
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, APC));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));

	level = 4;
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, HEAVY));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, APC));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));

	level = 5;
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, JEEP));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, LIGHT));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, MEDIUM));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, HEAVY));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, APC));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(VEHICLE_OBJECT, MISSILE_LAUNCHER));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GATLING));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, GUN));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, HOWITZER));
	buildlist_data[VEHICLE_FACTORY][level].push_back(buildlist_object(CANNON_OBJECT, MISSILE_CANNON));
}

vector<buildlist_object> &ZBuildList::GetBuildList(int building_type, int level)
{
	if(building_type >= MAX_BUILDING_TYPES || building_type < 0)
	{
		printf("ZBuildList::GetBuildList:: invalid building_type\n");
		building_type = 0;
	}

	if(level >= MAX_BUILDING_LEVELS || level < 0)
	{
		printf("ZBuildList::GetBuildList:: invalid level\n");
		level = 0;
	}

	if(building_type == FORT_BACK)
		building_type = FORT_FRONT;

	return buildlist_data[building_type][level];
}

bool ZBuildList::GetFirstUnitInBuildList(int building_type, int level, unsigned char &ot, unsigned char &oid)
{
	vector<buildlist_object> &the_list = GetBuildList(building_type, level);

	if(!the_list.size()) return false;

	ot = the_list.begin()->ot;
	oid = the_list.begin()->oid;

	return true;
}

bool ZBuildList::UnitInBuildList(int building_type, int level, unsigned char ot_, unsigned char oid_)
{
	vector<buildlist_object> &the_list = GetBuildList(building_type, level);
	vector<buildlist_object>::iterator i;

	for(i=the_list.begin();i!=the_list.end();i++)
		if(i->ot == ot_ && i->oid == oid_)
			return true;

	return false;
}

int ZBuildList::UnitBuildTime(unsigned char ot, unsigned char oid)
{
	if(!zsettings)
	{
		printf("ZBuildList::UnitBuildTime:zsettings not set\n");	
		return 5;
	}

	return zsettings->GetUnitSettings(ot, oid).build_time;

 //  	switch(ot)
 //  	{
 //  		case CANNON_OBJECT:
 //  			switch(oid)
 //  			{
 //  				case GATLING:
 //  					return GATLING_BUILD_TIME;
 //  					break;
 //  				case GUN:
 //  					return GUN_BUILD_TIME;
 //  					break;
 //  				case HOWITZER:
 //  					return HOWITZER_BUILD_TIME;
 //  					break;
 //  				case MISSILE_CANNON:
 //  					return MISSILE_CANNON_BUILD_TIME;
 //  					break;
 //  			}
 //  			break;
 //  		case VEHICLE_OBJECT:
 //  			switch(oid)
 //  			{
 //  				case JEEP:
 //  					return JEEP_BUILD_TIME;
 //  					break;
 //  				case LIGHT:
 //  					return LIGHT_BUILD_TIME;
 //  					break;
 //  				case MEDIUM:
 //  					return MEDIUM_BUILD_TIME;
 //  					break;
 //  				case HEAVY:
 //  					return HEAVY_BUILD_TIME;
 //  					break;
 //  				case APC:
 //  					return APC_BUILD_TIME;
 //  					break;
 //  				case MISSILE_LAUNCHER:
 //  					return MMISSILE_BUILD_TIME;
 //  					break;
 //  				case CRANE:
 //  					return CRANE_BUILD_TIME;
 //  					break;
 //  			}
 //  			break;
 //  		case ROBOT_OBJECT:
 //  			switch(oid)
 //  			{
 //  				case GRUNT:
 //  					return GRUNT_BUILD_TIME;
 //  					break;
 //  				case PSYCHO:
 //  					return PSYCHO_BUILD_TIME;
 //  					break;
 //  				case SNIPER:
 //  					return SNIPER_BUILD_TIME;
 //  					break;
 //  				case TOUGH:
 //  					return TOUGH_BUILD_TIME;
 //  					break;
 //  				case PYRO:
 //  					return PYRO_BUILD_TIME;
 //  					break;
 //  				case LASER:
 //  					return LASER_BUILD_TIME;
 //  					break;
 //  			}
 //  			break;
 //  	}

	//return 5;
}

