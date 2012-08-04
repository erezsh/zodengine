#ifndef _ZMAP_H_
#define _ZMAP_H_

#include <vector>
#include <string>

#include "constants.h"
#include "zsdl.h"
#include "zsdl_opengl.h"
#include "zpath_finding.h"
#include "zteam.h"
#include "zmap_crater_graphics.h"

using namespace std;

#define MAX_SHIFT_CLICK 1
#define SHIFT_CLICK_S 320
#define SHIFT_CLICK_STREAM 0.1


enum map_object_type
{
	ROCK_OBJECT,
	BRIDGE_OBJECT,
	BUILDING_OBJECT,
	CANNON_OBJECT,
	VEHICLE_OBJECT,
	ROBOT_OBJECT,
	ANIMAL_OBJECT,
	MAP_ITEM_OBJECT,
	MAX_MAP_OBJECT_TYPES
};

const string map_object_type_string[MAX_MAP_OBJECT_TYPES] =
{
	"rock", "bridge", "building", "cannon", "vehicle", "robot", "animal", "map_item"
};

//ensure the follow has a uniform size
#pragma pack(1)

struct palette_tile_info
{
	bool is_water;
	bool is_passable;
	bool is_usable;
	bool is_road;
	bool is_effect;
	bool is_water_effect; //does it happen randomly within a water tile?
	unsigned short next_tile_in_effect;
	bool takes_tank_tracks;
	short crater_type;
	
	//items for the map editor
	bool is_starter_tile;
};

struct palette_tile_info_new
{
	bool is_water;
	bool is_passable;
	bool is_usable;
	bool is_road;
	bool is_effect;
	bool is_water_effect; //does it happen randomly within a water tile?
	unsigned short next_tile_in_effect;
	bool takes_tank_tracks;
	short crater_type;
	
	//items for the map editor
	bool is_starter_tile;
};

//undo the uniform size command
#pragma pack()

class map_zone_info_tile
{
	public:
		map_zone_info_tile(int x=0, int y=0, bool is_water_ = false)
		{
			render_loc.x = x;
			render_loc.y = y;
			render_loc.w = 0;
			render_loc.h = 0;
			is_water = is_water_;
			bob_i = rand() % 2;
			next_time=0;
		}
		
		SDL_Rect render_loc;
		bool is_water;
		int bob_i;
		double next_time;
};

struct map_zone_info
{
	team_type owner;
	vector<map_zone_info_tile> tile;
	int x, y, w, h;
	int id;
};

class map_effect_info
{
public:
	map_effect_info();
	map_effect_info(unsigned int tile_) {tile = tile_; next_effect_time = 0;};
	unsigned int tile;
	double next_effect_time;
};

class map_basics
{
public:
	map_basics() { clear(); }

	void clear()
	{
		width=0;
		height=0;
		map_name[0] = 0;
		player_count = 0;
		object_count = 0;
		terrain_type = 0;
		zone_count = 0;
	}

	unsigned short width;
	unsigned short height;
	char map_name[50];
	unsigned char player_count;
	unsigned short object_count;
	unsigned char terrain_type;
	unsigned short zone_count;
};

struct map_zone
{
	unsigned short x, y, w, h; //in tile form
};

struct map_object_old
{
	unsigned short x,y; //this is in tile form, not pixel
	char owner;
	unsigned char object_type;
	unsigned char object_id;
	char blevel;
};

struct map_object
{
	unsigned short x,y; //this is in tile form, not pixel
	char owner;
	unsigned char object_type;
	unsigned char object_id;
	char blevel;
	unsigned short extra_links;
	int health_percent;
};

struct map_tile
{
	unsigned short tile;
};

class ZMap
{
	public:
		ZMap();
		~ZMap();
		int Read(const char* filename);
		int Read(char* data, int size, bool scrap_data = true);
		int Write(const char* filename);
		bool GetMapData(char *&data, int &size);
		void RenderMap();
		void ClearMap();
		void MakeRandomMap();
		void MakeNewMap(const char *new_name, planet_type palette, int width, int height);
		ZSDL_Surface &GetRender();
		void ReplaceUnusableTiles();
		void DoRender(SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DebugMapInfo();
		int DoEffects(double the_time, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		map_basics &GetMapBasics();
		map_tile &GetTile(int x, int y, bool is_shifted = false);
		map_tile &GetTile(unsigned int index);
		int GetTileIndex(int x, int y, bool is_shifted = false);
		void GetTile(unsigned int index, int &x, int &y, bool is_shifted = false);
		bool CoordIsRoad(int x, int y);
		double GetTileWalkSpeed(int x, int y, bool is_shifted = false);
		void DoZoneEffects(double the_time, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void SetupAllZoneInfo();
		map_zone_info *GetZone(int x, int y);
		vector<map_zone_info> &GetZoneInfoList();
		//void PlayMusic(bool normal = true);
		bool Loaded();
		void MarkAreaStamped(int x, int y, int w, int h);
		bool PermStamp(int x_, int y_, ZSDL_Surface *surface, bool mark_stamped = true);
		bool PermStamp(int x_, int y_, SDL_Surface *surface, bool mark_stamped = true);
		vector<map_effect_info> &GetMapWaterList() { return map_water_list; }
		void CreateCrater(int x, int y, bool is_big, double chance = 1.0);
		int CoordCraterType(int tx, int ty);
		
		
		//viewport / shift stuff
		void SetViewingDimensions(int w, int h);
		void SetViewShift(int x, int y);
		bool ShiftViewRight();
		bool ShiftViewUp();
		bool ShiftViewDown();
		bool ShiftViewLeft();
		bool ShiftViewRight(int amt);
		bool ShiftViewUp(int amt);
		bool ShiftViewDown(int amt);
		bool ShiftViewLeft(int amt);
		void GetViewShift(int &x, int &y);
		void GetViewShiftFull(int &x, int &y, int &view_w, int &view_h);
		void GetViewLimits(int &map_left, int &map_right, int &map_top, int &map_bottom);
		void GetMapCoords(int mouse_x, int mouse_y, int &map_x, int &map_y);
		int GetBlitInfo(SDL_Surface *src, int x, int y, SDL_Rect &from_rect, SDL_Rect &to_rect);
		int GetBlitInfo(int x, int y, int w, int h, SDL_Rect &from_rect, SDL_Rect &to_rect);
		int WithinView(int x, int y, int w, int h);
		void RenderZSurface(ZSDL_Surface *surface, int x, int y, bool render_hit = false, bool about_center = false);
		void RenderZSurfaceHorzRepeat(ZSDL_Surface *surface, int x, int y, int w_total, bool render_hit = false);
		void RenderZSurfaceVertRepeat(ZSDL_Surface *surface, int x, int y, int h_total, bool render_hit = false);
		
		//map editor stuff
		void ChangeTile(unsigned int index, map_tile new_tile);
		void PlaceObject(map_object new_object);
		vector<map_object> &GetObjectList();
		vector<map_zone> &GetZoneList();
		static ZSDL_Surface *GetZoneMarkers();
		int AddZone(map_zone new_zone);
		int RemoveZone(int x, int y);
		map_zone *GetZoneExact(int x, int y);
		
		static void Init();
		static void ServerInit();
		static SDL_mutex *init_mutex;
		
		//palette stuff
		static ZSDL_Surface &GetMapPalette(planet_type palette);
		static palette_tile_info &GetMapPaletteTileInfo(planet_type palette, int tile);
		static int WriteMapPaletteTileInfo(planet_type palette);
		static int UpdatePalettesTileFormat();
		static int GetPaletteTile(int x, int y);
		static int GetPaletteTile(unsigned short index, int &x, int &y);

		//pathfinding stuff
		void InitPathfinding();
		void DeletePathfindingInfo();
		inline void SetImpassable(int x, int y, bool impassable = true, bool destroyable = false)
			{ path_finder.SetImpassable(x, y, impassable, destroyable); }
		inline bool WithinImpassable(int x, int y, int w, int h, int &stop_x, int &stop_y, bool is_robot)
			{ return path_finder.WithinImpassable(x, y, w, h, stop_x, stop_y, is_robot); }
		inline ZPath_Finding_Engine &GetPathFinder()
			{ return path_finder; }
		inline void RebuildRegions() { path_finder.RebuildRegions(); }
		bool EngageBarrierBetweenCoords(int x1, int y1, int x2, int y2);

		//submerge amounts
		void InitSubmergeAmounts();
		void DeleteSubmergeAmounts();
		int SubmergeAmount(int x, int y);

		//rock list
		void InitRockList();
		void DeleteRockList();
		bool **GetRockList();

		//stamp list
		void InitStampList();
		void DeleteStampList();
		void MakeSureStampListExists();
		bool CoordStamped(int x, int y);

	private:
		void DeRenderMap();
		void FreeMapData();
		bool CheckLoad();
		void InitEffects();
		void RenderTile(unsigned int index);
		double ShiftViewDifference();
		static void LoadPaletteInfo(int terrain_type);
		
		bool file_loaded;
		char *map_data;
		int map_data_size;

		
		//the basics that exist in the file
		map_basics basic_info;
		vector<map_zone> zone_list;
		vector<map_object> object_list;
		vector<map_tile> tile_list;

		//for speed increases
		int width_pix;
		int height_pix;

		//pathfinding stuff
		ZPath_Finding_Engine path_finder;

		//submerge info
		bool submerge_info_setup;
		int **submerge_amount;

		//rock list
		bool rock_list_setup;
		bool **rock_list;

		//stamped list
		bool stamp_list_setup;
		bool **stamp_list;
		unsigned short stamp_list_w;
		unsigned short stamp_list_h;
		
		ZSDL_Surface full_render;
		static ZSDL_Surface planet_template[MAX_PLANET_TYPES];
		static palette_tile_info planet_tile_info[MAX_PLANET_TYPES][MAX_PLANET_TILES];
		static ZSDL_Surface zone_marker[MAX_TEAM_TYPES];
		static ZSDL_Surface zone_marker_water[MAX_TEAM_TYPES];
		
		//music
		//static Mix_Music *music[MAX_PLANET_TYPES];

		//effect stuff
		vector<map_effect_info> map_effect_list;
		vector<map_effect_info> map_water_list;
		static vector<unsigned int> map_water_plist[MAX_PLANET_TYPES];
		static vector<unsigned int> map_water_effect_plist[MAX_PLANET_TYPES];

		//other runtime
		vector<map_zone_info> zone_list_info;
		
		//shift stuff
		int shift_x, shift_y;
		int view_w, view_h;
		double last_shift_time;
		double shift_overflow;

		//screen placement
		int place_x, place_y;
};

#endif
