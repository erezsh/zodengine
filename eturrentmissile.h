#ifndef _ETURRENTMISSILE_H_
#define _ETURRENTMISSILE_H_

#include "zeffect.h"

enum eturrent_missile
{
	ETURRENTMISSILE_LIGHT, ETURRENTMISSILE_MEDIUM, ETURRENTMISSILE_HEAVY,
	ETURRENTMISSILE_GATLING, ETURRENTMISSILE_GUN, ETURRENTMISSILE_HOWITZER,
	ETURRENTMISSILE_MISSILE_CANNON, ETURRENTMISSILE_BUILDING_PEICE0, 
	ETURRENTMISSILE_BUILDING_PEICE1, ETURRENTMISSILE_FORT_BUILDING_PEICE0, 
	ETURRENTMISSILE_FORT_BUILDING_PEICE1, ETURRENTMISSILE_FORT_BUILDING_PEICE2, 
	ETURRENTMISSILE_FORT_BUILDING_PEICE3, ETURRENTMISSILE_FORT_BUILDING_PEICE4,
	ETURRENTMISSILE_GRENADE
};

class ETurrentMissile : ZEffect
{
	public:
		ETurrentMissile(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y, double offset_time, int object_, int team_ = NULL_TEAM);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;

		void EndExplosion();
		static ZSDL_Surface light_turrent[8];
		static ZSDL_Surface medium_turrent[8];
		static ZSDL_Surface heavy_turrent[MAX_TEAM_TYPES][8];
		static ZSDL_Surface gatling_wasted;
		static ZSDL_Surface gun_wasted;
		static ZSDL_Surface howitzer_wasted;
		static ZSDL_Surface missile_wasted;
		static ZSDL_Surface building_piece[2][12];
		static ZSDL_Surface fort_building_piece[5][12];
		static ZSDL_Surface grenade_img[4];
		ZSDL_Surface *render_img;

		double init_time, final_time;
		int x, y;
		double dx, dy;
		int sx, sy;
		int ex, ey;
		
		double dangle;
		int angle;

		double rise;
		double next_render_time;
		int object;
		int render_i;
		int max_render;
		int radius;
		int team;
		double size;
};

#endif
