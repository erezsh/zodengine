#ifndef _ABIRD_H_
#define _ABIRD_H_

#include "zobject.h"

class ABird : public ZObject
{
	public:
		ABird(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT, int map_w_ = 0, int map_h_ = 0);
		
		static void Init();

		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		int Process();

		void ResetLocation();

	private:
		static ZSDL_Surface bird_img[MAX_PLANET_TYPES][MAX_ANGLE_TYPES][5];

		planet_type palette;
		int map_w, map_h;

		double dx, dy;
		double dx_over, dy_over;
		double dangle;

		double rise;
		double angle;
		int render_i;

		double bird_speed;
		double next_render_int;
		
		double next_render_time;
		double last_process_time;
		double next_dangle_time;
		double next_caw_sound_time;
		double next_height_change_time;

		//rise change stuff
		double rise_change_time_end;
		double rise_change_time_init;
		double rise_change_target;
		double rise_change_start;
};

#endif
