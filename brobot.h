#ifndef _BROBOT_H_
#define _BROBOT_H_

#include "zbuilding.h"

class BRobot : public ZBuilding
{
	public:
		BRobot(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		ZGuiWindow *MakeGuiWindow();
		void SetMapImpassables(ZMap &tmap);
		bool CanSetRallypoints() { return true; }
		bool ProducesUnits() { return true; }

		bool GetCraneEntrance(int &x, int &y, int &x2, int &y2);
		bool GetCraneCenter(int &x, int &y);
	private:
		static ZSDL_Surface base[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
		static ZSDL_Surface base_destroyed[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
		static ZSDL_Surface base_color[MAX_TEAM_TYPES];
		static ZSDL_Surface base_color_destroyed[MAX_TEAM_TYPES];
		
		static ZSDL_Surface spin[8];
		static ZSDL_Surface green_box[6];
		static ZSDL_Surface robot[2];
		static ZSDL_Surface single_light[2];
		static ZSDL_Surface double_light[2];
		
		int spin_i;
		int green_box_i;
		int robot_i;
		int single_light_on[3];
		int double_light_i;
		int exhaust_i;
};

#endif
