#include "brobot.h"
#include "gwproduction.h"

ZSDL_Surface BRobot::base[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
ZSDL_Surface BRobot::base_destroyed[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
ZSDL_Surface BRobot::base_color[MAX_TEAM_TYPES];
ZSDL_Surface BRobot::base_color_destroyed[MAX_TEAM_TYPES];

ZSDL_Surface BRobot::spin[8];
ZSDL_Surface BRobot::green_box[6];
ZSDL_Surface BRobot::robot[2];
ZSDL_Surface BRobot::single_light[2];
ZSDL_Surface BRobot::double_light[2];

BRobot::BRobot(ZTime *ztime_, ZSettings *zsettings_, planet_type palette_) : ZBuilding(ztime_, zsettings_, palette_)
{
	width = 4;
	height = 5;
	width_pix = width * 16;
	height_pix = height * 16;
	object_name = "robot_factory";
	object_id = ROBOT_FACTORY;
	palette = palette_;

	InitTypeId(object_type, object_id);

	//max_health = ROBOT_BUILDING_HEALTH;
	//health = max_health;
	
	spin_i = 0;
	green_box_i = 0;
	robot_i = 0;
	double_light_i = 0;
	exhaust_i = 0;

	unit_create_x = 43;
	unit_create_y = 53;
	unit_move_x = unit_create_x;
	unit_move_y = 80 + 16;

	//fire effects
	effects_box.x = 8;
	effects_box.y = 8;
	effects_box.w = width_pix - (16 + 8);
	effects_box.h = height_pix - (16 + 8);
	max_effects = 8 + (rand() % 4);
}

// SDL_Surface *BRobot::GetRender()
// {
// 	if(destroyed)
// 	{
// 		return base_destroyed[palette][owner];
// 	}
// 	else
// 	{
// 		return base[palette][owner];
// 	}
// }

void BRobot::Init()
{
	int i, j;
	string filename;
	char filename_c[500];
	
	//load colors
	//base_color[0] = NULL;
	for(j=1;j<MAX_TEAM_TYPES;j++)
	{
		filename = "assets/buildings/robot/" + team_type_string[j] + ".png";
		//base_color[j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		ZTeam::LoadZSurface(j, base_color[ZTEAM_BASE_TEAM], base_color[j], filename);
		
		filename = "assets/buildings/robot/" + team_type_string[j] + "_destroyed.png";
		//base_color_destroyed[j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		ZTeam::LoadZSurface(j, base_color_destroyed[ZTEAM_BASE_TEAM], base_color_destroyed[j], filename);
	}
	
	//load images pertaining to a fort
	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/buildings/robot/base_" + planet_type_string[i] + ".png";
		base[i][0].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		filename = "assets/buildings/robot/base_destroyed_" + planet_type_string[i] + ".png";
		base_destroyed[i][0].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		//make the other colors
		for(j=1;j<MAX_TEAM_TYPES;j++)
		{
			SDL_Rect dest;
			
			//base[i][j] = CopyImage(base[i][0]);
			//base_destroyed[i][j] = CopyImage(base_destroyed[i][0]);
			base[i][j] = base[i][0].GetBaseSurface();
			base_destroyed[i][j] = base_destroyed[i][0].GetBaseSurface();
			
			dest.x = 16;
			dest.y = 64;
			
			//SDL_BlitSurface(base_color[j], NULL, base[i][j], &dest);
			//SDL_BlitSurface(base_color_destroyed[j], NULL, base_destroyed[i][j], &dest);
			base_color[j].BlitSurface(NULL, &dest, &base[i][j]);
			base_color_destroyed[j].BlitSurface(NULL, &dest, &base_destroyed[i][j]);
		}
	}
	
	//load effects
	for(i=0;i<8;i++)
	{
		sprintf(filename_c, "assets/buildings/robot/spin_%d.png", i);
		spin[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(i=0;i<6;i++)
	{
		sprintf(filename_c, "assets/buildings/robot/green_box_%d.png", i);
		green_box[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(i=0;i<2;i++)
	{
		sprintf(filename_c, "assets/buildings/robot/light_%d.png", i);
		single_light[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		
		sprintf(filename_c, "assets/buildings/robot/double_light_%d.png", i);
		double_light[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		
		sprintf(filename_c, "assets/buildings/robot/robot_%d.png", i);
		robot[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
}

int BRobot::Process()
{
	double &the_time = ztime->ztime;
	const double min_interval_time = 0.25;

	ProcessBuildingsEffects(the_time);
	
	if(the_time - last_process_time >= min_interval_time)
	{
		last_process_time = the_time;
		
		spin_i++;
		if(spin_i >= 8) spin_i = 0;
		
		green_box_i++;
		if(green_box_i >= 6) green_box_i = 0;
		
		robot_i++;
		if(robot_i >= 2) robot_i = 0;
		
		//double_light_i++;
		//if(double_light_i >= 2) double_light_i = 0;
		
		exhaust_i++;
		if(exhaust_i >= 13) exhaust_i = 0;
		
		if(!(rand() % 3))
		for(int i=0;i<3;i++)
			single_light_on[i] = rand() % 2;
	}

	if(build_state != BUILDING_SELECT)
		ResetShowTime((int)ProductionTimeLeft(the_time));
	else
		ResetShowTime(-1);

	return 1;
}

void BRobot::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
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
				base_surface = &base_destroyed[palette][owner];
			else
				base_surface = &base[palette][owner];
			
			if(the_map.PermStamp(x, y, base_surface))
				do_base_rerender = false;
		}
	}
	else
	{
		if(IsDestroyed())
			base_surface = &base_destroyed[palette][owner];
		else
			base_surface = &base[palette][owner];

		the_map.RenderZSurface(base_surface, x, y);
		//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
		//}
	}
}

void BRobot::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	const int spin_x = 9;
	const int spin_y = -2;
	const int robot_x = 16;
	const int robot_y = 48;
	const int double_light_x = 16;
	const int double_light_y = 32;
	const int single_light_x[3] = {13,16,19};
	const int single_light_y = 68;
	const int level_x = 8;
	const int level_y = 56;
	const int green_box_x = 38;
	const int green_box_y = 39;
	const int exhaust_x = 28;
	const int exhaust_y = -24;
	
	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	//render cover
	if(!IsDestroyed())
	{
		if(owner != NULL_TEAM && build_state != BUILDING_SELECT)
		{
			if(the_map.GetBlitInfo(x + 31, y + 47, 24, 20, from_rect, to_rect))
			{
				from_rect.x += 31;
				from_rect.y += 47;

				to_rect.x += shift_x;
				to_rect.y += shift_y;

				base[palette][owner].BlitSurface(&from_rect, &to_rect);
				//SDL_BlitSurface( base[palette][owner], &from_rect, dest, &to_rect);
			}

			lx = x + exhaust_x;
			ly = y + exhaust_y + (exhaust_i * -2);

			the_map.RenderZSurface(&exhaust[exhaust_i], lx, ly);
			//if(the_map.GetBlitInfo(exhaust[exhaust_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( exhaust[exhaust_i], &from_rect, dest, &to_rect);
			//}

			lx = x + green_box_x;
			ly = y + green_box_y;

			the_map.RenderZSurface(&green_box[green_box_i], lx, ly);
			//if(the_map.GetBlitInfo(green_box[green_box_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( green_box[green_box_i], &from_rect, dest, &to_rect);
			//}

			for(int i=0;i<3;i++)
				if(single_light_on[i])
				{
					lx = x + single_light_x[i];
					ly = y + single_light_y;

					the_map.RenderZSurface(&single_light[1], lx, ly);
					//if(the_map.GetBlitInfo(single_light[1], lx, ly, from_rect, to_rect))
					//{
					//	to_rect.x += shift_x;
					//	to_rect.y += shift_y;

					//	SDL_BlitSurface( single_light[1], &from_rect, dest, &to_rect);
					//}
				}

			lx = x + double_light_x;
			ly = y + double_light_y;

			//double_light_i
			the_map.RenderZSurface(&double_light[1], lx, ly);
			//if(the_map.GetBlitInfo(double_light[1], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( double_light[1], &from_rect, dest, &to_rect);
			//}

			lx = x + robot_x;
			ly = y + robot_y;

			the_map.RenderZSurface(&robot[robot_i], lx, ly);
			//if(the_map.GetBlitInfo(robot[robot_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( robot[robot_i], &from_rect, dest, &to_rect);
			//}

			lx = x + spin_x;
			ly = y + spin_y;

			the_map.RenderZSurface(&spin[spin_i], lx, ly);
			//if(the_map.GetBlitInfo(spin[spin_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( spin[spin_i], &from_rect, dest, &to_rect);
			//}

			//render time
			lx = x + 35;
			ly = y + 58;

			the_map.RenderZSurface(&show_time_img, lx, ly);
			//if(show_time_img && the_map.GetBlitInfo(show_time_img, x + 35, y + 58, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( show_time_img, &from_rect, dest, &to_rect);
			//}
		}
		else
		{
			lx = x + robot_x;
			ly = y + robot_y;

			the_map.RenderZSurface(&robot[robot_i], lx, ly);
			//if(the_map.GetBlitInfo(robot[robot_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( robot[robot_i], &from_rect, dest, &to_rect);
			//}
		}
	}
	else
	{
		//IsDestroyed()
		const int x_plus = 35 - 16;
		const int y_plus = 32 - 24;

		if(the_map.GetBlitInfo(x + x_plus, y + y_plus, 32, 67 - (y_plus), from_rect, to_rect))
		{
			from_rect.x += x_plus;
			from_rect.y += y_plus;

			to_rect.x += shift_x;
			to_rect.y += shift_y;

			base_destroyed[palette][owner].BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface( base_destroyed[palette][owner], &from_rect, dest, &to_rect);
		}
	}

	//render level
	lx = x + level_x;
	ly = y + level_y;

	the_map.RenderZSurface(&level_img[level], lx, ly);
	//if(the_map.GetBlitInfo(level_img[level], lx, ly, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	SDL_BlitSurface( level_img[level], &from_rect, dest, &to_rect);
	//}

	//render effects
	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->DoRender(the_map, dest);
}

ZGuiWindow *BRobot::MakeGuiWindow()
{
	GWProduction *ret;

	if(IsDestroyed()) return NULL;

	ret = new GWProduction(ztime);

	ret->SetMap(zmap);
	ret->SetType(GWPROD_ROBOT);
	ret->SetCords(loc.x, loc.y);
	ret->SetBuildingObj(this);

	return (ZGuiWindow *)ret;
}

void BRobot::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	int sx, sy;
	int ex, ey;
	double offset_time;
	int peices;
	int fireballs;
	int i;

	//fireballs
	{
		fireballs = 8 + (rand() % 3);

		for(i=0;i<fireballs;i++)
		{
			sx = loc.x + effects_box.x + (rand() % effects_box.w);
			sy = loc.y + effects_box.y + (rand() % effects_box.h);

			if(effect_list) effect_list->push_back((ZEffect*)(new ESideExplosion(ztime, sx, sy, 1.3)));
		}
	}

	peices = 6 + (rand() % 3);

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
	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);
	do_base_rerender = true;
}

void BRobot::SetMapImpassables(ZMap &tmap)
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
	//tmap.SetImpassable(tx+3,ty+2, false);
}

bool BRobot::GetCraneEntrance(int &x, int &y, int &x2, int &y2)
{
	x = x2 = loc.x + 35;
	y = y2 = loc.y + height_pix + 32;
	return true;
}

bool BRobot::GetCraneCenter(int &x, int &y)
{
	x = loc.x + 35;
	y = loc.y + 32;
	return true;
}
