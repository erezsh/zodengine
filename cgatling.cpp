#include "cgatling.h"

ZSDL_Surface CGatling::passive[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface CGatling::fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
ZSDL_Surface CGatling::place[MAX_TEAM_TYPES][4];
ZSDL_Surface CGatling::wasted;

CGatling::CGatling(ZTime *ztime_, ZSettings *zsettings_, bool just_placed) : ZCannon(ztime_, zsettings_)
{
	hover_name = "Gatling";
	object_name = "gatling";

	can_snipe = true;

	InitTypeId(object_type, GATLING);

	//object_id = GATLING;
	//attack_radius = GATLING_ATTACK_RADIUS;
	//max_health = GATLING_MAX_HEALTH;
	//health = max_health;
	//
	//damage = GATLING_DAMAGE;
	//damage_chance = GATLING_DAMAGE_CHANCE;
	//damage_int_time = 0.1;

	renderfire = false;
	
	mode = ROTATING_MODE;
	
	place_i = 0;
	if(just_placed) mode = JUST_PLACED_MODE;
}

SDL_Surface *CGatling::GetPlaceImage(int team_)
{
	return passive[team_][4].GetBaseSurface();
}

SDL_Surface *CGatling::GetNPlaceImage(int team_)
{
	return passive[team_][4].GetBaseSurface();
}

void CGatling::Init()
{
	int i, j;
	char filename_c[500];
	SDL_Surface *temp_surface;
	
	strcpy(filename_c, "assets/units/cannons/gatling/wasted.png");
	wasted.LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/cannons/gatling/empty_r%03d.png", ROTATION[j]);
		temp_surface = IMG_Load(filename_c);
		fire[0][j].LoadBaseImage(temp_surface, false);
		passive[0][j].LoadBaseImage(temp_surface, false);
		//fire[0][j] = passive[0][j] = IMG_Load_Error(filename_c);
	}
	
	for(j=0;j<4;j++)
	{
		//place[0][j] = passive[0][4];
		place[0][j].LoadBaseImage(passive[0][4].GetBaseSurface(), false);
	}
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<4;j++)
	{
		sprintf(filename_c, "assets/units/cannons/gatling/place_%s_n%02d.png", team_type_string[i].c_str(), j);
		//place[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, place[ZTEAM_BASE_TEAM][j], place[i][j], filename_c);
	}
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/cannons/gatling/fire_%s_r%03d_n00.png", team_type_string[i].c_str(), ROTATION[j]);
		//passive[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, passive[ZTEAM_BASE_TEAM][j], passive[i][j], filename_c);
		
		sprintf(filename_c, "assets/units/cannons/gatling/fire_%s_r%03d_n01.png", team_type_string[i].c_str(), ROTATION[j]);
		//fire[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, fire[ZTEAM_BASE_TEAM][j], fire[i][j], filename_c);
	}
}

void CGatling::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	const int unit_x = 0;
	const int unit_y = -7;
	const int base_x[8] = {1, 0, 0, 0, -1, 0, 0, 0};
	//const int base_y[8] = {0, 0, 0, 0, 0, 3, 3, 3};
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	if(IsDestroyed())
	{
		base_surface = &wasted;
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
				if(renderfire)
					base_surface = &fire[owner][direction];
				else
					base_surface = &passive[owner][direction];
				break;
		}
	}

	lx = x + unit_x + base_x[direction];
	ly = y + unit_y;
	
	the_map.RenderZSurface(base_surface, lx, ly, do_hit_effect);
	//if(the_map.GetBlitInfo(base_surface->GetBaseSurface(), lx, ly, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	base_surface->BlitHitSurface(&from_rect, &to_rect, NULL, do_hit_effect);
	//	//ZSDL_BlitHitSurface( base_surface, &from_rect, dest, &to_rect, do_hit_effect);
	//}

	do_hit_effect = false;
}

// SDL_Surface *CGatling::GetRender()
// {
// 	if(destroyed)
// 	{
// 		return wasted;
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

int CGatling::Process()
{
	double &the_time = ztime->ztime;

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
			//are we really attacking though?
			if(attack_object)
			{
				int tx, ty;
				int new_dir;

				if(the_time < next_attack_time) break;

				if(renderfire) renderfire = false;
				else renderfire = true;

				next_attack_time = the_time + 0.07 + ((rand() % 100) * 0.03 * 0.01);

				attack_object->GetCenterCords(tx, ty);
				new_dir = DirectionFromLoc(tx - loc.x, ty - loc.y);
				if(new_dir != -1) direction = new_dir;

				//now do a bullet effect
				if(renderfire)
				{
					const int unit_x = 0;
					const int unit_y = -7;
					const int bullet_x[8] = {18,  13,   0, -13, -18, -16, -1, 13};
					const int bullet_y[8] = {-3, -16, -18, -16,  -3,  10, 13, 10};
					int lx, ly;
					int w, h;

					attack_object->GetCords(tx, ty);
					attack_object->GetDimensionsPixel(w,h);

					//lx = loc.x + (width_pix / 2);
					//ly = loc.y + (height_pix / 2);

					tx += (rand() % w);
					ty += (rand() % h);

					lx = unit_x + center_x + bullet_x[direction];
					ly = unit_y + center_y + bullet_y[direction];

					//effect_list->push_back((ZEffect*)(new EBullet(owner, lx, ly, tx, ty)));
					if(effect_list) effect_list->push_back((ZEffect*)(new EBullet(ztime, owner, lx, ly, tx, ty)));
					ZSoundEngine::PlayWavRestricted(GATLING_FIRE_SND, loc.x, loc.y, width_pix, height_pix);
				}

				break;
			}

			if(the_time - last_process_time < 1.0) break;
			if(owner == NULL_TEAM) break;
			last_process_time = the_time;
			
			direction++;
			if(direction >= MAX_ANGLE_TYPES) direction = 0;
			
			break;
		case C_ATTACKING:

			break;
	}

	return 1;
}

void CGatling::SetAttackObject(ZObject *obj)
{
	attack_object = obj;

	if(!obj) renderfire = false;
	else
	{
		int tx, ty;
		int new_dir;

		attack_object->GetCenterCords(tx, ty);
		new_dir = DirectionFromLoc(tx - loc.x, ty - loc.y);
		if(new_dir != -1) direction = new_dir;
	}
}

void CGatling::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	if(owner == NULL_TEAM) return;

	//effect_list->insert(effect_list->begin(), (ZEffect*)(new ECannonDeath(loc.x, loc.y, ECANNONDEATH_GATLING)));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}

//bool CGatling::ServerFireTurrentMissile(int &x_, int &y_, int &damage, int &radius, double &offset_time)
//{
//	x_ = (loc.x + 16) + (30 - (rand() % 60));
//	y_ = (loc.y + 16) + (30 - (rand() % 60));
//	damage = 40;
//	radius = 40;
//	offset_time = 7 + (0.01 * (rand() % 300));
//	return true;
//}

vector<fire_missile_info> CGatling::ServerFireTurrentMissile(int &damage, int &radius)
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

void CGatling::FireTurrentMissile(int x_, int y_, double offset_time)
{
	if(effect_list) effect_list->insert(effect_list->begin(), (ZEffect*)(new ECannonDeath(ztime, loc.x, loc.y, ECANNONDEATH_GATLING, x_, y_, offset_time)));
	//effect_list->push_back((ZEffect*)(new ETurrentMissile(loc.x + 8, loc.y + 8, x_, y_, offset_time, ETURRENTMISSILE_LIGHT)));
}
