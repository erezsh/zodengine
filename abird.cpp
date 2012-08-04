#include "abird.h"

#define BIRD_MAP_PADDING 160

ZSDL_Surface ABird::bird_img[MAX_PLANET_TYPES][MAX_ANGLE_TYPES][5];

ABird::ABird(ZTime *ztime_, ZSettings *zsettings_, planet_type palette_, int map_w_, int map_h_) : ZObject(ztime_, zsettings_)
{
	palette = palette_;
	map_w = map_w_;
	map_h = map_h_;

	//defaults
	render_i = 0;
	dx = 0;
	dy = 0;
	dx_over = 0;
	dy_over = 0;
	dangle = 0;// + (rand() % 30) - 15;
	rise = 1;
	angle = 0;

	switch(palette)
	{
	case CITY:
		next_render_int = 0.03;
		break;
	default:
		next_render_int = 0.3;
		break;
	}

	ResetLocation();
}
		
void ABird::Init()
{
	char filename_c[500];
	int i, j, k;

	j=0;
	for(i=0;i<MAX_PLANET_TYPES;i++)
		for(k=0;k<5;k++)
		{
			sprintf(filename_c, "assets/other/birds/bird_%s_r%03d_n%02d.png", planet_type_string[i].c_str(), ROTATION[j], k);
			bird_img[i][j][k].LoadBaseImage(filename_c);
		}
}

void ABird::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;
	ZSDL_Surface *render_img;

	//render_img = bird_img[palette][0][render_i].CreateImage(angle, rise);
	render_img = &bird_img[palette][0][render_i];
	render_img->SetAngle(angle);
	render_img->SetSize(rise);

	if(!render_img) return;

	the_map.RenderZSurface(render_img, x, y - ((rise - 1) * 50), false, true);
	//if(the_map.GetBlitInfo(render_img->GetBaseSurface(), x - (render_img->GetBaseSurface()->w >> 1), (y - ((rise - 1) * 50)) - (render_img->GetBaseSurface()->h >> 1), from_rect, to_rect))
	//	render_img->BlitSurface(&from_rect, &to_rect);

	//if(the_map.GetBlitInfo(render_img, x - (render_img->w >> 1), (y - ((rise - 1) * 50)) - (render_img->h >> 1), from_rect, to_rect))
	//	SDL_BlitSurface( render_img, &from_rect, dest, &to_rect);

	//SDL_FreeSurface(render_img);
}

int ABird::Process()
{
	double &the_time = ztime->ztime;
	int &x = loc.x;
	int &y = loc.y;
	double time_dif;
	double dx_shift, dy_shift;

	if(the_time >= next_render_time)
	{
		next_render_time = the_time + next_render_int;

		render_i++;
		if(render_i>=5) render_i = 0;
	}

	if(the_time >= next_dangle_time)
	{
		if(dangle < 0.00001 & dangle > -0.00001)
		{
			//start it
			next_dangle_time = the_time + 5 + ((rand() % 30) * 0.1);

			dangle = (rand() % 100) - 50;
		}
		else
		{
			//stop it
			next_dangle_time = the_time + 9 + ((rand() % 50) * 0.1);

			dangle = 0;
		}
	}

	if(the_time >= next_caw_sound_time)
	{
		next_caw_sound_time = the_time + 15 + ((rand() % 50) * 0.1);

		if(!(rand()%3))
		{
			switch(palette)
			{
			case CITY:
				ZSoundEngine::PlayWavRestricted(BAT_CHIRP_SND, loc.x, loc.y, 16, 16);
				break;
			default:
				//CROW_SND
				ZSoundEngine::PlayWavRestricted(CROW_SND, loc.x, loc.y, 16, 16);
				break;
			}
		}
	}

	if(the_time >= next_height_change_time)
	{
		next_height_change_time = the_time + 15 + ((rand() % 100) * 0.1);

		rise_change_time_end = the_time + 4 + ((rand() % 60) * 0.1);
		rise_change_time_init = the_time;
		rise_change_start = rise;
		rise_change_target = 1.0 + ((rand() % 10) * 0.1);
	}

	if(rise != rise_change_target)
	{
		if(the_time > rise_change_time_end)
		{
			rise = rise_change_target;
		}
		else
		{
			rise = rise_change_start + (rise_change_target - rise_change_start) * ((the_time - rise_change_time_init) / (rise_change_time_end - rise_change_time_init));
		}
	}
	
	time_dif = (the_time - last_process_time);
	last_process_time = the_time;

	//angle
	angle += dangle * time_dif;
	while(angle >= 360) angle -= 360;
	while(angle < 0) angle += 360;
	//angle = 0;

	//recalc dx
	dx = bird_speed * cos(PI * angle / 180);
	dy = -bird_speed * sin(PI * angle / 180);

	//move
	dx_shift = (dx * time_dif) + dx_over;
	dy_shift = (dy * time_dif) + dy_over;

	//printf("%lf ... sin:%lf cos:%lf dx:%lf dy:%lf dx_dif:%lf dy_dif:%lf\n", 
	//	PI * angle / 180, 
	//	sin(PI * angle / 180), cos(PI * angle / 180), 
	//	dx, dy, 
	//	dx_shift, dy_shift);

	x += (int)dx_shift;
	y += (int)dy_shift;

	dx_over = dx_shift - (int)dx_shift;
	dy_over = dy_shift - (int)dy_shift;

	//still in an ok spot?
	if((x < -BIRD_MAP_PADDING) ||
		(y < -BIRD_MAP_PADDING) ||
		(x > BIRD_MAP_PADDING + map_w) ||
		(y > BIRD_MAP_PADDING + map_h))
			ResetLocation();
	
	return 1;
}

void ABird::ResetLocation()
{
	double &the_time = ztime->ztime;
	int &x = loc.x;
	int &y = loc.y;

	x = 0;
	y = 0;

	switch(rand() % 4)
	{
	case 0: //from the top
		x = (rand() % (map_w + BIRD_MAP_PADDING + BIRD_MAP_PADDING)) - BIRD_MAP_PADDING;
		y = -((rand() % BIRD_MAP_PADDING) + 16);
		break;
	case 1: //from the bottom
		x = (rand() % (map_w + BIRD_MAP_PADDING + BIRD_MAP_PADDING)) - BIRD_MAP_PADDING;
		y = map_h + ((rand() % BIRD_MAP_PADDING) + 16);
		break;
	case 2: //from the left
		x = -((rand() % BIRD_MAP_PADDING) + 16);
		y = (rand() % (map_h + BIRD_MAP_PADDING + BIRD_MAP_PADDING)) - BIRD_MAP_PADDING;
		break;
	case 3: //from the right
		x = map_w + ((rand() % BIRD_MAP_PADDING) + 16);
		y = (rand() % (map_h + BIRD_MAP_PADDING + BIRD_MAP_PADDING)) - BIRD_MAP_PADDING;
		break;
	}

	//testing
	//x = map_w >> 1;
	//x = map_w;
	//y = map_h >> 1;

	//point towards center
	angle = (180 / PI) * atan2(-(double)((map_h >> 1) - y), (double)((map_w >> 1) - x));
	while(angle >= 360) angle -= 360;
	while(angle < 0) angle += 360;

	//clear out
	dx_over = 0;
	dy_over = 0;

	//other stuff
	next_dangle_time = the_time + 9 + ((rand() % 50) * 0.1);
	next_caw_sound_time = the_time + 3 + ((rand() % 50) * 0.1);
	next_height_change_time = the_time + 5 + ((rand() % 100) * 0.1);
	next_render_time = the_time + next_render_int;
	last_process_time = the_time;

	rise = 1;
	rise_change_target = rise;

	switch(palette)
	{
	case CITY:
		bird_speed = 80 + (rand() % 20);
		break;
	default:
		bird_speed = 20 + (rand() % 10);
		break;
	}

	//printf("angle:%lf\n", angle);
}
