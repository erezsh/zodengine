#ifndef _EBRIDGETURRENT_H_
#define _EBRIDGETURRENT_H_

#include "zeffect.h"

class EBridgeTurrent : ZEffect
{
	public:
		EBridgeTurrent(ZTime *ztime_, int x_, int y_, int palette_, int max_horz, int max_vert, bool is_reversed_ = false);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface debri_large_img[MAX_PLANET_TYPES][12];

		void EndExplosion();

		int palette;
		bool is_reversed;
		//int large_i;

		int x, y;
		int ex, ey;
		int sx, sy;
		double dx, dy;
		double next_process_time;
		double init_time, final_time;
		int rise;

		int render_i;
		double size;
		double dangle;
		int angle;
};

#endif
