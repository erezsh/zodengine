#include "etanksmoke.h"

bool ETankSmoke::finished_init = false;
ZSDL_Surface ETankSmoke::tank_smoke[MAX_ANGLE_TYPES][7];
ZSDL_Surface ETankSmoke::tank_spark[MAX_ANGLE_TYPES][4];

#define ETANKSMOKE_TIME 0.15

ETankSmoke::ETankSmoke(ZTime *ztime_, int cx_, int cy_, int direction_, bool do_spark_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	cx = cx_;
	cy = cy_;
	ni = 0;
	direction = direction_;
	do_spark = do_spark_;

	SetCoords(cx, cy, direction, x, y);

	next_ni_time = the_time + ETANKSMOKE_TIME;
}

void ETankSmoke::Init()
{
	char filename[500];

	for(int r=0;r<MAX_ANGLE_TYPES;r++)
		for(int i=0;i<7;i++)
		{
			sprintf(filename, "assets/units/vehicles/track_dust_r%03d_n%02d.png", ROTATION[r], i);
			tank_smoke[r][i].LoadBaseImage(filename);

			if(i<4)
			{
				sprintf(filename, "assets/units/vehicles/track_spark_r%03d_n%02d.png", ROTATION[r], i);
				tank_spark[r][i].LoadBaseImage(filename);
			}
		}

	finished_init = true;
}

void ETankSmoke::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_ni_time)
	{
		next_ni_time = the_time + ETANKSMOKE_TIME;

		ni++;

		if(ni>=7)
		{
			ni=0;
			killme=true;
		}
	}
}

void ETankSmoke::DoPreRender(ZMap &zmap, SDL_Surface *dest)
{
	ZSDL_Surface *render_img;

	if(killme) return;

	if(do_spark && ni<4)
		render_img = &tank_spark[direction][ni];
	else
		render_img = &tank_smoke[direction][ni];

	if(!render_img) return;
	if(!render_img->GetBaseSurface()) return;

	zmap.RenderZSurface(render_img, x, y);
}

void ETankSmoke::SetCoords(int cx_, int cy_, int dir_, int &x_, int &y_)
{
	int shift;

	x_ = cx_;
	y_ = cy_;

	switch(dir_)
	{
	case R0:
		x_-=15;
		y_+=rand()%8;

		if(tank_smoke[dir_][0].GetBaseSurface())
		{
			x_ -= tank_smoke[dir_][0].GetBaseSurface()->w;
			y_ -= tank_smoke[dir_][0].GetBaseSurface()->h;
		}
		break;
	case R180:
		x_+=15;
		y_+=rand()%8;

		if(tank_smoke[dir_][0].GetBaseSurface())
			y_ -= tank_smoke[dir_][0].GetBaseSurface()->h;
		break;
	case R90:
		y_+=15;
		x_-=12;
		x_+=rand()%9;
		break;
	case R270:
		y_-=15;
		x_-=12;
		x_+=rand()%9;

		if(tank_smoke[dir_][0].GetBaseSurface())
			y_ -= tank_smoke[dir_][0].GetBaseSurface()->h;
		break;
	case R45:
		x_-=13;
		y_+=7;
		
		shift=rand()%7;
		x_+=shift;
		y_+=shift;

		if(tank_smoke[dir_][0].GetBaseSurface())
			x_ -= tank_smoke[dir_][0].GetBaseSurface()->w;
		break;
	case R135:
		x_+=13;
		y_+=7;

		shift=rand()%7;
		x_-=shift;
		y_+=shift;

		break;
	case R225:
		x_+=6;
		y_-=12;

		shift=rand()%7;
		x_+=shift;
		y_+=shift;

		if(tank_smoke[dir_][0].GetBaseSurface())
			y_ -= tank_smoke[dir_][0].GetBaseSurface()->h;
		break;
	case R315:
		x_-=6;
		y_-=12;

		shift=rand()%7;
		x_-=shift;
		y_+=shift;

		if(tank_smoke[dir_][0].GetBaseSurface())
		{
			x_ -= tank_smoke[dir_][0].GetBaseSurface()->w;
			y_ -= tank_smoke[dir_][0].GetBaseSurface()->h;
		}
		break;
	}
}
