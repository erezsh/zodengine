#ifndef _ETOUGHMUSHROOM_H_
#define _ETOUGHMUSHROOM_H_

#include "zeffect.h"

class EToughMushroom : ZEffect
{
	public:
		EToughMushroom(ZTime *ztime_, int x_, int y_, double size = 1.0);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface base_img[12];
		ZSDL_Surface *render_img[12];

		int x, y;
		double next_process_time;
		double zoom_size;

		int render_i;
};

#endif
