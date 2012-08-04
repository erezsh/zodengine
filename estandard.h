#ifndef _ESTANDARD_H_
#define _ESTANDARD_H_

#include "zeffect.h"

enum estandard_objects
{
	EDEATH_BIG_SMOKE, EDEATH_LITTLE_FIRE, EDEATH_SMALL_FIRE_SMOKE, EDEATH_FIRE
};

class EStandard : ZEffect
{
	public:
		EStandard(ZTime *ztime_, int x_, int y_, int object);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface edeath_big_smoke[4];
		static ZSDL_Surface edeath_little_fire[4];
		static ZSDL_Surface edeath_small_fire_smoke[4];
		static ZSDL_Surface edeath_fire[4];

		ZSDL_Surface *render_img;

		int x, y;
		int bx, by;
		double next_process_time;

		int render_i;
		int max_render;

		friend bool sort_estandards_func (EStandard *a, EStandard *b);
};

extern bool sort_estandards_func (EStandard *a, EStandard *b);

#endif
