#ifndef _ECANNONDEATH_H_
#define _ECANNONDEATH_H_

#include "zeffect.h"
#include "estandard.h"

enum ecannondeath_objects
{
	ECANNONDEATH_GATLING, ECANNONDEATH_GUN, ECANNONDEATH_HOWITZER, ECANNONDEATH_MISSILE
};

class ECannonDeath : ZEffect
{
	public:
		ECannonDeath(ZTime *ztime_, int x_, int y_, int object_, int ex_, int ey_, double offset_time_);
		~ECannonDeath();

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;

		void DoSparks();
		static ZSDL_Surface gatling_wasted;
		static ZSDL_Surface gun_wasted;
		static ZSDL_Surface howitzer_wasted;
		static ZSDL_Surface missile_wasted;

		ZSDL_Surface wasted_img;
		vector<EStandard*> extra_effects;

		int x, y;
		int ex, ey;
		double offset_time;
		double final_time;
		int object;
};

#endif
