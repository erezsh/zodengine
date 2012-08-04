#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>
#include <SDL/SDL.h>

using namespace std;

#define GAME_VERSION "2011-09-06"

#define MAX_PLAYER_NAME_SIZE 30
#define MAX_BUILDING_LEVELS 6

#define MAX_PLANET_TILES (20*24)
#define MAX_ANGLE_TYPES 8

#define HUD_WIDTH 100
#define HUD_HEIGHT 36

#define MAX_STORED_CANNONS 4
#define DEFAULT_MAX_UNITS_PER_TEAM 70

#define ROAD_SPEED 1.689
#define WATER_SPEED 0.7

#define TAN1 1.55740772

#define LIFE_AFTER_DEATH_TIME 0

#define MAX_BOT_BYPASS_SIZE 512
#define MAX_BOT_BYPASS_RANDOM_SIZE_OFFSET 64

#define REGISTRATION_COST 1

//#define COLLECT_TEAM_COLORS
#define USE_TEAM_COLORS
//#define ONLY_TWO_TEAMS

//#define BUILDING_AUTO_REPAIR_TIME (5 * 60)

//health
#define MAX_UNIT_HEALTH 10000

#define VEHICLE_MOVE_ANIM_SPEED 0.1

#define PI 3.14159

enum planet_type
{
	DESERT, VOLCANIC, ARCTIC, JUNGLE, CITY, MAX_PLANET_TYPES
};

const string planet_type_string[MAX_PLANET_TYPES] = 
{
	"desert", "volcanic", "arctic", "jungle", "city"
};

enum robot_type
{
	GRUNT, PSYCHO, SNIPER, TOUGH, PYRO, LASER, MAX_ROBOT_TYPES
};

const string robot_type_string[MAX_ROBOT_TYPES] = 
{
	"grunt", "psycho", "sniper", "tough", "pyro", "laser"
};

const string robot_production_string[MAX_ROBOT_TYPES] = 
{
	"Grunt", "Psycho", "Sniper", "Tough", "Pyro", "Laser"
};

enum cannon_type
{
	GATLING, GUN, HOWITZER, MISSILE_CANNON, MAX_CANNON_TYPES
};

const string cannon_type_string[MAX_CANNON_TYPES] = 
{
	"gatling", "gun", "howitzer", "missile_cannon"
};

const string cannon_production_string[MAX_CANNON_TYPES] = 
{
	"Gatling", "Gun", "Howitzer", "Missile"
};

enum vehicle_type
{
	JEEP, LIGHT, MEDIUM, HEAVY, APC, MISSILE_LAUNCHER, CRANE, MAX_VEHICLE_TYPES
};

const string vehicle_type_string[MAX_VEHICLE_TYPES] = 
{
	"jeep", "light", "medium", "heavy", "apc", "missile_launcher", "crane"
};

const string vehicle_production_string[MAX_VEHICLE_TYPES] = 
{
	"Jeep", "Light", "Medium", "Heavy", "APC", "M Missile", "Crane"
};

enum building_type
{
	FORT_FRONT, FORT_BACK, RADAR, REPAIR, ROBOT_FACTORY, VEHICLE_FACTORY, 
	BRIDGE_VERT, BRIDGE_HORZ, MAX_BUILDING_TYPES
};

const string building_type_string[MAX_BUILDING_TYPES] = 
{
	"fort_front", "fort_back", "radar", "repair", "robot_factory", "vehicle_factory",
	"bridge_vert", "bridge_horz"
};

enum item_type
{
	FLAG_ITEM, ROCK_ITEM, GRENADES_ITEM, ROCKETS_ITEM, HUT_ITEM,
	MAP0_ITEM, MAP1_ITEM, MAP2_ITEM, MAP3_ITEM, MAP4_ITEM, MAP5_ITEM,
	MAP6_ITEM, MAP7_ITEM, MAP8_ITEM, MAP9_ITEM, MAP10_ITEM, MA11_ITEM,
	MAP12_ITEM, MAP13_ITEM, MAP14_ITEM, MAP15_ITEM, MAP16_ITEM, MAP17_ITEM,
	MAP18_ITEM, MAP19_ITEM, MAP20_ITEM, MAP21_ITEM,
	MAX_ITEM_TYPES
};

#define MAP_ITEMS_AMOUNT 22

const string item_type_string[MAX_ITEM_TYPES] = 
{
	"flag", "rock", "grenades", "rockets", "hut", "map_object0"
};

#ifdef ONLY_TWO_TEAMS
enum team_type
	{
		NULL_TEAM, RED_TEAM, BLUE_TEAM, MAX_TEAM_TYPES
	};

	const string team_type_string[MAX_TEAM_TYPES] = 
	{
		"null", "red", "blue"
	};
#else
	#ifdef USE_TEAM_COLORS

	enum team_type
	{
		NULL_TEAM, RED_TEAM, BLUE_TEAM, GREEN_TEAM, YELLOW_TEAM, 
		PURPLE_TEAM, TEAL_TEAM, WHITE_TEAM, BLACK_TEAM, MAX_TEAM_TYPES
	};


	const string team_type_string[MAX_TEAM_TYPES] = 
	{
		"null", "red", "blue", "green", "yellow", 
		"purple", "teal", "white", "black"
	};

	#else

	enum team_type
	{
		NULL_TEAM, RED_TEAM, BLUE_TEAM, GREEN_TEAM, YELLOW_TEAM, MAX_TEAM_TYPES
	};

	const string team_type_string[MAX_TEAM_TYPES] = 
	{
		"null", "red", "blue", "green", "yellow"
	};

	#endif
#endif

enum player_mode
{
	NOBODY_MODE, PLAYER_MODE, BOT_MODE, SPECTATOR_MODE, TRAY_MODE, MAX_PLAYER_MODES
};

const string player_mode_string[MAX_PLAYER_MODES] = 
{
	"nobody_mode", "player_mode", "bot_mode", "spectator_mode", "tray_mode"
};

enum cursor_type
{
	CURSOR_C, PLACE_C, PLACED_C, ATTACK_C, 
	ATTACKED_C, GRAB_C, GRABBED_C, GRENADE_C, GRENADED_C,
	REPAIR_C, REPAIRED_C, NONO_C, CANNON_C, CANNONED_C,
	ENTER_C, ENTERED_C, EXIT_C, EXITED_C, MAX_CURSOR_TYPES
};

//const SDL_Color team_color[MAX_TEAM_TYPES] =
//{
//	{115, 115, 115, 0},
//	{233, 0, 0, 0},
//	{19, 55, 251, 0},
//	{23, 143, 19, 0},
//	{203, 99, 47, 0}
//};

enum rotation_enum
{
	R0, R45, R90, R135, R180, R225, R270, R315
};

const int ROTATION[MAX_ANGLE_TYPES] =
{
	0, 45, 90, 135, 180, 225, 270, 315
};

const int ROTATION_INVERTED[MAX_ANGLE_TYPES] =
{
	180, 225, 270, 315, 0, 45, 90, 135
};

#endif

