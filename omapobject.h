#ifndef _OMAPOBJECT_H_
#define _OMAPOBJECT_H_

#include "zobject.h"

class OMapObject : public ZObject
{
	public:
		OMapObject(ZTime *ztime_, ZSettings *zsettings_ = NULL, int object_i_ = MAP0_ITEM);
		
		static void Init();

		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		int Process();

		void SetOwner(team_type owner_);
		void SetMapImpassables(ZMap &tmap);
		void UnSetMapImpassables(ZMap &tmap);
		//bool ServerFireTurrentMissile(int &x_, int &y_, int &damage, int &radius, double &offset_time);
		vector<fire_missile_info> ServerFireTurrentMissile(int &damage, int &radius);
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		void FireTurrentMissile(int x_, int y_, double offset_time);

		void SetType(int object_i_);

		bool IsDestroyableImpass() { return true; }
		bool CausesImpassAtCoord(int x, int y);
	private:
		static ZSDL_Surface render_img[MAP_ITEMS_AMOUNT];

		int object_i;
};

#endif
