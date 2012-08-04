#include "zgui_main_menu_widgets.h"

bool GMMWTeamColor::finished_init=false;

ZSDL_Surface GMMWTeamColor::team_color_img[MAX_TEAM_TYPES];

GMMWTeamColor::GMMWTeamColor() : ZGMMWidget()
{
	widget_type = MMTEAM_COLOR_WIDGET;

	h = MMTEAM_COLOR_HEIGHT;
	w = MMTEAM_COLOR_WIDTH;

	team = NULL_TEAM;
}

void GMMWTeamColor::Init()
{
	for(int i=0;i<MAX_TEAM_TYPES;i++)
	{
		string filename;

		filename = "assets/other/main_menu_gui/team_color_" + team_type_string[i] + ".png";

		ZTeam::LoadZSurface(i, team_color_img[ZTEAM_BASE_TEAM], team_color_img[i], filename);
	}

	finished_init=true;
}

void GMMWTeamColor::SetTeam(int team_)
{
	team=team_;

	if(team<0) team = 0;
	if(team>=MAX_TEAM_TYPES) team = 0;
}

void GMMWTeamColor::DoRender(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	if(!finished_init) return;
	if(!active) return;

	if(team<0) return;
	if(team>=MAX_TEAM_TYPES) return;

	tx += x;
	ty += y;

	team_color_img[team].BlitSurface(NULL, tx, ty);
}
