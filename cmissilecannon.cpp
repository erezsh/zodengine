#include "cmissilecannon.h"
#include "emissilecrockets.h"

ZSDL_Surface CMissileCannon::empty[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface CMissileCannon::passive[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface CMissileCannon::fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface CMissileCannon::place[MAX_TEAM_TYPES][4];
ZSDL_Surface CMissileCannon::wasted[MAX_TEAM_TYPES];

CMissileCannon::CMissileCannon(ZTime *ztime_, ZSettings *zsettings_, bool just_placed) : ZCannon(ztime_, zsettings_)
{
	hover_name = "Missile";
	object_name = "missilecannon";

	InitTypeId(object_type, MISSILE_CANNON);

	//object_id = MISSILE_CANNON;
	//attack_radius = MISSILE_CANNON_ATTACK_RADIUS;
	//max_health = MISSILE_CANNON_MAX_HEALTH;
	//health = max_health;

	//damage = MISSILE_CANNON_DAMAGE;
	//damage_int_time = MISSILE_CANNON_ATTACK_SPEED;
	//damage_radius = MISSILE_CANNON_DAMAGE_RADIUS;
	//missile_speed = MISSILE_CANNON_MISSILE_SPEED;

	render_fire = false;
	has_explosives = true;
	damage_is_missile = true;
	
	mode = ROTATING_MODE;
	
	place_i = 0;
	if(just_placed) mode = JUST_PLACED_MODE;
}

SDL_Surface *CMissileCannon::GetPlaceImage(int team_)
{
	return passive[team_][4].GetBaseSurface();
}

SDL_Surface *CMissileCannon::GetNPlaceImage(int team_)
{
	return passive[team_][4].GetBaseSurface();
}

void CMissileCannon::Init()
{
	int i, j;
	char filename_c[500];
	SDL_Surface *temp_surface;
	
	//wasted[0] = NULL;
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
	{
		sprintf(filename_c, "assets/units/cannons/missile_cannon/wasted_%s.png", team_type_string[i].c_str());
		//wasted[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, wasted[ZTEAM_BASE_TEAM], wasted[i], filename_c);
	}
	
	strcpy(filename_c, "assets/units/cannons/missile_cannon/empty_null.png");
	temp_surface = IMG_Load_Error(filename_c);
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		empty[0][j] = fire[0][j] = passive[0][j] = temp_surface;
	
	for(j=0;j<4;j++)
	{
		place[0][j] = passive[0][4];
	}
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<4;j++)
	{
		sprintf(filename_c, "assets/units/cannons/missile_cannon/place_%s_n%02d.png", team_type_string[i].c_str(), j);
		//place[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, place[ZTEAM_BASE_TEAM][j], place[i][j], filename_c);
	}
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/cannons/missile_cannon/equiped_%s_r%03d.png", team_type_string[i].c_str(), ROTATION[j]);
		ZTeam::LoadZSurface(i, passive[ZTEAM_BASE_TEAM][j], passive[i][j], filename_c);
		fire[i][j] = passive[i][j];
		//fire[i][j] = passive[i][j] = IMG_Load_Error(filename_c);
		
		sprintf(filename_c, "assets/units/cannons/missile_cannon/empty_%s_r%03d.png", team_type_string[i].c_str(), ROTATION[j]);
		//empty[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, empty[ZTEAM_BASE_TEAM][j], empty[i][j], filename_c);
	}

}

void CMissileCannon::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	const int unit_x = 0;
	const int unit_y = -8;
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
//	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	if(destroyed)
	{
		base_surface = &wasted[owner];
	}
	else
	{
		switch(mode)
		{
			case JUST_PLACED_MODE:
				if(place_i<3)
					base_surface = &init_place[place_i];
				else
					base_surface = &place[owner][place_i-3];
				break;
			default:
				if(render_fire)
					base_surface = &fire[owner][direction];
				else
					base_surface = &passive[owner][direction];
				break;
		}
	}

	lx = x + unit_x;
	ly = y + unit_y;
	
	the_map.RenderZSurface(base_surface, lx, ly, do_hit_effect);
	//if(the_map.GetBlitInfo(base_surface->GetBaseSurface(), x + unit_x, y + unit_y, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	base_surface->BlitHitSurface(&from_rect, &to_rect, NULL, do_hit_effect);
	//	//ZSDL_BlitHitSurface( base_surface, &from_rect, dest, &to_rect, do_hit_effect);
	//}

	do_hit_effect = false;
}

// SDL_Surface *CMissileCannon::GetRender()
// {
// 	if(destroyed)
// 	{
// 		return wasted[owner];
// 	}
// 	else
// 	{
// 		switch(mode)
// 		{
// 			case JUST_PLACED_MODE:
// 				if(place_i<3)
// 					return init_place[place_i];
// 				else
// 					return place[owner][place_i-3];
// 				break;
// 			default:
// 				return passive[owner][direction];
// 				break;
// 		}
// 	}
// }

int CMissileCannon::Process()
{
	double &the_time = ztime->ztime;

	if(render_fire && the_time >= end_render_fire_time)
		render_fire = false;

	switch(mode)
	{
		case JUST_PLACED_MODE:
			if(the_time - last_process_time < 0.1) break;
			last_process_time = the_time;
			
			place_i++;
			if(place_i >= 7)
			{
				//we are done with this mode, go to the next
				place_i = 0;
				mode = ROTATING_MODE;
			}
			break;
		case ROTATING_MODE:
			if(the_time - last_process_time < 1.0) break;
			if(owner == NULL_TEAM) break;
			last_process_time = the_time;
			
			if(attack_object)
			{
				int tx, ty;
				int new_dir;

				attack_object->GetCenterCords(tx, ty);
				new_dir = DirectionFromLoc((float)(tx - loc.x), (float)(ty - loc.y));
				if(new_dir != -1) direction = new_dir;
			}
			else
			{
				direction++;
				if(direction >= MAX_ANGLE_TYPES) direction = 0;
			}
			
			break;
	}

	return 1;
}

void CMissileCannon::FireMissile(int x_, int y_)
{
	double &the_time = ztime->ztime;
	const int sx[8] = {20, 12, 0, -12, -20, -12, 0, 12};
	const int sy[8] = {0, -12, -20, -12, 0, 12, 20, 12};

	int &x = loc.x;
	int &y = loc.y;

	end_render_fire_time = the_time + 0.05 + ((rand() % 100) * 0.03 * 0.01);
	if(effect_list) effect_list->push_back((ZEffect*)(new EMissileCRockets(ztime, x+17+sx[direction], y+14+sy[direction], x_, y_)));
	ZSoundEngine::PlayWavRestricted(MOMISSILE_FIRE_SND, loc.x, loc.y, width_pix, height_pix);
}

void CMissileCannon::SetAttackObject(ZObject *obj)
{
	attack_object = obj;

	if(!obj) render_fire = false;
	else
	{
		int tx, ty;
		int new_dir;

		attack_object->GetCenterCords(tx, ty);
		new_dir = DirectionFromLoc((float)(tx - loc.x), (float)(ty - loc.y));
		if(new_dir != -1) direction = new_dir;
	}
}

void CMissileCannon::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	if(owner == NULL_TEAM) return;

	//effect_list->insert(effect_list->begin(), (ZEffect*)(new ECannonDeath(loc.x, loc.y, ECANNONDEATH_MISSILE)));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}

//bool CMissileCannon::ServerFireTurrentMissile(int &x_, int &y_, int &damage, int &radius, double &offset_time)
//{
//	x_ = (loc.x + 16) + (30 - (rand() % 60));
//	y_ = (loc.y + 16) + (30 - (rand() % 60));
//	damage = 40;
//	radius = 40;
//	offset_time = 7 + (0.01 * (rand() % 300));
//	return true;
//}

vector<fire_missile_info> CMissileCannon::ServerFireTurrentMissile(int &damage, int &radius)
{
	vector<fire_missile_info> ret;
	fire_missile_info a_missile;
	int &max_horz = zsettings->max_turrent_horizontal_distance;
	int &max_vert = zsettings->max_turrent_vertical_distance;

	a_missile.missile_x = (loc.x + 16) + (max_horz - (rand() % (max_horz + max_horz)));
	a_missile.missile_y = (loc.y + 16) + (max_vert - (rand() % (max_vert + max_vert)));
	a_missile.missile_offset_time = 7 + (0.01 * (rand() % 300));

	damage = 40;
	radius = 40;

	ret.push_back(a_missile);

	return ret;
}

void CMissileCannon::FireTurrentMissile(int x_, int y_, double offset_time)
{
	if(effect_list) effect_list->insert(effect_list->begin(), (ZEffect*)(new ECannonDeath(ztime, loc.x, loc.y, ECANNONDEATH_MISSILE, x_, y_, offset_time)));
	//effect_list->push_back((ZEffect*)(new ETurrentMissile(loc.x + 8, loc.y + 8, x_, y_, offset_time, ETURRENTMISSILE_LIGHT)));
}
