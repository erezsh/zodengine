#include "etoughsmoke.h"

bool EToughSmoke::finished_init = false;
ZSDL_Surface EToughSmoke::render_img[8];

EToughSmoke::EToughSmoke(ZTime *ztime_, int x_, int y_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	if(!render_img[0].GetBaseSurface())
	{
		killme = true;
		return;
	}

	render_i = 0;

	//x = x_ - (render_img[0].GetBaseSurface()->w >> 1);
	//y = y_ - (render_img[0].GetBaseSurface()->h >> 1);
	x = x_;
	y = y_;

	next_process_time = the_time + 0.12;
}

void EToughSmoke::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<8;i++)
		{
			sprintf(filename_c, "assets/units/robots/tough/smoke_n%02d.png", i);
			render_img[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		}

	finished_init = true;
}

void EToughSmoke::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_process_time)
	{
		next_process_time = the_time + 0.12;

		render_i++;
		if(render_i>=8)
			killme = true;
	}

}

void EToughSmoke::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&render_img[render_i], x, y, false, true);
	//if(zmap.GetBlitInfo( render_img[render_i], x, y, from_rect, to_rect))
	//	SDL_BlitSurface( render_img[render_i], &from_rect, dest, &to_rect);
}
