#ifndef _RGRUNT_H_
#define _RGRUNT_H_

#include "zrobot.h"

class RGrunt : public ZRobot
{
	public:
		RGrunt(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void PlaySelectedWav();
		void PlaySelectedAnim(ZPortrait &portrait);
	private:
		//static SDL_Surface *fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][5];
		static ZSDL_Surface fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][5];
};

#endif
