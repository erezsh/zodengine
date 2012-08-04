#include "orockets.h"

ZSDL_Surface ORockets::render_img;

ORockets::ORockets(ZTime *ztime_, ZSettings *zsettings_) : ZObject(ztime_, zsettings_)
{
	object_name = "rockets";
	object_type = MAP_ITEM_OBJECT;
	object_id = ROCKETS_ITEM;
	width = 1;
	height = 1;
	width_pix = width * 16;
	height_pix = height * 16;
	attacked_by_explosives = true;

	InitTypeId(object_type, object_id);

	//max_health = ROCKETS_MAX_HEALTH;
	//health = max_health;
}
		
void ORockets::Init()
{
	render_img.LoadBaseImage("assets/other/map_items/rockets.png");
}

void ORockets::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;

	the_map.RenderZSurface(&render_img, x, y);
	//if(the_map.GetBlitInfo( render_img, x, y, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	SDL_BlitSurface( render_img, &from_rect, dest, &to_rect);
	//}
}

int ORockets::Process()
{
	return 0;
}

void ORockets::SetOwner(team_type owner_)
{
	//do nothing
}
