#ifndef _ZEFFECT_H_
#define _ZEFFECT_H_

#include "constants.h"
#include "zsettings.h"
#include "zsdl_opengl.h"
#include "zsdl.h"
#include "zmap.h"
#include "zsound_engine.h"
#include "ztime.h"
#include "common.h"
#include <math.h>

using namespace COMMON;

class effect_flags
{
public:
	effect_flags(){ Clear(); };

	bool unit_particles;
	int unit_particles_radius;
	int unit_particles_amount;
	int x, y;

	void Clear()
	{
		unit_particles = false;
		unit_particles_radius = 0;
		unit_particles_amount = 0;
	}
};

class ZEffect
{
	public:
		ZEffect(ZTime *ztime_);
		virtual ~ZEffect() {}

		static void SetEffectList(vector<ZEffect*> *effect_list_);
		static void SetSettings(ZSettings *zsettings_);
		static void SetMap(ZMap *zmap_) { zmap = zmap_; }

		virtual void Process();
		virtual void DoPreRender(ZMap &the_map, SDL_Surface *dest) {}
		virtual void DoRender(ZMap &the_map, SDL_Surface *dest) {}
		bool KillMe();
		effect_flags &GetEFlags();
	protected:
		bool killme;

		static ZSettings *zsettings;
		static ZSettings default_settings;
		static vector<ZEffect*> *effect_list;
		static ZMap *zmap;
		ZTime *ztime;

		effect_flags eflags;
};

#endif
