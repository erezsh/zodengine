#include "orock.h"

ZSDL_Surface ORock::rock_pal[MAX_PLANET_TYPES];

ZSDL_Surface ORock::rock_center_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_up_left_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_up_right_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_down_right_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_down_left_top[MAX_PLANET_TYPES];

ZSDL_Surface ORock::rock_up_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_down_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_right_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_left_top[MAX_PLANET_TYPES];

ZSDL_Surface ORock::rock_vert_up_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_vert_mid_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_vert_down_top[MAX_PLANET_TYPES];

ZSDL_Surface ORock::rock_horz_left_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_horz_mid_top[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_horz_right_top[MAX_PLANET_TYPES];

ZSDL_Surface ORock::rock_left_mid_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_left_bottom_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_mid_mid_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_mid_bottom_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_right_mid_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_right_bottom_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_single_mid_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_single_bottom_under[MAX_PLANET_TYPES];

ZSDL_Surface ORock::rock_up_shadow[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_mid_shadow[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_bottom_shadow[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_mid_mid_shadow_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_mid_bottom_shadow_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_right_mid_shadow_under[MAX_PLANET_TYPES];
ZSDL_Surface ORock::rock_right_bottom_shadow_under[MAX_PLANET_TYPES];

ZSDL_Surface ORock::rock_destroyed[MAX_PLANET_TYPES][6];

ORock::ORock(ZTime *ztime_, ZSettings *zsettings_, int palette_) : ZObject(ztime_, zsettings_)
{
	object_name = "rock";
	object_type = MAP_ITEM_OBJECT;
	object_id = ROCK_ITEM;
	width = 1;
	height = 3;
	width_pix = width * 16;
	height_pix = height * 16;
	attacked_by_explosives = true;

	InitTypeId(object_type, object_id);

	//max_health = ROCK_MAX_HEALTH;
	//health = max_health;

	palette = palette_;

	SetDefaultRender();
}
		
void ORock::Init()
{
	string filename;
//	char filename_c[500];
	int i, j;

	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/planets/rocks_" + planet_type_string[i] + ".png";
		rock_pal[i].LoadBaseImage(filename);// = ZSDL_IMG_Load(filename);

		if(rock_pal[i].GetBaseSurface())
		{
			//now set it up
			//rock_center_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 1 * 16, 1 * 16, 16, 16, rock_center_top[i], 0, 0);
			rock_center_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(1 * 16, 1 * 16, 16, 16, &rock_center_top[i], 0, 0);

			//rock_up_left_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 0 * 16, 0 * 16, 16, 16, rock_up_left_top[i], 0, 0);
			rock_up_left_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(0 * 16, 0 * 16, 16, 16, &rock_up_left_top[i], 0, 0);

			//rock_up_right_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 2 * 16, 0 * 16, 16, 16, rock_up_right_top[i], 0, 0);
			rock_up_right_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(2 * 16, 0 * 16, 16, 16, &rock_up_right_top[i], 0, 0);

			//rock_down_right_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 2 * 16, 2 * 16, 16, 16, rock_down_right_top[i], 0, 0);
			rock_down_right_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(2 * 16, 2 * 16, 16, 16, &rock_down_right_top[i], 0, 0);

			//rock_down_left_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 0 * 16, 2 * 16, 16, 16, rock_down_left_top[i], 0, 0);
			rock_down_left_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(0 * 16, 2 * 16, 16, 16, &rock_down_left_top[i], 0, 0);

			//---
			//rock_up_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 1 * 16, 0 * 16, 16, 16, rock_up_top[i], 0, 0);
			rock_up_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(1 * 16, 0 * 16, 16, 16, &rock_up_top[i], 0, 0);

			//rock_down_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 1 * 16, 2 * 16, 16, 16, rock_down_top[i], 0, 0);
			rock_down_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(1 * 16, 2 * 16, 16, 16, &rock_down_top[i], 0, 0);

			//rock_right_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 2 * 16, 1 * 16, 16, 16, rock_right_top[i], 0, 0);
			rock_right_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(2 * 16, 1 * 16, 16, 16, &rock_right_top[i], 0, 0);

			//rock_left_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 0 * 16, 1 * 16, 16, 16, rock_left_top[i], 0, 0);
			rock_left_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(0 * 16, 1 * 16, 16, 16, &rock_left_top[i], 0, 0);

			//---
			//rock_vert_up_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 3 * 16, 0 * 16, 16, 16, rock_vert_up_top[i], 0, 0);
			rock_vert_up_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(3 * 16, 0 * 16, 16, 16, &rock_vert_up_top[i], 0, 0);

			//rock_vert_mid_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 3 * 16, 1 * 16, 16, 16, rock_vert_mid_top[i], 0, 0);
			rock_vert_mid_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(3 * 16, 1 * 16, 16, 16, &rock_vert_mid_top[i], 0, 0);

			//rock_vert_down_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 3 * 16, 2 * 16, 16, 16, rock_vert_down_top[i], 0, 0);
			rock_vert_down_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(3 * 16, 2 * 16, 16, 16, &rock_vert_down_top[i], 0, 0);

			//---
			//rock_horz_left_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 0 * 16, 5 * 16, 16, 16, rock_horz_left_top[i], 0, 0);
			rock_horz_left_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(0 * 16, 5 * 16, 16, 16, &rock_horz_left_top[i], 0, 0);

			//rock_horz_mid_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 1 * 16, 5 * 16, 16, 16, rock_horz_mid_top[i], 0, 0);
			rock_horz_mid_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(1 * 16, 5 * 16, 16, 16, &rock_horz_mid_top[i], 0, 0);

			//rock_horz_right_top[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 2 * 16, 5 * 16, 16, 16, rock_horz_right_top[i], 0, 0);
			rock_horz_right_top[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(2 * 16, 5 * 16, 16, 16, &rock_horz_right_top[i], 0, 0);

			//---
			//rock_left_mid_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 0 * 16, 3 * 16, 16, 16, rock_left_mid_under[i], 0, 0);
			rock_left_mid_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(0 * 16, 3 * 16, 16, 16, &rock_left_mid_under[i], 0, 0);

			//rock_left_bottom_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 0 * 16, 4 * 16, 16, 16, rock_left_bottom_under[i], 0, 0);
			rock_left_bottom_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(0 * 16, 4 * 16, 16, 16, &rock_left_bottom_under[i], 0, 0);

			//rock_mid_mid_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 1 * 16, 3 * 16, 16, 16, rock_mid_mid_under[i], 0, 0);
			rock_mid_mid_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(1 * 16, 3 * 16, 16, 16, &rock_mid_mid_under[i], 0, 0);

			//rock_mid_bottom_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 1 * 16, 4 * 16, 16, 16, rock_mid_bottom_under[i], 0, 0);
			rock_mid_bottom_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(1 * 16, 4 * 16, 16, 16, &rock_mid_bottom_under[i], 0, 0);

			//rock_right_mid_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 2 * 16, 3 * 16, 16, 16, rock_right_mid_under[i], 0, 0);
			rock_right_mid_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(2 * 16, 3 * 16, 16, 16, &rock_right_mid_under[i], 0, 0);

			//rock_right_bottom_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 2 * 16, 4 * 16, 16, 16, rock_right_bottom_under[i], 0, 0);
			rock_right_bottom_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(2 * 16, 4 * 16, 16, 16, &rock_right_bottom_under[i], 0, 0);

			//rock_single_mid_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 3 * 16, 3 * 16, 16, 16, rock_single_mid_under[i], 0, 0);
			rock_single_mid_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(3 * 16, 3 * 16, 16, 16, &rock_single_mid_under[i], 0, 0);

			//rock_single_bottom_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 3 * 16, 4 * 16, 16, 16, rock_single_bottom_under[i], 0, 0);
			rock_single_bottom_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(3 * 16, 4 * 16, 16, 16, &rock_single_bottom_under[i], 0, 0);

			//---
			//rock_up_shadow[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 4 * 16, 2 * 16, 16, 16, rock_up_shadow[i], 0, 0);
			rock_up_shadow[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(4 * 16, 2 * 16, 16, 16, &rock_up_shadow[i], 0, 0);

			//rock_mid_shadow[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 4 * 16, 3 * 16, 16, 16, rock_mid_shadow[i], 0, 0);
			rock_mid_shadow[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(4 * 16, 3 * 16, 16, 16, &rock_mid_shadow[i], 0, 0);

			//rock_bottom_shadow[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 4 * 16, 4 * 16, 16, 16, rock_bottom_shadow[i], 0, 0);
			rock_bottom_shadow[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(4 * 16, 4 * 16, 16, 16, &rock_bottom_shadow[i], 0, 0);

			//rock_mid_mid_shadow_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 5 * 16, 0 * 16, 16, 16, rock_mid_mid_shadow_under[i], 0, 0);
			rock_mid_mid_shadow_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(5 * 16, 0 * 16, 16, 16, &rock_mid_mid_shadow_under[i], 0, 0);

			//rock_mid_bottom_shadow_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 5 * 16, 1 * 16, 16, 16, rock_mid_bottom_shadow_under[i], 0, 0);
			rock_mid_bottom_shadow_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(5 * 16, 1 * 16, 16, 16, &rock_mid_bottom_shadow_under[i], 0, 0);

			//rock_right_mid_shadow_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 4 * 16, 0 * 16, 16, 16, rock_right_mid_shadow_under[i], 0, 0);
			rock_right_mid_shadow_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(4 * 16, 0 * 16, 16, 16, &rock_right_mid_shadow_under[i], 0, 0);

			//rock_right_bottom_shadow_under[i] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 4 * 16, 1 * 16, 16, 16, rock_right_bottom_shadow_under[i], 0, 0);
			rock_right_bottom_shadow_under[i].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(4 * 16, 1 * 16, 16, 16, &rock_right_bottom_shadow_under[i], 0, 0);

			//for(j=0;j<6;j++)
			//	rock_destroyed[i][j] = ZSDL_NewSurface(16, 16);
			//ZSDL_BlitSurface(rock_pal[i], 3 * 16, 5 * 16, 16, 16, rock_destroyed[i][0], 0, 0);
			//ZSDL_BlitSurface(rock_pal[i], 4 * 16, 5 * 16, 16, 16, rock_destroyed[i][1], 0, 0);
			//ZSDL_BlitSurface(rock_pal[i], 5 * 16, 5 * 16, 16, 16, rock_destroyed[i][2], 0, 0);
			//ZSDL_BlitSurface(rock_pal[i], 5 * 16, 2 * 16, 16, 16, rock_destroyed[i][3], 0, 0);
			//ZSDL_BlitSurface(rock_pal[i], 5 * 16, 3 * 16, 16, 16, rock_destroyed[i][4], 0, 0);
			//ZSDL_BlitSurface(rock_pal[i], 5 * 16, 4 * 16, 16, 16, rock_destroyed[i][5], 0, 0);
			for(j=0;j<6;j++)
				rock_destroyed[i][j].LoadNewSurface(16, 16);
			rock_pal[i].BlitSurface(3 * 16, 5 * 16, 16, 16, &rock_destroyed[i][0], 0, 0);
			rock_pal[i].BlitSurface(4 * 16, 5 * 16, 16, 16, &rock_destroyed[i][1], 0, 0);
			rock_pal[i].BlitSurface(5 * 16, 5 * 16, 16, 16, &rock_destroyed[i][2], 0, 0);
			rock_pal[i].BlitSurface(5 * 16, 2 * 16, 16, 16, &rock_destroyed[i][3], 0, 0);
			rock_pal[i].BlitSurface(5 * 16, 3 * 16, 16, 16, &rock_destroyed[i][4], 0, 0);
			rock_pal[i].BlitSurface(5 * 16, 4 * 16, 16, 16, &rock_destroyed[i][5], 0, 0);
		}
		else
		{
			rock_up_left_top[i] = NULL;
			rock_up_right_top[i] = NULL;
			rock_down_right_top[i] = NULL;
			rock_down_left_top[i] = NULL;
		}
	}
}

void ORock::DoPreRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;
	int j;
	int ys;

	//do shadows as prerender
	for(j=0,ys=0;j<3;j++,ys+=16)
		if(render_img[1][j] && render_img[1][j]->GetBaseSurface())
			{
				if(the_map.GetBlitInfo( render_img[1][j]->GetBaseSurface(), x+16, y+ys, from_rect, to_rect))
				{
					to_rect.x += shift_x;
					to_rect.y += shift_y;

					render_img[1][j]->BlitSurface(&from_rect, &to_rect);
					//SDL_BlitSurface( render_img[1][j], &from_rect, dest, &to_rect);
				}
			}
}
		
void ORock::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;
	int j;
	int ys;

	//do shadows as prerender
	for(j=0,ys=0;j<3;j++,ys+=16)
			if(render_img[0][j] && render_img[0][j]->GetBaseSurface())
			{
				if(the_map.GetBlitInfo( render_img[0][j]->GetBaseSurface(), x, y+ys, from_rect, to_rect))
				{
					to_rect.x += shift_x;
					to_rect.y += shift_y;

					render_img[0][j]->BlitSurface(&from_rect, &to_rect);
					//SDL_BlitSurface( render_img[0][j], &from_rect, dest, &to_rect);
				}
			}
}

int ORock::Process()
{
	//rocks dont process!
	return 1;
}

void ORock::ChangePalette(int palette_)
{
	palette = palette_;
}

void ORock::ClearRender()
{
	int i, j;

	for(i=0;i<2;i++)
		for(j=0;j<3;j++)
			render_img[i][j] = NULL;
}

void ORock::SetDefaultRender()
{
	ClearRender();

	render_img[0][0] = &rock_vert_down_top[palette];
	render_img[0][1] = &rock_single_mid_under[palette];
	render_img[0][2] = &rock_single_bottom_under[palette];
}

void ORock::EditRockRender(ZMap &the_map, vector<ZObject*> object_list, ZObject *robj, bool do_add)
{
	bool **rock_list;
	int map_w, map_h;
	int x, y;
	int tx, ty;

	rock_list = the_map.GetRockList();

	map_w = the_map.GetMapBasics().width;
	map_h = the_map.GetMapBasics().height;

	//add
	{
		robj->GetCords(x, y);

		tx = x / 16;
		ty = y / 16;

		//a check...
		if(tx < 0) return;
		if(ty < 0) return;
		if(tx >= map_w) return;
		if(ty >= map_h) return;

		//set!
		rock_list[tx][ty] = do_add;
	}

	//now tell all the rocks to do their job
	{
		int sx, lx, sy, ly;

		sx = x - 16;
		sy = y - 32;
		lx = x + 16;
		ly = y + 32;

		for(vector<ZObject*>::iterator obj=object_list.begin(); obj!=object_list.end(); obj++)
		{
			unsigned char ot, oid;
			int ox, oy;

			(*obj)->GetObjectID(ot, oid);

			//only rocks
			if(ot != MAP_ITEM_OBJECT) continue;
			if(oid != ROCK_ITEM) continue;

			//it an effected rock?
			(*obj)->GetCords(ox, oy);

			if(ox < sx) continue;
			if(oy < sy) continue;
			if(ox > lx) continue;
			if(oy > ly) continue;

			(*obj)->SetupRockRender(rock_list, map_w, map_h);
		}
	}
}

void ORock::SetupRockRenders(ZMap &the_map, vector<ZObject*> object_list)
{
	bool **rock_list;
	int map_w, map_h;
	int i, j;

	//make the rock_list and tell all the rocks to set themselves up
	map_w = the_map.GetMapBasics().width;
	map_h = the_map.GetMapBasics().height;

	if(map_w <= 0) return;
	if(map_h <= 0) return;

	//alloc
	rock_list = (bool**)malloc(map_w * sizeof(bool *));
	for(i=0;i<map_w;i++) rock_list[i] = (bool*)malloc(map_h * sizeof(bool));

	//clear the list
	for(i=0;i<map_w;i++)
		for(j=0;j<map_h;j++)
			rock_list[i][j] = false;

	//populate the list
	for(vector<ZObject*>::iterator obj=object_list.begin(); obj!=object_list.end(); obj++)
	{
		unsigned char ot, oid;
		int x, y;
		int tx, ty;

		(*obj)->GetObjectID(ot, oid);

		//only rocks
		if(ot != MAP_ITEM_OBJECT) continue;
		if(oid != ROCK_ITEM) continue;

		(*obj)->GetCords(x, y);

		tx = x / 16;
		ty = y / 16;

		//a check...
		if(tx < 0) continue;
		if(ty < 0) continue;
		if(tx >= map_w) continue;
		if(ty >= map_h) continue;

		//set!
		rock_list[tx][ty] = true;
	}

	//now tell all the rocks to do their job
	for(vector<ZObject*>::iterator obj=object_list.begin(); obj!=object_list.end(); obj++)
	{
		unsigned char ot, oid;
//		int x, y;
//		int tx, ty;

		(*obj)->GetObjectID(ot, oid);

		//only rocks
		if(ot != MAP_ITEM_OBJECT) continue;
		if(oid != ROCK_ITEM) continue;

		(*obj)->SetupRockRender(rock_list, map_w, map_h);
	}

	//free
	for(i=0;i<map_w;i++) free(rock_list[i]);
	free(rock_list);
}

void ORock::SetupRockRender(bool **rock_list, int map_w, int map_h)
{
	int &x = loc.x;
	int &y = loc.y;
	int tx, ty;

	tx = x / 16;
	ty = y / 16;

	//clear out
	ClearRender();

	//a double check...
	if(tx < 0) return;
	if(ty < 0) return;
	if(tx >= map_w) return;
	if(ty >= map_h) return;

	//ok lets do this
	bool r, l, up, dn;
	bool dl, ddn, uup;
	bool uur, ur, dr, ddr;
	r = l = up = dn = false;

	if(tx == 0 || rock_list[tx-1][ty]) l = true;
	if(ty == 0 || rock_list[tx][ty-1]) up = true;
	if(tx == map_w-1 || rock_list[tx+1][ty]) r = true;
	if(ty == map_h-1 || rock_list[tx][ty+1]) dn = true;

	//the down left
	if(tx == 0) dl = false;
	else if(ty == map_h-1) dl = false;
	else if(rock_list[tx-1][ty+1]) dl = true;
	else dl = false;

	//the double down
	if(ty >= map_h-2) ddn = true;
	else if(rock_list[tx][ty+2]) ddn = true;
	else ddn = false;

	//the up up
	if(ty < 2) uup = true;
	else if(rock_list[tx][ty-2]) uup = true;
	else uup = false;

	//the up up right
	if(ty < 2 || tx >= map_w-1) uur = false;
	else if(rock_list[tx+1][ty-2]) uur = true;
	else uur = false;

	//the up right
	if(ty < 1 || tx >= map_w-1) ur = false;
	else if(rock_list[tx+1][ty-1]) ur = true;
	else ur = false;

	//the down right
	if(ty >= map_h-1 || tx >= map_w-1) dr = false;
	else if(rock_list[tx+1][ty+1]) dr = true;
	else dr = false;

	//the down down right
	if(ty >= map_h-2 || tx >= map_w-1) ddr = false;
	else if(rock_list[tx+1][ty+2]) ddr = true;
	else ddr = false;

	//normal center
	if(r && l && up && dn) render_img[0][0] = &rock_center_top[palette];
	//top left
	else if(r && !l && !up && dn) render_img[0][0] = &rock_up_left_top[palette];
	//top right
	else if(!r && l && !up && dn) render_img[0][0] = &rock_up_right_top[palette];
	//bottom right
	else if(!r && l && up && !dn) render_img[0][0] = &rock_down_right_top[palette];
	//bottom left
	else if(r && !l && up && !dn) render_img[0][0] = &rock_down_left_top[palette];

	//top
	else if(r && l && !up && dn) render_img[0][0] = &rock_up_top[palette];
	//bottom
	else if(r && l && up && !dn) render_img[0][0] = &rock_down_top[palette];
	//right
	else if(!r && l && up && dn) render_img[0][0] = &rock_right_top[palette];
	//left
	else if(r && !l && up && dn) render_img[0][0] = &rock_left_top[palette];

	//vert up
	else if(!r && !l && !up && dn) render_img[0][0] = &rock_vert_up_top[palette];
	//vert mid
	else if(!r && !l && up && dn) render_img[0][0] = &rock_vert_mid_top[palette];
	//vert bottom
	else if(!r && !l && up && !dn) render_img[0][0] = &rock_vert_down_top[palette];

	//horz left
	else if(r && !l && !up && !dn) render_img[0][0] = &rock_horz_left_top[palette];
	//horz mid
	else if(r && l && !up && !dn) render_img[0][0] = &rock_horz_mid_top[palette];
	//horz right
	else if(!r && l && !up && !dn) render_img[0][0] = &rock_horz_right_top[palette];
	
	//otherwise... no neighbors
	else if(!r && !l && !up && !dn) render_img[0][0] = &rock_vert_down_top[palette];

	//default the under
	//render_img[0][1] = rock_single_mid_under[palette];
	render_img[0][2] = &rock_single_bottom_under[palette];

	//do the mid under
	if(ty+1 < map_h)
	{
		if(dn)
		{
			render_img[0][1] = NULL;
		}
		else if(dl)
		{
			//we got a shadow
			if(r) render_img[0][1] = &rock_mid_mid_shadow_under[palette];
			else render_img[0][1] = &rock_right_mid_shadow_under[palette];
		}
		else
		{
			//no shadow
			if(r && l) render_img[0][1] = &rock_mid_mid_under[palette];
			else if(r && !l) render_img[0][1] = &rock_left_mid_under[palette];
			else if(!r && l) render_img[0][1] = &rock_right_mid_under[palette];
			else render_img[0][1] = &rock_single_mid_under[palette];
		}
	}

	//do the bottom under
	if(ty+2 < map_h)
	{
		if(ddn || dn)
		{
			render_img[0][2] = NULL;
		}
		else if(dl)
		{
			//we got a shadow
			if(r) render_img[0][2] = &rock_mid_bottom_shadow_under[palette];
			else render_img[0][2] = &rock_right_bottom_shadow_under[palette];
		}
		else
		{
			//no shadow
			if(r && l) render_img[0][2] = &rock_mid_bottom_under[palette];
			else if(r && !l) render_img[0][2] = &rock_left_bottom_under[palette];
			else if(!r && l) render_img[0][2] = &rock_right_bottom_under[palette];
			else render_img[0][2] = &rock_single_bottom_under[palette];
		}
	}

	//extra shadows
	if(tx < map_w-1)
	{
		if(!(uur || ur || r))
		{
			if(up && !uup)
				render_img[1][0] = &rock_up_shadow[palette];
			else if(up || uup)
				render_img[1][0] = &rock_mid_shadow[palette];
		}

		if(!dn)
		if(!(ur || r || dr))
		{
			if(!up)
				render_img[1][1] = &rock_up_shadow[palette];
			else
				render_img[1][1] = &rock_mid_shadow[palette];
		}

		if(!dn && !ddn)
		if(!(r || dr || ddr)) render_img[1][2] = &rock_bottom_shadow[palette];
	}
}

void ORock::SetOwner(team_type owner_)
{
	owner = NULL_TEAM;
}

void ORock::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	int small_particles;
	int mid_particles;
	int large_particles;
	int i;

	if(!effect_list) return;

	small_particles = 12 + (rand() % 6);
	mid_particles = 4 + (rand() % 3);
	large_particles = (rand() % 2);

	for(i=0;i<small_particles;i++)
		effect_list->push_back((ZEffect*)(new ERockParticle(ztime, loc.x, loc.y, palette, ROCK_SMALL_PARTICLE, 80, 60)));

	for(i=0;i<mid_particles;i++)
		effect_list->push_back((ZEffect*)(new ERockParticle(ztime, loc.x, loc.y, palette, ROCK_MID_PARTICLE, 40, 40)));

	for(i=0;i<large_particles;i++)
		effect_list->push_back((ZEffect*)(new ERockTurrent(ztime, loc.x, loc.y, palette, 140, 140)));
}

bool ORock::CausesImpassAtCoord(int x, int y)
{
	return x == loc.x && (y == loc.y + 32);
}

void ORock::SetMapImpassables(ZMap &tmap)
{
	int tx, ty;

	tx = loc.x / 16;
	ty = loc.y / 16;

	tmap.SetImpassable(tx, ty+2, true, true);
}

void ORock::UnSetMapImpassables(ZMap &tmap)
{
	int tx, ty;

	tx = loc.x / 16;
	ty = loc.y / 16;

	tmap.SetImpassable(tx, ty+2, false, true);
}

void ORock::CreationMapEffects(ZMap &tmap)
{

}

void ORock::DeathMapEffects(ZMap &tmap)
{
	int tx, ty;

	ZSDL_Surface *surface;

	tx = loc.x;
	ty = loc.y + 32;

	//extra checks are good
	if(tx < 0) return;
	if(ty < 0) return;
	if(tx + 16 > tmap.GetMapBasics().width * 16) return;
	if(ty + 16 > tmap.GetMapBasics().height * 16) return;

	//don't place if something was already placed there
	if(tmap.CoordStamped(tx, ty)) return;

	surface = &rock_destroyed[palette][rand()%6];

	tmap.PermStamp(tx, ty, surface, false);
}
