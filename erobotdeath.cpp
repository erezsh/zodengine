#include "erobotdeath.h"

bool ERobotDeath::finished_init = false;
ZSDL_Surface ERobotDeath::melt[MAX_TEAM_TYPES][17];
ZSDL_Surface ERobotDeath::die[4][MAX_TEAM_TYPES][10];

const int die_max_i[4] = {10, 10, 10, 8};

ERobotDeath::ERobotDeath(ZTime *ztime_, int x_, int y_, int team, bool do_melt_death) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	die_i = rand() % 4;
	render_i = 0;
	owner = team;
	

	x = x_;
	y = y_;

	if(owner == NULL_TEAM)
	{
		killme = true;
		return;
	}

	next_process_time = the_time + 0.16;

	if(do_melt_death)
	{
		max_render_i = 17;
		render_img = melt[owner];
	}
	else
	{
		max_render_i = die_max_i[die_i];
		render_img = die[die_i][owner];
	}
}

void ERobotDeath::Init()
{
	int i, j, k;
	char filename_c[500];

	for(i=1;i<MAX_TEAM_TYPES;i++)
	{
		for(j=0;j<4;j++)
			for(k=0;k<die_max_i[j];k++)
			{
				sprintf(filename_c, "assets/units/robots/die%d_%s_n%02d.png", j+1, team_type_string[i].c_str(), k);
				//die[j][i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
				ZTeam::LoadZSurface(i, die[j][ZTEAM_BASE_TEAM][k], die[j][i][k], filename_c);
			}

		for(k=0;k<17;k++)
		{
			sprintf(filename_c, "assets/units/robots/melt_%s_n%02d.png", team_type_string[i].c_str(), k);
			//melt[i][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, melt[ZTEAM_BASE_TEAM][k], melt[i][k], filename_c);
		}
	}

	finished_init = true;
}

void ERobotDeath::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_process_time)
	{
		next_process_time = the_time + 0.16;

		render_i++;
		if(render_i>=max_render_i)
			killme = true;
	}

}

void ERobotDeath::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&render_img[render_i], x, y);
	//if(zmap.GetBlitInfo(render_img[render_i], x, y, from_rect, to_rect))
	//	SDL_BlitSurface( render_img[render_i], &from_rect, dest, &to_rect);
}