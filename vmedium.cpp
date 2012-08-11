#include "vmedium.h"

using namespace COMMON;

const double turrent_time_int = 1.0;

ZSDL_Surface VMedium::base[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
ZSDL_Surface VMedium::base_damaged[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
ZSDL_Surface VMedium::top[MAX_ANGLE_TYPES];
ZSDL_Surface VMedium::top_pop[8];

VMedium::VMedium(ZTime *ztime_, ZSettings *zsettings_) : ZVehicle(ztime_, zsettings_)
{
	double &the_time = ztime->ztime;

	width = 2;
	height = 2;
	width_pix = 32;
	height_pix = 32;
	hover_name = "Medium";
	object_name = "medium";

	can_be_sniped = true;

	InitTypeId(object_type, MEDIUM);

	//object_id = MEDIUM;
	//move_speed = MEDIUM_SPEED;
	//attack_radius = MEDIUM_ATTACK_RADIUS;
	//max_health = MEDIUM_MAX_HEALTH;
	//health = max_health;

	//damage = MEDIUM_DAMAGE;
	//damage_int_time = MEDIUM_ATTACK_SPEED;
	//damage_radius = MEDIUM_DAMAGE_RADIUS;
	//missile_speed = MEDIUM_MISSILE_SPEED;

	has_explosives = true;
	damage_is_missile = true;
	has_lid = true;
	
	t_direction = direction;
	move_i = rand() % 3;
	
	next_turrent_time = the_time;
	next_turrent_time += turrent_time_int;
}

void VMedium::Init()
{
	int i, j, k;
	char filename_c[500];
	SDL_Surface *temp_surface;
	
	strcpy(filename_c, "assets/units/vehicles/medium/empty_null.png");
	temp_surface = IMG_Load_Error(filename_c);
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		for(k=0;k<3;k++)
			base_damaged[0][j][k] = base[0][j][k] = temp_surface;

	ZSDL_FreeSurface(temp_surface);
	
	//bases
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<4;j++)
	{
		//a kind of fluke in bitmaps design vs ours
		if(j==3) j = 7;
		
		for(k=0;k<3;k++)
		{
			sprintf(filename_c, "assets/units/vehicles/medium/base_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
			//base[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, base[ZTEAM_BASE_TEAM][j][k], base[i][j][k], filename_c);
			if(j!=7)
				base[i][j+4][2-k] = base[i][j][k].GetBaseSurface();
			else
				base[i][j-4][2-k] = base[i][j][k].GetBaseSurface();
			
			sprintf(filename_c, "assets/units/vehicles/medium/base_damaged_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
			//base_damaged[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, base_damaged[ZTEAM_BASE_TEAM][j][k], base_damaged[i][j][k], filename_c);
			if(j!=7)
				base_damaged[i][j+4][2-k] = base_damaged[i][j][k].GetBaseSurface();
			else
				base_damaged[i][j-4][2-k] = base_damaged[i][j][k].GetBaseSurface();
		}
	}
	
	//top
	for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/vehicles/medium/topf_r%03d.png", ROTATION[j]);
		top[j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(k=0;k<8;k++)
	{
		sprintf(filename_c, "assets/units/vehicles/medium/top_pop_n%02d.png", k);
		top_pop[k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
}
		
// SDL_Surface *VMedium::GetRender()
// {
// 	return base[owner][direction][base_i];
// }

int VMedium::Process()
{
	double &the_time = ztime->ztime;
	int tx, ty;
	int new_dir;

	ProcessLid();

	if(moving && the_time >= next_move_time)
	{
		move_i--;
		if(move_i < 0) move_i = 2;

		next_move_time = the_time + (VEHICLE_MOVE_ANIM_SPEED * SpeedOffsetPercentInv());
	}

	if(the_time >= next_turrent_time)
	{
		if(attack_object)
		{
			attack_object->GetCenterCords(tx, ty);
			new_dir = DirectionFromLoc((float)(tx - loc.x), (float)(ty - loc.y));
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

void VMedium::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	//const int unit_x[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	const int unit_y[8] = {6, 0, 5, 0, 6, 0, 5, 0};
	const int turrent_x[8] = {0, 0, -1, -2, 0, 0, -1, -2};
	const int turrent_y[8] = {0, 6, 0, 6, 0, 6, 0, 6};
	const int turrent_shift_x[8] = { 4,  5,  7,  5,  2,  6,  7,  5};
	const int turrent_shift_y[8] = {-5, -3, -4, -5, -5, -5, -5, -5};
	const int lid_shift_x = 12;
	const int lid_shift_y = -5;
	ZSDL_Surface *base_surface;
//	SDL_Rect from_rect, to_rect;
	int lx, ly;

	if(IsDestroyed())
	{
		the_map.RenderZSurface(&base_damaged[owner][direction][move_i], x, y);
		//if(the_map.GetBlitInfo(base_damaged[owner][direction][move_i], x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( base_damaged[owner][direction][move_i], &from_rect, dest, &to_rect);
		//}

		return;
	}
	
	if(!ShowDamaged())
		base_surface = &base[owner][direction][move_i];
	else
		base_surface = &base_damaged[owner][direction][move_i];
	
	lx = x;// + unit_x[direction];
	ly = y + unit_y[direction];

	the_map.RenderZSurface(base_surface, lx, ly, do_hit_effect);
	//if(the_map.GetBlitInfo(base_surface, lx, ly, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	ZSDL_BlitHitSurface( base_surface, &from_rect, dest, &to_rect, do_hit_effect);
	//}

	if(owner != NULL_TEAM)
	{
		lx = x + turrent_x[direction] + turrent_shift_x[t_direction];// + unit_x[direction];
		ly = y + turrent_y[direction] + turrent_shift_y[t_direction] + unit_y[direction];

		the_map.RenderZSurface(&top[t_direction], lx, ly, do_hit_effect);
		//if(the_map.GetBlitInfo(top[t_direction], lx, ly, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;
	
		//	ZSDL_BlitHitSurface(top[t_direction], &from_rect, dest, &to_rect, do_hit_effect);
		//}
		
		lx = x + turrent_x[direction] + lid_shift_x;// + unit_x[direction];
		ly = y + turrent_y[direction] + lid_shift_y + unit_y[direction];
		
		RenderLid(the_map, dest, lx, ly, shift_x, shift_y);
	}

	do_hit_effect = false;
}

void VMedium::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	//int &x = loc.x;
	//int &y = loc.y;
	//SDL_Rect from_rect, to_rect;
	//int lx, ly;
}

void VMedium::FireMissile(int x_, int y_)
{
	int &x = loc.x;
	int &y = loc.y;

	const int sx[8] = {20, 12, 0, -12, -20, -12, 0, 12};
	const int sy[8] = {0, -12, -20, -12, 0, 12, 20, 12};

	//effect_list->push_back((ZEffect*)(new ELightRocket(x+17+sx[t_direction], y+14+sy[t_direction], x_, y_, MEDIUM_MISSILE_SPEED, 0, 1)));
	if(effect_list) effect_list->push_back((ZEffect*)(new ELightRocket(ztime, x+17+sx[t_direction], y+14+sy[t_direction], x_, y_, missile_speed, 0, 1)));
	ZSoundEngine::PlayWavRestricted(MEDIUM_FIRE_SND, loc.x, loc.y, width_pix, height_pix);
}

void VMedium::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	if(owner == NULL_TEAM) return;

	if(effect_list) effect_list->insert(effect_list->begin(), (ZEffect*)(new EDeath(ztime, loc.x, loc.y, EDEATH_TANK, &base_damaged[owner][direction][move_i])));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}

//bool VMedium::ServerFireTurrentMissile(int &x_, int &y_, int &damage, int &radius, double &offset_time)
//{
//	x_ = (loc.x + 16) + (30 - (rand() % 60));
//	y_ = (loc.y + 16) + (30 - (rand() % 60));
//	damage = 40;
//	radius = 40;
//	offset_time = 3 + (0.01 * (rand() % 100));
//	return true;
//}

vector<fire_missile_info> VMedium::ServerFireTurrentMissile(int &damage, int &radius)
{
	vector<fire_missile_info> ret;
	fire_missile_info a_missile;
	int &max_horz = zsettings->max_turrent_horizontal_distance;
	int &max_vert = zsettings->max_turrent_vertical_distance;

	a_missile.missile_x = (loc.x + 16) + (max_horz - (rand() % (max_horz + max_horz)));
	a_missile.missile_y = (loc.y + 16) + (max_vert - (rand() % (max_vert + max_vert)));
	a_missile.missile_offset_time = 3 + (0.01 * (rand() % 100));

	damage = 40;
	radius = 40;

	ret.push_back(a_missile);

	return ret;
}

void VMedium::FireTurrentMissile(int x_, int y_, double offset_time)
{
	if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, loc.x + 8, loc.y + 8, x_, y_, offset_time, ETURRENTMISSILE_MEDIUM)));
}
