#ifndef _RPYRO_H_
#define _RPYRO_H_

#include "zrobot.h"

class RPyro : public ZRobot
{
	public:
		RPyro(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void PlaySelectedWav();
		void PlaySelectedAnim(ZPortrait &portrait);
	private:
		static ZSDL_Surface fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
};

#endif
