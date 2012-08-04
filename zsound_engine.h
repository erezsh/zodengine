#ifndef _ZSOUND_ENGINE_H_
#define _ZSOUND_ENGINE_H_

#include "zsdl.h"
#include "zmap.h"

#define ZSOUND_MIX_CHANNELS 32

#define MAX_COMP_LOSING_MESSAGES 10

enum sound_engine_sound
{
	PSYCHO_FIRE_SND, RIOCHET_SND, RANDOM_EXPLOSION_SND, LIGHT_RANDOM_EXPLOSION_SND,
	LIGHT_FIRE_SND, MEDIUM_FIRE_SND, HEAVY_FIRE_SND, PYRO_FIRE_SND, LASER_FIRE_SND,
	RIFLE_FIRE_SND, GUN_FIRE_SND, GATLING_FIRE_SND, TURRENT_EXPLOSION_SND, JEEP_FIRE_SND,
	TOUGH_FIRE_SND, MOMISSILE_FIRE_SND, RADAR_SND, ROBOT_FACTORY_SND, VEHICLE_FACTORY_SND,
	COMP_VEHICLE_SND, COMP_ROBOT_SND, COMP_GUN_SND, COMP_STARTING_MANUFACTURE_SND,
	COMP_MANUFACTURING_CANCELED_SND, COMP_STARTING_REPAIR_SND, COMP_VEHICLE_REPAIRED_SND,
	COMP_TERRITORY_LOST_SND, COMP_RADAR_ACTIVATED_SND, COMP_FORT_UNDER_ATTACK,
	COMP_YOUR_LOSING_0, COMP_YOUR_LOSING_1, COMP_YOUR_LOSING_2, COMP_YOUR_LOSING_3,
	COMP_YOUR_LOSING_4, COMP_YOUR_LOSING_5, COMP_YOUR_LOSING_6, COMP_YOUR_LOSING_7,
	COMP_YOUR_LOSING_8, COMP_YOUR_LOSING_9, BAT_CHIRP_SND, CROW_SND, THROW_GRENADE_SND,
	YES_SIR1_SND, YES_SIR2_SND, YES_SIR3_SND, UNIT_REPORTING1_SND, UNIT_REPORTING2_SND,
	UNIT_REPORTING3_SND, GRUNTS_REPORTING_SND, PSYCHOS_REPORTING_SND, SNIPERS_REPORTING_SND,
	TOUGHS_REPORTING_SND, LASERS_REPORTING_SND, PYROS_REPORTING_SND, WERE_ON_OUR_WAY_SND,
	HERE_WE_GO_SND, YOU_GOT_IT_SND, MOVING_IN_SND, OKAY_SND, ALRIGHT_SND, NO_PROBLEM_SND,
	OVER_AND_OUT_SND, AFFIRMATIVE_SND, GOING_IN_SND, LETS_DO_IT_SND, LETS_GET_THEM_SND,
	WERE_UNDER_ATTACK_SND, I_SAID_WERE_UNDER_ATTACK_SND, HELP_HELP_SND,
	THEYRE_ALL_OVER_US_SND, WERE_LOSING_IT_SND, AAAHHH_SND, OH_MY_GOD_SND, FOR_CHRIST_SAKE_SND,
	YOUR_JOKING_SND, NO_WAY_SND, FORGET_IT_SND, GET_OUTTA_HERE_SND, TARGET_DESTROYED_SND,
	BRIDGE_DESTROYED_SND, BUILDING_DESTROYED_SND, GOOD_HIT_SND, NICE_ONE_SND, OH_YEAH_SND,
	GOTCHA_SND, SMOKIN_SND, COOL_SND, WIPE_OUT_SND, BRIDGE_REPAIRED_SND, BUILDING_REPAIRED_SND,
	TERRITORY_TAKEN_SND, FIRE_EXTINGUISHED_SND, GUN_CAPTURED_SND, VEHICLE_CAPTURED_SND,
	GRENADES_COLLECTED_SND, LETS_TAKE_THE_FORT_SND, LETS_DO_THEM_SND, THEYRE_ON_THE_RUN_SND,
	LETS_FINISH_THEM_OFF_SND, WHAT_ARE_YOU_WAITING_FOR_SND, LETS_END_IT_NOW_SND,
	LETS_GO_FOR_IT_SND, YEEHAA_END_SND, NICE_ONE_END_SND, ALRIGHT_END_SND, EXCELENT_END_SND,
	WEVE_DONE_IT_END_SND, YEAHHH_END_SND, YOURE_CRAP_LOSE_SND, WEVE_LOST_IT_LOSE_SND,
	YOUVE_BLOWN_IT_LOSE_SND, WE_HATE_YOU_LOSE_SND, MORON_LOSE_SND, ASSHOLE_LOSE_SND,
	ITS_OVER_LOSE_SND, LETS_DO_IT1_START_SND, LETS_DO_IT2_START_SND,
	MAX_ENGINE_SOUNDS
};

class ZSound
{
public:
	ZSound();

	void LoadSound(string filename, int base_volume_ = 40, int volume_shift_ = 20, double play_time_shift_ = 0);
	void PlaySound();
	void RepeatSound();
	void StopRepeatSound();

	Mix_Chunk *snd_chunk;
	double next_play_time;
	double play_time_shift;
	int base_volume;
	int volume_shift;
	int repeat_channel;
};

class ZSoundEngine
{
	public:
		ZSoundEngine();

		static void Init(ZMap *zmap_);
		static void PlayWav(int snd);
		static void PlayWavRestricted(int snd, int x, int y, int w = 0, int h = 0);
		static void RepeatWav(int snd);
		static void StopRepeatWav(int snd);
		
	private:
		static bool finished_init;
		static ZMap *zmap;
		static ZSound engine_sound[MAX_ENGINE_SOUNDS];

		static ZSound rifle_fire;
		static ZSound psycho_fire;
		static ZSound tough_fire;
		static ZSound pyro_fire;
		static ZSound laser_fire;
		static ZSound gun_fire;
		static ZSound gatling_fire;
		static ZSound jeep_fire;
		static ZSound light_fire;
		static ZSound medium_fire;
		static ZSound heavy_fire;
		static ZSound momissile_fire;
		static ZSound ricochet;
		static ZSound radar;
		static ZSound robot_factory;
		static ZSound vehicle_factory;
		static ZSound explosion[5];
		static ZSound turrent_explosion;
		static ZSound bat_chirp;
		static ZSound crow_sound;

		static ZSound comp_vehicle;
		static ZSound comp_robot;
		static ZSound comp_gun;
		static ZSound comp_starting_manufacture;
		static ZSound comp_manufacturing_canceled;
		static ZSound comp_starting_repair;
		static ZSound comp_vehicle_repaired;
		static ZSound comp_territory_lost;
		static ZSound comp_radar_activated;
		static ZSound comp_fort_under_attack;

};

#endif
