#include "vjeep.h"

using namespace COMMON;

const double base_time_int = 0.25;
const double turrent_time_int = 1.0;

ZSDL_Surface VJeep::turrent[MAX_ANGLE_TYPES];
ZSDL_Surface VJeep::fire[MAX_ANGLE_TYPES];
ZSDL_Surface VJeep::base[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][2];
ZSDL_Surface VJeep::under[MAX_ANGLE_TYPES][4];
ZSDL_Surface VJeep::wasted;

VJeep::VJeep(ZTime *ztime_, ZSettings *zsettings_) : ZVehicle(ztime_, zsettings_)
{
	double &the_time = ztime->ztime;

	width = 2;
	height = 2;
	width_pix = 32;
	height_pix = 32;
	hover_name = "Jeep";
	object_name = "jeep";

	can_snipe = true;
	can_be_sniped = true;

	InitTypeId(object_type, JEEP);

	//object_id = JEEP;
	//move_speed = JEEP_SPEED;
	//attack_radius = JEEP_ATTACK_RADIUS;
	//max_health = JEEP_MAX_HEALTH;
	//health = max_health;
	//
	//damage = JEEP_DAMAGE;
	//damage_chance = JEEP_DAMAGE_CHANCE;
	//damage_int_time = 0.1;

	renderfire = false;
	
	next_turrent_time = next_base_time = the_time;
	
	next_base_time += base_time_int;
	next_turrent_time += turrent_time_int;
	
	t_direction = direction;
	base_i = rand() % 2;
	move_i = rand() % 4;

	track_type = ET_JEEP;
}

void VJeep::Init()
{
	int i, j, k;
	char filename_c[500];
	
	strcpy(filename_c, "assets/units/vehicles/jeep/wasted.png");
	wasted.LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	
	//empty base
	for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/vehicles/jeep/empty_r%03d.png", ROTATION[j]);
		//base[0][j][0] = base[0][j][1] = IMG_Load_Error(filename_c);
		base[0][j][1].LoadBaseImage(filename_c);
		base[0][j][0] = base[0][j][1].GetBaseSurface();
	}
	
	//normal bases
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
			for(k=0;k<2;k++)
	{
		sprintf(filename_c, "assets/units/vehicles/jeep/base_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
		//base[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, base[ZTEAM_BASE_TEAM][j][k], base[i][j][k], filename_c);
	}
	
	//under stuff
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		for(k=0;k<4;k++)
	{
		//don't try to load angles 90 and 270
		if(j==2 || j==6) 
		{
			//under[j][k] = NULL;
			continue;
		}
		
		sprintf(filename_c, "assets/units/vehicles/jeep/under_r%03d_n%02d.png", ROTATION[j], k);
		under[j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	//turrent
	for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/vehicles/jeep/fire_r%03d_n00.png", ROTATION[j]);
		turrent[j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		
		sprintf(filename_c, "assets/units/vehicles/jeep/fire_r%03d_n01.png", ROTATION[j]);
		fire[j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	//tires
}

// SDL_Surface *VJeep::GetRender()
// {
// 	if(destroyed)
// 	{
// 		return wasted;
// 	}
// 	else
// 	{
// 		if(direction == 2 || direction == 6)
// 			return base[owner][direction][base_i];
// 		else
// 			return under[direction][wheel_i];
// 	}
// }

void VJeep::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	const int turrent_x[8] = {0, 6, 12, 20, 25, 20, 15,  5};
	const int turrent_y[8] = {2, 7, 4,  8,   2, -4, -3, -4};
	const int turrent_shift_x[8] = {0, -2, -5, -8, -10, -8, -5, -2};
	const int turrent_shift_y[8] = {0,  0,  0,  0,   0,   5,   6,   5};
	ZSDL_Surface *base_surface;
	SDL_Rect from_rect, to_rect;
	int lx, ly;

	if(IsDestroyed())
	{
		the_map.RenderZSurface(&wasted, x, y, do_hit_effect);
		//if(the_map.GetBlitInfo(wasted, x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( wasted, &from_rect, dest, &to_rect);
		//}

		return;
	}
	
	if(direction == 2 || direction == 6)
		base_surface = &base[owner][direction][base_i];
	else
		base_surface = &under[direction][move_i];
	
	the_map.RenderZSurface(base_surface, x, y, do_hit_effect);
	//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	ZSDL_BlitHitSurface( base_surface, &from_rect, dest, &to_rect, do_hit_effect);
	//}

	//only render if we have to
	if(direction != 2 && direction != 6)
		the_map.RenderZSurface(&base[owner][direction][base_i], x, y, do_hit_effect);
	//if(the_map.GetBlitInfo(base[owner][direction][base_i], x, y, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	ZSDL_BlitHitSurface(base[owner][direction][base_i], &from_rect, dest, &to_rect, do_hit_effect);
	//}
	
	lx = x + turrent_x[direction] + turrent_shift_x[t_direction];
	ly = y + turrent_y[direction] + turrent_shift_y[t_direction] - base_i;
	
	if(owner != NULL_TEAM)
	{
		if(renderfire)
			base_surface = &fire[t_direction];
		else
			base_surface = &turrent[t_direction];

		the_map.RenderZSurface(base_surface, lx, ly, do_hit_effect);
		//if(the_map.GetBlitInfo(base_surface, lx, ly, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	ZSDL_BlitHitSurface( base_surface, &from_rect, dest, &to_rect, do_hit_effect);
		//}
	}

	do_hit_effect = false;
}

void VJeep::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	//int &x = loc.x;
	//int &y = loc.y;
	//SDL_Rect from_rect, to_rect;
	//int lx, ly;
}

int VJeep::Process()
{
	double &the_time = ztime->ztime;
	int &x = loc.x;
	int &y = loc.y;

	if(moving && the_time >= next_move_time)
	{
		move_i++;
		if(move_i >= 4) move_i = 0;

		next_move_time = the_time + (VEHICLE_MOVE_ANIM_SPEED * SpeedOffsetPercentInv());
	}

	if(the_time >= next_base_time)
	{
		next_base_time = the_time + (base_time_int * SpeedOffsetPercentInv());
		
		base_i = !base_i;
	}
	
	if(attack_object)
	{
		if(the_time >= next_attack_time)
		{
			int tx, ty;
			int new_dir;

			if(renderfire) renderfire = false;
			else renderfire = true;

			next_attack_time = the_time + 0.07 + ((rand() % 100) * 0.03 * 0.01);

			attack_object->GetCenterCords(tx, ty);
			new_dir = DirectionFromLoc(tx - loc.x, ty - loc.y);
			if(new_dir != -1) t_direction = new_dir;

			//now do a bullet effect
			if(renderfire)
			{
				int lx, ly;
				int w,h;
				const int turrent_x[8] = {0, 6, 12, 20, 25, 20, 15,  5};
				const int turrent_y[8] = {2, 7, 4,  8,   2, -4, -3, -4};
				const int turrent_shift_x[8] = {0, -2, -5, -8, -10, -8, -5, -2};
				const int turrent_shift_y[8] = {0,  0,  0,  0,   0,   5,   6,   5};
				const int bullet_shift_x[8] = {17, 14, 7, 0, -3, -3,  7, 15};
				const int bullet_shift_y[8] = {10, 1, -2, 0, 10, 16, 17, 15};

				lx = x + turrent_x[direction] + turrent_shift_x[t_direction] + bullet_shift_x[t_direction];
				ly = y + turrent_y[direction] + turrent_shift_y[t_direction] + bullet_shift_y[t_direction] - base_i;

				attack_object->GetCords(tx, ty);
				attack_object->GetDimensionsPixel(w,h);
				tx += (rand() % w);
				ty += (rand() % h);

				if(effect_list) effect_list->push_back((ZEffect*)(new EBullet(ztime, owner, lx, ly, tx, ty)));
				ZSoundEngine::PlayWavRestricted(JEEP_FIRE_SND, x, y, width_pix, height_pix);
			}
		}

	}
	else
	{
		if(the_time >= next_turrent_time)
		{
			next_turrent_time = the_time + turrent_time_int;

			t_direction++;
			if(t_direction >= MAX_ANGLE_TYPES) t_direction = 0;
		}
	}

	return 1;
}

void VJeep::SetAttackObject(ZObject *obj)
{
	attack_object = obj;

	if(!obj) renderfire = false;
}

void VJeep::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	if(effect_list) effect_list->insert(effect_list->begin(), (ZEffect*)(new EDeath(ztime, loc.x, loc.y, EDEATH_JEEP)));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}
