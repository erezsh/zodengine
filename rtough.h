#ifndef _RTOUGH_H_
#define _RTOUGH_H_

#include "zrobot.h"

class RTough : public ZRobot
{
	public:
		RTough(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void PlaySelectedWav();
		void PlaySelectedAnim(ZPortrait &portrait);
		void FireMissile(int x_, int y_);
		bool CanPickupGrenades() { return false; }
		virtual bool CanHaveGrenades() { return false; }
		virtual bool CanThrowGrenades() { return false; }
	private:
		static ZSDL_Surface fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
};

#endif
