#include "bradar.h"

ZSDL_Surface BRadar::base[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
ZSDL_Surface BRadar::base_destroyed[MAX_PLANET_TYPES];
ZSDL_Surface BRadar::base_color[MAX_TEAM_TYPES];

ZSDL_Surface BRadar::box_spinner[12];
ZSDL_Surface BRadar::dish[8];
ZSDL_Surface BRadar::front_light[2];
ZSDL_Surface BRadar::side_light[2];

BRadar::BRadar(ZTime *ztime_, ZSettings *zsettings_, planet_type palette_) : ZBuilding(ztime_, zsettings_, palette_)
{
	width = 4;
	height = 3;
	width_pix = width * 16;
	height_pix = height * 16;
	object_name = "radar";
	object_id = RADAR;
	palette = palette_;

	InitTypeId(object_type, object_id);

	//max_health = RADAR_BUILDING_HEALTH;
	//health = max_health;
	
	box_spinner_i = 0;
	front_light_i = 0;
	side_light_i = 0;
	dish_i = 0;

	//fire effects
	effects_box.x = 1;
	effects_box.y = 6;
	effects_box.w = 44;
	effects_box.h = 30;
	max_effects = 6 + (rand() % 3);
}

// SDL_Surface *BRadar::GetRender()
// {
// 	if(destroyed)
// 	{
// 		return base_destroyed[palette];
// 	}
// 	else
// 	{
// 		return base[palette][owner];
// 	}
// }

void BRadar::Init()
{
	int i, j;
	string filename;
	char filename_c[500];
	
	//load colors
	//base_color[0] = NULL;
	for(j=1;j<MAX_TEAM_TYPES;j++)
	{
		filename = "assets/buildings/radar/" + team_type_string[j] + ".png";
		//base_color[j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		ZTeam::LoadZSurface(j, base_color[ZTEAM_BASE_TEAM], base_color[j], filename);
	}
	
	//load images pertaining to a fort
	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/buildings/radar/base_" + planet_type_string[i] + ".png";
		base[i][0].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		filename = "assets/buildings/radar/base_destroyed_" + planet_type_string[i] + ".png";
		base_destroyed[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		//make the other colors
		for(j=1;j<MAX_TEAM_TYPES;j++)
		{
			SDL_Rect dest;
			
			//base[i][j] = CopyImage(base[i][0]);
			base[i][j] = base[i][0].GetBaseSurface();
			
			dest.x = 0;
			dest.y = 32;
			
			//SDL_BlitSurface(base_color[j], NULL, base[i][j], &dest);
			base_color[j].BlitSurface(NULL, &dest, &base[i][j]);
		}
	}
	
	//now load effect images
	for(i=0;i<12;i++)
	{
		sprintf(filename_c, "assets/buildings/radar/box_spinner_%d.png", i);
		box_spinner[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(i=0;i<8;i++)
	{
		sprintf(filename_c, "assets/buildings/radar/dish_%d.png", i);
		dish[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(i=0;i<2;i++)
	{
		sprintf(filename_c, "assets/buildings/radar/front_light_%d.png", i);
		front_light[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		
		sprintf(filename_c, "assets/buildings/radar/side_light_%d.png", i);
		side_light[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
}

int BRadar::Process()
{
	double &the_time = ztime->ztime;
	const double min_interval_time = 0.25;
	
	ProcessBuildingsEffects(the_time);

	if(the_time - last_process_time >= min_interval_time)
	{
		last_process_time = the_time;
		
		front_light_i++;
		if(front_light_i >= 2) front_light_i = 0;
		
		side_light_i++;
		if(side_light_i >= 2) side_light_i = 0;
		
		box_spinner_i++;
		if(box_spinner_i >= 12) box_spinner_i = 0;
		
		dish_i++;
		if(dish_i >= 8) dish_i = 0;
	}

	return 1;
}

void BRadar::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	if(!dont_stamp)
	{
		if(do_base_rerender)
		{
			if(IsDestroyed())
			{
				base_surface = &base_destroyed[palette];
			}
			else
			{
				base_surface = &base[palette][owner];
			}

			if(the_map.PermStamp(x, y, base_surface))
				do_base_rerender = false;
		}
	}
	else
	{
		if(IsDestroyed())
		{
			base_surface = &base_destroyed[palette];
		}
		else
		{
			base_surface = &base[palette][owner];
		}
		
		the_map.RenderZSurface(base_surface, x, y);
		//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
		//}
	}

}

void BRadar::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	const int front_light_x = 16;
	const int front_light_y = 22;
	const int side_light_x = 41;
	const int side_light_y = 0;
	const int box_spinner_x = 18;
	const int box_spinner_y = 13;
	const int dish_x = 15;
	const int dish_y[8] = {-5, -6, -10, -13, -15, -13, -10, -6};
	
	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	if(!IsDestroyed())
	{
		if(owner != NULL_TEAM)
		{
			lx = x + front_light_x;
			ly = y + front_light_y;

			the_map.RenderZSurface(&front_light[front_light_i], lx, ly);
			//if(the_map.GetBlitInfo(front_light[front_light_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( front_light[front_light_i], &from_rect, dest, &to_rect);
			//}
			
			lx = x + side_light_x;
			ly = y + side_light_y;
			
			the_map.RenderZSurface(&side_light[side_light_i], lx, ly);
			//if(the_map.GetBlitInfo(side_light[side_light_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( side_light[side_light_i], &from_rect, dest, &to_rect);
			//}
			
			lx = x + box_spinner_x;
			ly = y + box_spinner_y;
			
			the_map.RenderZSurface(&box_spinner[box_spinner_i], lx, ly);
			//if(the_map.GetBlitInfo(box_spinner[box_spinner_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( box_spinner[box_spinner_i], &from_rect, dest, &to_rect);
			//}
			
			lx = x + dish_x;
			ly = y + dish_y[dish_i];
			
			the_map.RenderZSurface(&dish[dish_i], lx, ly);
			//if(the_map.GetBlitInfo(dish[dish_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( dish[dish_i], &from_rect, dest, &to_rect);
			//}
		}
		else
		{
			lx = x + front_light_x;
			ly = y + front_light_y;

			the_map.RenderZSurface(&front_light[front_light_i], lx, ly);
			//if(the_map.GetBlitInfo(front_light[front_light_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( front_light[front_light_i], &from_rect, dest, &to_rect);
			//}
		}
	}
	else
	{
		//IsDestroyed()
		const int x_plus = 28 - 16;
		const int y_plus = 24 - 24;

		if(the_map.GetBlitInfo(x + x_plus, y + y_plus, 32, 32 - (y_plus), from_rect, to_rect))
		{
			from_rect.x += x_plus;
			from_rect.y += y_plus;

			to_rect.x += shift_x;
			to_rect.y += shift_y;

			base_destroyed[palette].BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface( base_destroyed[palette], &from_rect, dest, &to_rect);
		}
	}

	//render effects
	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->DoRender(the_map, dest);
}

void BRadar::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	int sx, sy;
	int ex, ey;
	double offset_time;
	int peices;
	int fireballs;
	int i;

	//fireballs
	{
		fireballs = 4 + (rand() % 3);

		for(i=0;i<fireballs;i++)
		{
			sx = loc.x + effects_box.x + (rand() % effects_box.w);
			sy = loc.y + effects_box.y + (rand() % effects_box.h);

			if(effect_list) effect_list->push_back((ZEffect*)(new ESideExplosion(ztime, sx, sy, 1.3)));
		}
	}

	peices = 3 + (rand() % 3);

	for(i=0;i<peices;i++)
	{
		sx = loc.x + effects_box.x + (rand() % effects_box.w);
		sy = loc.y + effects_box.y + (rand() % effects_box.h);
		ex = loc.x + (width_pix >> 1) + (200 - (rand() % 400));
		ey = loc.y + (height_pix >> 1) + (200 - (rand() % 400));

		offset_time = 1.5 + (0.01 * (rand() % 200));

		switch(rand()%2)
		{
		case 0:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_BUILDING_PEICE0)));
			break;
		case 1:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_BUILDING_PEICE1)));
			break;
		}
	}

	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);

	do_base_rerender = true;
}

void BRadar::SetMapImpassables(ZMap &tmap)
{
	int tx, ty, ex, ey;
	int i, j;

	tx = loc.x / 16;
	ty = loc.y / 16;
	ex = tx + width;
	ey = ty + height;

	for(i=tx;i<ex;i++)
		for(j=ty;j<ey;j++)
			tmap.SetImpassable(i,j);

	//unset
	tmap.SetImpassable(tx+3,ty+2, false);
}

bool BRadar::GetCraneEntrance(int &x, int &y, int &x2, int &y2)
{
	x = x2 = loc.x + 28;
	y = y2 = loc.y + height_pix + 32;
	return true;
}

bool BRadar::GetCraneCenter(int &x, int &y)
{
	x = loc.x + 28;
	y = loc.y + 24;
	return true;
}
