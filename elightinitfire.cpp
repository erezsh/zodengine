#include "elightinitfire.h"

bool ELightInitFire::finished_init = false;
ZSDL_Surface ELightInitFire::render_img[4];

ELightInitFire::ELightInitFire(ZTime *ztime_, int x_, int y_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	render_i = rand() % 4;

	x = x_ - 8;//(render_img[0]->w / 2);
	y = y_ - 7;//(render_img[0]->h / 2);

	next_process_time = the_time + 0.02;
}

void ELightInitFire::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<4;i++)
		{
			sprintf(filename_c, "assets/units/vehicles/light/initfire_n%02d.png", i);
			render_img[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		}

	finished_init = true;
}

void ELightInitFire::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_process_time)
	{
		next_process_time = the_time + 0.02;

		killme = true;
	}
}

void ELightInitFire::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&render_img[render_i], x, y);
	//if(zmap.GetBlitInfo( render_img[render_i], x, y, from_rect, to_rect))
	//	SDL_BlitSurface( render_img[render_i], &from_rect, dest, &to_rect);
}
