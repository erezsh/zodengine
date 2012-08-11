#include "bfort.h"
#include "gwproduction.h"

ZSDL_Surface BFort::base_front[MAX_PLANET_TYPES];
ZSDL_Surface BFort::base_back[MAX_PLANET_TYPES];
ZSDL_Surface BFort::base_front_destroyed[MAX_PLANET_TYPES];
ZSDL_Surface BFort::base_back_destroyed[MAX_PLANET_TYPES];
ZSDL_Surface BFort::base_front_destroyed_overlay[MAX_PLANET_TYPES];
ZSDL_Surface BFort::base_back_destroyed_overlay[MAX_PLANET_TYPES];
ZSDL_Surface BFort::flag[MAX_TEAM_TYPES][4];
ZSDL_Surface BFort::destroyed_overlay;

BFort::BFort(ZTime *ztime_, ZSettings *zsettings_, planet_type palette_, bool is_front_) : ZBuilding(ztime_, zsettings_, palette_)
{
	object_name = "fort";
	palette = palette_;
	//is_front = is_front_;
	next_flag_time = 0;
	flag_i = 0;
	destroyed_fade = 0;
	last_fade_time = 0;
	fade_dir = 100;

	//max_health = FORT_BUILDING_HEALTH;
	//health = max_health;
	
	SetIsFront(is_front_);

	InitTypeId(object_type, object_id);

	//if(is_front)
	//{
	//	object_id = FORT_FRONT;
	//	width = 10;
	//	height = 12;
	//	width_pix = 10 * 16;
	//	height_pix = 12 * 16;
	//	unit_create_x = 80;
	//	unit_create_y = 128;
	//	unit_move_x = 80;
	//	unit_move_y = 192 + 16;
	//}
	//else
	//{
	//	object_id = FORT_BACK;
	//	width = 10;
	//	height = 11;
	//	width_pix = 10 * 16;
	//	height_pix = 11 * 16;
	//	unit_create_x = 80;
	//	unit_create_y = 32;
	//	unit_move_x = 80;
	//	unit_move_y = -16;
	//}

	//fire effects
	effects_box.x = 18;
	effects_box.y = 18;
	effects_box.w = 136;
	effects_box.h = 118;
	max_effects = 20 + (rand() % 8);
}

void BFort::Init()
{
	string filename;
	char filename_c[500];
	int i, j;

	destroyed_overlay.LoadBaseImage("assets/buildings/fort/destroyed_overlay.png");// = IMG_Load_Error("assets/buildings/fort/destroyed_overlay.png");

	//the flags
	for(i=0;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<4;j++)
	{
		sprintf(filename_c, "assets/buildings/fort/flag_%s_n%02d.png", team_type_string[i].c_str(), j);
		//flag[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, flag[ZTEAM_BASE_TEAM][j], flag[i][j], filename_c);
	}
	
	//load images pertaining to a fort
	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/buildings/fort/fort_" + planet_type_string[i] + "_front.png";
		base_front[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		filename = "assets/buildings/fort/fort_" + planet_type_string[i] + "_back.png";
		base_back[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		filename = "assets/buildings/fort/fort_" + planet_type_string[i] + "_front_destroyed.png";
		base_front_destroyed[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		filename = "assets/buildings/fort/fort_" + planet_type_string[i] + "_back_destroyed.png";
		base_back_destroyed[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);

		//copies
		//if(destroyed_overlay && base_front_destroyed[i])
		{
			//base_front_destroyed_overlay[i] = SDL_DisplayFormat(base_front_destroyed[i]);//CopyImage(base_front_destroyed[i]);
			//SDL_BlitSurface( destroyed_overlay, NULL, base_front_destroyed_overlay[i], NULL);
			base_front_destroyed_overlay[i] = base_front_destroyed[i].GetBaseSurface();
			destroyed_overlay.BlitSurface(NULL, NULL, &base_front_destroyed_overlay[i]);

			base_front_destroyed_overlay[i].UseDisplayFormat();
		}

		//if(destroyed_overlay && base_back_destroyed[i])
		{
			//base_back_destroyed_overlay[i] = SDL_DisplayFormat(base_back_destroyed[i]);//CopyImage(base_back_destroyed[i]);
			//SDL_BlitSurface( destroyed_overlay, NULL, base_back_destroyed_overlay[i], NULL);
			base_back_destroyed_overlay[i] = base_back_destroyed[i].GetBaseSurface();
			destroyed_overlay.BlitSurface(NULL, NULL, &base_back_destroyed_overlay[i]);

			base_back_destroyed_overlay[i].UseDisplayFormat();
		}
	}
}

// SDL_Surface *BFort::GetRender()
// {
// 	if(is_front)
// 	{
// 		if(destroyed)
// 			return base_front_destroyed[palette];
// 		else
// 			return base_front[palette];
// 	}
// 	else
// 	{
// 		if(destroyed)
// 			return base_back_destroyed[palette];
// 		else
// 			return base_back[palette];
// 	}
// }

void BFort::SetIsFront(bool is_front_)
{
	is_front = is_front_;

	if(is_front)
	{
		object_id = FORT_FRONT;
		width = 10;
		height = 12;
		width_pix = width * 16;
		height_pix = height * 16;
		unit_create_x = 80;
		unit_create_y = 128;
		unit_move_x = 80;
		unit_move_y = 192 + 16;

		//jungle is 16 less
		if(palette==JUNGLE) 
		{
			height--;
			height_pix = height * 16;
		}
	}
	else
	{
		object_id = FORT_BACK;
		width = 10;
		height = 11;
		width_pix = width * 16;
		height_pix = height * 16;
		unit_create_x = 80;
		unit_create_y = 32;
		unit_move_x = 80;
		unit_move_y = -16;
	}

	center_x = loc.x + (width_pix >> 1);
	center_y = loc.y + (height_pix >> 1);
}

int BFort::Process()
{
	double &the_time = ztime->ztime;

	ProcessBuildingsEffects(the_time);

	if(the_time > next_flag_time)
	{
		flag_i++;
		if(flag_i >= 4) flag_i = 0;

		next_flag_time = the_time + 0.2;
	}


	if(build_state != BUILDING_SELECT)
		ResetShowTime((int)ProductionTimeLeft(the_time));
	else
		ResetShowTime(-1);

	//fade
	destroyed_fade += (the_time - last_fade_time) * fade_dir;
	last_fade_time = the_time;
	if(destroyed_fade > 254)
	{
		destroyed_fade = 254;
		fade_dir *= -1;
	}
	else if(destroyed_fade < 1)
	{
		destroyed_fade = 1;
		fade_dir *= -1;
	}

	return 1;
}

void BFort::DoPreRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
//	SDL_Rect from_rect, to_rect;
	ZSDL_Surface *destroyed_overlay_img;

	if(IsDestroyed())
	{
		if(is_front)
			destroyed_overlay_img = &base_front_destroyed_overlay[palette];
		else
			destroyed_overlay_img = &base_back_destroyed_overlay[palette];

		destroyed_overlay_img->SetAlpha(destroyed_fade);
		the_map.RenderZSurface(destroyed_overlay_img, x, y);
		//if(the_map.GetBlitInfo(destroyed_overlay_img, x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	//SDL_SetAlpha(destroyed_overlay_img, SDL_RLEACCEL | SDL_SRCALPHA,(Uint8)destroyed_fade);
		//	//SDL_BlitSurface( destroyed_overlay_img, &from_rect, dest, &to_rect);
		//}
	}
}

void BFort::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
//	SDL_Rect from_rect, to_rect;
//	int lx, ly;

	if(!dont_stamp)
	{
		if(do_base_rerender)
		{
			if(is_front)
			{
				if(IsDestroyed())
					base_surface = &base_front_destroyed[palette];
				else
					base_surface = &base_front[palette];
			}
			else
			{
				if(IsDestroyed())
					base_surface = &base_back_destroyed[palette];
				else
					base_surface = &base_back[palette];
			}

			if(the_map.PermStamp(x, y, base_surface))
				do_base_rerender = false;
			
			
		}
	}
	else
	{
		if(is_front)
		{
			if(IsDestroyed())
				base_surface = &base_front_destroyed[palette];
			else
				base_surface = &base_front[palette];
		}
		else
		{
			if(IsDestroyed())
				base_surface = &base_back_destroyed[palette];
			else
				base_surface = &base_back[palette];
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

void BFort::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
//	SDL_Rect from_rect, to_rect;
	int lx, ly;

	//unit creation cover
	RenderUnitCover(the_map, dest, shift_x, shift_y);

	//render only alive stuff
	if(!IsDestroyed())
	{
		//render time
		lx = x + 74;
		ly = y + 90;

		the_map.RenderZSurface(&show_time_img, lx, ly);
		//if(show_time_img && the_map.GetBlitInfo(show_time_img, x + 74, y + 90, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( show_time_img, &from_rect, dest, &to_rect);
		//}


		//render flag
		lx = x + 85;
		ly = y + 29;

		the_map.RenderZSurface(&flag[owner][flag_i], lx, ly);
		//if(the_map.GetBlitInfo(flag[owner][flag_i], lx, ly, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( flag[owner][flag_i], &from_rect, dest, &to_rect);
		//}
	}

	//render effects
	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->DoRender(the_map, dest);
}

void BFort::RenderUnitCover(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;
//	int lx, ly;

	//unit creation cover
	if(is_front)
	{
		if(the_map.GetBlitInfo(x + 64 - 8, y + 112 - 8, 32 + 16, 32 + 9, from_rect, to_rect))
		{
			from_rect.x += 64 - 8;
			from_rect.y += 112 - 8 ;

			to_rect.x += shift_x;
			to_rect.y += shift_y;

			if(IsDestroyed())
			{
				base_front_destroyed[palette].BlitSurface(&from_rect, &to_rect);
				base_front_destroyed_overlay[palette].BlitSurface(&from_rect, &to_rect);
			}
			else
				base_front[palette].BlitSurface(&from_rect, &to_rect);

			//base_surface->BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface( base_front[palette], &from_rect, dest, &to_rect);
		}
	}
	else
	{
		if(the_map.GetBlitInfo(x + 64 - 8, y + 16, 32 + 16, 32 + 9, from_rect, to_rect))
		{
			from_rect.x += 64 - 8;
			from_rect.y += 16 ;

			to_rect.x += shift_x;
			to_rect.y += shift_y;

			if(IsDestroyed())
			{
				base_back_destroyed[palette].BlitSurface(&from_rect, &to_rect);
				base_back_destroyed_overlay[palette].BlitSurface(&from_rect, &to_rect);
			}
			else
				base_back[palette].BlitSurface(&from_rect, &to_rect);

			//base_surface->BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface( base_front[palette], &from_rect, dest, &to_rect);
		}
	}
}

ZGuiWindow *BFort::MakeGuiWindow()
{
	GWProduction *ret;

	if(IsDestroyed()) return NULL;

	ret = new GWProduction(ztime);

	ret->SetMap(zmap);
	ret->SetType(GWPROD_FORT);
	ret->SetCords(loc.x, loc.y);
	ret->SetBuildingObj(this);

	return (ZGuiWindow *)ret;
}

void BFort::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	int sx, sy;
	int ex, ey;
	double offset_time;
	int peices;
	int fireballs;
	int i;

	//fireballs
	{
		fireballs = 12 + (rand() % 6);

		for(i=0;i<fireballs;i++)
		{
			sx = loc.x + effects_box.x + (rand() % effects_box.w);
			sy = loc.y + effects_box.y + (rand() % effects_box.h);

			if(effect_list) effect_list->push_back((ZEffect*)(new ESideExplosion(ztime, sx, sy, 1.3)));
		}
	}

	//turrents
	{
		peices = 16 + (rand() % 6);

		for(i=0;i<peices;i++)
		{
			sx = loc.x + effects_box.x + (rand() % effects_box.w);
			sy = loc.y + effects_box.y + (rand() % effects_box.h);
			ex = loc.x + (width_pix >> 1) + (200 - (rand() % 400));
			ey = loc.y + (height_pix >> 1) + (200 - (rand() % 400));

			offset_time = 3 + (0.01 * (rand() % 200));

			switch(rand()%5)
			{
			case 0:
				if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_FORT_BUILDING_PEICE0)));
				break;
			case 1:
				if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_FORT_BUILDING_PEICE1)));
				break;
			case 2:
				if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_FORT_BUILDING_PEICE2)));
				break;
			case 3:
				if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_FORT_BUILDING_PEICE3)));
				break;
			case 4:
				if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_FORT_BUILDING_PEICE4)));
				break;
			}
		}
	}

	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);
	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);
	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);
	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);

	do_base_rerender = true;
}

bool BFort::CanEnterFort(int team)
{
	if(team == owner) return false;
	if(IsDestroyed()) return false;

	return true;
}

bool BFort::UnderCursorCanAttack(int &map_x, int &map_y)
{
	int rx = map_x - loc.x;
	int ry = map_y - loc.y;

	//within main area?
	if(points_within_area(rx, ry, 16, 16, 16 * 8, 16 * 7)) return true;

	//side areas?
	if(points_within_area(rx, ry, 0, 16 * 3, 16 * 10, 16 * 4)) return true;

	//top left tower?
	if(points_within_area(rx, ry, 16, 0, 16 * 2, 16)) return true;

	//top right tower?
	if(points_within_area(rx, ry, 16 * 7, 0, 16 * 2, 16)) return true;

	//bottom left foot?
	if(points_within_area(rx, ry, 16 * 2, 16 * 8, 16, 16)) return true;

	//bottom right foot?
	if(points_within_area(rx, ry, 16 * 7, 16 * 8, 16, 16)) return true;

	return false;
}

bool BFort::UnderCursorFortCanEnter(int &map_x, int &map_y)
{
	int rx = map_x - loc.x;
	int ry = map_y - loc.y;

	if(is_front)
	{
		if(points_within_area(rx, ry, 16 * 4, 16 * 2, 32, 16 * 6)) return true;
	}
	else
	{
		if(points_within_area(rx, ry, 16 * 4, 16 * 1, 32, 16 * 4)) return true;
	}

	return false;
}

bool BFort::CannonNotPlacable(int &map_left, int &map_right, int &map_top, int &map_bottom)
{
	int &x = loc.x;
	int &y = loc.y;

	int local_x, local_y;

	local_x = map_left - x;
	local_y = map_top - y;

	if(local_x == 16 && (local_y == 0 || local_y == 48)) return false;
	if(local_x == 112 && (local_y == 0 || local_y == 48)) return false;

	return WithinSelection(map_left, map_right, map_top, map_bottom);
}

void BFort::SetMapImpassables(ZMap &tmap)
{
	int tx, ty, ex, ey;
	int i, j;

	tx = loc.x / 16;
	ty = loc.y / 16;
	ex = tx + 10;
	ey = ty + 9;

	for(i=tx;i<ex;i++)
		for(j=ty;j<ey;j++)
			tmap.SetImpassable(i,j);

	//unset

	//left side
	tmap.SetImpassable(tx, ty, false);
	tmap.SetImpassable(tx, ty+1, false);
	tmap.SetImpassable(tx, ty+2, false);
	tmap.SetImpassable(tx, ty+7, false);
	tmap.SetImpassable(tx, ty+8, false);
	tmap.SetImpassable(tx, ty+9, false);

	//right side
	tmap.SetImpassable(tx+9, ty, false);
	tmap.SetImpassable(tx+9, ty+1, false);
	tmap.SetImpassable(tx+9, ty+2, false);
	tmap.SetImpassable(tx+9, ty+7, false);
	tmap.SetImpassable(tx+9, ty+8, false);
	tmap.SetImpassable(tx+9, ty+9, false);

	//top
	tmap.SetImpassable(tx+3, ty, false);
	tmap.SetImpassable(tx+4, ty, false);
	tmap.SetImpassable(tx+5, ty, false);
	tmap.SetImpassable(tx+6, ty, false);

	//bottom
	tmap.SetImpassable(tx+1, ty+8, false);
	tmap.SetImpassable(tx+3, ty+8, false);
	tmap.SetImpassable(tx+4, ty+8, false);
	tmap.SetImpassable(tx+5, ty+8, false);
	tmap.SetImpassable(tx+6, ty+8, false);
	tmap.SetImpassable(tx+8, ty+8, false);

	if(is_front)
	{
		tmap.SetImpassable(tx+3, ty+8);
		tmap.SetImpassable(tx+3, ty+9);
		tmap.SetImpassable(tx+6, ty+8);
		tmap.SetImpassable(tx+6, ty+9);

		tmap.SetImpassable(tx+4, ty+7, false);
		tmap.SetImpassable(tx+5, ty+7, false);
		tmap.SetImpassable(tx+4, ty+6, false);
		tmap.SetImpassable(tx+5, ty+6, false);
	}
	else
	{
		tmap.SetImpassable(tx+4, ty+1, false);
		tmap.SetImpassable(tx+5, ty+1, false);
		tmap.SetImpassable(tx+4, ty+2, false);
		tmap.SetImpassable(tx+5, ty+2, false);
	}

}
