#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include "zmap.h"
#include "zsdl.h"
#include "common.h"

#include <SDL/SDL_ttf.h>

using namespace COMMON;
using namespace std;

enum editor_mode
{
	NORMAL_MODE, MAP_MODE, USABLE_MODE, PASSABLE_MODE,
	TAKES_TRACKS_MODE, CRATER_TYPE_MODE, MAX_MODES
};

const string editor_mode_string[MAX_MODES] =
{
	"NORMAL_MODE", "MAP_MODE", "USABLE_MODE", "PASSABLE_MODE", 
	"TAKES_TRACKS_MODE", "CRATER_TYPE_MODE"
};

SDL_Surface *screen;
TTF_Font *ttf_font;
int palette_chosen;
SDL_Thread *process_thread;
SDL_mutex *process_mutex;

int current_process_tile;
int current_mode;

int process_tile(void *tile_ptr);
void process_tile_normal(int tile);
void process_tile_map(int tile);
void process_tile_usable(int tile);
void process_tile_passable(int tile);
void process_tile_takes_tracks(int tile);
void process_tile_crater_type(int tile);

void display_tile_info_normal(int tile);
void display_tile_info_map(int tile);

void hover_tile(int tile);
void blit_message(const char *message, int x, int y);
void xmark_process_tile();
void redraw_all(int tile);

#undef main
int main(int argc, char **argv)
{
	current_process_tile = -1;
	
	//got the right inputs?
	if(argc < 2)
	{
		printf("proper usage:%s palette_name editor_mode\n", argv[0]);
		return 0;
	}
	
	//because we want to short cut things.
	current_mode = NORMAL_MODE;

	//find palette number
	for(palette_chosen=0;palette_chosen<MAX_PLANET_TYPES;palette_chosen++)
		if(planet_type_string[palette_chosen] == argv[1])
			break;
	
	if(palette_chosen == MAX_PLANET_TYPES)
	{
		printf("unable to find palette '%s' (maybe try all lowercase?)\n", argv[1]);
		return 0;
	}
	
	//seed
	srand(time(0));
	
	//init SDL
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	screen = SDL_SetVideoMode(320,384+40,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Zod Tile Info Editor", "Zod Tile Info Editor");
	SDL_EnableUNICODE(SDL_ENABLE);
	atexit(SDL_Quit);

	//just force software mode
	ZSDL_Surface::SetUseOpenGL(false);
	ZSDL_Surface::SetScreenDimensions(800, 600);
	ZSDL_Surface::SetMainSoftwareSurface(screen);
	
	//we'll need this
	process_mutex = SDL_CreateMutex();
	
	//TTF
	TTF_Init();
	ttf_font = TTF_OpenFont("assets/arial.ttf",10);
	if (!ttf_font) printf("could not load arial.ttf\n");

	//init map class
	ZMap::Init();

	//update tile format
	//ZMap::UpdatePalettesTileFormat();

	//display palette
	//SDL_BlitSurface(ZMap::GetMapPalette((planet_type)palette_chosen), NULL, screen, NULL);
	ZMap::GetMapPalette((planet_type)palette_chosen).BlitSurface(NULL, NULL);
	SDL_Flip(screen);
	
	SDL_Event event;
	int tile;
	while(1)
	{
		while(SDL_PollEvent(&event))
		switch( event.type ) 
		{
			case SDL_QUIT:
				return 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
// 				event.button.x;
// 				event.button.y;
				tile = ZMap::GetPaletteTile(event.button.x, event.button.y);
				process_thread = SDL_CreateThread(process_tile, (void*)&tile);
				break;
			case SDL_MOUSEMOTION:
// 				event.motion.x;
// 				event.motion.y;
				hover_tile(ZMap::GetPaletteTile(event.motion.x, event.motion.y));
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.unicode)
				{
					case 'm':
						current_mode++;
						if(current_mode >= MAX_MODES) current_mode = 0;
						printf("Current Mode:%s\n", editor_mode_string[current_mode].c_str());
						break;
				}
				break;
		}
		
		uni_pause(10);
	}
	
	return 0;
}

void process_tile_map(int tile)
{
	int value;
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	p_info.is_starter_tile = 0;
	
	printf("Tile is a starter tile? ");
	scanf("%d", &value);
	
	p_info.is_starter_tile = value;
}

void process_tile_usable(int tile)
{
	int value;
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	p_info.is_usable = 0;
	
	printf("Tile usable? ");
	scanf("%d", &value);
	
	p_info.is_usable = value;
}

void process_tile_passable(int tile)
{
	int value;
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	p_info.is_passable = 0;
	
	printf("Tile passable? ");
	scanf("%d", &value);
	
	p_info.is_passable = value;
}

void process_tile_takes_tracks(int tile)
{
	int value;
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	p_info.takes_tank_tracks = 0;
	
	printf("Tile takes tank tracks? ");
	scanf("%d", &value);
	
	p_info.takes_tank_tracks = value;
}

void process_tile_crater_type(int tile)
{
	int value;
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	p_info.crater_type = -1;
	
	printf("Tile crater type? ");
	scanf("%d", &value);
	
	p_info.crater_type = value;
}

void process_tile_normal(int tile)
{
	
	int value;
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	//p_info.is_usable = 0;
	//p_info.is_passable = 0;
	p_info.is_road = 0;
	p_info.is_effect = 0;
	p_info.is_water = 0;
	p_info.is_water_effect = 0;
	p_info.next_tile_in_effect = 0;
	
	//printf("Tile usable? ");
	//scanf("%d", &value);
	//
	//p_info.is_usable = value;
	//
	//if(p_info.is_usable)
	//{
		//printf("Tile passable? ");
		//scanf("%d", &value);
		//
		//p_info.is_passable = value;
		
		if(p_info.is_passable)
		{
			printf("Tile is water? ");
			scanf("%d", &value);
			
			p_info.is_water = value;
			
			if(!p_info.is_water)
			{
				printf("Tile is road? ");
				scanf("%d", &value);
				
				p_info.is_road = value;
			}
		}
		
		printf("Tile is apart of an effect? ");
		scanf("%d", &value);
		
		p_info.is_effect = value;
		
		if(p_info.is_effect)
		{
			if(p_info.is_water)
			{
				printf("Tile is apart of a water effect? ");
				scanf("%d", &value);
				
				p_info.is_water_effect = value;
			}
			
			printf("What is the next tile in the effect? ");
			scanf("%d", &value);
			
			p_info.next_tile_in_effect = value;
		}
	//}
	
	printf("\n");
}

int process_tile(void *tile_ptr)
{
	int tile = *(int*)tile_ptr;
	
	if(tile == -1) return 0;
	
	SDL_LockMutex ( process_mutex );
	
	printf("\nProcess Tile:%d\n", tile);
	
	current_process_tile = tile;
	
	//mark the tile we're working on
	xmark_process_tile();
	SDL_Flip(screen);
	
	switch(current_mode)
	{
		case NORMAL_MODE:
			process_tile_normal(tile);
			break;
		case MAP_MODE:
			process_tile_map(tile);
			break;
		case USABLE_MODE:
			process_tile_usable(tile);
			break;
		case PASSABLE_MODE:
			process_tile_passable(tile);
			break;
		case TAKES_TRACKS_MODE:
			process_tile_takes_tracks(tile);
			break;
		case CRATER_TYPE_MODE:
			process_tile_crater_type(tile);
			break;
	}
	
	ZMap::WriteMapPaletteTileInfo((planet_type)palette_chosen);
	
	current_process_tile = -1;
	
	redraw_all(tile);
	
	SDL_UnlockMutex ( process_mutex );
	
	return 1;
}

void hover_tile(int tile)
{
	static int previous_tile = -1;
	
	if(tile == -1) return;
	if(tile == previous_tile) return;
	
	previous_tile = tile;
	
	redraw_all(tile);
}

void display_tile_info_normal(int tile)
{
	//write info
	char message[500];
	int j=384+5;
	
	sprintf(message, "N tile:%d", tile);
	
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	//clear space
	SDL_Rect clr_box;
	clr_box.x = 0;
	clr_box.y = 384;
	clr_box.w = 320;
	clr_box.h = 40;
	SDL_FillRect(screen, &clr_box, SDL_MapRGB(screen->format, 0, 0, 0));
	
	//append attribute info
	if(p_info.is_usable) strcat(message, " usable");
	if(p_info.is_passable) strcat(message, " passable");
	if(p_info.is_water) strcat(message, " is_water");
	if(p_info.is_road) strcat(message, " is_road");
	if(p_info.is_starter_tile) strcat(message, " is_starter");
	if(p_info.takes_tank_tracks) strcat(message, " takes_tracks");
	
	blit_message(message, 5, j);
	
	if(p_info.is_effect)
	{
		sprintf(message, "next_tile_in_effect:%d", p_info.next_tile_in_effect);
		if(p_info.is_water_effect) strcat(message, " is_water_effect");
		blit_message(message, 5, j+10);
	}

	if(p_info.crater_type > -1)
	{
		sprintf(message, "crater_type:%d", p_info.crater_type);
		blit_message(message, 5, j+20);
	}
}

void display_tile_info_map(int tile)
{
		//write info
	char message[500];
	int j=384+5;
	
	sprintf(message, "M tile:%d", tile);
	
	
	palette_tile_info &p_info = ZMap::GetMapPaletteTileInfo((planet_type)palette_chosen, tile);
	
	//clear space
	SDL_Rect clr_box;
	clr_box.x = 0;
	clr_box.y = 384;
	clr_box.w = 320;
	clr_box.h = 40;
	SDL_FillRect(screen, &clr_box, SDL_MapRGB(screen->format, 0, 0, 0));
	
	//append attribute info
	if(p_info.is_starter_tile) strcat(message, " starter");
	
	blit_message(message, 5, j);
}

void redraw_all(int tile)
{
	//SDL_BlitSurface(ZMap::GetMapPalette((planet_type)palette_chosen), NULL, screen, NULL);
	ZMap::GetMapPalette((planet_type)palette_chosen).BlitSurface(NULL, NULL);
	
	//place dot
	int x, y;
	SDL_Color marker_color;
	
	marker_color.r = 0;
	marker_color.g = 255;
	marker_color.b = 255;
	
	ZMap::GetPaletteTile(tile,x,y);
	
	//draw the selection box
	for(int i=0;i<16;i++) put32pixel(screen, x+i, y, marker_color);
	for(int i=0;i<16;i++) put32pixel(screen, x, y+i, marker_color);
	for(int i=0;i<16;i++) put32pixel(screen, x+15, y+i, marker_color);
	for(int i=0;i<16;i++) put32pixel(screen, x+i, y+15, marker_color);
	
	
	marker_color.r = 125;
	marker_color.g = 0;
	marker_color.b = 0;
	put32pixel(screen, x+8, y+8, marker_color);
	
	switch(current_mode)
	{
		case NORMAL_MODE:
		case USABLE_MODE:
		case PASSABLE_MODE:
		case TAKES_TRACKS_MODE:
		case CRATER_TYPE_MODE:
			display_tile_info_normal(tile);
			break;
		case MAP_MODE:
			display_tile_info_map(tile);
			break;
	}
	
	//keep an marker on the tile we are processing
	xmark_process_tile();
	
	//actually write to screen now
	SDL_Flip(screen);
}

void xmark_process_tile()
{
	int x,y;
	
	if(current_process_tile != -1)
	{
		SDL_Color x_color;
		
		ZMap::GetPaletteTile(current_process_tile,x,y);
		
		x_color.r = 255;
		x_color.g = 0;
		x_color.b = 0;
		
		for(int i=0;i<16;i++)
		{
			put32pixel(screen, x+i, y+i, x_color);
			put32pixel(screen, x+15-i, y+i, x_color);
		}
	}
}

void blit_message(const char *message, int x, int y)
{
	SDL_Surface *text;
	SDL_Color textcolor;
	SDL_Rect location;
			
	textcolor.r = 255;
	textcolor.g = 255;
	textcolor.b = 255;
	textcolor.unused = 0;
	
	location.x = x;
	location.y = y;
			
	text = TTF_RenderText_Solid(ttf_font, message, textcolor);
	
	SDL_BlitSurface( text, NULL, screen, &location);
}

