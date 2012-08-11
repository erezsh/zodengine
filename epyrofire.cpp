#include "epyrofire.h"

bool EPyroFire::finished_init = false;
ZSDL_Surface EPyroFire::fire_img[5][6];
const int pyro_fire_max[5] = {4,4,4,6,6};

EPyroFire::EPyroFire(ZTime *ztime_, int x_, int y_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	fire_i = rand() % 5;
	fire_j = 0;

	if(!fire_img[fire_i][0].GetBaseSurface())
	{
		killme = true;
		return;
	}

	x = x_ - (fire_img[fire_i][0].GetBaseSurface()->w >> 1);
	y = y_ - (fire_img[fire_i][0].GetBaseSurface()->h >> 1);

	next_process_time = the_time + 0.06;
}

void EPyroFire::Init()
{
	int i,j;
	char filename_c[500];

	for(i=0;i<5;i++)
		for(j=0;j<pyro_fire_max[i];j++)
		{
			sprintf(filename_c, "assets/other/fire/fire%d_n%02d.png", i, j);
			fire_img[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		}

	finished_init = true;
}

void EPyroFire::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_process_time)
	{
		next_process_time = the_time + 0.06;

		fire_j++;
		if(fire_j>=pyro_fire_max[fire_i])
			killme = true;
	}

}

void EPyroFire::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&fire_img[fire_i][fire_j], x, y);
	//if(zmap.GetBlitInfo(fire_img[fire_i][fire_j], x, y, from_rect, to_rect))
	//	SDL_BlitSurface( fire_img[fire_i][fire_j], &from_rect, dest, &to_rect);
}
