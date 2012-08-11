#include "ahutanimal.h"

void hut_animal_graphics::LoadGraphics(int ha_num)
{
	char filename[1500];

	//walk_i
	switch(ha_num)
	{
	case GREEN_SNAKE: walk_i = 8; break;
	default: walk_i = 4; break;
	}

	//look_i
	switch(ha_num)
	{
	case GREEN_SNAKE:
	case YELLOW_WORM:
	case RED_WORM:
		look_i = 0; break;
	default: 
		look_i = 4; break;
	}

	//walk_to_zero
	switch(ha_num)
	{
		case DESERT_RABIT:
		case PIG_DINO:
		case ARCTIC_RABIT:
			walk_to_zero = true;
			break;
		default:
			walk_to_zero = false;
			break;
	}

	//dead images
	sprintf(filename, "assets/other/hut_animals/%s_dead_down.png", hut_animal_type_string[ha_num].c_str());
	dead_down.LoadBaseImage(filename);
	sprintf(filename, "assets/other/hut_animals/%s_dead_up.png", hut_animal_type_string[ha_num].c_str());
	dead_up.LoadBaseImage(filename);

	//walk images
	for(int r=0;r<MAX_ANGLE_TYPES;r++)
		for(int i=0;i<walk_i;i++)
		{
			sprintf(filename, "assets/other/hut_animals/%s_walk_r%03d_n%02d.png", hut_animal_type_string[ha_num].c_str(), ROTATION[r], i);
			walk[r][i].LoadBaseImage(filename);
		}

	//look images
	for(int r=0;r<MAX_ANGLE_TYPES;r+=2)
		for(int i=0;i<look_i;i++)
		{
			sprintf(filename, "assets/other/hut_animals/%s_look_r%03d_n%02d.png", hut_animal_type_string[ha_num].c_str(), ROTATION[r], i);
			look[r][i].LoadBaseImage(filename);

			if(r+1<MAX_ANGLE_TYPES) look[r+1][i] = look[r][i];
		}

}

bool AHutAnimal::finished_init = false;
hut_animal_graphics AHutAnimal::ha_graphics[MAX_HUT_ANIMAL_TYPES];
vector<int> AHutAnimal::animals_in_palette[MAX_PLANET_TYPES];

AHutAnimal::AHutAnimal(ZTime *ztime_, ZSettings *zsettings_, int palette_) : ZObject(ztime_, zsettings_)
{
	if(!finished_init)
	{
		killme = true;
		return;
	}

	palette = palette_;
	going_home = false;
	home_x = 0;
	home_y = 0;
	ha_type = ChooseRandomAnimal(palette);
	ha_state = HA_NOTHING;
	walk_i = 0;
	look_i = 0;

	walk_speed = 0.2;
	look_speed = 0.35;

	move_speed = (int)15.0;
	real_move_speed = 15.0;

	next_walk_time = 0;
	next_look_time = 0;
	next_nothing_time = 0;
}

void AHutAnimal::Init()
{
	for(int i=0;i<MAX_HUT_ANIMAL_TYPES;i++) ha_graphics[i].LoadGraphics(i);

	//populate lists for random pulling etc
	{
		animals_in_palette[DESERT].push_back(GREEN_SNAKE);
		animals_in_palette[DESERT].push_back(GREEN_LIZARD);
		animals_in_palette[DESERT].push_back(DESERT_RABIT);

		animals_in_palette[VOLCANIC].push_back(RAPTOR);
		animals_in_palette[VOLCANIC].push_back(MINI_RAPTOR);
		animals_in_palette[VOLCANIC].push_back(PIG_DINO);
		animals_in_palette[VOLCANIC].push_back(YELLOW_WORM);

		animals_in_palette[ARCTIC].push_back(ARCTIC_RABIT);
		animals_in_palette[ARCTIC].push_back(PENGUIN);
		animals_in_palette[ARCTIC].push_back(WHITE_WOLF);

		animals_in_palette[JUNGLE].push_back(OSTRICH);
		animals_in_palette[JUNGLE].push_back(RAT);
		animals_in_palette[JUNGLE].push_back(TURTLE);

		animals_in_palette[CITY].push_back(RED_WORM);
		animals_in_palette[CITY].push_back(RAT);
		animals_in_palette[CITY].push_back(GREEN_EYED_FOX);
	}

	finished_init = true;
}

int AHutAnimal::ChooseRandomAnimal(int palette)
{
	int rand_choice;

	if(palette < 0) return 0;
	if(palette >= MAX_PLANET_TYPES) return 0;

	if(!animals_in_palette[palette].size()) return 0;

	rand_choice = rand() % animals_in_palette[palette].size();

	return animals_in_palette[palette][rand_choice];
}

void AHutAnimal::SetStateNothing()
{
	double &the_time = ztime->ztime;

	ha_state = HA_NOTHING;
	next_nothing_time = the_time + 0.1 + ((rand() % 10) * 0.1);

	if(ha_graphics[ha_type].walk_to_zero) walk_i = 0;
}

bool AHutAnimal::IsPrefferedDirection(int cur_dir, int new_dir)
{
	int diff;

	if(cur_dir==-1) return false;
	if(new_dir==-1) return false;

	diff = cur_dir - new_dir;
	diff = abs(diff);
	
	if(diff>=3 && diff<=5) return false;

	//some bad combos

	//0-3=3
	//0-4=4
	//0-5=5

	//2-5=3
	//2-6=4
	//2-7=5

	return true;
}

bool AHutAnimal::TileIsTooFar(int tx, int ty)
{
	int ex, ey;

	ex = (tx<<4) + 8;
	ey = (ty<<4) + 8;

	return !points_within_distance(home_x, home_y, ex, ey, zsettings->hut_animal_roam_distance);
}

void AHutAnimal::GoHome()
{
	going_home = true;

	GotoTile(home_x>>4,home_y>>4);
}

void AHutAnimal::GotoRandomTile()
{
	int tx, ty;
	int ctx, cty;

	ctx = loc.x>>4;
	cty = loc.y>>4;

	if(!zmap)
	{
		//true random selection
		//because no passable tile info
		tx = ctx + ((rand()%3)-1);
		ty = cty + ((rand()%3)-1);
	}
	else
	{
		vector<xy_struct> possible_list;
		vector<xy_struct> preferred_list;
		int itx, ity;
		int rand_choice;

		//populate lists
		for(itx=ctx-1;itx<=ctx+1;itx++)
			for(ity=cty-1;ity<=cty+1;ity++)
			{	
				if(ity==cty && itx==ctx) continue;

				if(!zmap->GetPathFinder().TilePassable(itx, ity, false)) continue;
				if(TileIsTooFar(itx, ity)) continue;

				xy_struct new_xy;
				new_xy.x = itx;
				new_xy.y = ity;
				possible_list.push_back(new_xy);

				if(IsPrefferedDirection(direction, DirectionFromLoc((float)(itx-ctx), (float)(ity-cty))))
					preferred_list.push_back(new_xy);
			}

		if(!possible_list.size())
		{
//			mprintf("hut animal at tile %d,%d has no where to move! killing animal...\n", ctx, cty);
			killme = true;
			return;
		}

		//mprintf("possible:%d prefered:%d\n", possible_list.size(), preferred_list.size());

		if(preferred_list.size() && rand()%5)
		{
			rand_choice = rand() % preferred_list.size();
			tx = preferred_list[rand_choice].x;
			ty = preferred_list[rand_choice].y;
		}
		else
		{
			rand_choice = rand() % possible_list.size();
			tx = possible_list[rand_choice].x;
			ty = possible_list[rand_choice].y;
		}
	}

	GotoTile(tx, ty);
}

void AHutAnimal::GotoTile(int tx, int ty)
{
	float mag;
	float &dx = loc.dx;
	float &dy = loc.dy;
	int new_direction;

	//lets us use the ReachedTarget() function
	cur_wp_info.sx = loc.x;
	cur_wp_info.sy = loc.y;
	cur_wp_info.x = ((tx<<4) + 8);
	cur_wp_info.y = ((ty<<4) + 8);
	cur_wp_info.adx = abs(cur_wp_info.x - cur_wp_info.sx);
	cur_wp_info.ady = abs(cur_wp_info.y - cur_wp_info.sy);

	xover = 0;
	yover = 0;

	dx = (float)(cur_wp_info.x - cur_wp_info.sx);
	dy = (float)(cur_wp_info.y - cur_wp_info.sy);

	mag = sqrt((dx * dx) + (dy * dy));

	//not moving because we are there?
	if(mag < 0.0001)
	{
		SetStateNothing();
		return;
	}

	dx /= mag;
	dy /= mag;

	dx *= (float)real_move_speed;
	dy *= (float)real_move_speed;

	new_direction = DirectionFromLoc(dx, dy);
	if(new_direction != -1) direction = new_direction;

	ha_state = HA_WALKING;
}

void AHutAnimal::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *render_img;

	if(killme) return;

	switch(ha_state)
	{
	case HA_LOOKING:
		render_img = &ha_graphics[ha_type].look[direction][look_i];
		break;
	case HA_WALKING:
	default:
		render_img = &ha_graphics[ha_type].walk[direction][walk_i];
		break;
	}

	if(!render_img) return;

	the_map.RenderZSurface(render_img, x, y, false, true);
}

int AHutAnimal::Process()
{
	double &the_time = ztime->ztime;
	int &x = loc.x;
	int &y = loc.y;
	float &dx = loc.dx;
	float &dy = loc.dy;
	double time_dif;

	if(killme) return 0;
	
	time_dif = the_time - last_process_time;
	last_process_time = the_time;

	//move
	if(IsMoving())
	{
		double nx, ny;
		int inx, iny;

		nx = x + (dx * time_dif) + xover;
		ny = y + (dy * time_dif) + yover;

		inx = (int)floor(nx);
		iny = (int)floor(ny);

		xover = (float)(nx - (double)inx);
		yover = (float)(ny - (double)iny);

		x = inx;
		y = iny;

		center_x = x;
		center_y = y;

		if(ReachedTarget())
		{
			if(going_home)
			{
				if(cur_wp_info.x == home_x && cur_wp_info.y == home_y)
				{
					//kill us if home
					killme = true;
					return 1;
				}
				else
				{
					//select next tile to home
				}
			}

			if(rand() % 3)
				GotoRandomTile();
			else
			{
				SetStateNothing();

				loc.dx = 0;
				loc.dy = 0;
			}
		}
	}

	//anim/state stuff
	switch(ha_state)
	{
	case HA_NOTHING:
		if(the_time >= next_nothing_time)
		{
			if(rand()%5 || !ha_graphics[ha_type].look_i)
				GotoRandomTile();
			else
			{
				look_i = 0;
				ha_state = HA_LOOKING;
			}
		}
		break;
	case HA_LOOKING:
		if(the_time >= next_look_time)
		{
			next_look_time = the_time + look_speed;

			look_i++;

			if(look_i >= ha_graphics[ha_type].look_i)
			{
				//do another look?
				if(rand()%5)
				{
					SetStateNothing();
					look_i = 0;
				}
				else
					look_i = 0;
			}
		}
		break;
	case HA_WALKING:
		if(the_time >= next_walk_time)
		{
			next_walk_time = the_time + walk_speed;

			walk_i++;

			if(walk_i >= ha_graphics[ha_type].walk_i) walk_i = 0;
		}
		break;
	}

	return 1;
}
