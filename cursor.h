#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "constants.h"
#include "zsdl.h"
#include "zmap.h"

class ZCursor
{
	public:
		ZCursor(team_type owner_ = RED_TEAM, cursor_type current_cursor_ = CURSOR_C);

		static void Init();

		void Process(double the_time);
		void Render(ZMap &the_map, SDL_Surface *dest, int &x, int &y, bool restrict_to_map = false);
		void SetCursor(cursor_type new_cursor);
		cursor_type GetCursor();
		void SetTeam(team_type owner_);
	private:
		static ZSDL_Surface cursor[MAX_CURSOR_TYPES][MAX_TEAM_TYPES][4];

		team_type owner;
		double next_process_time;
		int cursor_i;
		cursor_type current_cursor;
		ZSDL_Surface *current_surface;
};

#endif
