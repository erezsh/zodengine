#ifndef _ESIDEEXPLOSION_H_
#define _ESIDEEXPLOSION_H_

#include "zeffect.h"

enum side_explosion_type
{
	ESIDEEXPLOSION_NORMAL
};

class ESideExplosion : ZEffect
{
	public:
		ESideExplosion(ZTime *ztime_, int x_, int y_, double size_ = 1.0, int type_ = ESIDEEXPLOSION_NORMAL);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface normal_img[7];
		ZSDL_Surface *render_img;


		int x, y;
		int ex, ey;
		int sx, sy;
		double dx, dy;
		double init_time;
		double next_render_time;

		int type;
		double size;

		int render_i;
};

#endif
