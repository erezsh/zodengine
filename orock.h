#ifndef _OROCK_H_
#define _OROCK_H_

#include "zobject.h"

class ORock : public ZObject
{
	public:
		ORock(ZTime *ztime_, ZSettings *zsettings_ = NULL, int palette_ = DESERT);
		
		static void Init();
		
		static void SetupRockRenders(ZMap &the_map, vector<ZObject*> object_list);
		static void EditRockRender(ZMap &the_map, vector<ZObject*> object_list, ZObject *robj, bool do_add);
		void SetupRockRender(bool **rock_list, int map_w, int map_h);

		void DoPreRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		int Process();
		void SetOwner(team_type owner_);
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		void SetMapImpassables(ZMap &tmap);
		void UnSetMapImpassables(ZMap &tmap);
		void CreationMapEffects(ZMap &tmap);
		void DeathMapEffects(ZMap &tmap);

		void ChangePalette(int palette_);
		void SetDefaultRender();
		void ClearRender();

		bool IsDestroyableImpass() { return true; }
		bool CausesImpassAtCoord(int x, int y);
	private:
		static ZSDL_Surface rock_pal[MAX_PLANET_TYPES];

		static ZSDL_Surface rock_center_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_up_left_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_up_right_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_down_right_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_down_left_top[MAX_PLANET_TYPES];

		static ZSDL_Surface rock_up_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_down_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_right_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_left_top[MAX_PLANET_TYPES];

		static ZSDL_Surface rock_vert_up_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_vert_mid_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_vert_down_top[MAX_PLANET_TYPES];

		static ZSDL_Surface rock_horz_left_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_horz_mid_top[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_horz_right_top[MAX_PLANET_TYPES];

		static ZSDL_Surface rock_left_mid_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_left_bottom_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_mid_mid_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_mid_bottom_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_right_mid_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_right_bottom_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_single_mid_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_single_bottom_under[MAX_PLANET_TYPES];

		static ZSDL_Surface rock_up_shadow[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_mid_shadow[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_bottom_shadow[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_mid_mid_shadow_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_mid_bottom_shadow_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_right_mid_shadow_under[MAX_PLANET_TYPES];
		static ZSDL_Surface rock_right_bottom_shadow_under[MAX_PLANET_TYPES];

		static ZSDL_Surface rock_destroyed[MAX_PLANET_TYPES][6];


		int palette;
		ZSDL_Surface *render_img[2][3];
};

#endif
