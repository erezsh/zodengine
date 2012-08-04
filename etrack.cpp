#include "etrack.h"

bool ETrack::finished_init = false;

ZSDL_Surface ETrack::track_img[MAX_ETRACK_TYPES][MAX_PLANET_TYPES][MAX_ANGLE_TYPES][3];

ETrack::ETrack(ZTime *ztime_, int cx_, int cy_, int dir_, int type_, int p_, bool lay1, bool lay2) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	start_time = the_time + (0.1 * (rand()%10));

	cx = cx_;
	cy = cy_;
	dir = dir_;
	type = type_;
	p = p_;

	if(type<0) type=0;
	if(type>=MAX_ETRACK_TYPES) type=0;
	if(p<0) p=0;
	if(p>=MAX_PLANET_TYPES) p=0;
	if(dir<0) dir=0;
	if(dir>=MAX_ANGLE_TYPES) dir=0;

	lay_track[0] = lay1;
	lay_track[1] = lay2;
	ti = 0;

	if(!track_img[type][p][dir][ti].GetBaseSurface())
	{
		killme = true;
		return;
	}

	SetTrackCoords(cx, cy, dir, x, y);
}

void ETrack::Init()
{
	int i,j,k,l;
	char filename_c[500];

	for(i=0;i<MAX_ETRACK_TYPES;i++) 
	for(j=0;j<MAX_PLANET_TYPES;j++) 
	{
		if(j==CITY) continue;
		if(i==ET_JEEP && j!=DESERT) continue;

		for(k=0;k<4;k++)
		for(l=0;l<3;l++)
		{
			sprintf(filename_c, "assets/units/vehicles/track_effects/%s_track_%s_r%03d_n%02d.png", 
				etrack_type_string[i].c_str(), 
				planet_type_string[j].c_str(),
				ROTATION[k],
				l);

			track_img[i][j][k][l].LoadBaseImage(filename_c);
			track_img[i][j][k+4][l] = track_img[i][j][k][l];
		}
	}
	
	finished_init = true;
}

void ETrack::Process()
{
	double &the_time = ztime->ztime;
	double delta;

	if(killme) return;

	delta = the_time - start_time;

	if(delta >= 3.9) killme = true;
	else if(delta >= 3.6) ti = 2;
	else if(delta >= 3.3) ti = 1;
	else ti = 0;
}

void ETrack::DoPreRender(ZMap &zmap, SDL_Surface *dest)
{
	if(killme) return;

	for(int i=0;i<2;i++)
		if(lay_track[i])
			zmap.RenderZSurface(&track_img[type][p][dir][ti], x[i], y[i], false, true);
}

void ETrack::SetTrackCoords(int cx_, int cy_, int dir_, int *x_, int *y_)
{
	switch(dir_)
	{
	case 0: //0
		x_[0]=x_[1]= cx_-15;
		y_[0]=cy_-6;
		y_[1]=cy_+6;
		y_[0]+=4;
		y_[1]+=4;
		break;
	case 4: //180
		x_[0]=x_[1]=cx_+17;
		y_[0]=cy_-6;
		y_[1]=cy_+6;
		y_[0]+=4;
		y_[1]+=4;
		break;
	case 2: //90
		y_[0]=y_[1]=cy_+15;
		x_[0]=cx_-8;
		x_[1]=cx_+8;
		break;
	case 6: //270
		y_[0]=y_[1]=cy_-11;
		x_[0]=cx_-8;
		x_[1]=cx_+8;
		break;
	case 1: //45
		x_[0]=-14;
		y_[0]=4;
		x_[1]=-y_[0];
		y_[1]=-x_[0];
		x_[0]+=cx_;
		x_[1]+=cx_;
		y_[0]+=(cy_+3);
		y_[1]+=(cy_+3);
		break;
	case 5: //225
		x_[0]=14;
		y_[0]=-4;
		x_[1]=-y_[0];
		y_[1]=-x_[0];
		x_[0]+=(cx_-1);
		x_[1]+=(cx_-1);
		y_[0]+=(cy_+4);
		y_[1]+=(cy_+4);
		break;
	case 3: //135
		x_[0]=14;
		y_[0]=4;
		x_[1]=y_[0];
		y_[1]=x_[0];
		x_[0]+=(cx_+1);
		x_[1]+=(cx_+1);
		y_[0]+=(cy_+4);
		y_[1]+=(cy_+4);
		break;
	case 7: //315
		x_[0]=-14;
		y_[0]=-4;
		x_[1]=y_[0];
		y_[1]=x_[0];
		x_[0]+=cx_;
		x_[1]+=cx_;
		y_[0]+=(cy_+3);
		y_[1]+=(cy_+3);
		break;
	default:
		x_[0]=x_[1]=cx_;
		y_[0]=y_[1]=cy_;
		break;
	}

	int x_r, y_r;

	x_r=rand()%2;
	y_r=rand()%2;

	x_[0]+=x_r;
	x_[1]+=x_r;
	y_[0]+=y_r;
	y_[1]+=y_r;
}
