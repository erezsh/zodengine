#include "zmini_map.h"

ZMiniMap::ZMiniMap()
{
	zmap = NULL;
	object_list = NULL;

	render_area.x = 0;
	render_area.y = 0;
	render_area.w = MINIMAP_W_MAX;
	render_area.h = MINIMAP_H_MAX;

	render_ratio = 0;

	show_terrain = false;
}

void ZMiniMap::Setup(ZMap *zmap_, vector<ZObject*> *object_list_)
{
	zmap = zmap_;
	object_list = object_list_;
}

void ZMiniMap::Setup_Boundaries()
{
	double map_ratio, max_mini_ratio;

	if(!zmap) return;
	if(!object_list) return;

	//now set our boundaries
	map_ratio = (1.0 * zmap->GetMapBasics().width) / zmap->GetMapBasics().height;
	max_mini_ratio = (1.0 * MINIMAP_W_MAX) / MINIMAP_H_MAX;

	if(map_ratio < max_mini_ratio)
	{
		render_area.w = (Uint16)(map_ratio * MINIMAP_H_MAX);
		render_area.h = MINIMAP_H_MAX;
		render_area.x = (MINIMAP_W_MAX - render_area.w) >> 1;
		render_area.y = (MINIMAP_H_MAX - render_area.h) >> 1;
	}
	else
	{
		render_area.w = MINIMAP_W_MAX;
		render_area.h = (Uint16)(MINIMAP_W_MAX / map_ratio);
		render_area.x = (MINIMAP_W_MAX - render_area.w) >> 1;
		render_area.y = (MINIMAP_H_MAX - render_area.h) >> 1;
	}

	render_area.x += 2;
	render_area.y += 2;
	render_area.w -= 4;
	render_area.h -= 4;

	if(render_area.w < 0) render_area.w = 0;
	if(render_area.h < 0) render_area.h = 0;

	render_ratio = (double)render_area.h / (zmap->GetMapBasics().height * 16.0);
}

bool ZMiniMap::ClickedMap(int x, int y, int &map_x, int &map_y)
{
	if(x < render_area.x) return false;
	if(x > render_area.x + render_area.w) return false;
	if(y < render_area.y) return false;
	if(y > render_area.y + render_area.h) return false;

	double x_percent = 1.0 * (x - render_area.x) / render_area.w;
	double y_percent = 1.0 * (y - render_area.y) / render_area.h;

	map_x = (int)(x_percent * (double)(zmap->GetMapBasics().width * 16.0));
	map_y = (int)(y_percent * (double)(zmap->GetMapBasics().height * 16.0));

	return true;
}

void ZMiniMap::DoRender(SDL_Surface *dest, int x, int y)
{
	SDL_Rect to_rect;

	//we need these to render
	if(!zmap) return;
	if(!object_list) return;

	to_rect = render_area;
	to_rect.x += x;
	to_rect.y += y;
	to_rect.w = render_area.w;
	to_rect.h = render_area.h;

	//SDL_FillRect(dest, &to_rect, SDL_MapRGB(dest->format, 10, 10, 10));
	ZSDL_FillRect(&to_rect, 10, 10, 10);

	//render terrain
	if(show_terrain)
	for(vector<map_effect_info>::iterator i=zmap->GetMapWaterList().begin(); i!=zmap->GetMapWaterList().end(); ++i)
	{
		SDL_Rect water_rect;
		int ix, iy;

		zmap->GetTile((unsigned int)i->tile, ix, iy);

		water_rect.x = ix;
		water_rect.y = iy;
		water_rect.w = water_rect.h = 1;

		water_rect.x = (Sint16)((double)water_rect.x * render_ratio);
		water_rect.y = (Sint16)((double)water_rect.y * render_ratio);

		water_rect.x += to_rect.x;
		water_rect.y += to_rect.y;

		ZSDL_FillRect(&water_rect, 0, 0, 250, NULL);
	}


	//render zones
	for(vector<map_zone_info>::iterator i=zmap->GetZoneInfoList().begin(); i!=zmap->GetZoneInfoList().end(); i++)
	{
		SDL_Rect zone_rect;

		zone_rect.x = i->x;
		zone_rect.y = i->y;
		zone_rect.w = i->w;
		zone_rect.h = i->h;

		zone_rect.x = (Sint16)((double)zone_rect.x * render_ratio);
		zone_rect.y = (Sint16)((double)zone_rect.y * render_ratio);
		zone_rect.w = (Sint16)((double)zone_rect.w * render_ratio);
		zone_rect.h = (Sint16)((double)zone_rect.h * render_ratio);

		zone_rect.x += to_rect.x;
		zone_rect.y += to_rect.y;

		zone_rect.x++;
		zone_rect.y++;
		zone_rect.w -= 2;
		zone_rect.h -= 2;

		//do we actually render?
		if(zone_rect.w > 0 && zone_rect.h > 0)
		{
//			int sdlmap;
			//sdlmap = SDL_MapRGB(dest->format, team_color[i->owner].r * 0.4, team_color[i->owner].g * 0.4, team_color[i->owner].b * 0.4);
			//SDL_FillRect(dest, &zone_rect, sdlmap);
			ZSDL_FillRect(&zone_rect, team_color[i->owner].r * 0.4, team_color[i->owner].g * 0.4, team_color[i->owner].b * 0.4);
		}
	}

	//draw objects
	for(vector<ZObject*>::iterator i=object_list->begin(); i!=object_list->end(); i++)
	{
		SDL_Rect obj_rect;
		int x, y, w, h;

		(*i)->GetCords(x, y);
		(*i)->GetDimensionsPixel(w, h);
		
		obj_rect.x = x;
		obj_rect.y = y;
		obj_rect.w = (Uint16)((float)w * 0.8);
		obj_rect.h = (Uint16)((float)h * 0.8);

		obj_rect.x = (Sint16)((double)obj_rect.x * render_ratio);
		obj_rect.y = (Sint16)((double)obj_rect.y * render_ratio);
		obj_rect.w = (Uint16)((double)obj_rect.w * render_ratio);
		obj_rect.h = (Uint16)((double)obj_rect.h * render_ratio);

		obj_rect.x += to_rect.x;
		obj_rect.y += to_rect.y;

		if(obj_rect.w < 1) obj_rect.w = 1;
		if(obj_rect.h < 1) obj_rect.h = 1;

//		int sdlmap;
		int towner;
		towner = (*i)->GetOwner();
		//sdlmap = SDL_MapRGB(dest->format, team_color[towner].r, team_color[towner].g , team_color[towner].b);
		//SDL_FillRect(dest, &obj_rect, sdlmap);
		ZSDL_FillRect(&obj_rect, team_color[towner].r, team_color[towner].g , team_color[towner].b);
	}

	//draw view area
	{
		int mshift_x, mshift_y, mview_w, mview_h;
		SDL_Color the_color;

		zmap->GetViewShiftFull(mshift_x, mshift_y, mview_w, mview_h);
		SDL_Rect view_box;
		view_box.x = mshift_x;
		view_box.y = mshift_y;
		view_box.w = mview_w;
		view_box.h = mview_h;

		view_box.x = (Sint16)((double)view_box.x * render_ratio);
		view_box.y = (Sint16)((double)view_box.y * render_ratio);
		view_box.w = (Uint16)((double)view_box.w * render_ratio);
		view_box.h = (Uint16)((double)view_box.h * render_ratio);

		view_box.x += to_rect.x;
		view_box.y += to_rect.y;

		the_color.r = 200;
		the_color.g = 200;
		the_color.b = 0;

		draw_box(dest, view_box, the_color, to_rect.x + to_rect.w, to_rect.y + to_rect.h);
	}
}
