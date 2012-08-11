#include "zportrait.h"
#include "common.h"
#include "zteam.h"

using namespace COMMON;

bool ZPortrait::finished_init = false;

ZSDL_Surface ZPortrait::backdrop[MAX_PLANET_TYPES];
ZSDL_Surface ZPortrait::backdrop_vehicle;
ZPortrait_Unit_Graphics ZPortrait::unit_graphics[MAX_ROBOT_TYPES][MAX_TEAM_TYPES];
ZPortrait_Anim ZPortrait::anim_info[MAX_PORTRAIT_ANIMS];
ZPortrait_Frame ZPortrait::still_frame;

ZPortrait::ZPortrait()
{
	x = 0;
	y = 0;
	over_map = false;
	terrain = DESERT;
	team = RED_TEAM;
	render_frame = NULL;
	ref_id = -1;
	do_random_anims = true;

	ClearRobotID();
}

void ZPortrait::Init()
{
	int i, j;
	string filename;

	filename = "assets/other/hud/backdrop_vehicle.bmp";
	backdrop_vehicle.LoadBaseImage(filename);

	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/other/hud/backdrop_" + planet_type_string[i] + ".bmp";
		backdrop[i].LoadBaseImage(filename);
	}

	for(i=0;i<MAX_ROBOT_TYPES;i++)
		for(j=0;j<MAX_TEAM_TYPES;j++)
			unit_graphics[i][j].Load((robot_type)i, (team_type)j, unit_graphics[i][ZTEAM_BASE_TEAM]);

	SetupFrames();

	finished_init = true;
}

void ZPortrait_Unit_Graphics::Load(robot_type oid, team_type team, ZPortrait_Unit_Graphics &bc)
{
	char filename_c[500];
	int old_z_face_id;
	int i, j;

	if(team == NULL_TEAM) return;

	switch(oid)
	{
	case GRUNT: old_z_face_id = 2; break;
	case PSYCHO: old_z_face_id = 3; break;
	case SNIPER: old_z_face_id = 4; break;
	case TOUGH: old_z_face_id = 0; break;
	case PYRO: old_z_face_id = 1; break;
	case LASER: old_z_face_id = 1; break;
	default:
		printf("ZPortrait_Unit_Graphics::invalid robot id!\n");
		return;
		break;
	}

	//load em
	i=0;
	sprintf(filename_c, "assets/other/hud/portraits/%s_%s/SHEADBI%d_%04d.png",  robot_type_string[oid].c_str(), team_type_string[team].c_str(), old_z_face_id, i++);
	//shoulders.LoadBaseImage(filename_c);
	ZTeam::LoadZSurface(team, bc.shoulders, shoulders, filename_c);

	sprintf(filename_c, "assets/other/hud/portraits/%s_%s/SHEADBI%d_%04d.png",  robot_type_string[oid].c_str(), team_type_string[team].c_str(), old_z_face_id, i++);
	//head[LOOK_STRAIGHT].LoadBaseImage(filename_c);
	ZTeam::LoadZSurface(team, bc.head[LOOK_STRAIGHT], head[LOOK_STRAIGHT], filename_c);

	sprintf(filename_c, "assets/other/hud/portraits/%s_%s/SHEADBI%d_%04d.png",  robot_type_string[oid].c_str(), team_type_string[team].c_str(), old_z_face_id, i++);
	//head[LOOK_RIGHT].LoadBaseImage(filename_c);
	ZTeam::LoadZSurface(team, bc.head[LOOK_RIGHT], head[LOOK_RIGHT], filename_c);

	sprintf(filename_c, "assets/other/hud/portraits/%s_%s/SHEADBI%d_%04d.png",  robot_type_string[oid].c_str(), team_type_string[team].c_str(), old_z_face_id, i++);
	//head[LOOK_LEFT].LoadBaseImage(filename_c);
	ZTeam::LoadZSurface(team, bc.head[LOOK_LEFT], head[LOOK_LEFT], filename_c);

	for(j=0;j<MAX_MOUTHS;j++)
	{
		sprintf(filename_c, "assets/other/hud/portraits/%s_%s/SHEADBI%d_%04d.png",  robot_type_string[oid].c_str(), team_type_string[team].c_str(), old_z_face_id, i++);
		//mouth[j].LoadBaseImage(filename_c);
		ZTeam::LoadZSurface(team, bc.mouth[j], mouth[j], filename_c);
	}

	for(j=0;j<MAX_EYES;j++)
	{
		sprintf(filename_c, "assets/other/hud/portraits/%s_%s/SHEADBI%d_%04d.png",  robot_type_string[oid].c_str(), team_type_string[team].c_str(), old_z_face_id, i++);
		//eyes[j].LoadBaseImage(filename_c);
		ZTeam::LoadZSurface(team, bc.eyes[j], eyes[j], filename_c);
	}

	for(j=0;j<MAX_HANDS;j++)
	{
		sprintf(filename_c, "assets/other/hud/portraits/%s_%s/SHEADBI%d_%04d.png",  robot_type_string[oid].c_str(), team_type_string[team].c_str(), old_z_face_id, i++);
		//hand[j].LoadBaseImage(filename_c);
		ZTeam::LoadZSurface(team, bc.hand[j], hand[j], filename_c);
	}
}

void ZPortrait::SetCords(int x_, int y_)
{
	x = x_;
	y = y_;
}

void ZPortrait::SetOverMap(bool over_map_)
{
	over_map = over_map_;
}

void ZPortrait::SetTerrainType(planet_type terrain_)
{
	terrain = terrain_;
}

void ZPortrait::SetInVehicle(bool in_vehicle_)
{
	in_vehicle = in_vehicle_;
}

void ZPortrait::SetTeam(int team_)
{
	team = team_;
}

void ZPortrait::SetRobotID(unsigned char oid_)
{
	oid = oid_;
	do_render = true;

	if(oid < 0) oid = GRUNT;
	if(oid >= MAX_ROBOT_TYPES) oid = GRUNT;
}

void ZPortrait::SetObject(ZObject *obj)
{
	ClearRobotID();

	if(!obj) return;

	unsigned char ot, oid;

	obj->GetObjectID(ot, oid);

	SetTeam(obj->GetOwner());

	ref_id = obj->GetRefID();

	switch(ot)
	{
	case ROBOT_OBJECT:
		SetInVehicle(false);
		SetRobotID(oid);
		break;
	case VEHICLE_OBJECT:
		SetRobotID(obj->GetDriverType());
		SetInVehicle(true);
		break;
	case CANNON_OBJECT:
		SetRobotID(obj->GetDriverType());
		SetInVehicle(true);
		break;
	}
}

void ZPortrait::ClearRobotID()
{
	oid = GRUNT;
	in_vehicle = false;
	do_render = false;
	render_frame = &still_frame;
	cur_anim = -1;
	anim_start_time = 0;
	ref_id = -1;
}

void ZPortrait::StartAnim(int anim_)
{
	if(!anim_info[anim_].frame_list.size()) return;

	cur_anim = anim_;
	render_frame = &anim_info[cur_anim].frame_list[0];
	anim_start_time = current_time();

	PlayAnimSound();
	//printf("StartAnim::%d:%s\n", cur_anim, portrait_anim_string[cur_anim].c_str());
}

void ZPortrait::StartRandomAnim()
{
	if(!do_random_anims) return;

	switch(rand() % 13)
	{
	case 0: StartAnim(BLINK_ANIM); break;
	case 1: StartAnim(WINK_ANIM); break;
	case 2: StartAnim(SURPRISE_ANIM); break;
	case 3: StartAnim(ANGER_ANIM); break;
	case 4: StartAnim(GRIN_ANIM); break;
	case 5: StartAnim(SCARED_ANIM); break;
	case 6: StartAnim(EYES_LEFT_ANIM); break;
	case 7: StartAnim(EYES_RIGHT_ANIM); break;
	case 8: StartAnim(EYES_UP_ANIM); break;
	case 9: StartAnim(EYES_DOWN_ANIM); break;
	case 10: StartAnim(WHISTLE_ANIM); break;
	case 11: StartAnim(LOOK_LEFT_ANIM); break;
	case 12: StartAnim(LOOK_RIGHT_ANIM); break;
	}
}

bool ZPortrait::DoingAnim()
{
	return cur_anim != -1;
}

int ZPortrait::Process()
{
	double the_time = current_time();
//	static int test_anim = 0;
	//static double next_test_anim_time = 0;

	if(!finished_init) return 1;

	if(cur_anim != -1)
	{
		double time_in;

		time_in = the_time - anim_start_time;

		//anim over?
		if(time_in > anim_info[cur_anim].total_duration)
		{
			cur_anim = -1;
			render_frame = &still_frame;

			//next_test_anim_time = the_time + 0.5;
			next_random_anim_time = the_time + 0.5 + ((rand() % 50) * 0.1);
		}
		else
		{
			//set next frame in this animation
			render_frame = &still_frame;

			//set current frame
			double cur_duration = 0;
			vector<ZPortrait_Frame> *cur_list = &anim_info[cur_anim].frame_list;
			for(vector<ZPortrait_Frame>::iterator i=cur_list->begin(); i!=cur_list->end(); ++i)
			{
				if(cur_duration <= time_in)
					render_frame = &(*i);
				else
					break;

				cur_duration += i->duration;
			}
		}
	}
	else
	{
		//start a random animation?
		if(the_time >= next_random_anim_time) StartRandomAnim();

		/*
		if(the_time < next_test_anim_time) return 1;

		//we have no animation
		if(test_anim>=MAX_PORTRAIT_ANIMS) test_anim=0;
		StartAnim(test_anim++);
		//StartAnim(FOR_CHRIST_SAKE_ANIM);
		*/
	}

	return 1;
}

void ZPortrait::DoRender()
{
	SDL_Rect /* from_rect,*/ to_rect;

	if(do_render)
	{
		ZSDL_Surface *backdrop_surface;

		//backdrop
		if(in_vehicle)
			backdrop_surface = &backdrop_vehicle;
		else
			backdrop_surface = &backdrop[terrain];

		if(!over_map)
		{
			to_rect.x = x;
			to_rect.y = y;

			backdrop_surface->BlitSurface(NULL, &to_rect);
		}
		else
			backdrop_surface->RenderSurface(x, y);

		RenderFace();
	}
	else
	{
		//clear
		if(!over_map)
		{
			to_rect.x = x;
			to_rect.y = y;
			to_rect.w = 86;
			to_rect.h = 74;

			ZSDL_FillRect(&to_rect, 0, 0, 0);
		}
		else
		{
			//different color and croppable when over the map.
		}
	}
}

void ZPortrait::RenderFace()
{
	SDL_Rect from_rect, to_rect;
	ZPortrait_Unit_Graphics *choosen_graphics;
	ZSDL_Surface *render_surface;
	int face_offset_y;

	//got face render info?
	if(!render_frame) return;

	choosen_graphics = &unit_graphics[oid][team];

	//head
	{
		//face
		{
			render_surface = &choosen_graphics->head[render_frame->look_direction];

			to_rect.x = render_frame->head_x;
			to_rect.y = render_frame->head_y;

			to_rect.x += x;
			to_rect.y += y;

			if(!over_map)
				render_surface->BlitSurface(NULL, &to_rect);
			else
				render_surface->RenderSurface(to_rect.x, to_rect.y);
		}

		if(render_frame->look_direction == LOOK_STRAIGHT)
		{
			switch(oid)
			{
				case GRUNT: face_offset_y = 0; break;
				case SNIPER: face_offset_y = 4; break;
				default: face_offset_y = 2; break;
			}

			//eyes
			{
				render_surface = &choosen_graphics->eyes[render_frame->eyes];

				to_rect.x = 14 + render_frame->head_x;
				to_rect.y = 8 + render_frame->head_y + face_offset_y;

				to_rect.x += x;
				to_rect.y += y;

				if(!over_map)
					render_surface->BlitSurface(NULL, &to_rect);
				else
					render_surface->RenderSurface(to_rect.x, to_rect.y);
			}

			//mouth
			{
				render_surface = &choosen_graphics->mouth[render_frame->mouth];

				to_rect.x = 22 + render_frame->head_x;
				to_rect.y = 24 + render_frame->head_y + face_offset_y;

				to_rect.x += x;
				to_rect.y += y;

				if(!over_map)
					render_surface->BlitSurface(NULL, &to_rect);
				else
					render_surface->RenderSurface(to_rect.x, to_rect.y);
			}
		}
	}

	//shoulders
	if(choosen_graphics->shoulders.GetBaseSurface())
	{
		to_rect.x = 0;
		to_rect.y = 74 - choosen_graphics->shoulders.GetBaseSurface()->h;

		to_rect.x += x;
		to_rect.y += y;

		if(!over_map)
			choosen_graphics->shoulders.BlitSurface(NULL, &to_rect);
		else
			choosen_graphics->shoulders.RenderSurface(to_rect.x, to_rect.y);
	}


	//hand
	if(render_frame->hand_do_render)
	{
		render_surface = &choosen_graphics->hand[render_frame->hand];

		to_rect.x = render_frame->hand_x;
		to_rect.y = render_frame->hand_y;

		if(GetBlitInfo(render_surface->GetBaseSurface(), to_rect.x, to_rect.y, from_rect, to_rect))
		{
			to_rect.x += x;
			to_rect.y += y;

			if(!over_map)
				render_surface->BlitSurface(&from_rect, &to_rect);
			else
				render_surface->RenderSurface(to_rect.x, to_rect.y);
		}
	}
}

ZPortrait_Anim::ZPortrait_Anim()
{
	total_duration = 0;
}

void ZPortrait::PlayAnimSound()
{
	static int last_end_anim = -1;
	int next_end_anim;

	switch(cur_anim)
	{
	case YES_SIR_ANIM:
		if(rand() % 2)
			ZSoundEngine::PlayWav(YES_SIR1_SND);
		else
			ZSoundEngine::PlayWav(YES_SIR2_SND);
		break;
	case YES_SIR3_ANIM: ZSoundEngine::PlayWav(YES_SIR3_SND); break;
	case UNIT_REPORTING1_ANIM: ZSoundEngine::PlayWav(UNIT_REPORTING1_SND); break;
	case UNIT_REPORTING2_ANIM:
		if(rand() % 2)
			ZSoundEngine::PlayWav(UNIT_REPORTING2_SND);
		else
			ZSoundEngine::PlayWav(UNIT_REPORTING3_SND);
		break;
	case GRUNTS_REPORTING_ANIM: ZSoundEngine::PlayWav(GRUNTS_REPORTING_SND); break;
	case PSYCHOS_REPORTING_ANIM: ZSoundEngine::PlayWav(PSYCHOS_REPORTING_SND); break;
	case SNIPERS_REPORTING_ANIM: ZSoundEngine::PlayWav(SNIPERS_REPORTING_SND); break;
	case TOUGHS_REPORTING_ANIM: ZSoundEngine::PlayWav(TOUGHS_REPORTING_SND); break;
	case LASERS_REPORTING_ANIM: ZSoundEngine::PlayWav(LASERS_REPORTING_SND); break;
	case PYROS_REPORTING_ANIM: ZSoundEngine::PlayWav(PYROS_REPORTING_SND); break;
	case WERE_ON_OUR_WAY_ANIM: ZSoundEngine::PlayWav(WERE_ON_OUR_WAY_SND); break;
	case HERE_WE_GO_ANIM: ZSoundEngine::PlayWav(HERE_WE_GO_SND); break;
	case YOUVE_GOT_IT_ANIM: ZSoundEngine::PlayWav(YOU_GOT_IT_SND); break;
	case MOVING_IN_ANIM: ZSoundEngine::PlayWav(MOVING_IN_SND); break;
	case OKAY_ANIM: ZSoundEngine::PlayWav(OKAY_SND); break;
	case ALRIGHT_ANIM: ZSoundEngine::PlayWav(ALRIGHT_SND); break;
	case NO_PROBLEM_ANIM: ZSoundEngine::PlayWav(NO_PROBLEM_SND); break;
	case OVER_N_OUT_ANIM: ZSoundEngine::PlayWav(OVER_AND_OUT_SND); break;
	case AFFIRMATIVE_ANIM: ZSoundEngine::PlayWav(AFFIRMATIVE_SND); break;
	case GOING_IN_ANIM: ZSoundEngine::PlayWav(GOING_IN_SND); break;
	case LETS_DO_IT_ANIM: ZSoundEngine::PlayWav(LETS_DO_IT_SND); break;
	case LETS_GET_EM_ANIM: ZSoundEngine::PlayWav(LETS_GET_THEM_SND); break;
	case WERE_UNDER_ATTACK_ANIM: ZSoundEngine::PlayWav(WERE_UNDER_ATTACK_SND); break;
	case I_SAID_WERE_UNDER_ATTACK_ANIM: ZSoundEngine::PlayWav(I_SAID_WERE_UNDER_ATTACK_SND); break;
	case HELP_HELP_ANIM: ZSoundEngine::PlayWav(HELP_HELP_SND); break;
	case THEYRE_ALL_OVER_US_ANIM: ZSoundEngine::PlayWav(THEYRE_ALL_OVER_US_SND); break;
	case WERE_LOSEING_IT_ANIM: ZSoundEngine::PlayWav(WERE_LOSING_IT_SND); break;
	case AAAHHH_ANIM: ZSoundEngine::PlayWav(AAAHHH_SND); break;
	//where is the "oh my god" anim? -> OH_MY_GOD_SND
	case FOR_CHRIST_SAKE_ANIM: ZSoundEngine::PlayWav(FOR_CHRIST_SAKE_SND); break;
	case YOURE_JOKING_ANIM: ZSoundEngine::PlayWav(YOUR_JOKING_SND); break;
	case TARGET_DESTROYED_ANIM: ZSoundEngine::PlayWav(TARGET_DESTROYED_SND); break;
	case YES_SIR_SALUTE_ANIM:
		if(rand() % 2)
			ZSoundEngine::PlayWav(YES_SIR1_SND);
		else
			ZSoundEngine::PlayWav(YES_SIR2_SND);
		break;
	case GOING_IN_THUMBS_UP_ANIM: ZSoundEngine::PlayWav(GOING_IN_SND); break;
	case FORGET_IT_ANIM: ZSoundEngine::PlayWav(FORGET_IT_SND); break;
	case GET_OUTTA_HERE_ANIM: ZSoundEngine::PlayWav(GET_OUTTA_HERE_SND); break;
	case GOOD_HIT_ANIM: ZSoundEngine::PlayWav(GOOD_HIT_SND); break;
	case NO_WAY_ANIM: ZSoundEngine::PlayWav(NO_WAY_SND); break;
	case NICE_ONE_ANIM: ZSoundEngine::PlayWav(NICE_ONE_SND); break;
	case OH_YEAH_ANIM: ZSoundEngine::PlayWav(OH_YEAH_SND); break;
	case GOTCHA_ANIM: ZSoundEngine::PlayWav(GOTCHA_SND); break;
	case SMOKIN_ANIM: ZSoundEngine::PlayWav(SMOKIN_SND); break;
	case COOL_ANIM: ZSoundEngine::PlayWav(COOL_SND); break;
	case WIPE_OUT_ANIM: ZSoundEngine::PlayWav(WIPE_OUT_SND); break;
	case TERRITORY_TAKEN_ANIM: ZSoundEngine::PlayWav(TERRITORY_TAKEN_SND); break;
	case FIRE_EXTINGUISHED_ANIM: ZSoundEngine::PlayWav(FIRE_EXTINGUISHED_SND); break;
	case GUN_CAPTURED_ANIM: ZSoundEngine::PlayWav(GUN_CAPTURED_SND); break;
	case VEHICLE_CAPTURED_ANIM: ZSoundEngine::PlayWav(VEHICLE_CAPTURED_SND); break;
	case GRENADES_COLLECTED_ANIM: ZSoundEngine::PlayWav(GRENADES_COLLECTED_SND); break;
	case ENDW1_ANIM:
	case ENDW2_ANIM:
	case ENDW3_ANIM:
		next_end_anim = rand() % 6;
		while(next_end_anim == last_end_anim) next_end_anim = rand() % 6;
		last_end_anim = next_end_anim;
		ZSoundEngine::PlayWav(YEEHAA_END_SND + next_end_anim); break;
	case ENDL1_ANIM:
	case ENDL2_ANIM:
	case ENDL3_ANIM:
		next_end_anim = rand() % 7;
		while(next_end_anim == last_end_anim) next_end_anim = rand() % 7;
		last_end_anim = next_end_anim;
		ZSoundEngine::PlayWav(YOURE_CRAP_LOSE_SND + next_end_anim); break;
	}
}

void ZPortrait_Anim::AddFrame(ZPortrait_Frame &new_frame)
{
	//some shifting
	new_frame.hand_x -= 4;
	new_frame.hand_y -= 4;

	//stick in the frame
	frame_list.push_back(new_frame);

	//recalc the total duration
	total_duration = 0;
	for(vector<ZPortrait_Frame>::iterator i=frame_list.begin(); i!=frame_list.end(); ++i)
		total_duration += i->duration;
}

void ZPortrait::SetupFrames()
{
	const double duration_multi = 0.015;
	ZPortrait_Anim *cur_anim;
	ZPortrait_Frame new_frame;

	//#00 yes-sir-1-and-2
	cur_anim = &anim_info[YES_SIR_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 0;
		new_frame.hand_x = 0;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 8;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 8;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 8;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 8;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 26;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 8;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 8;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 2 + 0;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#01 yes-sir-3
	cur_anim = &anim_info[YES_SIR3_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 12 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#02 unit-reporting-1
	cur_anim = &anim_info[UNIT_REPORTING1_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 48;
		new_frame.hand_y = 26;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//23
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#03 unit-reporting-2
	cur_anim = &anim_info[UNIT_REPORTING2_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#04 grunts-reporting
	cur_anim = &anim_info[GRUNTS_REPORTING_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#05 psychos-reporting
	cur_anim = &anim_info[PSYCHOS_REPORTING_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#06 snipers-reporting
	cur_anim = &anim_info[SNIPERS_REPORTING_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 26;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#07 toughs-reporting
	cur_anim = &anim_info[TOUGHS_REPORTING_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 24;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 24;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 22;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 44;
		cur_anim->AddFrame(new_frame);

		//23
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#08 lazers-reporting
	cur_anim = &anim_info[LASERS_REPORTING_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#09 pyros-reporting
	cur_anim = &anim_info[PYROS_REPORTING_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#10 were-on-our-way
	cur_anim = &anim_info[WERE_ON_OUR_WAY_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 21 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 24 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 24 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 24 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#11 here-we-go
	cur_anim = &anim_info[HERE_WE_GO_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#12 youve-got-it
	cur_anim = &anim_info[YOUVE_GOT_IT_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 21 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 21 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 21 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#13 moving-in
	cur_anim = &anim_info[MOVING_IN_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#14 okay
	cur_anim = &anim_info[OKAY_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#15 alright
	cur_anim = &anim_info[ALRIGHT_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#16 no-problem
	cur_anim = &anim_info[NO_PROBLEM_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#17 over-n-out
	cur_anim = &anim_info[OVER_N_OUT_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#18 affirmative
	cur_anim = &anim_info[AFFIRMATIVE_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 48;
		new_frame.hand_y = 26;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#19 going-in
	cur_anim = &anim_info[GOING_IN_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 12 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#20 lets-do-it
	cur_anim = &anim_info[LETS_DO_IT_ANIM];
	{
		//1
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#21 lets-get-em
	cur_anim = &anim_info[LETS_GET_EM_ANIM];
	{
		//1
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#22 we're-under-attack
	cur_anim = &anim_info[WERE_UNDER_ATTACK_ANIM];
	{
		//1
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#23 i-said-we're-under-attack
	cur_anim = &anim_info[I_SAID_WERE_UNDER_ATTACK_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//23
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//24
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//25
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//26
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//27
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//28
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//29
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//30
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//31
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//32
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//33
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//34
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#24 help-help
	cur_anim = &anim_info[HELP_HELP_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 46;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 78;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//23
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//24
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//25
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#25 theyer-all-over-us
	cur_anim = &anim_info[THEYRE_ALL_OVER_US_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#26 wereloseing-it
	cur_anim = &anim_info[WERE_LOSEING_IT_ANIM];
	{
		//1
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#27 wereloseing-it
	cur_anim = &anim_info[AAAHHH_ANIM];
	{
		//1
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#28 for-chris-sake
	cur_anim = &anim_info[FOR_CHRIST_SAKE_ANIM];
	{
		//1
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//23
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//24
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//25
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//26
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//27
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//28
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#29 you're-joking
	cur_anim = &anim_info[YOURE_JOKING_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#30 target-destroyed
	cur_anim = &anim_info[TARGET_DESTROYED_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 11 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#31 blink
	cur_anim = &anim_info[BLINK_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#32 wink
	cur_anim = &anim_info[WINK_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 9 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 9 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 18 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 9 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 9 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 9 - 4;
		new_frame.eyes = 28 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#33 surprise
	cur_anim = &anim_info[SURPRISE_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 8 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 8 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 28 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 8 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 8 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 8 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#34 anger
	cur_anim = &anim_info[ANGER_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 28 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 22 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#35 grin
	cur_anim = &anim_info[GRIN_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 28 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#36 scared
	cur_anim = &anim_info[SCARED_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 27 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 27 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 28 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 27 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 27 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 27 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#37 eyes-left
	cur_anim = &anim_info[EYES_LEFT_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 60 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 21 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#38 eyes-right
	cur_anim = &anim_info[EYES_RIGHT_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 60 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 24 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#39 eyes-up
	cur_anim = &anim_info[EYES_UP_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 60 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 29 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#40 eyes-down
	cur_anim = &anim_info[EYES_DOWN_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 60 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 26 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#41 whistle
	cur_anim = &anim_info[WHISTLE_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 20 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 20 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#42 look-left
	cur_anim = &anim_info[LOOK_LEFT_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_LEFT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#43 look-right
	cur_anim = &anim_info[LOOK_RIGHT_ANIM];
	{
		//1
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_RIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 31 - 31;
		new_frame.hand_x = 42;
		new_frame.hand_y = 94;
		cur_anim->AddFrame(new_frame);
	}

	//#44 salute
	cur_anim = &anim_info[SALUTE_ANIM];
	{
		//1
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 26;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 25 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#45 thumbsup
	cur_anim = &anim_info[THUMBS_UP_ANIM];
	{
		//1
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 24;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 22;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 50 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 44;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#46 yes-sir-salute
	cur_anim = &anim_info[YES_SIR_SALUTE_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 48;
		new_frame.hand_y = 26;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#47 goingin-thumbsup
	cur_anim = &anim_info[GOING_IN_THUMBS_UP_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 24;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 22;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 22;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 22;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 44;
		cur_anim->AddFrame(new_frame);

		//23
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 44;
		cur_anim->AddFrame(new_frame);

		//24
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//25
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//26
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#48 forget-it
	cur_anim = &anim_info[FORGET_IT_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#49 get-outta-here
	cur_anim = &anim_info[GET_OUTTA_HERE_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 11 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 16 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#50 good-hit
	cur_anim = &anim_info[GOOD_HIT_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#51 no-way
	cur_anim = &anim_info[NO_WAY_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 15 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 30 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#52 nice-one
	cur_anim = &anim_info[NICE_ONE_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 24;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 22;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 44;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#53 oh-yeah
	cur_anim = &anim_info[OH_YEAH_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 14 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 15 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#54 gotche
	cur_anim = &anim_info[GOTCHA_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 10 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 12 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#55 smookin
	cur_anim = &anim_info[SMOKIN_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 12 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 18 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 24 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 9 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#56 cool
	cur_anim = &anim_info[COOL_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 20 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#57 wipe-out
	cur_anim = &anim_info[WIPE_OUT_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 60;
		new_frame.hand_y = 52;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 34;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 24;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 22;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 11 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 18 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 54;
		new_frame.hand_y = 28;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 39 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 62;
		new_frame.hand_y = 44;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#58 territory-taken
	cur_anim = &anim_info[TERRITORY_TAKEN_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 11 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 25 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#59 fire-extinguished
	cur_anim = &anim_info[FIRE_EXTINGUISHED_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 8 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#60 gun-captueed
	cur_anim = &anim_info[GUN_CAPTURED_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 23 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 38 - 31;
		new_frame.hand_x = 68;
		new_frame.hand_y = 58;
		cur_anim->AddFrame(new_frame);
	}

	//#61 vehicle-captueed
	cur_anim = &anim_info[VEHICLE_CAPTURED_ANIM];
	{
		//1
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 33 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 62;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 34 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 48;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 36;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 35 - 31;
		new_frame.hand_x = 46;
		new_frame.hand_y = 26;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 6 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 36 - 31;
		new_frame.hand_x = 50;
		new_frame.hand_y = 12;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//15
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 52;
		new_frame.hand_y = 0;
		cur_anim->AddFrame(new_frame);

		//16
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 58;
		new_frame.hand_y = 4;
		cur_anim->AddFrame(new_frame);

		//17
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//18
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = true;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//19
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//20
		new_frame.duration = 3 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//21
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//22
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 14 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//23
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//24
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 10 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//25
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//26
		new_frame.duration = 1 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//#62 grenades-collected
	cur_anim = &anim_info[GRENADES_COLLECTED_ANIM];
	{
		//1
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//2
		new_frame.duration = 6 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 10 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//3
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//4
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 13 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//5
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//6
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//7
		new_frame.duration = 2 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 4 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//8
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//9
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 16 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//10
		new_frame.duration = 7 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//11
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 5 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//12
		new_frame.duration = 5 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 11 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//13
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 12 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);

		//14
		new_frame.duration = 4 * duration_multi;
		new_frame.look_direction = LOOK_STRAIGHT;
		new_frame.head_y = 6 - 4;
		new_frame.mouth = 7 - 4;
		new_frame.eyes = 20 - 20;
		new_frame.hand_do_render = false;
		new_frame.hand = 37 - 31;
		new_frame.hand_x = 66;
		new_frame.hand_y = 14;
		cur_anim->AddFrame(new_frame);
	}

	//homemade end anim
	anim_info[ENDW1_ANIM] = anim_info[LETS_GET_EM_ANIM];
	anim_info[ENDW2_ANIM] = anim_info[GUN_CAPTURED_ANIM];
	anim_info[ENDW3_ANIM] = anim_info[GOING_IN_ANIM];

	anim_info[ENDL1_ANIM] = anim_info[ENDW1_ANIM];
	anim_info[ENDL2_ANIM] = anim_info[ENDW2_ANIM];
	anim_info[ENDL3_ANIM] = anim_info[ENDW3_ANIM];

	////
	//cur_anim = &anim_info[xxxxxxxxxxxxxxxxxxxxxxxxxxxx];
	//{
	//}

	//for(int i=0;i<MAX_PORTRAIT_ANIMS;i++)
	//	printf("SetupFrames::%d:Duration:%f \tName:%s\n", i, anim_info[i].total_duration, portrait_anim_string[i].c_str());
}

int ZPortrait::GetBlitInfo(SDL_Surface *src, int x, int y, SDL_Rect &from_rect, SDL_Rect &to_rect)
{
	if(!src) return 0;

	int view_w = 86;
	int view_h = 74;
	int shift_x = 0;
	int shift_y = 0;

	//is this visable at ??
	if(x > shift_x + view_w) return 0;
	if(y > shift_y + view_h) return 0;
	if(x + src->w < shift_x) return 0;
	if(y + src->h < shift_y) return 0;

	//setup to
	to_rect.x = x - shift_x;
	to_rect.y = y - shift_y;
	to_rect.w = 0;
	to_rect.h = 0;

	//setup from
	from_rect.x = shift_x - x;
	from_rect.y = shift_y - y;

	if(to_rect.x + src->w > view_w)
		from_rect.w = view_w - to_rect.x;
	else
		from_rect.w = to_rect.x - view_w;

	if(to_rect.y + src->h > view_h)
		from_rect.h = view_h - to_rect.y;
	else
		from_rect.h = to_rect.y - view_h;

	if(from_rect.x < 0) from_rect.x = 0;
	if(from_rect.y < 0) from_rect.y = 0;

	to_rect.x += from_rect.x;
	to_rect.y += from_rect.y;

	return 1;
}
