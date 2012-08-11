#include "bbridge.h"

ZSDL_Surface BBridge::planet_template[MAX_PLANET_TYPES];

BBridge::BBridge(ZTime *ztime_, ZSettings *zsettings_, planet_type palette_, bool is_vertical_, unsigned short extra_links_) : ZBuilding(ztime_, zsettings_, palette_)
{
	object_name = "bridge";

	//render_img = NULL;
	//render_damaged_img = NULL;
	//render_destroyed_img = NULL;
	palette = palette_;
	is_vertical = is_vertical_;
	extra_links = extra_links_;
	do_rerender = true;
	do_revive_rerender = false;
	next_revive_rerender_time = 0;

	//max_health = BRIDGE_BUILDING_HEALTH;
	//health = max_health;

	last_process_health = max_health;

	ResetStats();

	InitTypeId(object_type, object_id);
}

BBridge::~BBridge()
{
	UnRenderImages();
}
	
void BBridge::Init()
{
	int i;
	string filename;

	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/planets/bridge_" + planet_type_string[i] + ".png";
		planet_template[i].LoadBaseImage(filename);// = IMG_Load_Error ( filename.c_str() );
	}
}

int BBridge::Process()
{
	double &the_time = ztime->ztime;

	if(last_process_health != health)
	{
		int half_health = max_health >> 1;
		//did we go from health to mid?
		if(last_process_health >= half_health && health && health < half_health)
		{
			DoExplosions();
			do_base_rerender = true;
		}

		last_process_health = health;
	}

	if(do_revive_rerender && the_time >= next_revive_rerender_time)
	{
		do_revive_rerender = false;
		do_base_rerender = true;
	}

	return 1;
}

void BBridge::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
//	SDL_Rect from_rect, to_rect;

	if(do_rerender) ReRenderImages();

	if(dont_stamp)
	{
		if(health >= max_health >> 1)
			base_surface = &render_img;
		else if(!IsDestroyed())
			base_surface = &render_damaged_img;
		else
			base_surface = &render_destroyed_img;

		//base_surface = render_destroyed_img;

		if(!base_surface) return;

		the_map.RenderZSurface(base_surface, x, y);
		//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
		//}
	}
	else if(do_base_rerender)
	{
		if(health >= max_health >> 1)
			base_surface = &render_img;
		else if(!IsDestroyed())
			base_surface = &render_damaged_img;
		else
			base_surface = &render_destroyed_img;

		if(!base_surface) return;

		if(the_map.PermStamp(x, y, base_surface))
			do_base_rerender = false;
	}
}

void BBridge::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	DoExplosions();
	do_base_rerender = true;
}

void BBridge::DoExplosions()
{
	DoTurrentEffect(false);
}

void BBridge::DoReviveEffect()
{
	double &the_time = ztime->ztime;

	do_revive_rerender = true;
	next_revive_rerender_time = the_time + 2.25;

	DoTurrentEffect(true);
}

void BBridge::DoTurrentEffect(bool is_reversed)
{
	int x, y;

	if(is_vertical)
	{
		x = loc.x + 16;
		y = loc.y + 16 + 5 + (rand() % 10);

		while(y < (height_pix + loc.y) - 16)
		{
			if(effect_list) effect_list->push_back((ZEffect*)(new EBridgeTurrent(ztime, x + (rand() % 32), y, palette, 140, 140, is_reversed)));	
			y += 5 + (rand() % 10);
		}
	}
	else
	{
		x = loc.x + 16 + 5 + (rand() % 10);
		y = loc.y + 16;

		while(x < (width_pix + loc.x) - 16)
		{
			if(effect_list) effect_list->push_back((ZEffect*)(new EBridgeTurrent(ztime, x, y + (rand() % 32), palette, 140, 140, is_reversed)));	
			x += 5 + (rand() % 10);
		}
	}
}

void BBridge::SetMapImpassables(ZMap &tmap)
{
	int tx, ty, ex, ey;
//	int i, j;

	tx = loc.x / 16;
	ty = loc.y / 16;
	ex = tx + width;
	ey = ty + height;

	if(is_vertical)
	{
		for(;ty<ey;ty++)
		{
			tmap.SetImpassable(tx, ty);
			tmap.SetImpassable(tx+3, ty);
		}
	}
	else
	{
		for(;tx<ex;tx++)
		{
			tmap.SetImpassable(tx, ty);
			tmap.SetImpassable(tx, ty+3);
		}
	}
}

void BBridge::ImpassCenter(ZMap &tmap, bool impassable)
{
	int tx, ty, ex, ey;
//	int i, j;

	tx = loc.x / 16;
	ty = loc.y / 16;
	ex = tx + width;
	ey = ty + height;

	if(is_vertical)
	{
		for(;ty<ey;ty++)
		{
			tmap.SetImpassable(tx+1, ty, impassable);
			tmap.SetImpassable(tx+2, ty, impassable);
		}
	}
	else
	{
		for(;tx<ex;tx++)
		{
			tmap.SetImpassable(tx, ty+1, impassable);
			tmap.SetImpassable(tx, ty+2, impassable);
		}
	}
}

void BBridge::SetDestroyMapImpassables(ZMap &tmap)
{
	ImpassCenter(tmap, true);
}

void BBridge::UnSetDestroyMapImpassables(ZMap &tmap)
{
	ImpassCenter(tmap, false);
}

bool BBridge::UnderCursorCanAttack(int &map_x, int &map_y)
{
	int rx = map_x - loc.x;
	int ry = map_y - loc.y;

	if(IsDestroyed()) return true;

	if(is_vertical)
	{
		//left section?
		if(points_within_area(rx, ry, 0, 0, 16, height_pix)) return true;

		//right section?
		if(points_within_area(rx, ry, 16 * 3, 0, 16, height_pix)) return true;
	}
	else
	{
		//top section?
		if(points_within_area(rx, ry, 0, 0, width_pix, 16)) return true;

		//bottom section?
		if(points_within_area(rx, ry, 0, 16 * 3, width_pix, 16)) return true;
	}

	return false;
}

void BBridge::UnRenderImages()
{
	//ZSDL_FreeSurface(render_img);
	//ZSDL_FreeSurface(render_damaged_img);
	//ZSDL_FreeSurface(render_destroyed_img);
	render_img.Unload();
	render_damaged_img.Unload();
	render_destroyed_img.Unload();

	do_rerender = true;
}

void BBridge::IndividualReRender(ZSDL_Surface &surface)
{
	if(!surface.GetBaseSurface())
	{
		//surface = ZSDL_NewSurface(width_pix, height_pix);
		surface.LoadNewSurface(width_pix, height_pix);
	}
	else if(surface.GetBaseSurface()->w != width_pix || surface.GetBaseSurface()->h != height_pix)
	{
		//ZSDL_FreeSurface(surface);	
		//surface = ZSDL_NewSurface(width_pix, height_pix);
		surface.LoadNewSurface(width_pix, height_pix);
	}
}

void BBridge::ReRenderImages()
{
	IndividualReRender(render_img);
	IndividualReRender(render_damaged_img);
	IndividualReRender(render_destroyed_img);

	//erm it all exist?
	if(!render_img.GetBaseSurface() || !render_damaged_img.GetBaseSurface() || !render_destroyed_img.GetBaseSurface())
	{
//		mprintf("error: not able to create base bridge image for rendering\n");
		return;
	}

	if(!planet_template[palette].GetBaseSurface())
	{
//		mprintf("error: required bridge palette not loaded\n");
		return;
	}
	
	if(is_vertical)
	{
		//double check dimensions?
		if((width < 4 || height < 5) || (render_img.GetBaseSurface()->w != width * 16 || render_img.GetBaseSurface()->h != height * 16))
		{
//			mprintf("error: bridge dimensions incorrect\n");
			return;
		}

		//insert edges
		//ZSDL_BlitSurface(planet_template[palette], 0, 0, 64, 16, render_img, 0, 0);
		//ZSDL_BlitSurface(planet_template[palette], 0, 16, 64, 16, render_img, 0, 16);
		//ZSDL_BlitSurface(planet_template[palette], 0, 2 * 16, 64, 16, render_img, 0, (height-2) * 16);
		//ZSDL_BlitSurface(planet_template[palette], 0, 3 * 16, 64, 16, render_img, 0, (height-1) * 16);
		planet_template[palette].BlitSurface(0, 0, 64, 16, &render_img, 0, 0);
		planet_template[palette].BlitSurface(0, 16, 64, 16, &render_img, 0, 16);
		planet_template[palette].BlitSurface(0, 2 * 16, 64, 16, &render_img, 0, (height-2) * 16);
		planet_template[palette].BlitSurface(0, 3 * 16, 64, 16, &render_img, 0, (height-1) * 16);

		//copy
		//SDL_BlitSurface(render_img, NULL, render_damaged_img, NULL);
		//SDL_BlitSurface(render_img, NULL, render_destroyed_img, NULL);
		render_img.BlitSurface(NULL, NULL, &render_damaged_img);
		render_img.BlitSurface(NULL, NULL, &render_destroyed_img);
		

		//insert filler
		for(int i=2;i<height-2;i++)
		{
			//ZSDL_BlitSurface(planet_template[palette], 0, 4 * 16, 64, 16, render_img, 0, i * 16);
			//ZSDL_BlitSurface(planet_template[palette], 0, (5 + (rand() % 2))* 16, 64, 16, render_damaged_img, 0, i * 16);
			//ZSDL_BlitSurface(planet_template[palette], 0, 7 * 16, 64, 16, render_destroyed_img, 0, i * 16);
			planet_template[palette].BlitSurface(0, 4 * 16, 64, 16, &render_img, 0, i * 16);
			planet_template[palette].BlitSurface(0, (5 + (rand() % 2))* 16, 64, 16, &render_damaged_img, 0, i * 16);
			planet_template[palette].BlitSurface(0, 7 * 16, 64, 16, &render_destroyed_img, 0, i * 16);
		}


	}
	else
	{
		//double check dimensions?
		if((width < 5 || height < 4) || (render_img.GetBaseSurface()->w != width * 16 || render_img.GetBaseSurface()->h != height * 16))
		{
//			mprintf("error: bridge dimensions incorrect\n");
			return;
		}

		//insert edges
		//ZSDL_BlitSurface(planet_template[palette], 0, 8 * 16, 16, 64, render_img, 0, 0);
		//ZSDL_BlitSurface(planet_template[palette], 16, 8 * 16, 16, 64, render_img, 16, 0);
		//ZSDL_BlitSurface(planet_template[palette], 2 * 16, 8 * 16, 16, 64, render_img, (width-2) * 16, 0);
		//ZSDL_BlitSurface(planet_template[palette], 3 * 16, 8 * 16, 16, 64, render_img, (width-1) * 16, 0);
		planet_template[palette].BlitSurface(0, 8 * 16, 16, 64, &render_img, 0, 0);
		planet_template[palette].BlitSurface(16, 8 * 16, 16, 64, &render_img, 16, 0);
		planet_template[palette].BlitSurface(2 * 16, 8 * 16, 16, 64, &render_img, (width-2) * 16, 0);
		planet_template[palette].BlitSurface(3 * 16, 8 * 16, 16, 64, &render_img, (width-1) * 16, 0);

		//copy
		//SDL_BlitSurface(render_img, NULL, render_damaged_img, NULL);
		//SDL_BlitSurface(render_img, NULL, render_destroyed_img, NULL);
		render_img.BlitSurface(NULL, NULL, &render_damaged_img);
		render_img.BlitSurface(NULL, NULL, &render_destroyed_img);

		//insert filler
		for(int i=2;i<width-2;i++)
		{
			//ZSDL_BlitSurface(planet_template[palette], 0, 12 * 16, 16, 64, render_img, i * 16, 0);
			//ZSDL_BlitSurface(planet_template[palette], (1 + (rand() %2)) * 16, 12 * 16, 16, 64, render_damaged_img, i * 16, 0);
			//ZSDL_BlitSurface(planet_template[palette], 3 * 16, 12 * 16, 16, 64, render_destroyed_img, i * 16, 0);
			planet_template[palette].BlitSurface(0, 12 * 16, 16, 64, &render_img, i * 16, 0);
			planet_template[palette].BlitSurface((1 + (rand() %2)) * 16, 12 * 16, 16, 64, &render_damaged_img, i * 16, 0);
			planet_template[palette].BlitSurface(3 * 16, 12 * 16, 16, 64, &render_destroyed_img, i * 16, 0);
		}

	}

	//do not reconstruct the base images
	do_rerender = false;

	//do a restamp of the new images onto the map
	do_base_rerender = true;
}

void BBridge::ChangePalette(planet_type palette_)
{
	if(palette != palette_)
	{
		palette = palette_;
		do_rerender = true;
	}
}

void BBridge::SetIsVertical(bool is_vertical_)
{
	if(is_vertical != is_vertical_)
	{
		is_vertical = is_vertical_;
		do_rerender = true;

		ResetStats();
	}
}

void BBridge::SetExtraLinks(unsigned short extra_links_)
{
	if(extra_links != extra_links_)
	{
		extra_links = extra_links_;
		do_rerender = true;

		ResetStats();
	}
}

unsigned short BBridge::GetExtraLinks()
{
	return extra_links;
}

void BBridge::ResetStats()
{
	if(is_vertical)
	{
		width = 4;
		height = 5 + extra_links;
		object_id = BRIDGE_VERT;
	}
	else
	{
		width = 5 + extra_links;
		height = 4;
		object_id = BRIDGE_HORZ;
	}

	width_pix = width * 16;
	height_pix = height * 16;

	center_x = loc.x + (width_pix >> 1);
	center_y = loc.y + (height_pix >> 1);
}

void BBridge::SetOwner(team_type owner_)
{
	owner = NULL_TEAM;
}

bool BBridge::GetCraneEntrance(int &x, int &y, int &x2, int &y2)
{
	if(is_vertical)
	{
		x = loc.x + 32;
		y = loc.y - 32;

		x2 = loc.x + 32;
		y2 = loc.y + height_pix + 32;
	}
	else
	{
		x = loc.x - 31;
		y = loc.y + 31;

		x2 = loc.x + width_pix + 32;
		y2 = loc.y + 32;
	}
	
	return true;
}

bool BBridge::GetCraneCenter(int &x, int &y)
{
	x = loc.x + (width_pix >> 1);
	y = loc.y + (height_pix >> 1);
	return true;
}
