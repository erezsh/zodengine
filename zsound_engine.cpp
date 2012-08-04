#include "zsound_engine.h"
#include "common.h"

using namespace COMMON;

bool ZSoundEngine::finished_init = NULL;
ZMap *ZSoundEngine::zmap = NULL;
ZSound ZSoundEngine::engine_sound[MAX_ENGINE_SOUNDS];

ZSound ZSoundEngine::rifle_fire;
ZSound ZSoundEngine::psycho_fire;
ZSound ZSoundEngine::tough_fire;
ZSound ZSoundEngine::pyro_fire;
ZSound ZSoundEngine::laser_fire;
ZSound ZSoundEngine::gun_fire;
ZSound ZSoundEngine::gatling_fire;
ZSound ZSoundEngine::jeep_fire;
ZSound ZSoundEngine::light_fire;
ZSound ZSoundEngine::medium_fire;
ZSound ZSoundEngine::heavy_fire;
ZSound ZSoundEngine::momissile_fire;
ZSound ZSoundEngine::ricochet;
ZSound ZSoundEngine::radar;
ZSound ZSoundEngine::robot_factory;
ZSound ZSoundEngine::vehicle_factory;
ZSound ZSoundEngine::explosion[5];
ZSound ZSoundEngine::turrent_explosion;
ZSound ZSoundEngine::comp_vehicle;
ZSound ZSoundEngine::comp_robot;
ZSound ZSoundEngine::comp_gun;
ZSound ZSoundEngine::comp_starting_manufacture;
ZSound ZSoundEngine::comp_manufacturing_canceled;
ZSound ZSoundEngine::comp_starting_repair;
ZSound ZSoundEngine::comp_vehicle_repaired;
ZSound ZSoundEngine::comp_territory_lost;
ZSound ZSoundEngine::comp_radar_activated;
ZSound ZSoundEngine::comp_fort_under_attack;
ZSound ZSoundEngine::bat_chirp;
ZSound ZSoundEngine::crow_sound;

ZSound::ZSound()
{
	snd_chunk = NULL;
	next_play_time = 0;
	repeat_channel = -1;
}

void ZSound::LoadSound(string filename, int base_volume_, int volume_shift_, double play_time_shift_)
{
	base_volume = base_volume_;
	volume_shift = volume_shift_;
	play_time_shift = play_time_shift_;
	snd_chunk = MIX_Load_Error(filename);

	if(snd_chunk)
		snd_chunk->volume = base_volume;
}

void ZSound::PlaySound()
{
	double the_time;

	if(!snd_chunk) return;

	the_time = current_time();

	if(the_time < next_play_time) return;

	next_play_time = the_time + play_time_shift + 0.01 * (rand() % 31);

	//add some uniqueness to each sound
	snd_chunk->volume = base_volume + (rand() % volume_shift);

	ZMix_PlayChannel(-1, snd_chunk, 0);
}

void ZSound::RepeatSound()
{
	int i;

	if(!snd_chunk) return;

	//we already repeating?
	if(repeat_channel != -1) return;

	//find a channel
	for(i=0;i<ZSOUND_MIX_CHANNELS;i++)
		if(!Mix_Playing(i))
			break;

	//find one?
	if(i==ZSOUND_MIX_CHANNELS) return;

	//set and play
	repeat_channel = i;

	ZMix_PlayChannel(repeat_channel, snd_chunk, -1);

}

void ZSound::StopRepeatSound()
{
	//are we repeating?
	if(repeat_channel == -1) return;

	Mix_HaltChannel(repeat_channel);

	repeat_channel = -1;
}

ZSoundEngine::ZSoundEngine()
{
	zmap = NULL;
}

void ZSoundEngine::Init(ZMap *zmap_)
{
	char filename_c[500];
	int i;

	zmap = zmap_;

	//this may not be the best place for this
	Mix_AllocateChannels(ZSOUND_MIX_CHANNELS);

	for(i=0;i<5;i++)
	{
		sprintf(filename_c, "assets/sounds/explosion_%02d.wav", i);
		explosion[i].LoadSound(filename_c, 30);
	}

	psycho_fire.LoadSound("assets/sounds/MACHGUN2.wav");
	ricochet.LoadSound("assets/sounds/RICOCH1.wav");
	jeep_fire.LoadSound("assets/sounds/JEEPMGUN.wav", 40, 20, 0.15);
	light_fire.LoadSound("assets/sounds/LTANKGUN.wav", 25);
	medium_fire.LoadSound("assets/sounds/MTANKGUN.wav", 25);
	heavy_fire.LoadSound("assets/sounds/HTANKGUN.wav", 25);
	momissile_fire.LoadSound("assets/sounds/MOBIMIS2.wav");
	rifle_fire.LoadSound("assets/sounds/RIFLE3.wav", 10, 10);
	tough_fire.LoadSound("assets/sounds/MOBIMISS.wav");
	pyro_fire.LoadSound("assets/sounds/FLAMER.wav", 20, 10);
	laser_fire.LoadSound("assets/sounds/LASERGUN.wav", 20, 10);
	gun_fire.LoadSound("assets/sounds/LTGUN.wav");
	gatling_fire.LoadSound("assets/sounds/GATTGUN.wav");
	turrent_explosion.LoadSound("assets/sounds/METGRND.wav");

	bat_chirp.LoadSound("assets/sounds/BATCHIRP.wav");
	crow_sound.LoadSound("assets/sounds/CROW2.wav");

	radar.LoadSound("assets/sounds/radar_sound.wav", 20);
	robot_factory.LoadSound("assets/sounds/ROBFACT5.wav", 5);

	//i could not find a unique vehicle factory sound
	vehicle_factory.LoadSound("assets/sounds/ROBFACT5.wav");

	comp_vehicle.LoadSound("assets/sounds/comp_vehicle_manufactured.wav");
	comp_robot.LoadSound("assets/sounds/comp_robot_manufactured.wav");
	comp_gun.LoadSound("assets/sounds/comp_gun_manufactured.wav");
	comp_starting_manufacture.LoadSound("assets/sounds/comp_starting_manufacture.wav");
	comp_manufacturing_canceled.LoadSound("assets/sounds/comp_manufacturing_canceled.wav");
	comp_starting_repair.LoadSound("assets/sounds/comp_starting_repair.wav");
	comp_vehicle_repaired.LoadSound("assets/sounds/comp_vehicle_repaired.wav");
	comp_territory_lost.LoadSound("assets/sounds/comp_territory_lost.wav");
	comp_radar_activated.LoadSound("assets/sounds/comp_radar_activated.wav");
	comp_fort_under_attack.LoadSound("assets/sounds/comp_fort_under_attack.wav");

	for(i=0;i<MAX_COMP_LOSING_MESSAGES;i++)
	{
		sprintf(filename_c, "assets/sounds/comp_youre_losing_%02d.wav", i);
		engine_sound[i+COMP_YOUR_LOSING_0].LoadSound(filename_c);
	}

	engine_sound[THROW_GRENADE_SND].LoadSound("assets/sounds/GRENLOBX.wav");

	engine_sound[YES_SIR1_SND].LoadSound("assets/sounds/ROB01.wav");
	engine_sound[YES_SIR2_SND].LoadSound("assets/sounds/ROB02.wav");
	engine_sound[YES_SIR3_SND].LoadSound("assets/sounds/ROB03.wav");
	engine_sound[UNIT_REPORTING1_SND].LoadSound("assets/sounds/ROB04.wav");
	engine_sound[UNIT_REPORTING2_SND].LoadSound("assets/sounds/ROB05.wav");
	engine_sound[UNIT_REPORTING3_SND].LoadSound("assets/sounds/ROB06.wav");
	engine_sound[GRUNTS_REPORTING_SND].LoadSound("assets/sounds/ROB07.wav");
	engine_sound[PSYCHOS_REPORTING_SND].LoadSound("assets/sounds/ROB08.wav");
	engine_sound[SNIPERS_REPORTING_SND].LoadSound("assets/sounds/ROB09.wav");
	engine_sound[TOUGHS_REPORTING_SND].LoadSound("assets/sounds/ROB10.wav");
	engine_sound[LASERS_REPORTING_SND].LoadSound("assets/sounds/ROB11.wav");
	engine_sound[PYROS_REPORTING_SND].LoadSound("assets/sounds/ROB12.wav");
	engine_sound[WERE_ON_OUR_WAY_SND].LoadSound("assets/sounds/ROB13.wav");
	engine_sound[HERE_WE_GO_SND].LoadSound("assets/sounds/ROB14.wav");
	engine_sound[YOU_GOT_IT_SND].LoadSound("assets/sounds/ROB15.wav");
	engine_sound[MOVING_IN_SND].LoadSound("assets/sounds/ROB16.wav");
	engine_sound[OKAY_SND].LoadSound("assets/sounds/ROB17.wav");
	engine_sound[ALRIGHT_SND].LoadSound("assets/sounds/ROB18.wav");
	engine_sound[NO_PROBLEM_SND].LoadSound("assets/sounds/ROB19.wav");
	engine_sound[OVER_AND_OUT_SND].LoadSound("assets/sounds/ROB20.wav");
	engine_sound[AFFIRMATIVE_SND].LoadSound("assets/sounds/ROB21.wav");
	engine_sound[GOING_IN_SND].LoadSound("assets/sounds/ROB22.wav");
	engine_sound[LETS_DO_IT_SND].LoadSound("assets/sounds/ROB23.wav");
	engine_sound[LETS_GET_THEM_SND].LoadSound("assets/sounds/ROB24.wav");
	engine_sound[WERE_UNDER_ATTACK_SND].LoadSound("assets/sounds/ROB25.wav");
	engine_sound[I_SAID_WERE_UNDER_ATTACK_SND].LoadSound("assets/sounds/ROB26.wav");
	engine_sound[HELP_HELP_SND].LoadSound("assets/sounds/ROB27.wav");
	engine_sound[THEYRE_ALL_OVER_US_SND].LoadSound("assets/sounds/ROB28.wav");
	engine_sound[WERE_LOSING_IT_SND].LoadSound("assets/sounds/ROB29.wav");
	engine_sound[AAAHHH_SND].LoadSound("assets/sounds/ROB30.wav");
	engine_sound[OH_MY_GOD_SND].LoadSound("assets/sounds/ROB31.wav");
	engine_sound[FOR_CHRIST_SAKE_SND].LoadSound("assets/sounds/ROB32.wav");
	engine_sound[YOUR_JOKING_SND].LoadSound("assets/sounds/ROB33.wav");
	engine_sound[NO_WAY_SND].LoadSound("assets/sounds/ROB34.wav");
	engine_sound[FORGET_IT_SND].LoadSound("assets/sounds/ROB35.wav");
	engine_sound[GET_OUTTA_HERE_SND].LoadSound("assets/sounds/ROB36.wav");
	engine_sound[TARGET_DESTROYED_SND].LoadSound("assets/sounds/ROB37.wav");
	engine_sound[BRIDGE_DESTROYED_SND].LoadSound("assets/sounds/ROB38.wav");
	engine_sound[BUILDING_DESTROYED_SND].LoadSound("assets/sounds/ROB39.wav");
	engine_sound[GOOD_HIT_SND].LoadSound("assets/sounds/ROB40.wav");
	engine_sound[NICE_ONE_SND].LoadSound("assets/sounds/ROB41.wav");
	engine_sound[OH_YEAH_SND].LoadSound("assets/sounds/ROB42.wav");
	engine_sound[GOTCHA_SND].LoadSound("assets/sounds/ROB43.wav");
	engine_sound[SMOKIN_SND].LoadSound("assets/sounds/ROB44.wav");
	engine_sound[COOL_SND].LoadSound("assets/sounds/ROB45.wav");
	engine_sound[WIPE_OUT_SND].LoadSound("assets/sounds/ROB46.wav");
	engine_sound[BRIDGE_REPAIRED_SND].LoadSound("assets/sounds/ROB47.wav");
	engine_sound[BUILDING_REPAIRED_SND].LoadSound("assets/sounds/ROB48.wav");
	engine_sound[TERRITORY_TAKEN_SND].LoadSound("assets/sounds/ROB49.wav");
	engine_sound[FIRE_EXTINGUISHED_SND].LoadSound("assets/sounds/ROB50.wav");
	engine_sound[GUN_CAPTURED_SND].LoadSound("assets/sounds/ROB51.wav");
	engine_sound[VEHICLE_CAPTURED_SND].LoadSound("assets/sounds/ROB52.wav");
	engine_sound[GRENADES_COLLECTED_SND].LoadSound("assets/sounds/ROB53.wav");
	engine_sound[LETS_TAKE_THE_FORT_SND].LoadSound("assets/sounds/ROB54.wav");
	engine_sound[LETS_DO_THEM_SND].LoadSound("assets/sounds/ROB55.wav");
	engine_sound[THEYRE_ON_THE_RUN_SND].LoadSound("assets/sounds/ROB56.wav");
	engine_sound[LETS_FINISH_THEM_OFF_SND].LoadSound("assets/sounds/ROB57.wav");
	engine_sound[WHAT_ARE_YOU_WAITING_FOR_SND].LoadSound("assets/sounds/ROB58.wav");
	engine_sound[LETS_END_IT_NOW_SND].LoadSound("assets/sounds/ROB59.wav");
	engine_sound[LETS_GO_FOR_IT_SND].LoadSound("assets/sounds/ROB60.wav");
	engine_sound[YEEHAA_END_SND].LoadSound("assets/sounds/ROB61.wav");
	engine_sound[NICE_ONE_END_SND].LoadSound("assets/sounds/ROB62.wav");
	engine_sound[ALRIGHT_END_SND].LoadSound("assets/sounds/ROB63.wav");
	engine_sound[EXCELENT_END_SND].LoadSound("assets/sounds/ROB64.wav");
	engine_sound[WEVE_DONE_IT_END_SND].LoadSound("assets/sounds/ROB65.wav");
	engine_sound[YEAHHH_END_SND].LoadSound("assets/sounds/ROB66.wav");
	engine_sound[YOURE_CRAP_LOSE_SND].LoadSound("assets/sounds/ROB67.wav");
	engine_sound[WEVE_LOST_IT_LOSE_SND].LoadSound("assets/sounds/ROB68.wav");
	engine_sound[YOUVE_BLOWN_IT_LOSE_SND].LoadSound("assets/sounds/ROB69.wav");
	engine_sound[WE_HATE_YOU_LOSE_SND].LoadSound("assets/sounds/ROB70.wav");
	engine_sound[MORON_LOSE_SND].LoadSound("assets/sounds/ROB71.wav");
	engine_sound[ASSHOLE_LOSE_SND].LoadSound("assets/sounds/ROB72.wav");
	engine_sound[ITS_OVER_LOSE_SND].LoadSound("assets/sounds/ROB73.wav");
	engine_sound[LETS_DO_IT1_START_SND].LoadSound("assets/sounds/ROB74.wav");
	engine_sound[LETS_DO_IT2_START_SND].LoadSound("assets/sounds/ROB75.wav");

	finished_init = true;
}

void ZSoundEngine::RepeatWav(int snd)
{
	if(!finished_init) return;

	switch(snd)
	{
	case RADAR_SND:
		radar.RepeatSound();
		break;
	case ROBOT_FACTORY_SND:
		robot_factory.RepeatSound();
		break;
	}
}

void ZSoundEngine::StopRepeatWav(int snd)
{
	if(!finished_init) return;

	switch(snd)
	{
	case RADAR_SND:
		radar.StopRepeatSound();
		break;
	case ROBOT_FACTORY_SND:
		robot_factory.StopRepeatSound();
		break;
	}
}

void ZSoundEngine::PlayWavRestricted(int snd, int x, int y, int w, int h)
{
	if(!finished_init) return;
	if(!zmap) return;

	//a check before we play
	if(!zmap->WithinView(x,y,w,h)) return;

	PlayWav(snd);
}

void ZSoundEngine::PlayWav(int snd)
{
	if(!finished_init) return;

	switch(snd)
	{
	default:
		engine_sound[snd].PlaySound();
		break;
	case RIFLE_FIRE_SND:
		rifle_fire.PlaySound();
		break;
	case PSYCHO_FIRE_SND:
		psycho_fire.PlaySound();
		break;
	case TOUGH_FIRE_SND:
		tough_fire.PlaySound();
		break;
	case PYRO_FIRE_SND:
		pyro_fire.PlaySound();
		break;
	case LASER_FIRE_SND:
		laser_fire.PlaySound();
		break;
	case GUN_FIRE_SND:
		gun_fire.PlaySound();
		break;
	case GATLING_FIRE_SND:
		gatling_fire.PlaySound();
		break;
	case JEEP_FIRE_SND:
		jeep_fire.PlaySound();
		break;
	case LIGHT_FIRE_SND:
		light_fire.PlaySound();
		break;
	case MEDIUM_FIRE_SND:
		medium_fire.PlaySound();
		break;
	case HEAVY_FIRE_SND:
		heavy_fire.PlaySound();
		break;
	case MOMISSILE_FIRE_SND:
		momissile_fire.PlaySound();
		break;
	case RIOCHET_SND:
		ricochet.PlaySound();
		break;
	case LIGHT_RANDOM_EXPLOSION_SND:
		explosion[rand()%2].PlaySound();
		break;
	case RANDOM_EXPLOSION_SND:
		explosion[rand()%5].PlaySound();
		break;
	case TURRENT_EXPLOSION_SND:
		turrent_explosion.PlaySound();
		break;
	case COMP_VEHICLE_SND:
		comp_vehicle.PlaySound();
		break;
	case COMP_ROBOT_SND:
		comp_robot.PlaySound();
		break;
	case COMP_GUN_SND:
		comp_gun.PlaySound();
		break;
	case COMP_STARTING_MANUFACTURE_SND:
		comp_starting_manufacture.PlaySound();
		break;
	case COMP_MANUFACTURING_CANCELED_SND:
		comp_manufacturing_canceled.PlaySound();
		break;
	case COMP_STARTING_REPAIR_SND:
		comp_starting_repair.PlaySound();
		break;
	case COMP_VEHICLE_REPAIRED_SND:
		comp_vehicle_repaired.PlaySound();
		break;
	case COMP_TERRITORY_LOST_SND:
		comp_territory_lost.PlaySound();
		break;
	case COMP_RADAR_ACTIVATED_SND:
		comp_radar_activated.PlaySound();
		break;
	case COMP_FORT_UNDER_ATTACK:
		comp_fort_under_attack.PlaySound();
		break;
	case BAT_CHIRP_SND:
		bat_chirp.PlaySound();
		break;
	case CROW_SND:
		crow_sound.PlaySound();
		break;
	}
}
