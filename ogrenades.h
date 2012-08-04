#ifndef _OGRENADES_H_
#define _OGRENADES_H_

#include "zobject.h"

class OGrenades : public ZObject
{
	public:
		OGrenades(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();

		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		int Process();
		virtual void SetGrenadeAmount(int grenade_amount_) { grenade_amount = grenade_amount_; }
		virtual int GetGrenadeAmount() { return grenade_amount; }

		void SetOwner(team_type owner_);
		void FireTurrentMissile(int x_, int y_, double offset_time);
		vector<fire_missile_info> ServerFireTurrentMissile(int &damage, int &radius);
	private:
		static ZSDL_Surface render_img;

		int grenade_amount;
};

#endif
