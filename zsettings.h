#ifndef _ZSETTINGS_H_
#define _ZSETTINGS_H_

#include <stdio.h>
#include <string>

#include "zmap.h"
#include "constants.h"

using namespace std;

#pragma pack(1)

class ZUnit_Settings
{
public:
	ZUnit_Settings()
	{
		group_amount = 0;
		move_speed = 0;
		attack_radius = 0;
		attack_damage = 0;
		attack_damage_chance = 0;
		attack_damage_radius = 0;
		attack_missile_speed = 0;
		attack_speed = 0;
		attack_snipe_chance = 0;
		health = 0;
		build_time = 0;
		max_run_time = 0;
	}

	int group_amount;
	int move_speed;
	int attack_radius;
	double attack_damage;
	double attack_damage_chance;
	int attack_damage_radius;
	int attack_missile_speed;
	double attack_speed;
	double attack_snipe_chance;
	double health;
	int build_time;
	double max_run_time;

	void SaveLine(FILE *fp, string obj_name);
	void ReadEntry(string variable, string value);
	void CensorNonMissileUnit();
	void CensorMissileUnit();
	void CensorNonWeaponUnit();
	void CensorNegatives();
};

class ZSettings
{
public:
	ZSettings();
	void SetDefaults();
	bool LoadSettings(string filename);
	bool SaveSettings(string filename);
	ZUnit_Settings &GetUnitSettings(unsigned char ot, unsigned char oid);

	ZUnit_Settings robot_settings[MAX_ROBOT_TYPES];
	ZUnit_Settings vehicle_settings[MAX_VEHICLE_TYPES];
	ZUnit_Settings cannon_settings[MAX_CANNON_TYPES];
	
	double fort_building_health;
	double robot_building_health;
	double vehicle_building_health;
	double repair_building_health;
	double radar_building_health;
	double bridge_building_health;

	double rock_item_health;
	double grenades_item_health;
	double rockets_item_health;
	double hut_item_health;
	double map_item_health;

	double grenade_damage;
	int grenade_damage_radius;
	int grenade_missile_speed;
	double grenade_attack_speed;
	double map_item_turrent_damage;

	int agro_distance;
	int auto_grab_vehicle_distance;
	int auto_grab_flag_distance;
	int building_auto_repair_time;
	int building_auto_repair_random_additional_time;
	int max_turrent_horizontal_distance;
	int max_turrent_vertical_distance;
	int grenades_per_box;
	double partially_damaged_unit_speed;
	double damaged_unit_speed;
	double run_unit_speed;
	double run_recharge_rate;
	int hut_animal_max;
	int hut_animal_min;
	int hut_animal_roam_distance;

private:
	void CensorSettings();
};

#pragma pack()

#endif
