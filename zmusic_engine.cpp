#include "zmusic_engine.h"
#include "common.h"

using namespace COMMON;

bool ZMusicEngine::sound_system_on = true;

Mix_Music *ZMusicEngine::splash_music;
Mix_Music *ZMusicEngine::planet_music[MAX_PLANET_TYPES];

bool ZMusicEngine::playing_planet_music = false;
int ZMusicEngine::d_level = M_CALM;
int ZMusicEngine::p_type = DESERT;
vector<d_level_start_info> ZMusicEngine::d_level_start[MAX_PLANET_TYPES][M_MAX_DANGER_LEVELS];

bool ZMusicEngine::do_next_reset = false;
double ZMusicEngine::next_reset_time = 0;
double ZMusicEngine::next_change_d_level_time = 0;

void ZMusicEngine::Init()
{
	string filename;

	splash_music = MUS_Load_Error("assets/sounds/ABATTLE.mp3");
	
	for(int i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/sounds/music_" + planet_type_string[i] + ".ogg";
		planet_music[i] = MUS_Load_Error ( filename.c_str() );
	}

	//fixes
	if(!planet_music[DESERT]) planet_music[DESERT] = planet_music[ARCTIC];
	if(!planet_music[ARCTIC]) planet_music[ARCTIC] = planet_music[DESERT];

	if(!planet_music[VOLCANIC]) planet_music[VOLCANIC] = planet_music[CITY];
	if(!planet_music[CITY]) planet_music[CITY] = planet_music[VOLCANIC];

	InitDLevelStarts();
}

void ZMusicEngine::InitDLevelStarts()
{
	double l1_end;
	double l2_end;
	double l3_end;
	double next_in;
	double next_end;
	int next_level;
	int next_planet;

	{
		next_planet = DESERT;
		l1_end = 180.23 - 1;
		l2_end = 275.87 - 1;
		l3_end = 371.47 - 1;

		{
			next_level = M_CALM;
			next_end = l1_end;
			next_in = 0;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 60;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 75;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 88.5;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 105;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 146.3;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}

		{
			next_level = M_ATTACKING;
			next_end = l2_end;
			next_in = 180.23;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 210;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}

		{
			next_level = M_FORT;
			next_end = l3_end;
			next_in = 275.87;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 335.77;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}
	}

	{
		next_planet = VOLCANIC;
		l1_end = 204.31 - 1;
		l2_end = 267.174 - 1;
		l3_end = 361.34 - 1;

		{
			next_level = M_CALM;
			next_end = l1_end;
			next_in = 0;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 62.68;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 94.73;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 141.93;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 157.15;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}

		{
			next_level = M_ATTACKING;
			next_end = l2_end;
			next_in = 204.31;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 235.75;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}

		{
			next_level = M_FORT;
			next_end = l3_end;
			next_in = 267.174;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 282.89;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 330.04;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}
	}

	{
		next_planet = JUNGLE;
		l1_end = 168.05 - 1;
		l2_end = 227.075 - 1;
		l3_end = 342.77 - 1;

		{
			next_level = M_CALM;
			next_end = l1_end;
			next_in = 0;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 46.03;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 107.4;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 122.74;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}

		{
			next_level = M_ATTACKING;
			next_end = l2_end;
			next_in = 168.05;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 197.79;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}

		{
			next_level = M_FORT;
			next_end = l3_end;
			next_in = 227.075;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 256.37;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
			next_in = 288.83;
			d_level_start[next_planet][next_level].push_back(d_level_start_info(next_in, next_end - next_in));
		}
	}

	//copies
	for(int i=0; i<M_MAX_DANGER_LEVELS; i++)
	{
		d_level_start[ARCTIC][i] = d_level_start[DESERT][i];
		d_level_start[CITY][i] = d_level_start[VOLCANIC][i];
	}
}

void ZMusicEngine::PlaySplashMusic()
{
	if(!sound_system_on) return;

	ZSDL_PlayMusic(splash_music, -1);
	//Mix_VolumeMusic(128);

	playing_planet_music = false;
}

void ZMusicEngine::PlayPlanetMusic(int p_type_)
{
	if(!sound_system_on) return;

	if(p_type_ < 0) return;
	if(p_type_ >= MAX_PLANET_TYPES) return;

	p_type = p_type_;

	ZSDL_PlayMusic(planet_music[p_type], -1);
	//Mix_VolumeMusic(80);

	playing_planet_music = true;

	do_next_reset = false;

	d_level = M_CALM;
}

void ZMusicEngine::Process(vector<ZObject*> &object_list, ZMap &tmap, int our_team, int fort_ref_id)
{
	if(!playing_planet_music) return;

	double the_time = current_time();
	static double next_check_time = 0;
	ZObject *our_fort = NULL;

	//get fort
	our_fort = ZObject::GetObjectFromID(fort_ref_id, object_list);

	//see what our current d level is
	if(our_fort && the_time >= next_check_time)
	{
		next_check_time = the_time + 0.25;

		int new_d_level = M_CALM;
		static int last_new_d_level = M_CALM;
		
		if(our_fort->IsDestroyed())
		{
			new_d_level = M_CALM;
		}
		else
		{
			//anyone in our fort area?
			{
				int fx, fy;

				our_fort->GetCenterCords(fx, fy);

				for(vector<ZObject*>::iterator obj=object_list.begin(); obj!=object_list.end(); obj++)
				{
					unsigned char ot, oid;

					if((*obj)->GetOwner() == NULL_TEAM) continue;
					if((*obj)->GetOwner() == our_team) continue;

					(*obj)->GetObjectID(ot, oid);

					if(ot == BUILDING_OBJECT) continue;
					if(ot == MAP_ITEM_OBJECT) continue;

					int ox, oy;

					(*obj)->GetCenterCords(ox, oy);

					if(points_within_distance(fx, fy, ox, oy, 250))
					{
						new_d_level = M_FORT;
						break;
					}
				}
			}

			//are we under attack?
			if(new_d_level == M_CALM)
			{
				for(vector<ZObject*>::iterator obj=object_list.begin(); obj!=object_list.end(); obj++)
				{
					unsigned char ot, oid;

					if(!(*obj)->GetAttackObject()) continue;

					if((*obj)->GetAttackObject()->GetOwner() == our_team)
					{
						new_d_level = M_ATTACKING;
						break;
					}

					if((*obj)->GetOwner() == our_team && (*obj)->GetAttackObject()->GetOwner() != NULL_TEAM)
					{
						new_d_level = M_ATTACKING;
						break;
					}
				}
			}
		}
		

		if(last_new_d_level != new_d_level)
		{
			int inc_time;

			if(our_fort->IsDestroyed()) 
				inc_time = 15;
			else
				inc_time = 3;

			last_new_d_level = new_d_level;

			if(the_time + inc_time > next_change_d_level_time)
				next_change_d_level_time = the_time + inc_time;
		}

		if(the_time >= next_change_d_level_time)
			SetDangerLevel(new_d_level);
	}

	if(do_next_reset && the_time >= next_reset_time)
	{
		do_next_reset = false;

		ResetMusic();
	}
}

void ZMusicEngine::SetDangerLevel(int d_level_)
{
	if(d_level_ < 0) return;
	if(d_level_ >= M_MAX_DANGER_LEVELS) return;

	//if(current_time() < next_change_d_level_time) return;

	if(d_level_ == d_level) return;

	d_level = d_level_;

	ResetMusic();
}

void ZMusicEngine::ResetMusic()
{
	if(!playing_planet_music) return;

	if(d_level < 0) return;
	if(d_level >= M_MAX_DANGER_LEVELS) return;

	//even got info for this level?
	if(!d_level_start[p_type][d_level].size())
	{
		printf("ZMusicEngine::ResetMusic:No start information for this danger level\n");
		return;
	}

	int random_s;

	random_s = rand() % d_level_start[p_type][d_level].size();

	//printf("ResetMusic:: p:%d d:%d ran:%d\n", p_type, d_level, random_s);

	//set the position
	if(Mix_SetMusicPosition(d_level_start[p_type][d_level][random_s].position)==-1)
		printf("Mix_SetMusicPosition: %s\n", Mix_GetError());

	//tell the system when to reset again
	double the_time = current_time();
	do_next_reset = true;
	next_reset_time = the_time + d_level_start[p_type][d_level][random_s].length;

	switch(d_level)
	{
	case M_CALM: next_change_d_level_time = the_time + 5; break;
	case M_ATTACKING: next_change_d_level_time = the_time + 7; break;
	case M_FORT: next_change_d_level_time = the_time + 3; break;
	default: next_change_d_level_time = the_time + 7; break;
	}
}
