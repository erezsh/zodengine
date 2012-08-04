#include <stdio.h>

#include "zmap.h"
#include "common.h"

ZSDL_Surface ZMap::planet_template[MAX_PLANET_TYPES];
palette_tile_info ZMap::planet_tile_info[MAX_PLANET_TYPES][MAX_PLANET_TILES];
vector<unsigned int> ZMap::map_water_plist[MAX_PLANET_TYPES];
vector<unsigned int> ZMap::map_water_effect_plist[MAX_PLANET_TYPES];
ZSDL_Surface ZMap::zone_marker[MAX_TEAM_TYPES];
ZSDL_Surface ZMap::zone_marker_water[MAX_TEAM_TYPES];
SDL_mutex *ZMap::init_mutex = SDL_CreateMutex();

//Mix_Music *ZMap::music[MAX_PLANET_TYPES];

using namespace COMMON;

ZMap::ZMap()
{
	//full_render = NULL;
	last_shift_time = 0;
	submerge_info_setup = false;
	rock_list_setup = false;
	stamp_list_setup = false;
	stamp_list_w = -1;
	stamp_list_h = -1;
	map_data = NULL;
	map_data_size = 0;
	
	ClearMap();
}

ZMap::~ZMap()
{
	DeRenderMap();
	DeletePathfindingInfo();
	DeleteSubmergeAmounts();
	DeleteRockList();
	DeleteStampList();
	FreeMapData();
}

void ZMap::Init()
{
	FILE *fp;
	int i,j;
	int ret;

	for(i=0;i<MAX_TEAM_TYPES;i++)
	{
		string filename;
		
		filename = "assets/planets/zone_marker_" + team_type_string[i] + ".png";
		//zone_marker[i].LoadBaseImage(filename);// = IMG_Load_Error ( filename );
		ZTeam::LoadZSurface(i, zone_marker[ZTEAM_BASE_TEAM], zone_marker[i], filename);
		
		filename = "assets/planets/zone_marker_water_" + team_type_string[i] + ".png";
		//zone_marker_water[i].LoadBaseImage(filename);// = IMG_Load_Error ( filename );
		ZTeam::LoadZSurface(i, zone_marker_water[ZTEAM_BASE_TEAM], zone_marker_water[i], filename);
	}
	
	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		string filename;
		
		//filename = "assets/sounds/music_" + planet_type_string[i] + ".mp3";
		//music[i] = MUS_Load_Error ( filename.c_str() );


		//load BMP palette
		filename = "assets/planets/" + planet_type_string[i] + ".bmp";
		planet_template[i].LoadBaseImage(filename);// = SDL_LoadBMP ( filename.c_str() );
		
		//if(!planet_template[i])
		//	printf("unable to load:%s\n", filename.c_str());
		
		//clear palette tile info
		for(j=0;j<MAX_PLANET_TILES;j++)
		{
			planet_tile_info[i][j].is_water = false;
			planet_tile_info[i][j].is_passable = true;
			planet_tile_info[i][j].is_usable = true;
			planet_tile_info[i][j].is_road = false;
			planet_tile_info[i][j].is_effect = false;
			planet_tile_info[i][j].is_water_effect = false;
			planet_tile_info[i][j].next_tile_in_effect = 0;
		}
		
		//load palette tile info
		LoadPaletteInfo(i);
		
		//now set some ptile info
		for(j=0;j<MAX_PLANET_TILES;j++)
		{
			if(!planet_tile_info[i][j].is_usable) continue;
			
			if(planet_tile_info[i][j].is_water && !planet_tile_info[i][j].is_effect)
				map_water_plist[i].push_back(j);
			
			if(planet_tile_info[i][j].is_water_effect) 
				map_water_effect_plist[i].push_back(j);
		}
	}

	//load up crater graphics
	ZMapCraterGraphics::Init();
}

void ZMap::ServerInit()
{
	int i;

	for(i=0;i<MAX_PLANET_TYPES;i++)
		LoadPaletteInfo(i);
}

void ZMap::LoadPaletteInfo(int terrain_type)
{
	FILE *fp;
	int &i = terrain_type;
	int ret;
	string filename;

	SDL_LockMutex(init_mutex);

	filename = "assets/planets/" + planet_type_string[i] + ".tileinfo";
	fp = fopen(filename.c_str(), "rb");
	
	if(!fp)
	{
		printf("unable to load:%s\n", filename.c_str());
		
		//so we'll write this default one...
		WriteMapPaletteTileInfo((planet_type)i);
	}
	
	//read em in
	ret = fread(planet_tile_info[i], sizeof(palette_tile_info), MAX_PLANET_TILES,  fp);
	
	if(ret != MAX_PLANET_TILES)
		printf("unable to fully load:%s (loaded %d tiles)\n", filename.c_str(), ret);
	
	fclose(fp);

	SDL_UnlockMutex(init_mutex);
}

//void ZMap::PlayMusic(bool normal)
//{
//	//are we about to lose our fort?
//	if(normal)
//		ZSDL_PlayMusic(music[basic_info.terrain_type], -1);
//	else
//		;
//
//	Mix_VolumeMusic(128);
//}

bool ZMap::Loaded()
{
	return file_loaded;
}

ZSDL_Surface *ZMap::GetZoneMarkers()
{
	return zone_marker;
}

int ZMap::GetPaletteTile(int x, int y)
{
	int ret;
	
	x /= 16;
	y /= 16;
	
	ret = (y*20) + x;
	
	if(ret >= MAX_PLANET_TILES)
		ret = -1;
	else if(ret < 0)
		ret = -1;
	
	return ret;
}

palette_tile_info &ZMap::GetMapPaletteTileInfo(planet_type palette, int tile)
{
	return planet_tile_info[palette][tile];
}

int ZMap::WriteMapPaletteTileInfo(planet_type palette)
{
	FILE *fp;
	string filename;
	
	filename = "assets/planets/" + planet_type_string[palette] + ".tileinfo";
	
	fp = fopen(filename.c_str(), "wb");
	
	if(!fp) return 0;
	
	fwrite(planet_tile_info[palette], sizeof(palette_tile_info), MAX_PLANET_TILES, fp);
	
	fclose(fp);
	
	return 1;
}

int ZMap::UpdatePalettesTileFormat()
{
	FILE *fp;
	string filename;
	palette_tile_info_new planet_tile_info_new[MAX_PLANET_TYPES][MAX_PLANET_TILES];

	for(int i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/planets/" + planet_type_string[i] + ".tileinfo";

		fp = fopen(filename.c_str(), "wb");
	
		if(!fp)
		{
			printf("ZMap::UpdatePalettesTileFormat: could update '%s'\n", filename.c_str());
			continue;
		}

		for(int j=0;j<MAX_PLANET_TILES;j++)
 		{
 			planet_tile_info_new[i][j].is_water = planet_tile_info[i][j].is_water;
 			planet_tile_info_new[i][j].is_passable = planet_tile_info[i][j].is_passable;
 			planet_tile_info_new[i][j].is_usable = planet_tile_info[i][j].is_usable;
 			planet_tile_info_new[i][j].is_road = planet_tile_info[i][j].is_road;
 			planet_tile_info_new[i][j].is_effect = planet_tile_info[i][j].is_effect;
 			planet_tile_info_new[i][j].is_water_effect = planet_tile_info[i][j].is_water_effect;
 			planet_tile_info_new[i][j].next_tile_in_effect = planet_tile_info[i][j].next_tile_in_effect;
 			planet_tile_info_new[i][j].is_starter_tile = planet_tile_info[i][j].is_starter_tile;
			planet_tile_info_new[i][j].takes_tank_tracks = false;
			planet_tile_info_new[i][j].crater_type = -1;
 		}

		fwrite(planet_tile_info_new[i], sizeof(palette_tile_info_new), MAX_PLANET_TILES, fp);

		fclose(fp);
	}
	
	return 1;
}

ZSDL_Surface &ZMap::GetMapPalette(planet_type palette)
{
	return planet_template[palette];
}

ZSDL_Surface &ZMap::GetRender()
{
	if(!full_render.GetBaseSurface())
	{
		//is a file loaded to render?
		if(!file_loaded) return full_render;//return NULL;
		else RenderMap();
		
		//it still not rendered?
		if(!full_render.GetBaseSurface()) return full_render;//return NULL;
	}
	
	return full_render;
}

void ZMap::DoRender(SDL_Surface *dest, int shift_x_dest, int shift_y_dest)
{
	SDL_Rect to_rect;
	SDL_Rect from_rect;
	
	from_rect.x = shift_x;
	from_rect.y = shift_y;
	from_rect.w = view_w;
	from_rect.h = view_h;
	
	to_rect.x = shift_x_dest;
	to_rect.y = shift_y_dest;

	GetRender().BlitSurface(&from_rect, &to_rect);
	//SDL_BlitSurface(GetRender(), &from_rect, dest, &to_rect);
}

void ZMap::RenderMap()
{
	int i,j,k;
	
	if(!file_loaded) return;
	
	if(full_render.GetBaseSurface()) DeRenderMap();
	
	//begin.
	//full_render = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, basic_info.width * 16, basic_info.height * 16, 32, 0xFF000000, 0x0000FF00, 0x00FF0000, 0x000000FF);
	//full_render.LoadBaseImage(SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, basic_info.width * 16, basic_info.height * 16, 32, 0xFF000000, 0x0000FF00, 0x00FF0000, 0x000000FF), false);
	full_render.LoadNewSurface(basic_info.width * 16, basic_info.height * 16);

	//SDL_Rect the_box;
	//the_box.x = 0;
	//the_box.y = 0;
	//the_box.w = basic_info.width * 16;
	//the_box.h = basic_info.height * 16;
	//ZSDL_FillRect(&the_box, 0, 0, 0, &full_render);
	
	if(!planet_template[basic_info.terrain_type].GetBaseSurface())
	{
		printf("could not render map because terrain_type:%s was not previously loaded\n", planet_type_string[basic_info.terrain_type].c_str());
		return;
	}
	
	for(i=k=0;i<basic_info.width;i++)
		for(j=0;j<basic_info.height;j++,k++)
			RenderTile(k);
}

void ZMap::RenderTile(unsigned int index)
{
	int x, y;
	SDL_Rect from_rect, to_rect;
		
	//get the tile cords
	if(!GetPaletteTile(tile_list[index].tile,x,y)) return;
		
	from_rect.w = 16;
	from_rect.h = 16;
	from_rect.x = x;
	from_rect.y = y;
		
	GetTile(index, x, y);
	to_rect.w = 16;
	to_rect.h = 16;
	to_rect.x = x;
	to_rect.y = y;
		
// 		printf("RenderMap:from(%d,%d) to(%d,%d)\n", from_rect.x, from_rect.y, to_rect.x, to_rect.y);
		
	//blit tile to the image
	planet_template[basic_info.terrain_type].BlitSurface(&from_rect, &to_rect, &full_render);
	//SDL_BlitSurface(planet_template[basic_info.terrain_type].GetBaseSurface(), &from_rect, full_render.GetBaseSurface(), &to_rect);
}



void ZMap::DebugMapInfo()
{
	if(!file_loaded)
	{
		printf("DebugMapInfo::map not loaded\n");
		return;
	}
	
	printf("\nDebugMapInfo...\n");
	printf("Map name:%s\n", basic_info.map_name);
	printf("Map width:%d\n", basic_info.width);
	printf("Map height:%d\n", basic_info.height);
	printf("Map player_count:%d\n", basic_info.player_count);
	printf("Map object_count:%d\n", basic_info.object_count);
	printf("Map zone_count:%d\n", basic_info.zone_count);
	printf("Map terrain_type:%s\n", planet_type_string[basic_info.terrain_type].c_str());
	
	printf("\n");
}

void ZMap::MakeNewMap(const char *new_name, planet_type palette, int width, int height)
{
	vector<unsigned short> start_tile_list;
	
	if(file_loaded) ClearMap();
	
	basic_info.width = width;
	basic_info.height = height;
	strcpy(basic_info.map_name, new_name);
	basic_info.player_count = 2;
	basic_info.object_count = 0;
	basic_info.terrain_type = palette;
	basic_info.zone_count = 0;
		
	//find all starter tiles
	for(int i=0;i<MAX_PLANET_TILES;i++)
	{
		palette_tile_info &t_info = planet_tile_info[basic_info.terrain_type][i];
			
		if(t_info.is_starter_tile) start_tile_list.push_back(i);
	}
	
	if(!start_tile_list.size())
	{
		printf("MakeNewMap::Palette %s has no starter tiles set\n", planet_type_string[basic_info.terrain_type].c_str());
	}
	
	for(int i=0;i< basic_info.width * basic_info.height;i++)
	{
		map_tile temp_tile;
		int new_tile;

		if(start_tile_list.size())
		{
			new_tile = rand() % start_tile_list.size();
			new_tile = start_tile_list[new_tile];
		}
		else
		{
			new_tile = 0;
		}
		
		temp_tile.tile = new_tile;
		
		tile_list.push_back(temp_tile);
	}
	
	file_loaded = true;
	InitEffects();
}

void ZMap::ReplaceUnusableTiles()
{
	vector<unsigned short> start_tile_list;

	for(int i=0;i<MAX_PLANET_TILES;i++)
	{
		palette_tile_info &t_info = planet_tile_info[basic_info.terrain_type][i];
			
		if(t_info.is_starter_tile) start_tile_list.push_back(i);
	}

	for(int i=0;i< basic_info.width * basic_info.height;i++)
	{
		//map_tile temp_tile;
		int new_tile;

		palette_tile_info &t_info = planet_tile_info[basic_info.terrain_type][tile_list[i].tile];
		if(t_info.is_usable) continue;

		if(start_tile_list.size())
		{
			new_tile = rand() % start_tile_list.size();
			new_tile = start_tile_list[new_tile];
		}
		else
		{
			new_tile = 0;
		}

		tile_list[i].tile = new_tile;
		
		//temp_tile.tile = new_tile;
		
		//tile_list.push_back(temp_tile);
	}
}

void ZMap::MakeRandomMap()
{
	if(file_loaded) ClearMap();
	
	basic_info.width = 60;
	basic_info.height = 180;
	strcpy(basic_info.map_name, "Random Map");
	basic_info.player_count = 2;
	basic_info.object_count = 0;
	basic_info.terrain_type = DESERT;
	basic_info.zone_count = 0;
	
	for(int i=0;i< basic_info.width * basic_info.height;i++)
	{
		map_tile temp_tile;
		int new_tile;
		
		while(1)
		{
			new_tile = rand() % MAX_PLANET_TILES;
			
			if(!planet_tile_info[basic_info.terrain_type][new_tile].is_usable) continue;
			if(!planet_tile_info[basic_info.terrain_type][new_tile].is_passable) continue;
			
			break;
		}

		tile_list[i].tile = new_tile;
		
		temp_tile.tile = new_tile;
		
		tile_list.push_back(temp_tile);
	}
	
	file_loaded = true;
	InitEffects();
}

bool ZMap::CheckLoad()
{
	//got as many tiles as we should?
	if(tile_list.size() != basic_info.width * basic_info.height)
	{
		printf("loaded map does not have right amount of tile information\n");
		return false;
	}
	
	//do all the tiles check out?
	for(vector<map_tile>::iterator i=tile_list.begin(); i != tile_list.end(); ++i)
	{
		if(i->tile > MAX_PLANET_TILES)
		{
			printf("loaded map has bad tiles\n");
			return false;
		}
	}
	
	//is it a good palette setting?
	if(basic_info.terrain_type >= MAX_PLANET_TYPES)
	{
		printf("loaded map has a bad terrain palette setting\n");
		return false;
	}
	
	return true;
}

int ZMap::GetPaletteTile(unsigned short index, int &x, int &y)
{
	//20x24 tiles, 16x16 pixels
	
	if(index >= MAX_PLANET_TILES)
	{
		printf("GetPaletteTile:requested invalid index:%d\n", index);
		return 0;
	}
	
	
	y = index / 20;
	x = index % 20;
	
	x *= 16;
	y *= 16;
	
	return 1;
}

void ZMap::GetTile(unsigned int index, int &x, int &y, bool is_shifted)
{
	y = index / basic_info.width;
	x = index % basic_info.width;
	
	x *= 16;
	y *= 16;
	
	if(is_shifted)
	{
		x -= shift_x;
		y -= shift_y;
	}
}

int ZMap::GetTileIndex(int x, int y, bool is_shifted)
{
	if(is_shifted)
	{
		x += shift_x;
		y += shift_y;
	}
	
	if(x >= basic_info.width * 16) return -1;
	if(y >= basic_info.height * 16) return -1;
	if(x < 0) return -1;
	if(y < 0) return -1;
	
	x /= 16;
	y /= 16;
	
	return (y*basic_info.width)+x;
}

map_tile &ZMap::GetTile(int x, int y, bool is_shifted)
{
	int index = GetTileIndex(x, y, is_shifted);
	
	if(index != -1)
		return tile_list[index];
	else
		return tile_list[0];
}

map_tile &ZMap::GetTile(unsigned int index)
{
	return tile_list[index];
}

bool ZMap::CoordIsRoad(int x, int y)
{
	int tile = GetTileIndex(x, y);

	if(tile == -1)return false;

	return planet_tile_info[basic_info.terrain_type][tile_list[tile].tile].is_road;
}

double ZMap::GetTileWalkSpeed(int x, int y, bool is_shifted)
{
	int index = GetTileIndex(x, y, is_shifted);
	
	if(index != -1)
	{
		map_tile &t = tile_list[index];
		palette_tile_info &t_info = planet_tile_info[basic_info.terrain_type][t.tile];

		if(!t_info.is_passable) return 0;

		if(t_info.is_road) return ROAD_SPEED;
		else if(t_info.is_water) return WATER_SPEED;
		else return 1.0;
	}
	else
		return 0;
}

void ZMap::FreeMapData()
{
	if(map_data) free(map_data);

	map_data = NULL;
	map_data_size = 0;
}

void ZMap::DeRenderMap()
{
	full_render.Unload();
	//if(full_render)
	//{
	//	SDL_FreeSurface(full_render);
	//	full_render = NULL;
	//}
}

void ZMap::ClearMap()
{
	file_loaded = false;

	basic_info.clear();
	zone_list.clear();
	object_list.clear();
	tile_list.clear();
	map_effect_list.clear();
	map_water_list.clear();
	
	shift_x = shift_y = 0;
	view_w = view_h = 0;

	width_pix = 0;
	height_pix = 0;
	
	DeRenderMap();
	DeletePathfindingInfo();
	DeleteSubmergeAmounts();
	DeleteRockList();
	DeleteStampList();
	FreeMapData();
}

void ZMap::InitEffects()
{
	unsigned int i;
	map_effect_list.clear();
	map_water_list.clear();
	
	//find all tiles which are apart of an effect and water tiles
	for(i=0; i<tile_list.size(); ++i)
	{
		map_tile &t = tile_list[i];
		palette_tile_info &t_info = planet_tile_info[basic_info.terrain_type][t.tile];
		
		if(!t_info.is_usable) continue;

		//we want to clear out all water tiles that start off as effects
		if(basic_info.terrain_type == DESERT)
		if(t_info.is_water && t_info.is_effect && map_water_plist[basic_info.terrain_type].size()) 
		{
			int new_tile;

			new_tile = rand() % map_water_plist[basic_info.terrain_type].size();
			new_tile = map_water_plist[basic_info.terrain_type][new_tile];
			t.tile = new_tile;

			map_water_list.push_back(i);
			continue;
		}
		
		if(t_info.is_effect)
			map_effect_list.push_back(i);
		
		if(t_info.is_water && !t_info.is_effect) 
		{
			map_water_list.push_back(i);
		}
	}
	
	//other stuff that needs init'd with a file load
	width_pix = basic_info.width * 16;
	height_pix = basic_info.height * 16;
	InitPathfinding();
	InitSubmergeAmounts();
	InitRockList();
	InitStampList();
	SetupAllZoneInfo();
	RebuildRegions();
}

int ZMap::DoEffects(double the_time, SDL_Surface *dest, int shift_x, int shift_y)
{
	double min_interval_time = 0.2;

	switch(basic_info.terrain_type)
	{
	case VOLCANIC:
		min_interval_time = 0.5;
		break;
	case ARCTIC:
		min_interval_time = 0.4;
		break;
	case DESERT:
	default:
		min_interval_time = 0.2;
		break;
	}
	
	//goto next frame and render
	for(vector<map_effect_info>::iterator i=map_effect_list.begin(); i != map_effect_list.end();)
	{
		unsigned int map_tile = i->tile;
		SDL_Rect src, dest;
		int x, y;

		palette_tile_info &p_info = planet_tile_info[basic_info.terrain_type][tile_list[map_tile].tile];


		//erasing is annoying
		if(the_time < i->next_effect_time)
		{
			//blit new tile
			src.w = 16;
			src.h = 16;
			dest.w = 16;
			dest.h = 16;
			GetPaletteTile(tile_list[map_tile].tile, x, y);
			src.x = x;
			src.y = y;
			GetTile(map_tile, x, y);
			dest.x = x;
			dest.y = y;

			SDL_Rect from_rect, to_rect;
			if(GetBlitInfo(dest.x,dest.y,16,16, from_rect, to_rect))
			{
				from_rect.x += src.x;
				from_rect.y += src.y;

				to_rect.x += shift_x;
				to_rect.y += shift_y;

				planet_template[basic_info.terrain_type].BlitSurface(&from_rect, &to_rect);
			}

			i++;
			continue;
		}

		i->next_effect_time = the_time + min_interval_time + ((rand() % 4) * 0.033);

		//unsigned int map_tile = i->tile;
		//SDL_Rect src, dest;
		//int x, y;
		//bool do_erase = false;
		
		//set next
// 			printf("ZMap::DoEffects mtile:%d %d to %d\n", map_tile, tile_list[map_tile].tile, planet_tile_info[basic_info.terrain_type][tile_list[map_tile].tile].next_tile_in_effect);
		tile_list[map_tile].tile = planet_tile_info[basic_info.terrain_type][tile_list[map_tile].tile].next_tile_in_effect;
		
		//is it a water effect start tile
		//(therefor remove it from the list and replace it with a water tile)
		if(planet_tile_info[basic_info.terrain_type][tile_list[map_tile].tile].is_water_effect)
		{
			int new_tile;
			
			new_tile = rand() % map_water_plist[basic_info.terrain_type].size();
			new_tile = map_water_plist[basic_info.terrain_type][new_tile];
			tile_list[map_tile].tile = new_tile;
			
			//do_erase = true;

			i = map_effect_list.erase(i);
			continue;
		}
		
		//blit new tile
		src.w = 16;
		src.h = 16;
		dest.w = 16;
		dest.h = 16;
		GetPaletteTile(tile_list[map_tile].tile, x, y);
		src.x = x;
		src.y = y;
		GetTile(map_tile, x, y);
		dest.x = x;
		dest.y = y;
		
// 			printf("doing map effect Ptile:%d Mtile:%d (%d,%d) to (%d,%d)\n", tile_list[map_tile].tile, map_tile, src.x, src.y, dest.x, dest.y);
		
		SDL_Rect from_rect, to_rect;
		if(GetBlitInfo(dest.x,dest.y,16,16, from_rect, to_rect))
		{
			from_rect.x += src.x;
			from_rect.y += src.y;

			to_rect.x += shift_x;
			to_rect.y += shift_y;

			planet_template[basic_info.terrain_type].BlitSurface(&from_rect, &to_rect);
		}

		//planet_template[basic_info.terrain_type].BlitSurface(&src, &dest, &full_render);
		//SDL_BlitSurface(planet_template[basic_info.terrain_type], &src, full_render, &dest);
		
		i++;

		//if(do_erase)  i = map_effect_list.erase(i);
		//else ++i;
	}
	
	//start a water effect?
	if(map_water_effect_plist[basic_info.terrain_type].size())
	for(vector<map_effect_info>::iterator i=map_water_list.begin(); i != map_water_list.end(); ++i)
	{
		if(the_time < i->next_effect_time) continue;

		i->next_effect_time = the_time + min_interval_time + ((rand() % 4) * 0.033);

		unsigned int map_tile = i->tile;
		palette_tile_info &p_info = planet_tile_info[basic_info.terrain_type][tile_list[map_tile].tile];
		
		//is it available?
		if(!p_info.is_effect)
		{
			unsigned int new_tile;
			SDL_Rect src, dest;
			int x,y;
			
			//one in...
			if(rand() % 40) continue;
			
			//add to effects list
			map_effect_list.push_back(map_tile);
			
// 				printf("map_water_effect_list.size():%d\n", map_water_effect_list.size());
			
			new_tile = rand() % map_water_effect_plist[basic_info.terrain_type].size();
			new_tile = map_water_effect_plist[basic_info.terrain_type][new_tile];
			
			tile_list[map_tile].tile = new_tile;
			
			////blit new tile
			//src.w = 16;
			//src.h = 16;
			//dest.w = 16;
			//dest.h = 16;
			//GetPaletteTile(tile_list[map_tile].tile, x, y);
			//src.x = x;
			//src.y = y;
			//GetTile(map_tile, x, y);
			//dest.x = x;
			//dest.y = y;
			//
			////render to the screen instead
			//SDL_Rect from_rect, to_rect;
			//if(GetBlitInfo(dest.x,dest.y,16,16, from_rect, to_rect))
			//{
			//	from_rect.x += src.x;
			//	from_rect.y += src.y;
			//	planet_template[basic_info.terrain_type].BlitSurface(&from_rect, &to_rect);
			//}
				
			//planet_template[basic_info.terrain_type].BlitSurface(&src, &dest, &full_render);
			//SDL_BlitSurface(planet_template[basic_info.terrain_type], &src, full_render, &dest);
		}
	}
		
	return 1;
}

bool ZMap::GetMapData(char *&data, int &size)
{
	data = map_data;
	size = map_data_size;

	return true;
}

int ZMap::Read(char* data, int size, bool scrap_data)
{
	int item_size;
	int i;
	
	//scrap it
	if(scrap_data) ClearMap();
	
	//sanity
	if(!data) return 0;
	if(size <= 0) return 0;
	
	//read in basic info
	item_size = sizeof(map_basics);
	if(size < item_size) return 0;
	memcpy(&basic_info, data, item_size);
	size -= item_size;
	data += item_size;
	
	//read in zones
	for(i=0;i<basic_info.zone_count;i++)
	{
		map_zone temp_zone;
		
		item_size = sizeof(map_zone);
		if(size < item_size) return 0;
		memcpy(&temp_zone, data, item_size);
		size -= item_size;
		data += item_size;

		zone_list.push_back(temp_zone);
	}
	
	//read in objects
	for(i=0;i<basic_info.object_count;i++)
	{
		map_object temp_object;
		
		item_size = sizeof(map_object);
		if(size < item_size) return 0;
		memcpy(&temp_object, data, item_size);
		size -= item_size;
		data += item_size;
		
		object_list.push_back(temp_object);
	}
	
	//read in tile info
	int tile_count = basic_info.width * basic_info.height;
	for(i=0;i<tile_count;i++)
	{
		map_tile temp_tile;
		
		item_size = sizeof(map_tile);
		if(size < item_size) return 0;
		memcpy(&temp_tile, data, item_size);
		size -= item_size;
		data += item_size;
		
		tile_list.push_back(temp_tile);
	}
	
	//is the in data good?
	if(CheckLoad()) 
	{
		file_loaded = true;
		InitEffects();
	}
	
	return 1;
}

int ZMap::Read(const char* filename)
{
	FILE *fp;
	int ret;
	unsigned int i;
	
	//scrap it
	ClearMap();
	
	//sanity checks
	if(!filename) return 0;
	if(!filename[0]) return 0;
	
	fp = fopen(filename, "rb");
	
	if(!fp) return 0;

	const int buf_size = 1024;
	char buf[buf_size];

	while(ret = fread(buf, 1, buf_size, fp))
	{
		//resize
		if(!map_data) 
			map_data = (char*)malloc(ret);
		else
			map_data = (char*)realloc(map_data, map_data_size + ret);

		memcpy(map_data + map_data_size, buf, ret);
		map_data_size += ret;
	}
	
	fclose(fp);

	return Read(map_data, map_data_size, false);

	/*
	//read in basic info
	ret = fread(&basic_info, 1, sizeof(map_basics), fp);
	if(!ret) return 0;
	
	//read in zones
	for(i=0;i<basic_info.zone_count;i++)
	{
		map_zone temp_zone;
		
		ret = fread(&temp_zone, 1, sizeof(map_zone), fp);
		if(!ret) return 0;
		
		zone_list.push_back(temp_zone);
	}
	
	//read in objects
	for(i=0;i<basic_info.object_count;i++)
	{
		map_object temp_object;
		//map_object_old temp_object_old;
		
		//ret = fread(&temp_object_old, 1, sizeof(map_object_old), fp);
		ret = fread(&temp_object, 1, sizeof(map_object), fp);
		if(!ret) return 0;

		//temp_object.blevel = temp_object_old.blevel;
		//temp_object.x = temp_object_old.x;
		//temp_object.y = temp_object_old.y;
		//temp_object.owner = temp_object_old.owner;
		//temp_object.object_type = temp_object_old.object_type;
		//temp_object.object_id = temp_object_old.object_id;
		//temp_object.extra_links = 0;
		//temp_object.health_percent = 100;
		
		object_list.push_back(temp_object);
	}
	
	//read in tile info
	int tile_count = basic_info.width * basic_info.height;
	for(i=0;i<tile_count;i++)
	{
		map_tile temp_tile;
		
		ret = fread(&temp_tile, 1, sizeof(map_tile), fp);
		if(!ret) return 0;
		
		tile_list.push_back(temp_tile);
	}
	
	//is the in data good?
	if(CheckLoad()) 
	{
		file_loaded = true;
		InitEffects();
	}

	//clean up
	fclose(fp);

	return 1;
	*/
}

int ZMap::Write(const char* filename)
{
	FILE *fp;
	int ret;
	
	//sanity checks
	if(!filename) return 0;
	if(!filename[0]) return 0;
	
	fp = fopen(filename, "wb");
	
	if(!fp) return 0;
	
	//lets just make this double sure...
	basic_info.zone_count = zone_list.size();
	basic_info.object_count = object_list.size();
	if(basic_info.width * basic_info.height != tile_list.size())
		printf("ZMap::Write::warning width * height != tile_list.size\n");
	
	ret = fwrite(&basic_info, sizeof(map_basics), 1, fp);
	
	if(!ret) return 0;

	//write zone_list
	for(vector<map_zone>::iterator i=zone_list.begin(); i != zone_list.end(); ++i)
	{
		ret = fwrite(&*i, sizeof(map_zone), 1, fp);
		if(!ret) return 0;
	}
	
	//write object_list map_object
	for(vector<map_object>::iterator i=object_list.begin(); i != object_list.end(); ++i)
	{
		ret = fwrite(&*i, sizeof(map_object), 1, fp);
		if(!ret) return 0;
	}
	
	//write tile_list map_tile
	for(vector<map_tile>::iterator i=tile_list.begin(); i != tile_list.end(); ++i)
	{
		ret = fwrite(&*i, sizeof(map_tile), 1, fp);
		if(!ret) return 0;
	}
	
	fclose(fp);
	return 1;
}

map_basics &ZMap::GetMapBasics()
{
	return basic_info;
}

void ZMap::ChangeTile(unsigned int index, map_tile new_tile)
{
	//take the old tile off any lists
	map_tile &to = tile_list[index];
	palette_tile_info &to_info = planet_tile_info[basic_info.terrain_type][to.tile];
		
	if(to_info.is_usable)
	{
		if(to_info.is_effect)
		{
			//map_effect_list.push_back(index);
			for(vector<map_effect_info>::iterator i=map_effect_list.begin();i!=map_effect_list.end();)
			{
				//if(*i == index)
				if(i->tile == index)
					i = map_effect_list.erase(i);
				else
					i++;
			}
		}
			
		if(to_info.is_water && !to_info.is_effect) 
		{
			//map_water_list.push_back(index);
			for(vector<map_effect_info>::iterator i=map_water_list.begin();i!=map_water_list.end();)
			{
				//if(*i == index)
				if(i->tile == index)
					i = map_water_list.erase(i);
				else
					i++;
			}
		}
	}
	
	//place
	tile_list[index] = new_tile;
	
	//does it need to enter a list?
	map_tile &t = tile_list[index];
	palette_tile_info &t_info = planet_tile_info[basic_info.terrain_type][t.tile];
		
	if(t_info.is_usable)
	{
		if(t_info.is_effect) map_effect_list.push_back(index);
			
		if(t_info.is_water && !t_info.is_effect) 
		{
			map_water_list.push_back(index);
		}
	}
	
	//rerender
	if(full_render.GetBaseSurface())
		RenderTile(index);
}

void ZMap::SetViewingDimensions(int w, int h)
{
	view_w = w;
	view_h = h;
	
	//fix them
	int &x = shift_x;
	int &y = shift_y;
	int full_height = (basic_info.height * 16);
	int full_width = (basic_info.width * 16);
	if(y > full_height - view_h) y = full_height - view_h;
	if(x > full_width - view_w) x = full_width - view_w;
	if(x < 0) x = 0;
	if(y < 0) y = 0;
}

void ZMap::SetViewShift(int x_, int y_)
{
	int full_height, full_width;
	
	full_height = (basic_info.height * 16);
	full_width = (basic_info.width * 16);

	// some old version of this function that was never used
	////fix them
	//if(y > full_height - view_h) y = full_height - view_h;
	//if(x > full_width - view_w) x = full_width - view_w;
	//if(x < 0) x = 0;
	//if(y < 0) y = 0;
	//
	//
	//x = shift_x;
	//y = shift_y;

	int &x = shift_x;
	int &y = shift_y;

	//set
	x = x_;
	y = y_;

	//fix
	if(x > full_width - view_w) x = full_width - view_w;
	if(x < 0) x = 0;
	if(y > full_height - view_h) y = full_height - view_h;
	if(y < 0) y = 0;
}

bool ZMap::ShiftViewRight(int amt)
{
	bool was_fixed = false;
	shift_x += amt;
	
	//fix them
	int &x = shift_x;
	int full_width = (basic_info.width * 16);
	if(x > full_width - view_w)
	{
		x = full_width - view_w;
		was_fixed = true;
	}
	if(x < 0)
	{
		x = 0;
		was_fixed = true;
	}

	//did it do the action without fixing?
	return !was_fixed;
}

bool ZMap::ShiftViewUp(int amt)
{
	bool was_fixed = false;
	shift_y -= amt;
	
	//fix them
	int &y = shift_y;
	int full_height = (basic_info.height * 16);
	if(y > full_height - view_h)
	{
		y = full_height - view_h;
		was_fixed = true;
	}
	if(y < 0)
	{
		y = 0;
		was_fixed = true;
	}

	//did it do the action without fixing?
	return !was_fixed;
}

bool ZMap::ShiftViewDown(int amt)
{
	bool was_fixed = false;
	shift_y += amt;

	//fix them
	int &y = shift_y;
	int full_height = (basic_info.height * 16);
	if(y > full_height - view_h)
	{
		y = full_height - view_h;
		was_fixed = true;
	}
	if(y < 0)
	{
		y = 0;
		was_fixed = true;
	}

	//did it do the action without fixing?
	return !was_fixed;
}

bool ZMap::ShiftViewLeft(int amt)
{
	bool was_fixed = false;
	shift_x -= amt;
	
	//fix them
	int &x = shift_x;
	int full_width = (basic_info.width * 16);
	if(x > full_width - view_w)
	{
		x = full_width - view_w;
		was_fixed = true;
	}
	if(x < 0)
	{
		x = 0;
		was_fixed = true;
	}

	//did it do the action without fixing?
	return !was_fixed;
}

bool ZMap::ShiftViewRight()
{
	return ShiftViewRight((int)ShiftViewDifference());
}

bool ZMap::ShiftViewUp()
{
	return ShiftViewUp((int)ShiftViewDifference());
}

bool ZMap::ShiftViewDown()
{
	return ShiftViewDown((int)ShiftViewDifference());
}

bool ZMap::ShiftViewLeft()
{
	return ShiftViewLeft((int)ShiftViewDifference());
}

double ZMap::ShiftViewDifference()
{
	double shift_difference;
	double time_difference;
	double the_time = current_time();
	
	time_difference = the_time - last_shift_time;
	if(time_difference > SHIFT_CLICK_STREAM)
	{
		shift_difference = MAX_SHIFT_CLICK;
		shift_overflow = 0;
		last_shift_time = the_time;
	}
	else
	{
		shift_difference = (time_difference * SHIFT_CLICK_S) + shift_overflow;
		shift_overflow = shift_difference - (int)shift_difference;
		last_shift_time = the_time;
	}
	
	return shift_difference;
}

void ZMap::GetViewLimits(int &map_left, int &map_right, int &map_top, int &map_bottom)
{
	map_left = shift_x;
	map_top = shift_y;
	map_right = map_left + view_w;
	map_bottom = map_top + view_h;
}

void ZMap::GetViewShiftFull(int &x, int &y, int &view_w_, int &view_h_)
{
	x = shift_x;
	y = shift_y;
	view_w_ = view_w;
	view_h_ = view_h;
}

void ZMap::GetViewShift(int &x, int &y)
{
	x = shift_x;
	y = shift_y;
}

void ZMap::GetMapCoords(int mouse_x, int mouse_y, int &map_x, int &map_y)
{
	map_x = mouse_x + shift_x;
	map_y = mouse_y + shift_y;
}

int ZMap::WithinView(int x, int y, int w, int h)
{
	if(x > shift_x + view_w) return 0;
	if(y > shift_y + view_h) return 0;
	if(x + w < shift_x) return 0;
	if(y + h < shift_y) return 0;

	return 1;
}

void ZMap::RenderZSurface(ZSDL_Surface *surface, int x, int y, bool render_hit, bool about_center)
{
	surface->RenderSurface((x - shift_x), (y - shift_y), render_hit, about_center);
}

void ZMap::RenderZSurfaceHorzRepeat(ZSDL_Surface *surface, int x, int y, int w_total, bool render_hit)
{
	SDL_Rect from_rect, to_rect;
	int fw, fh;

	if(!surface) return;
	if(!surface->GetBaseSurface()) return;

	fw = surface->GetBaseSurface()->w;
	fh = surface->GetBaseSurface()->h;

	while(w_total>0)
	{
		if(w_total > fw)
		{
			if(GetBlitInfo(x, y, fw, fh, from_rect, to_rect))
				surface->BlitHitSurface(&from_rect, &to_rect, NULL, render_hit);

			w_total -= fw;
			x += fw;
		}
		else
		{
			if(GetBlitInfo(x, y, w_total, fh, from_rect, to_rect))
				surface->BlitHitSurface(&from_rect, &to_rect, NULL, render_hit);

			w_total = 0;
		}
	}
}

void ZMap::RenderZSurfaceVertRepeat(ZSDL_Surface *surface, int x, int y, int h_total, bool render_hit)
{
	SDL_Rect from_rect, to_rect;
	int fw, fh;

	if(!surface) return;
	if(!surface->GetBaseSurface()) return;

	fw = surface->GetBaseSurface()->w;
	fh = surface->GetBaseSurface()->h;

	while(h_total>0)
	{
		if(h_total > fh)
		{
			if(GetBlitInfo(x, y, fw, fh, from_rect, to_rect))
				surface->BlitHitSurface(&from_rect, &to_rect, NULL, render_hit);

			h_total -= fh;
			y += fh;
		}
		else
		{
			if(GetBlitInfo(x, y, fw, h_total, from_rect, to_rect))
				surface->BlitHitSurface(&from_rect, &to_rect, NULL, render_hit);

			h_total = 0;
		}
	}
}

int ZMap::GetBlitInfo(int x, int y, int w, int h, SDL_Rect &from_rect, SDL_Rect &to_rect)
{
	// 	int full_width = (basic_info.width * 16);
	// 	int full_height = (basic_info.height * 16);

	//is this visable at ??
	if(x > shift_x + view_w) return 0;
	if(y > shift_y + view_h) return 0;
	if(x + w < shift_x) return 0;
	if(y + h < shift_y) return 0;

	//setup to
	to_rect.x = x - shift_x;
	to_rect.y = y - shift_y;
	to_rect.w = 0;
	to_rect.h = 0;

	if(to_rect.x < 0)
	{
		from_rect.x = -to_rect.x;
		from_rect.w = w + to_rect.x;
		to_rect.x += from_rect.x;

		if(from_rect.w > view_w) from_rect.w = view_w;
	}
	else if(to_rect.x + w > view_w)
	{
		from_rect.x = 0;
		from_rect.w = view_w - to_rect.x;
	}
	else
	{
		from_rect.x = 0;
		from_rect.w = w;
	}

	if(to_rect.y < 0)
	{
		from_rect.y = -to_rect.y;
		from_rect.h = h + to_rect.y;
		to_rect.y += from_rect.y;

		if(from_rect.h > view_h) from_rect.h = view_h;
	}
	else if(to_rect.y + h > view_h)
	{
		from_rect.y = 0;
		from_rect.h = view_h - to_rect.y;
	}
	else
	{
		from_rect.y = 0;
		from_rect.h = h;
	}

	//if(to_rect.x > 0) to_rect.x += from_rect.x;
	//if(to_rect.y > 0) to_rect.y += from_rect.y;

	////setup from
	//from_rect.x = shift_x - x;
	//from_rect.y = shift_y - y;

	//if(to_rect.x + w > view_w)
	//	from_rect.w = view_w - to_rect.x;
	//else if(to_rect.x < 0)
	//	from_rect.w = w + to_rect.x;
	//else
	//	from_rect.w = to_rect.x - view_w;

	//if(to_rect.y + h > view_h)
	//	from_rect.h = view_h - to_rect.y;
	//else if(to_rect.y < 0)
	//	from_rect.h = h + to_rect.y;
	//else
	//	from_rect.h = to_rect.y - view_h;

	//if(from_rect.h > h) from_rect.h = h;
	//if(from_rect.w > w) from_rect.w = w;

	//if(from_rect.x < 0) from_rect.x = 0;
	//if(from_rect.y < 0) from_rect.y = 0;

	//if(to_rect.x > 0) to_rect.x += from_rect.x;
	//if(to_rect.y > 0) to_rect.y += from_rect.y;

	return 1;
}

int ZMap::GetBlitInfo(SDL_Surface *src, int x, int y, SDL_Rect &from_rect, SDL_Rect &to_rect)
{
	if(!src) return 0;

	// 	int full_width = (basic_info.width * 16);
	// 	int full_height = (basic_info.height * 16);

	//is this visable at ??
	if(x > shift_x + view_w) return 0;
	if(y > shift_y + view_h) return 0;
	if(x + src->w < shift_x) return 0;
	if(y + src->h < shift_y) return 0;

	//setup to
	to_rect.x = x - shift_x;
	to_rect.y = y - shift_y;
	to_rect.w = 0;
	to_rect.h = 0;

	//setup from
	from_rect.x = shift_x - x;
	from_rect.y = shift_y - y;

	if(to_rect.x + src->w > view_w)
		from_rect.w = view_w - to_rect.x;
	else
		from_rect.w = to_rect.x - view_w;

	if(to_rect.y + src->h > view_h)
		from_rect.h = view_h - to_rect.y;
	else
		from_rect.h = to_rect.y - view_h;

	if(from_rect.x < 0) from_rect.x = 0;
	if(from_rect.y < 0) from_rect.y = 0;

	to_rect.x += from_rect.x;
	to_rect.y += from_rect.y;

	return 1;
}

void ZMap::PlaceObject(map_object new_object)
{
	object_list.push_back(new_object);
}

vector<map_object> &ZMap::GetObjectList()
{
	return object_list;
}

vector<map_zone> &ZMap::GetZoneList()
{
	return zone_list;
}

int ZMap::AddZone(map_zone new_zone)
{
	//is it a good zone?
	if(new_zone.x >= basic_info.width) return 0;
	if(new_zone.y >= basic_info.height) return 0;
	if(new_zone.w > basic_info.width) return 0;
	if(new_zone.h > basic_info.height) return 0;
	
	//does it already exist?
	for(vector<map_zone>::iterator i=zone_list.begin(); i!=zone_list.end(); i++)
		if(i->x == new_zone.x && i->y == new_zone.y)
			return 0;
	
	zone_list.push_back(new_zone);
	
	SetupAllZoneInfo();
	
	return 1;
}

int ZMap::RemoveZone(int x, int y)
{
	for(vector<map_zone>::iterator i=zone_list.begin(); i!=zone_list.end(); i++)
		if(i->x == x && i->y == y)
	{
		zone_list.erase(i);
		
		SetupAllZoneInfo();
		return 1;
	}
	
	
	return 0;
}

map_zone *ZMap::GetZoneExact(int x, int y)
{
	for(vector<map_zone>::iterator i=zone_list.begin(); i!=zone_list.end(); i++)
		if(i->x == x && i->y == y)
			return &(*i);
	
	return NULL;
}

map_zone_info *ZMap::GetZone(int x, int y)
{
   for(vector<map_zone_info>::iterator i = zone_list_info.begin();i!=zone_list_info.end();i++)
   {
      if(x < i->x) continue;
      if(y < i->y) continue;
      if(x > i->x + i->w) continue;
      if(y > i->y + i->h) continue;
      
      return &(*i);
   }
   
   return NULL;
}

vector<map_zone_info> &ZMap::GetZoneInfoList()
{
   return zone_list_info;
}

void ZMap::SetupAllZoneInfo()
{
	int i,j,k;
	
	zone_list_info.clear();
	
	for(i=0; i<zone_list.size(); i++)
	{
		int x,y,w,h;
		int mtile, mtile_x, mtile_y;
		map_zone &cur_zone = zone_list[i];
		map_zone_info new_map_zone_info;
		
		//set id
		new_map_zone_info.id = i;


		//find owner
		new_map_zone_info.owner = NULL_TEAM;
		
		//set dimensions
		new_map_zone_info.x = cur_zone.x * 16;
		new_map_zone_info.y = cur_zone.y * 16;
		new_map_zone_info.w = cur_zone.w * 16;
		new_map_zone_info.h = cur_zone.h * 16;
		
		//add in tiles
		new_map_zone_info.tile.clear();
		for(int j=1;j<cur_zone.w-1;j++)
		{
			mtile_x = (cur_zone.x + j);
			mtile_y = cur_zone.y;
			mtile = (mtile_y*basic_info.width)+mtile_x;
			
			x = mtile_x * 16 + 6;
			y = mtile_y * 16 + 6;
			
			if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_passable)
			{
				if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_water)
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y, true));
				else
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y));
			}
			
			mtile_x = (cur_zone.x + j);
			mtile_y = (cur_zone.y + (cur_zone.h - 1));
			mtile = (mtile_y*basic_info.width)+mtile_x;
			
			x = mtile_x * 16 + 6;
			y = mtile_y * 16 + 6;
			
			if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_passable)
			{
				if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_water)
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y, true));
				else
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y));
			}
		}
		for(int j=0;j<cur_zone.h;j++) 
		{
			mtile_x = cur_zone.x;
			mtile_y = (cur_zone.y + j);
			mtile = (mtile_y*basic_info.width)+mtile_x;
			
			x = mtile_x * 16 + 6;
			y = mtile_y * 16 + 6;
			
			if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_passable)
			{
				if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_water)
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y, true));
				else
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y));
			}
			
			mtile_x = (cur_zone.x + (cur_zone.w - 1));
			mtile_y = (cur_zone.y + j);
			mtile = (mtile_y*basic_info.width)+mtile_x;
			
			x = mtile_x * 16 + 6;
			y = mtile_y * 16 + 6;
			
			if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_passable)
			{
				if(planet_tile_info[basic_info.terrain_type][tile_list[mtile].tile].is_water)
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y, true));
				else
					new_map_zone_info.tile.push_back(map_zone_info_tile(x,y));
			}
		}
		
		zone_list_info.push_back(new_map_zone_info);
	}
}

void ZMap::DoZoneEffects(double the_time, SDL_Surface *dest, int shift_x, int shift_y)
{
	
	for(vector<map_zone_info>::iterator i=zone_list_info.begin();i!=zone_list_info.end();i++)
	{
		//is it rendered at all?
		
		//go through the tiles
		for(vector<map_zone_info_tile>::iterator j=i->tile.begin();j!=i->tile.end();j++)
		{
			SDL_Rect from_rect, to_rect;
			
			if(GetBlitInfo(zone_marker[0].GetBaseSurface(), j->render_loc.x, j->render_loc.y, from_rect, to_rect))
			{
				to_rect.x += shift_x;
				to_rect.y += shift_y;
				
				if(j->is_water)
				{
					if(the_time > j->next_time)
					{
						j->bob_i = !j->bob_i;
						
						j->next_time = the_time + (((rand() % 300000) + 500000) *0.000001);
					}
					
					if(j->bob_i)
						to_rect.y +=1;
					
					zone_marker_water[i->owner].BlitSurface(&from_rect, &to_rect);
					//RenderZSurface(&zone_marker_water[i->owner], j->render_loc.x, j->render_loc.y);
					//SDL_BlitSurface( zone_marker_water[i->owner], &from_rect, dest, &to_rect);
				}
				else
					zone_marker[i->owner].BlitSurface(&from_rect, &to_rect);
					//RenderZSurface(&zone_marker[i->owner], j->render_loc.x, j->render_loc.y);
					//SDL_BlitSurface( zone_marker[i->owner], &from_rect, dest, &to_rect);
			}
		}
	}
}

void ZMap::MarkAreaStamped(int x, int y, int w, int h)
{
	int sx, sy, ex, ey;
	int tx, ty;

	MakeSureStampListExists();

	sx = x>>4;
	sy = y>>4;
	ex = (x+w)>>4;
	ey = (y+h)>>4;

	if(!((x+w)%16)) ex--;
	if(!((y+h)%16)) ey--;

	if(sx<0) sx=0;
	if(sy<0) sy=0;
	if(ex>=stamp_list_w) ex=stamp_list_w-1;
	if(ey>=stamp_list_h) ey=stamp_list_h-1;

	for(tx=sx;tx<=ex;tx++)
		for(ty=sy;ty<=ey;ty++)
			stamp_list[tx][ty] = true;
}

bool ZMap::PermStamp(int x_, int y_, ZSDL_Surface *surface, bool mark_stamped)
{
	SDL_Rect to_rect;

	if(!full_render.GetBaseSurface()) return false;

	if(!surface) return true;
	if(!surface->GetBaseSurface()) return true;

	if(mark_stamped) MarkAreaStamped(x_, y_, surface->GetBaseSurface()->w, surface->GetBaseSurface()->h);

	to_rect.x = x_;
	to_rect.y = y_;

	surface->BlitSurface(NULL, &to_rect, &full_render);
	//SDL_BlitSurface( surface, NULL, full_render, &to_rect);

	return true;
}

bool ZMap::PermStamp(int x_, int y_, SDL_Surface *surface, bool mark_stamped)
{
	SDL_Rect to_rect;

	if(!full_render.GetBaseSurface()) return false;

	if(!surface) return true;

	if(mark_stamped) MarkAreaStamped(x_, y_, surface->w, surface->h);

	to_rect.x = x_;
	to_rect.y = y_;

	full_render.BlitOnToMe(NULL, &to_rect, surface);
	return true;
}

int ZMap::CoordCraterType(int tx, int ty)
{
	unsigned int tindex;
	int tile;

	if(tx >= basic_info.width) return -1;
	if(ty >= basic_info.height) return -1;
	if(tx < 0) return -1;
	if(ty < 0) return -1;
	
	tindex =  (ty*basic_info.width)+tx;

	tile = tile_list[tindex].tile;
	return planet_tile_info[basic_info.terrain_type][tile].crater_type;
}

void ZMap::CreateCrater(int x, int y, bool is_big, double chance)
{
	int tx, ty;
	int tile_crater_type;

	//even pass chance?
	if(frand() > chance) return;

	//basic coord check / manipulation
	{
		if(is_big)
		{
			x-=8;
			y-=8;
		}

		tx = x>>4;
		ty = y>>4;

		if(tx >= basic_info.width) return;
		if(ty >= basic_info.height) return;
		if(tx < 0) return;
		if(ty < 0) return;

		if(is_big)
		{
			//downgrade?
			if(tx+1 >= basic_info.width) is_big = false;
			if(ty+1 >= basic_info.height) is_big = false;
		}
	}

	tile_crater_type = CoordCraterType(tx, ty);

	//big crater even exist?
	if(is_big && !ZMapCraterGraphics::CraterExists(false, basic_info.terrain_type, tile_crater_type)) is_big = false;

	//this already already stamped? (with a building?)
	{
		MakeSureStampListExists();

		if(!is_big)
		{
			if(stamp_list[tx][ty]) return;
		}
		else
		{
			vector<xy_struct> ok_points;

			if(!stamp_list[tx][ty]) ok_points.push_back(xy_struct(tx, ty));
			if(!stamp_list[tx+1][ty]) ok_points.push_back(xy_struct(tx+1, ty));
			if(!stamp_list[tx][ty+1]) ok_points.push_back(xy_struct(tx, ty+1));
			if(!stamp_list[tx+1][ty+1]) ok_points.push_back(xy_struct(tx+1, ty+1));

			if(!ok_points.size()) return;
			if(ok_points.size() < 4)
			{
				int choice;

				is_big = false;
				choice = rand() % ok_points.size();
				tx = ok_points[choice].x;
				ty = ok_points[choice].y;
			}
		}
	}

	//all four covered tiles the same crater type?
	if(is_big)
	{
		int rct, dct, drct;

		//all following tiles are ok?
		rct = CoordCraterType(tx+1, ty);
		dct = CoordCraterType(tx, ty+1);
		drct = CoordCraterType(tx+1, ty+1);

		if(tile_crater_type != rct || tile_crater_type != dct || tile_crater_type != drct)
		{
			//can't be one uniform big crater if
			//all four squares do not have the same
			//crater type
			is_big = false;

			//can not do a big but maybe a small?
			vector<xy_struct> ok_points;

			if(ZMapCraterGraphics::CraterExists(true, basic_info.terrain_type, tile_crater_type)) ok_points.push_back(xy_struct(tx,ty));
			if(ZMapCraterGraphics::CraterExists(true, basic_info.terrain_type, rct)) ok_points.push_back(xy_struct(tx+1,ty));
			if(ZMapCraterGraphics::CraterExists(true, basic_info.terrain_type, dct)) ok_points.push_back(xy_struct(tx,ty+1));
			if(ZMapCraterGraphics::CraterExists(true, basic_info.terrain_type, drct)) ok_points.push_back(xy_struct(tx+1,ty+1));

			//no good choices at all?
			if(!ok_points.size()) return;

			//randomly choose one of the four
			//to make a small crater
			{
				int choice;

				choice = rand() % ok_points.size();
				tx = ok_points[choice].x;
				ty = ok_points[choice].y;
			}
		}
	}

	if(ZMapCraterGraphics::CraterExists(!is_big, basic_info.terrain_type, tile_crater_type))
		PermStamp(tx<<4, ty<<4, &ZMapCraterGraphics::RandomCrater(!is_big, basic_info.terrain_type, tile_crater_type), is_big);
}

void ZMap::InitPathfinding()
{
	int i, j;

	path_finder.ResetTileInfo(basic_info.width, basic_info.height);

	i=0;
	j=0;
	for(vector<map_tile>::iterator x=tile_list.begin(); x!=tile_list.end(); x++)
	{
		if(planet_tile_info[basic_info.terrain_type][x->tile].is_passable)
		{
			if(planet_tile_info[basic_info.terrain_type][x->tile].is_water)
				path_finder.SetTileInfo(i, j, PF_WATER);
			else if(planet_tile_info[basic_info.terrain_type][x->tile].is_road)
				path_finder.SetTileInfo(i, j, PF_ROAD);
			else
				path_finder.SetTileInfo(i, j, PF_NORMAL);
		}
		else
			path_finder.SetTileInfo(i, j, PF_IMPASSABLE);


		i++;
		if(i>=basic_info.width)
		{
			i=0;
			j++;
		}
	}

	path_finder.SetTileWideWeights();
}

void ZMap::MakeSureStampListExists()
{
	if(!stamp_list_setup)
	{
		printf("ZMap::MakeSureStampListExists:!stamp_list_setup\n");
		InitStampList();
	}
	else
	{
		if(basic_info.width != stamp_list_w || basic_info.height != stamp_list_h)
		{
			printf("ZMap::MakeSureStampListExists:invalid stamp list!!\n");
			InitStampList();
		}
	}
}

bool ZMap::CoordStamped(int x, int y)
{
	int tx, ty;

	MakeSureStampListExists();

	tx = x>>4;
	ty = y>>4;

	if(tx<0) return false;
	if(ty<0) return false;
	if(tx>=stamp_list_w) return false;
	if(ty>=stamp_list_h) return false;

	return stamp_list[tx][ty];
}

void ZMap::InitStampList()
{
	int i, j;

	//delete if already setup
	if(stamp_list_setup) DeleteStampList();

	//we even loaded?
	if(!file_loaded) return;

	//alloc
	stamp_list = (bool**)malloc(basic_info.width * sizeof(bool *));
	for(i=0;i<basic_info.width;i++) stamp_list[i] = (bool*)malloc(basic_info.height * sizeof(bool));

	//set
	stamp_list_w = basic_info.width;
	stamp_list_h = basic_info.height;

	//clear the list
	for(i=0;i<basic_info.width;i++)
		for(j=0;j<basic_info.height;j++)
			stamp_list[i][j] = false;

	//set
	stamp_list_setup = true;
}

void ZMap::DeleteStampList()
{
	//dont delete it if it doesnt exist
	if(!stamp_list_setup) return;

	//dealloc
	for(int i=0;i<basic_info.width;i++)
		free(stamp_list[i]);
	free(stamp_list);

	//done
	stamp_list_setup = false;
	stamp_list_w = -1;
	stamp_list_h = -1;
}

void ZMap::InitRockList()
{
	int i, j;

	//delete if already setup
	if(rock_list_setup) DeleteRockList();

	//we even loaded?
	if(!file_loaded) return;

	//alloc
	rock_list = (bool**)malloc(basic_info.width * sizeof(bool *));
	for(i=0;i<basic_info.width;i++) rock_list[i] = (bool*)malloc(basic_info.height * sizeof(bool));

	//clear the list
	for(i=0;i<basic_info.width;i++)
		for(j=0;j<basic_info.height;j++)
			rock_list[i][j] = false;

	//set
	rock_list_setup = true;
}

void ZMap::DeleteRockList()
{
	//dont delete it if it doesnt exist
	if(!rock_list_setup) return;

	//dealloc
	for(int i=0;i<basic_info.width;i++)
		free(rock_list[i]);
	free(rock_list);

	//done
	rock_list_setup = false;
}

bool **ZMap::GetRockList()
{
	if(!rock_list_setup) 
	{
		InitRockList();
		printf("ZMap::GetRockList: had to initrocklist\n");
	}
		
	return rock_list;
}

void ZMap::InitSubmergeAmounts()
{
	int i, j;

	//delete if already setup
	if(submerge_info_setup) DeleteSubmergeAmounts();

	//we even loaded?
	if(!file_loaded) return;

	//alloc
	submerge_amount = (int **)malloc(basic_info.width * sizeof(int *));
	for(int i=0;i<basic_info.width;i++)
		submerge_amount[i] = (int *)malloc(basic_info.height * sizeof(int));

	//populate
	i=0;
	j=0;
	for(vector<map_tile>::iterator x=tile_list.begin(); x!=tile_list.end(); x++)
	{
		if(planet_tile_info[basic_info.terrain_type][x->tile].is_water)
			submerge_amount[i][j] = 8;
		else
			submerge_amount[i][j] = 0;

		i++;
		if(i>=basic_info.width)
		{
			i=0;
			j++;
		}
	}

	//second run
	for(i=0;i<basic_info.width;i++)
		for(j=0;j<basic_info.height;j++)
			if(submerge_amount[i][j] == 8)
			{
				int i2, j2;

				for(i2=i-1;i2<i+3;i2++)
				{
					if(i2<0) continue;
					if(i2>=basic_info.width) continue;

					for(j2=j-1;j2<j+3;j2++)
					{
						if(j2<0) continue;
						if(j2>=basic_info.height) continue;

						if(submerge_amount[i2][j2] == 0)
						{
							submerge_amount[i][j] = 6;
							//escape the double loop now
							i2=i+3;
							break;
						}
					}
				}
			}

	//set
	submerge_info_setup = true;
}

void ZMap::DeleteSubmergeAmounts()
{
	//dont delete it if it doesnt exist
	if(!submerge_info_setup) return;

	//dealloc
	for(int i=0;i<basic_info.width;i++)
		free(submerge_amount[i]);
	free(submerge_amount);

	//done
	submerge_info_setup = false;
}

int ZMap::SubmergeAmount(int x, int y)
{
	int tx, ty;

	tx = x / 16;
	ty = y / 16;

	if(!submerge_info_setup) return 0;

	//checks
	if(tx < 0) return 0;
	if(ty < 0) return 0;
	if(tx >= basic_info.width) return 0;
	if(ty >= basic_info.height) return 0;

	//return path_robot_tile[tx][ty].submerge_amount;
	return submerge_amount[tx][ty];
}

void ZMap::DeletePathfindingInfo()
{
	path_finder.DeleteAllTileInfo();
}

bool ZMap::EngageBarrierBetweenCoords(int x1, int y1, int x2, int y2)
{
	ZPath_Finding_Bresenham bline;
	int x, y;

	if(!file_loaded) return false;

	//... nevermind if the target is a destroyable barrier
	//if(path_finder.HasDestroyableBarrier(x2>>4,y2>>4)) return false;

	//start needs checked by itself
	//if(path_finder.HasDestroyableBarrier(x1>>4,y1>>4)) return true;
	//if(path_finder.HasDestroyableBarrier(x1>>4,y1>>4)) return true;

	//forward...
	{
		bline.Init(x1>>4, y1>>4, x2>>4, y2>>4, basic_info.width, basic_info.height);

		while(bline.GetNext(x, y)) if(path_finder.HasDestroyableBarrier(x,y)) return true;
	}

	//reverse...
	{
		bline.Init(x2>>4, y2>>4, x1>>4, y1>>4, basic_info.width, basic_info.height);

		while(bline.GetNext(x, y)) if(path_finder.HasDestroyableBarrier(x,y)) return true;
	}

	return false;
}
