#ifndef _EDEATH_H_
#define _EDEATH_H_

#include "zeffect.h"
#include "estandard.h"

enum edeath_objects
{
	EDEATH_JEEP, EDEATH_MOMISSILE, EDEATH_APC, EDEATH_TANK, EDEATH_CRANE
};

class EDeath : ZEffect
{
	public:
		EDeath(ZTime *ztime_, int x_, int y_, int object, ZSDL_Surface *prefered_surface = NULL);
		~EDeath();

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		void DoSparks();

		static bool finished_init;
		static ZSDL_Surface jeep_wasted;
		static ZSDL_Surface momissile_wasted;
		static ZSDL_Surface apc_wasted;
		static ZSDL_Surface crane_wasted;

		ZSDL_Surface *wasted_img;
		vector<EStandard*> extra_effects;

		int x, y;
		double next_process_time;
		double final_time;
};

#endif
