#include "etankspark.h"

bool ETankSpark::finished_init = false;
ZSDL_Surface ETankSpark::tank_spark[6];

#define ETANKSPARK_TIME 0.1

ETankSpark::ETankSpark(ZTime *ztime_, int cx_, int cy_, int direction_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	SetCoords(cx_, cy_, direction_);

	ni_max = 36 + (rand()%25);
	ni_i = 0;
	ni = 0;

	next_ni_time = the_time + ETANKSPARK_TIME;
}

void ETankSpark::Init()
{
	char filename[500];

	for(int i=0;i<6;i++)
	{
		sprintf(filename, "assets/units/vehicles/ground_spark_n%02d.png", i);
		tank_spark[i].LoadBaseImage(filename);
	}

	finished_init = true;
}

void ETankSpark::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_ni_time)
	{
		next_ni_time = the_time + ETANKSPARK_TIME;

		ni++;
		ni_i++;

		if(ni>=6) ni=0;
		if(ni_i>=ni_max) killme=true;
	}
}

void ETankSpark::DoPreRender(ZMap &zmap, SDL_Surface *dest)
{
	ZSDL_Surface *render_img;

	if(killme) return;

	render_img = &tank_spark[ni];

	if(!render_img) return;

	zmap.RenderZSurface(render_img, cx, cy, false, true);
}

void ETankSpark::SetCoords(int cx_, int cy_, int dir_)
{
	cx = cx_;
	cy = cy_;
	direction = dir_;

	//if(tank_spark[0].GetBaseSurface())
	//{
	//	cx -= tank_spark[0].GetBaseSurface()->w >> 1;
	//	cy -= tank_spark[0].GetBaseSurface()->h;
	//}


	switch(direction)
	{
	case R0:
		cx-=5;
		break;
	case R180:
		cx+=5;
		break;
	case R90:
		cy+=5;
		break;
	case R270:
		cy-=5;
		break;
	case R45:
		cx-=4;
		cy+=4;
		break;
	case R135:
		cx+=4;
		cy+=4;
		break;
	case R225:
		cx+=4;
		cy-=4;
		break;
	case R315:
		cx-=4;
		cy-=4;
		break;
	}

	cy+=3;

	cx-=5;
	cy-=5;
	cx+=rand()%11;
	cy+=rand()%11;
}
