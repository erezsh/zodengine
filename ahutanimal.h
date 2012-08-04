#ifndef _AHUTANIMAL_H_
#define _AHUTANIMAL_H_

#include "zobject.h"

enum hut_animal_type
{
	GREEN_SNAKE, GREEN_LIZARD, DESERT_RABIT, RAPTOR,
	MINI_RAPTOR, PIG_DINO, YELLOW_WORM, ARCTIC_RABIT, 
	PENGUIN, WHITE_WOLF, OSTRICH, RAT, TURTLE, RED_WORM, 
	GREEN_EYED_FOX,
	MAX_HUT_ANIMAL_TYPES
};

const string hut_animal_type_string[MAX_HUT_ANIMAL_TYPES] =
{
	"green_snake", "green_lizard", "desert_rabit", "raptor",
	"mini_raptor", "pig_dino", "yellow_worm", "arctic_rabit", 
	"penguin", "white_wolf", "ostrich", "rat", "turtle", "red_worm", 
	"green_eyed_fox"
};

enum hut_animal_state
{
	HA_NOTHING, HA_WALKING, HA_LOOKING, MAX_HA_STATES
};

class hut_animal_graphics
{
public:
	hut_animal_graphics() { walk_i = look_i = 0; walk_to_zero = false; }

	void LoadGraphics(int ha_num);

	ZSDL_Surface walk[MAX_ANGLE_TYPES][8];
	ZSDL_Surface look[MAX_ANGLE_TYPES][4];
	ZSDL_Surface dead_up;
	ZSDL_Surface dead_down;

	int walk_i;
	int look_i;
	bool walk_to_zero;
};

class AHutAnimal : public ZObject
{
public:
	AHutAnimal(ZTime *ztime_, ZSettings *zsettings_ = NULL, int palette_ = DESERT);

	static void Init();
	static int ChooseRandomAnimal(int palette);
	static bool IsPrefferedDirection(int cur_dir, int new_dir);

	void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
	int Process();

	void SetHomeCoords(int x_, int y_) { home_x = x_; home_y = y_; }
	bool IsGoingHome() { return going_home; }
	void GoHome();
	void GotoTile(int tx, int ty);
	void GotoRandomTile();
	bool TileIsTooFar(int tx, int ty);
private:
	static bool finished_init;
	static hut_animal_graphics ha_graphics[MAX_HUT_ANIMAL_TYPES];
	static vector<int> animals_in_palette[MAX_PLANET_TYPES];

	void SetStateNothing();

	int ha_type;
	int palette;
	bool going_home;
	int home_x, home_y;
	double walk_speed;
	double look_speed;

	double next_walk_time;
	double next_look_time;
	double next_nothing_time;
	int walk_i;
	int look_i;
	int ha_state;
};

#endif
