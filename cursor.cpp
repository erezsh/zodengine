#include "cursor.h"
#include "zteam.h"

ZSDL_Surface ZCursor::cursor[MAX_CURSOR_TYPES][MAX_TEAM_TYPES][4];

ZCursor::ZCursor(team_type owner_, cursor_type current_cursor_)
{
	owner = owner_;
	current_cursor = current_cursor_;
	cursor_i = 0;
	current_surface = &cursor[current_cursor][owner][cursor_i];
}

void ZCursor::Init()
{
	int i,j;
	char filename_c[500];
	SDL_Surface *temp_surface;

	//load up the null cursors
	for(j=0;j<4;j++)
	{
		sprintf(filename_c, "assets/cursors/cursor_null_n%02d.png", j);
		cursor[CURSOR_C][0][j].LoadBaseImage(filename_c);

		sprintf(filename_c, "assets/cursors/placed_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[PLACED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[PLACE_C][0][j].LoadBaseImage(temp_surface);
		//cursor[PLACED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[PLACE_C][0][j] = cursor[PLACED_C][0][j];

		sprintf(filename_c, "assets/cursors/attacked_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[ATTACKED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[ATTACK_C][0][j].LoadBaseImage(temp_surface);
		//cursor[ATTACKED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[ATTACK_C][0][j] = cursor[ATTACKED_C][0][j];

		sprintf(filename_c, "assets/cursors/grabbed_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[GRABBED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[GRAB_C][0][j].LoadBaseImage(temp_surface);
		//cursor[GRABBED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[GRAB_C][0][j] = cursor[GRABBED_C][0][j];

		sprintf(filename_c, "assets/cursors/grenaded_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[GRENADED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[GRENADE_C][0][j].LoadBaseImage(temp_surface);
		//cursor[GRENADED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[GRENADE_C][0][j] = cursor[GRENADED_C][0][j];

		sprintf(filename_c, "assets/cursors/repaired_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[REPAIRED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[REPAIR_C][0][j].LoadBaseImage(temp_surface);
		//cursor[REPAIRED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[REPAIR_C][0][j] = cursor[REPAIRED_C][0][j];
		
		sprintf(filename_c, "assets/cursors/entered_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[ENTERED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[ENTER_C][0][j].LoadBaseImage(temp_surface);
		//cursor[ENTERED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[ENTER_C][0][j] = cursor[ENTERED_C][0][j];

		sprintf(filename_c, "assets/cursors/exited_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[EXITED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[EXIT_C][0][j].LoadBaseImage(temp_surface);
		//cursor[EXITED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[EXIT_C][0][j] = cursor[EXITED_C][0][j];

		sprintf(filename_c, "assets/cursors/cannoned_n%02d.png", j);
		temp_surface = IMG_Load(filename_c);
		cursor[CANNONED_C][0][j].LoadBaseImage(temp_surface, false);
		cursor[CANNON_C][0][j].LoadBaseImage(temp_surface);
		//cursor[CANNONED_C][0][j] = IMG_Load_Error(filename_c);
		//cursor[CANNON_C][0][j] = cursor[CANNONED_C][0][j];
	}

	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<4;j++)
		{
			sprintf(filename_c, "assets/cursors/cursor_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[CURSOR_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[CURSOR_C][ZTEAM_BASE_TEAM][j], cursor[CURSOR_C][i][j], filename_c);

			sprintf(filename_c, "assets/cursors/place_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[PLACE_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[PLACE_C][ZTEAM_BASE_TEAM][j], cursor[PLACE_C][i][j], filename_c);

			//cursor[PLACED_C][i][j] = cursor[PLACED_C][0][j];
			cursor[PLACED_C][i][j].LoadBaseImage(cursor[PLACED_C][0][j].GetBaseSurface() ,false);

			sprintf(filename_c, "assets/cursors/attack_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[ATTACK_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[ATTACK_C][ZTEAM_BASE_TEAM][j], cursor[ATTACK_C][i][j], filename_c);

			//cursor[ATTACKED_C][i][j] = cursor[ATTACKED_C][0][j];
			cursor[ATTACKED_C][i][j].LoadBaseImage(cursor[ATTACKED_C][0][j].GetBaseSurface() ,false);

			sprintf(filename_c, "assets/cursors/grab_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[GRAB_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[GRAB_C][ZTEAM_BASE_TEAM][j], cursor[GRAB_C][i][j], filename_c);

			//cursor[GRABBED_C][i][j] = cursor[GRABBED_C][0][j];
			cursor[GRABBED_C][i][j].LoadBaseImage(cursor[GRABBED_C][0][j].GetBaseSurface() ,false);

			sprintf(filename_c, "assets/cursors/grenade_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[GRENADE_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[GRENADE_C][ZTEAM_BASE_TEAM][j], cursor[GRENADE_C][i][j], filename_c);

			//cursor[GRENADED_C][i][j] = cursor[GRENADED_C][0][j];
			cursor[GRENADED_C][i][j].LoadBaseImage(cursor[GRENADED_C][0][j].GetBaseSurface() ,false);

			sprintf(filename_c, "assets/cursors/repair_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[REPAIR_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[REPAIR_C][ZTEAM_BASE_TEAM][j], cursor[REPAIR_C][i][j], filename_c);

			//cursor[REPAIRED_C][i][j] = cursor[REPAIRED_C][0][j];
			cursor[REPAIRED_C][i][j].LoadBaseImage(cursor[REPAIRED_C][0][j].GetBaseSurface() ,false);

			sprintf(filename_c, "assets/cursors/nono_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[NONO_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[NONO_C][ZTEAM_BASE_TEAM][j], cursor[NONO_C][i][j], filename_c);

			sprintf(filename_c, "assets/cursors/cannon_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[CANNON_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[CANNON_C][ZTEAM_BASE_TEAM][j], cursor[CANNON_C][i][j], filename_c);

			//cursor[CANNONED_C][i][j] = cursor[CANNONED_C][0][j];
			cursor[CANNONED_C][i][j].LoadBaseImage(cursor[CANNONED_C][0][j].GetBaseSurface() ,false);

			sprintf(filename_c, "assets/cursors/enter_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[ENTER_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[ENTER_C][ZTEAM_BASE_TEAM][j], cursor[ENTER_C][i][j], filename_c);

			//cursor[ENTERED_C][i][j] = cursor[ENTERED_C][0][j];
			cursor[ENTERED_C][i][j].LoadBaseImage(cursor[ENTERED_C][0][j].GetBaseSurface() ,false);

			sprintf(filename_c, "assets/cursors/exit_%s_n%02d.png", team_type_string[i].c_str(), j);
			//cursor[EXIT_C][i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, cursor[EXIT_C][ZTEAM_BASE_TEAM][j], cursor[EXIT_C][i][j], filename_c);

			//cursor[EXITED_C][i][j] = cursor[EXITED_C][0][j];
			cursor[EXITED_C][i][j].LoadBaseImage(cursor[EXITED_C][0][j].GetBaseSurface() ,false);
		}
}

void ZCursor::SetCursor(cursor_type new_cursor)
{
	current_cursor = new_cursor;
	current_surface = &cursor[current_cursor][owner][cursor_i];
}

cursor_type ZCursor::GetCursor()
{
	return current_cursor;
}

void ZCursor::SetTeam(team_type owner_)
{
	owner = owner_;
	current_surface = &cursor[current_cursor][owner][cursor_i];
}

void ZCursor::Render(ZMap &the_map, SDL_Surface *dest, int &x, int &y, bool restrict_to_map)
{
	SDL_Rect from_rect, to_rect;
	int x_shift, y_shift;

	if(!current_surface) return;

	if(current_cursor > CURSOR_C)
	{
		x_shift = -8;
		y_shift = -8;
	}
	else
	{
		x_shift = 0;
		y_shift = 0;
	}

	if(restrict_to_map)
	{

		if(the_map.GetBlitInfo(current_surface->GetBaseSurface(), x + x_shift, y + y_shift, from_rect, to_rect))
			current_surface->BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface( current_surface, &from_rect, dest, &to_rect);
	}
	else
	{
		to_rect.x = x + x_shift;
		to_rect.y = y + y_shift;

		current_surface->BlitSurface(NULL, &to_rect);
		//SDL_BlitSurface( current_surface, NULL, dest, &to_rect);
	}
}

void ZCursor::Process(double the_time)
{
	const double time_inc = 0.2;

	if(the_time < next_process_time) return;

	next_process_time = the_time + time_inc;

	cursor_i++;
	if(cursor_i >= 4) cursor_i = 0;

	current_surface = &cursor[current_cursor][owner][cursor_i];
}
