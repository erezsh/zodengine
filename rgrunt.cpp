#include "rgrunt.h"

//SDL_Surface *RGrunt::fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][5];
ZSDL_Surface RGrunt::fire[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][5];

RGrunt::RGrunt(ZTime *ztime_, ZSettings *zsettings_) : ZRobot(ztime_, zsettings_)
{
	hover_name = "Grunt";
	object_name = "grunt";

	can_snipe = true;

	InitTypeId(object_type, GRUNT);

	//object_id = GRUNT;
	//attack_radius = GRUNT_ATTACK_RADIUS;
	//max_health = GRUNT_MAX_HEALTH;
	//health = max_health;

	//damage = GRUNT_DAMAGE;
	//damage_chance = GRUNT_DAMAGE_CHANCE;
	//damage_int_time = GRUNT_ATTACK_SPEED;
}

void RGrunt::Init()
{
	int i, j, k;
	char filename_c[500];
	SDL_Surface *temp_surface;
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
			for(k=0;k<5;k++)
	{
		sprintf(filename_c, "assets/units/robots/grunt/fire_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
		//fire[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, fire[ZTEAM_BASE_TEAM][j][k], fire[i][j][k], filename_c);
	}
}

void RGrunt::PlaySelectedWav()
{
	//int ch;

	//if(rand()%2)
	//{
	//	ch = rand() % 6;

	//	ZMix_PlayChannel(-1, selected_wav[ch], 0);
	//}
	//else
	//	ZMix_PlayChannel(-1, selected_robot_wav[GRUNT], 0);
}

void RGrunt::PlaySelectedAnim(ZPortrait &portrait)
{
	if(rand()%2)
	{
		switch(rand() % 4)
		{
		case 0: portrait.StartAnim(YES_SIR_ANIM); break;
		case 1: portrait.StartAnim(YES_SIR3_ANIM); break;
		case 2: portrait.StartAnim(UNIT_REPORTING1_ANIM); break;
		case 3: portrait.StartAnim(UNIT_REPORTING2_ANIM); break;
		}
	}
	else
		portrait.StartAnim(GRUNTS_REPORTING_ANIM);
}

// SDL_Surface *RGrunt::GetRender()
// {
// 	if(owner != NULL_TEAM)
// 	{
// 		switch(mode)
// 		{
// 			case R_STANDING: return stand[owner][direction];
// 			case R_BEER: return beer[owner][action_i];
// 			case R_CIGARETTE: return cigarette[owner][action_i];
// 			case R_FULLSCAN: return full_area_scan[owner][action_i];
// 			case R_HEADSTRETCH: return head_stretch[owner][action_i];
// 		}
// 	}
// 	else
// 		return null_img;
// }

void RGrunt::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	if(owner != NULL_TEAM)
	{
		switch(mode)
		{
			case R_WALKING: base_surface = &walk[owner][direction][move_i]; break;
			case R_STANDING: base_surface = &stand[owner][direction]; break;
			case R_BEER: base_surface = &beer[owner][action_i]; break;
			case R_CIGARETTE: base_surface = &cigarette[owner][action_i]; break;
			case R_FULLSCAN: base_surface = &full_area_scan[owner][action_i]; break;
			case R_HEADSTRETCH: base_surface = &head_stretch[owner][action_i]; break;
			case R_PICKUP_UP_GRENADES: base_surface = &pickup_up[owner][action_i]; break;
			case R_PICKUP_DOWN_GRENADES: base_surface = &pickup_down[owner][action_i]; break;
			case R_ATTACKING: 
				if(this->CanThrowGrenades() || (attack_object && attack_object->AttackedOnlyByExplosives()))
					base_surface = &throw_something[owner][direction][grenade_i];
				else
					base_surface = &fire[owner][direction][action_i];
				break;
			default: base_surface = &null_img;
		}
	}
	else
		base_surface = &null_img;
	
	if(!base_surface) return;

	submerge_amount = the_map.SubmergeAmount(loc.x+8,loc.y+8);
	
	//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
	if(the_map.GetBlitInfo(x, y + submerge_amount, 16, 16 - submerge_amount, from_rect, to_rect))
	{
		to_rect.x += shift_x;
		to_rect.y += shift_y;

		base_surface->BlitHitSurface(&from_rect, &to_rect, NULL, do_hit_effect);
		//ZSDL_BlitHitSurface( base_surface, &from_rect, dest, &to_rect, do_hit_effect);
	}

	do_hit_effect = false;
}

void RGrunt::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	
}

int RGrunt::Process()
{
	double &the_time = ztime->ztime;
	int &x = loc.x;
	int &y = loc.y;

	Common_Process(the_time);

	if(this->CanThrowGrenades() || (attack_object && attack_object->AttackedOnlyByExplosives())) return 1;

	if(mode == R_ATTACKING && the_time >= next_attack_time)
	{
		action_i++;
		if(action_i>=5) action_i = 3;

		if(action_i >= 3)
			next_attack_time = the_time + 0.40 + ((rand() % 100) * 0.2 * 0.01);
		else
			next_attack_time = the_time + 0.02;

		//just switched to firing?
		if(action_i == 4 && attack_object)
		{
			int tx, ty;
			int w, h;

			attack_object->GetCords(tx, ty);
			attack_object->GetDimensionsPixel(w,h);

			tx += (rand() % w);
			ty += (rand() % h);

			if(effect_list) effect_list->push_back((ZEffect*)(new EBullet(ztime, owner, x+8, y+8, tx, ty)));
			ZSoundEngine::PlayWavRestricted(RIFLE_FIRE_SND, loc.x, loc.y, width_pix, height_pix);
		}
	}

	return 1;
}
