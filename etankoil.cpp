#include "etankoil.h"

bool ETankOil::finished_init = false;
ZSDL_Surface ETankOil::tank_oil[3][3];

#define ETANKOIL_TIME 3.0

ETankOil::ETankOil(ZTime *ztime_, int cx_, int cy_, int direction_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	SetCoords(cx_, cy_, direction_);

	oil_i = rand()%3;
	ni=0;

	next_ni_time = the_time + ETANKOIL_TIME + ((rand()%10) * 0.1);
}

void ETankOil::Init()
{
	char filename[500];

	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
		{
			sprintf(filename, "assets/units/vehicles/tank_oil_%d_n%02d.png", i, j);
			tank_oil[i][j].LoadBaseImage(filename);
		}

	finished_init = true;
}

void ETankOil::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_ni_time)
	{
		next_ni_time = the_time + ETANKOIL_TIME + ((rand()%10) * 0.1);

		ni++;

		if(ni>=3)
		{
			ni=0;
			killme=true;
		}
	}
}

void ETankOil::DoPreRender(ZMap &zmap, SDL_Surface *dest)
{
	ZSDL_Surface *render_img;

	if(killme) return;

	render_img = &tank_oil[oil_i][ni];

	if(!render_img) return;

	zmap.RenderZSurface(render_img, cx, cy, false, true);
}

void ETankOil::SetCoords(int cx_, int cy_, int direction_)
{
	cx = cx_;
	cy = cy_;
	direction = direction_;

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

	cy+=5;

	cx-=3;
	cy-=3;
	cx+=rand()%7;
	cy+=rand()%7;
}
