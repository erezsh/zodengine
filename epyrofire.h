#ifndef _EPYROFIRE_H_
#define _EPYROFIRE_H_

#include "zeffect.h"

class EPyroFire : ZEffect
{
	public:
		EPyroFire(ZTime *ztime_, int x_, int y_);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface fire_img[5][6];

		int x, y;
		double next_process_time;

		int fire_i, fire_j;
};

#endif
