#include "zrobot.h"

using namespace COMMON;

#define GRENADE_TIME_INT 0.15

ZSDL_Surface ZRobot::null_img;
ZSDL_Surface ZRobot::stand[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface ZRobot::point[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface ZRobot::dodge[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][2];
ZSDL_Surface ZRobot::throw_something[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][4];
ZSDL_Surface ZRobot::enter_apc[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][5];
ZSDL_Surface ZRobot::walk[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][4];
ZSDL_Surface ZRobot::look_around[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
ZSDL_Surface ZRobot::beat_ground[MAX_TEAM_TYPES][9];
ZSDL_Surface ZRobot::beer[MAX_TEAM_TYPES][10];
ZSDL_Surface ZRobot::celebrate[MAX_TEAM_TYPES][4];
ZSDL_Surface ZRobot::cigarette[MAX_TEAM_TYPES][11];
ZSDL_Surface ZRobot::confused[MAX_TEAM_TYPES][2];
ZSDL_Surface ZRobot::escape_tank[MAX_TEAM_TYPES][8];
ZSDL_Surface ZRobot::full_area_scan[MAX_TEAM_TYPES][12];
ZSDL_Surface ZRobot::head_stretch[MAX_TEAM_TYPES][11];
ZSDL_Surface ZRobot::jump_up[MAX_TEAM_TYPES][5];
ZSDL_Surface ZRobot::jump_down[MAX_TEAM_TYPES][5];
ZSDL_Surface ZRobot::jump_left[MAX_TEAM_TYPES][5];
ZSDL_Surface ZRobot::jump_right[MAX_TEAM_TYPES][5];
ZSDL_Surface ZRobot::pickup_down[MAX_TEAM_TYPES][4];
ZSDL_Surface ZRobot::pickup_up[MAX_TEAM_TYPES][4];
ZSDL_Surface ZRobot::pope[MAX_TEAM_TYPES][17];
ZSDL_Surface ZRobot::praise_the_lord[MAX_TEAM_TYPES][5];

ZRobot::ZRobot(ZTime *ztime_, ZSettings *zsettings_) : ZObject(ztime_, zsettings_)
{
	double &the_time = ztime->ztime;

	width = 1;
	height = 1;
	width_pix = 16;
	height_pix = 16;
	object_name = "robot";
	object_type = ROBOT_OBJECT;
	selectable = true;
	
	action_i = 0;
	grenade_i = 0;
	throw_grenade = false;
	direction = 6;
	mode = R_STANDING;
	
	process_time_int = 0.3;
	next_process_time = the_time + process_time_int;

	grenade_amount = 0;

	//move_speed = ROBOT_SPEED;
}

void ZRobot::Common_Process(double the_time)
{
	int tx, ty;
	int new_dir;

	if(throw_grenade && the_time >= next_grenade_time)
	{
		next_grenade_time = the_time + GRENADE_TIME_INT;

		grenade_i++;
		if(grenade_i >= 4)
		{
			grenade_i = 0;
			throw_grenade = false;
		}
	}

	if(the_time < next_process_time) return;
	
	switch(mode)
	{
		case R_WALKING:
			move_i++;
			if(move_i >= 4) move_i = 0;

			break;
		case R_STANDING:
			//one in 10 that we do something
			if(rand() % 10) break;
			
			//1 in 3 that something is just turn
			if(rand() % 3)
				direction = rand() % MAX_ANGLE_TYPES;
			else
			{
				//which random other mode is it then?
				int choice = rand() % 4;
				
				action_i = 0;
				direction = 6;
				
				//printf("action_i robot:%d\n", action_i);
				
				switch(choice)
				{
					case 0:
						mode = R_CIGARETTE;
						break;
					case 1:
						mode = R_BEER;
						break;
					case 2:
						mode = R_FULLSCAN;
						break;
					case 3:
						mode = R_HEADSTRETCH;
						break;
				}
			}
			break;
		case R_CIGARETTE:
			action_i++;
			if(action_i >= 11) mode = R_STANDING;
			break;
		case R_BEER:
			action_i++;
			if(action_i >= 10) mode = R_STANDING;
			break;
		case R_FULLSCAN:
			action_i++;
			if(action_i >= 12) mode = R_STANDING;
			break;
		case R_HEADSTRETCH:
			action_i++;
			if(action_i >= 11) mode = R_STANDING;
			break;
		case R_PICKUP_UP_GRENADES:
		case R_PICKUP_DOWN_GRENADES:
			action_i++;
			if(action_i >= 4) mode = R_STANDING;
			break;
		case R_ATTACKING:
			if(!attack_object) break;
			attack_object->GetCenterCords(tx, ty);
			new_dir = DirectionFromLoc(tx - loc.x, ty - loc.y);
			if(new_dir != -1) direction = new_dir;
			break;
	}
	
	next_process_time = the_time + (process_time_int * SpeedOffsetPercentInv());
}

void ZRobot::DoPickupGrenadeAnim()
{
	//dont bother
	if(!CanHaveGrenades()) return;

	if(mode == R_ATTACKING) return;

	if(direction < 4)
		mode = R_PICKUP_UP_GRENADES;
	else
		mode = R_PICKUP_DOWN_GRENADES;
	action_i = 0;
}

void ZRobot::Init()
{
	int i, j, k;
	char filename_c[500];
	SDL_Surface *temp_surface;
	
	strcpy(filename_c, "assets/units/robots/null.png");
	temp_surface = IMG_Load(filename_c);
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		stand[0][j].LoadBaseImage(temp_surface, false);

	null_img.LoadBaseImage(temp_surface);
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
	{
		for(j=0;j<MAX_ANGLE_TYPES;j++)
		{
			sprintf(filename_c, "assets/units/robots/stand_%s_r%03d.png", team_type_string[i].c_str(), ROTATION[j]);
			//stand[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, stand[ZTEAM_BASE_TEAM][j], stand[i][j], filename_c);
			
			sprintf(filename_c, "assets/units/robots/point_%s_r%03d.png", team_type_string[i].c_str(), ROTATION[j]);
			//point[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, point[ZTEAM_BASE_TEAM][j], point[i][j], filename_c);
			
			for(k=0;k<2;k++)
			{
				sprintf(filename_c, "assets/units/robots/dodge_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
				//dodge[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
				ZTeam::LoadZSurface(i, dodge[ZTEAM_BASE_TEAM][j][k], dodge[i][j][k], filename_c);
			}
			
			for(k=0;k<4;k++)
			{
				sprintf(filename_c, "assets/units/robots/throw_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
				//throw_something[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
				ZTeam::LoadZSurface(i, throw_something[ZTEAM_BASE_TEAM][j][k], throw_something[i][j][k], filename_c);
			}
			
			for(k=0;k<5;k++)
			{
				sprintf(filename_c, "assets/units/robots/enter_apc_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
				//enter_apc[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
				ZTeam::LoadZSurface(i, enter_apc[ZTEAM_BASE_TEAM][j][k], enter_apc[i][j][k], filename_c);
			}
			
			for(k=0;k<4;k++)
			{
				sprintf(filename_c, "assets/units/robots/walk_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
				//walk[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
				ZTeam::LoadZSurface(i, walk[ZTEAM_BASE_TEAM][j][k], walk[i][j][k], filename_c);
			}
			
			for(k=0;k<3;k++)
			{
				sprintf(filename_c, "assets/units/robots/look_around_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
				//look_around[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
				ZTeam::LoadZSurface(i, look_around[ZTEAM_BASE_TEAM][j][k], look_around[i][j][k], filename_c);
			}
		}
		
		for(k=0;k<9;k++)
		{
			sprintf(filename_c, "assets/units/robots/beat_ground_%s_n%02d.png", team_type_string[i].c_str(), k);
			//beat_ground[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, beat_ground[ZTEAM_BASE_TEAM][k], beat_ground[i][k], filename_c);
		}
		
		for(k=0;k<10;k++)
		{
			sprintf(filename_c, "assets/units/robots/beer_%s_n%02d.png", team_type_string[i].c_str(), k);
			//beer[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, beer[ZTEAM_BASE_TEAM][k], beer[i][k], filename_c);
		}
		
		for(k=0;k<4;k++)
		{
			sprintf(filename_c, "assets/units/robots/celebrate_%s_n%02d.png", team_type_string[i].c_str(), k);
			//celebrate[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, celebrate[ZTEAM_BASE_TEAM][k], celebrate[i][k], filename_c);
		}
		
		for(k=0;k<11;k++)
		{
			sprintf(filename_c, "assets/units/robots/cigarette_%s_n%02d.png", team_type_string[i].c_str(), k);
			//cigarette[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cigarette[ZTEAM_BASE_TEAM][k], cigarette[i][k], filename_c);
		}
		
		for(k=0;k<2;k++)
		{
			sprintf(filename_c, "assets/units/robots/confused_%s_n%02d.png", team_type_string[i].c_str(), k);
			//confused[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, confused[ZTEAM_BASE_TEAM][k], confused[i][k], filename_c);
		}
		
		for(k=0;k<8;k++)
		{
			sprintf(filename_c, "assets/units/robots/escape_tank_%s_n%02d.png", team_type_string[i].c_str(), k);
			//escape_tank[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, escape_tank[ZTEAM_BASE_TEAM][k], escape_tank[i][k], filename_c);
		}
		
		for(k=0;k<12;k++)
		{
			sprintf(filename_c, "assets/units/robots/full_area_scan_%s_n%02d.png", team_type_string[i].c_str(), k);
			//full_area_scan[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, full_area_scan[ZTEAM_BASE_TEAM][k], full_area_scan[i][k], filename_c);
		}
		
		for(k=0;k<11;k++)
		{
			sprintf(filename_c, "assets/units/robots/head_stretch_%s_n%02d.png", team_type_string[i].c_str(), k);
			//head_stretch[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, head_stretch[ZTEAM_BASE_TEAM][k], head_stretch[i][k], filename_c);
		}
		
		for(k=0;k<5;k++)
		{
			sprintf(filename_c, "assets/units/robots/jump-up_%s_n%02d.png", team_type_string[i].c_str(), k);
			//jump_up[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, jump_up[ZTEAM_BASE_TEAM][k], jump_up[i][k], filename_c);
			
			sprintf(filename_c, "assets/units/robots/jump-down_%s_n%02d.png", team_type_string[i].c_str(), k);
			//jump_down[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, jump_down[ZTEAM_BASE_TEAM][k], jump_down[i][k], filename_c);
			
			sprintf(filename_c, "assets/units/robots/jump-left_%s_n%02d.png", team_type_string[i].c_str(), k);
			//jump_left[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, jump_left[ZTEAM_BASE_TEAM][k], jump_left[i][k], filename_c);
			
			sprintf(filename_c, "assets/units/robots/jump-right_%s_n%02d.png", team_type_string[i].c_str(), k);
			//jump_right[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, jump_right[ZTEAM_BASE_TEAM][k], jump_right[i][k], filename_c);
		}
		
		for(k=0;k<4;k++)
		{
			sprintf(filename_c, "assets/units/robots/pickup-down_%s_n%02d.png", team_type_string[i].c_str(), k);
			//pickup_down[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, pickup_down[ZTEAM_BASE_TEAM][k], pickup_down[i][k], filename_c);
			
			sprintf(filename_c, "assets/units/robots/pickup-up_%s_n%02d.png", team_type_string[i].c_str(), k);
			//pickup_up[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, pickup_up[ZTEAM_BASE_TEAM][k], pickup_up[i][k], filename_c);
		}
		
		for(k=0;k<17;k++)
		{
			sprintf(filename_c, "assets/units/robots/pope_%s_n%02d.png", team_type_string[i].c_str(), k);
			//pope[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, pope[ZTEAM_BASE_TEAM][k], pope[i][k], filename_c);
		}
		
		for(k=0;k<5;k++)
		{
			sprintf(filename_c, "assets/units/robots/praise_the_lord_%s_n%02d.png", team_type_string[i].c_str(), k);
			//praise_the_lord[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, praise_the_lord[ZTEAM_BASE_TEAM][k], praise_the_lord[i][k], filename_c);
		}
	}
}

void ZRobot::RecalcDirection()
{
	int new_dir;

	new_dir = DirectionFromLoc(loc.dx, loc.dy);

	if(new_dir != -1) 
	{
		if(mode != R_WALKING)
			move_i = 0;

		mode = R_WALKING;
		direction = new_dir;
		//printf("mode:walking\n");
	}
	else
	{
		mode = R_STANDING;
		//printf("mode:standing\n");
	}
}

void ZRobot::SetAttackObject(ZObject *obj)
{
	double &the_time = ztime->ztime;

	attack_object = obj;

	if(attack_object)
	{
		mode = R_ATTACKING;
		action_i = 0;
		next_attack_time = the_time + 0.1;
	}
	else
	{
		if(mode != R_WALKING && mode != R_STANDING)
			mode = R_STANDING;
	}
}

void ZRobot::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	if(!effect_list) return;

	if(do_missile_death)
		effect_list->push_back((ZEffect*)(new ERobotTurrent(ztime, center_x, center_y, owner)));
	else
		effect_list->insert(effect_list->begin(), (ZEffect*)(new ERobotDeath(ztime, loc.x, loc.y, owner, do_fire_death)));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}

void ZRobot::FireMissile(int x_, int y_)
{
	double &the_time = ztime->ztime;
	int &x = center_x;
	int &y = center_y;

	double dist;
	double dist_time;
	int dx = x - x_;
	int dy = y - y_;

	dist = sqrt((float)((dx * dx) + (dy * dy)));
	dist_time = dist / zsettings->grenade_missile_speed;

	if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, x + 2, y + 2, x_, y_, dist_time, ETURRENTMISSILE_GRENADE)));	
	//if(effect_list) effect_list->push_back((ZEffect*)(new ELightRocket(ztime, x+17+sx[t_direction], y+14+sy[t_direction], x_, y_, missile_speed, 0, 1, 1)));

	ZSoundEngine::PlayWavRestricted(THROW_GRENADE_SND, loc.x, loc.y, width_pix, height_pix);

	grenade_i = 0;
	throw_grenade = true;
	next_grenade_time = the_time + GRENADE_TIME_INT;
}

bool ZRobot::CanThrowGrenades()
{
	return GetGrenadeAmount() || (GetGroupLeader() && GetGroupLeader()->GetGrenadeAmount());
}

void ZRobot::SetGrenadeAmount(int grenade_amount_) 
{ 
	grenade_amount = grenade_amount_;

	if(grenade_amount < 0 || grenade_amount > 99)
	{
		printf("ZRobot::SetGrenadeAmount: had to adjust bad value:%d\n", grenade_amount);
		grenade_amount = 0;
	}
}
