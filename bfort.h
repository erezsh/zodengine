#ifndef _BFORT_H_
#define _BFORT_H_

#include "zbuilding.h"

class BFort : public ZBuilding
{
	public:
		BFort(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT, bool is_front_ = true);
		
		static void Init();
		void SetIsFront(bool is_front_);
// 		SDL_Surface *GetRender();
		void DoPreRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		ZGuiWindow *MakeGuiWindow();
		void SetMapImpassables(ZMap &tmap);
		bool CanEnterFort(int team);
		bool UnderCursorCanAttack(int &map_x, int &map_y);
		bool UnderCursorFortCanEnter(int &map_x, int &map_y);
		bool CannonNotPlacable(int &map_left, int &map_right, int &map_top, int &map_bottom);
		bool CanSetRallypoints() { return true; }
		bool ProducesUnits() { return true; }
	private:
		void RenderUnitCover(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);

		static ZSDL_Surface base_front[MAX_PLANET_TYPES];
		static ZSDL_Surface base_back[MAX_PLANET_TYPES];
		static ZSDL_Surface base_front_destroyed[MAX_PLANET_TYPES];
		static ZSDL_Surface base_back_destroyed[MAX_PLANET_TYPES];
		static ZSDL_Surface base_front_destroyed_overlay[MAX_PLANET_TYPES];
		static ZSDL_Surface base_back_destroyed_overlay[MAX_PLANET_TYPES];
		static ZSDL_Surface flag[MAX_TEAM_TYPES][4];
		static ZSDL_Surface destroyed_overlay;
		
		bool is_front;
		int flag_i;
		double next_flag_time;
		double destroyed_fade;
		double last_fade_time;
		double fade_dir;
};

#endif
