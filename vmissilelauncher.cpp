#include "vmissilelauncher.h"

using namespace COMMON;

const double turrent_time_int = 1.0;

ZSDL_Surface VMissileLauncher::base[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
ZSDL_Surface VMissileLauncher::top[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface VMissileLauncher::wasted[MAX_TEAM_TYPES];

VMissileLauncher::VMissileLauncher(ZTime *ztime_, ZSettings *zsettings_) : ZVehicle(ztime_, zsettings_)
{
	double &the_time = ztime->ztime;

	width = 2;
	height = 2;
	width_pix = 32;
	height_pix = 32;
	hover_name = "M Missile";
	object_name = "missilelauncher";

	InitTypeId(object_type, MISSILE_LAUNCHER);

	//object_id = MISSILE_LAUNCHER;
	//move_speed = MISSILE_LAUNCHER_SPEED;
	//attack_radius = MMISSILE_ATTACK_RADIUS;
	//max_health = MMISSILE_MAX_HEALTH;
	//health = max_health;

	//damage = MMISSILE_DAMAGE;
	//damage_int_time = MMISSILE_ATTACK_SPEED;
	//damage_radius = MMISSILE_DAMAGE_RADIUS;
	//missile_speed = MMISSILE_MISSILE_SPEED;

	has_explosives = true;
	damage_is_missile = true;
	
	t_direction = direction;
	move_i = rand() % 3;
	
	next_turrent_time = the_time;
	next_turrent_time += turrent_time_int;
}

void VMissileLauncher::Init()
{
	int i, j, k;
	char filename_c[500];
	SDL_Surface *temp_surface;
	
	strcpy(filename_c, "assets/units/vehicles/missile_launcher/empty_null.png");
	temp_surface = IMG_Load_Error(filename_c);
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		for(k=0;k<3;k++)
			base[0][j][k] = temp_surface;

	ZSDL_FreeSurface(temp_surface);
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
			for(k=0;k<3;k++)
	{
		sprintf(filename_c, "assets/units/vehicles/missile_launcher/base_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
		//base[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, base[ZTEAM_BASE_TEAM][j][k], base[i][j][k], filename_c);
	}
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/vehicles/missile_launcher/top_%s_r%03d.png", team_type_string[i].c_str(), ROTATION[j]);
		//top[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, top[ZTEAM_BASE_TEAM][j], top[i][j], filename_c);
	}
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
	{
		sprintf(filename_c, "assets/units/vehicles/missile_launcher/wasted_%s.png", team_type_string[i].c_str());
		//wasted[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, wasted[ZTEAM_BASE_TEAM], wasted[i], filename_c);
	}
	wasted[0] = wasted[1].GetBaseSurface();
}

// SDL_Surface *VMissileLauncher::GetRender()
// {
// 	if(destroyed)
// 		return wasted[owner];
// 	else
// 		return base[owner][direction][base_i];
// }

int VMissileLauncher::Process()
{
	double &the_time = ztime->ztime;

	if(moving && the_time >= next_move_time)
	{
		move_i++;
		if(move_i >= 3) move_i = 0;

		next_move_time = the_time + (VEHICLE_MOVE_ANIM_SPEED * SpeedOffsetPercentInv());
	}

	if(the_time >= next_turrent_time)
	{
		if(attack_object)
		{
			int tx, ty;
			int new_dir;

			attack_object->GetCenterCords(tx, ty);
			new_dir = DirectionFromLoc(tx - loc.x, ty - loc.y);
			if(new_dir != -1) t_direction = new_dir;
		}
		else
		{
			next_turrent_time = the_time + turrent_time_int;
			
			t_direction++;
			if(t_direction >= MAX_ANGLE_TYPES) t_direction = 0;
		}
	}
	
	return 1;
}

void VMissileLauncher::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	const int turrent_x[MAX_ANGLE_TYPES] = {0, 2, 3, 8, 9, 7, 2, 0};
	const int turrent_y[MAX_ANGLE_TYPES] = {0, 3, 0, 4, 0, -2, -3, -3};
	const int turrent_shift_x[MAX_ANGLE_TYPES] = {2, 0, 0,  0,  0, -2, 0, 0};
	const int turrent_shift_y[MAX_ANGLE_TYPES] = {0, 0, 0, -2, -2, 0,  2, -2};
	ZSDL_Surface *base_surface;
	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	if(IsDestroyed())
	{
		the_map.RenderZSurface(&wasted[owner], x, y);
		//if(the_map.GetBlitInfo(wasted[owner], x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( wasted[owner], &from_rect, dest, &to_rect);
		//}

		return;
	}

	//base_surface = base[owner][direction][move_i];
	
	the_map.RenderZSurface(&base[owner][direction][move_i], x, y, do_hit_effect);
	//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	ZSDL_BlitHitSurface( base_surface, &from_rect, dest, &to_rect, do_hit_effect);
	//}

	if(owner != NULL_TEAM && !destroyed)
	{
		lx = x + turrent_x[direction] + turrent_shift_x[t_direction];
		ly = y + turrent_y[direction] + turrent_shift_y[t_direction];
		
		the_map.RenderZSurface(&top[owner][t_direction], lx, ly, do_hit_effect);
		//if(the_map.GetBlitInfo(top[owner][t_direction], lx, ly, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;
	
		//	ZSDL_BlitHitSurface(top[owner][t_direction], &from_rect, dest, &to_rect, do_hit_effect);
		//}
	}

	do_hit_effect = false;
}

void VMissileLauncher::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	//int &x = loc.x;
	//int &y = loc.y;
	//SDL_Rect from_rect, to_rect;
	//int lx, ly;
}

void VMissileLauncher::FireMissile(int x_, int y_)
{
	int &x = loc.x;
	int &y = loc.y;

	const int sx[8] = {20, 12, 0, -12, -20, -12, 0, 12};
	const int sy[8] = {0, -12, -20, -12, 0, 12, 20, 12};

	if(effect_list) effect_list->push_back((ZEffect*)(new EMoMissileRockets(ztime, x+17+sx[t_direction], y+14+sy[t_direction], x_, y_)));
	ZSoundEngine::PlayWavRestricted(MOMISSILE_FIRE_SND, loc.x, loc.y, width_pix, height_pix);
}

void VMissileLauncher::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	if(effect_list) effect_list->insert(effect_list->begin(), (ZEffect*)(new EDeath(ztime, loc.x, loc.y, EDEATH_MOMISSILE)));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}
