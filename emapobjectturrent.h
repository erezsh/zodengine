#ifndef _EMAPOBJECTTURRENT_H_
#define _EMAPOBJECTTURRENT_H_

#include "zeffect.h"

class EMapObjectTurrent : ZEffect
{
	public:
		EMapObjectTurrent(ZTime *ztime_, int start_x, int start_y, int dest_x_, int dest_y_, double offset_time, int object_);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		void EndExplosion();

		static bool finished_init;
		static ZSDL_Surface object_img[MAP_ITEMS_AMOUNT];

		double init_time, final_time;
		int x, y;
		double dx, dy;
		int sx, sy;
		int ex, ey;

		int dest_x, dest_y;
		
		double dangle;
		int angle;

		double rise;
		double size;
		int object;
};

#endif
