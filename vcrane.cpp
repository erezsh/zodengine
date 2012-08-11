#include "vcrane.h"

using namespace COMMON;

const double turrent_time_int = 1.0;
const double hook_time_int = 0.7;

ZSDL_Surface VCrane::base[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
ZSDL_Surface VCrane::crane[MAX_ANGLE_TYPES];
ZSDL_Surface VCrane::hook[16];
ZSDL_Surface VCrane::wasted[MAX_TEAM_TYPES];

VCrane::VCrane(ZTime *ztime_, ZSettings *zsettings_) : ZVehicle(ztime_, zsettings_)
{
	double &the_time = ztime->ztime;

	width = 2;
	height = 2;
	width_pix = 32;
	height_pix = 32;
	hover_name = "Crane";
	object_name = "crane";

	InitTypeId(object_type, CRANE);

	//object_id = CRANE;
	//move_speed = CRANE_SPEED;
	//max_health = CRANE_MAX_HEALTH;
	//health = max_health;
	
	t_direction = direction;
	move_i = rand() % 3;
	hook_i = 0;
	anim_on = false;
	conco_anim = NULL;
	
	next_turrent_time = the_time;
	next_hook_time = next_turrent_time;
	
	next_turrent_time += turrent_time_int;
	next_hook_time += hook_time_int;
}

VCrane::~VCrane()
{
	if(conco_anim) delete conco_anim;
}

void VCrane::Init()
{
	int i, j, k;
	char filename_c[500];
	SDL_Surface *temp_surface;
	
	strcpy(filename_c, "assets/units/vehicles/crane/empty_null.png");
	temp_surface = IMG_Load_Error(filename_c);
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		for(k=0;k<3;k++)
			base[0][j][k] = temp_surface;

	ZSDL_FreeSurface(temp_surface);
	
	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
			for(k=0;k<3;k++)
	{
		sprintf(filename_c, "assets/units/vehicles/crane/base_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
		//base[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, base[ZTEAM_BASE_TEAM][j][k], base[i][j][k], filename_c);
	}
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
	{
		sprintf(filename_c, "assets/units/vehicles/crane/crane_r%03d.png", ROTATION_INVERTED[j]);
		crane[j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(j=0;j<8;j++)
	{
		sprintf(filename_c, "assets/units/vehicles/crane/hook_n%02d.png", j);
		hook[j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		hook[15-j] = hook[j];
	}
	
	for(i=0;i<MAX_TEAM_TYPES;i++)
	{
		sprintf(filename_c, "assets/units/vehicles/crane/wasted_%s.png", team_type_string[i].c_str());
		//wasted[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, wasted[ZTEAM_BASE_TEAM], wasted[i], filename_c);
	}
}

// SDL_Surface *VCrane::GetRender()
// {
// 	if(destroyed)
// 	{
// 		return wasted[owner];
// 	}
// 	else
// 	{
// 		return base[owner][direction][base_i];
// 	}
// }

int VCrane::Process()
{
	double &the_time = ztime->ztime;

	if(moving && the_time >= next_move_time)
	{
		move_i++;
		if(move_i >= 3) move_i = 0;

		next_move_time = the_time + (VEHICLE_MOVE_ANIM_SPEED * SpeedOffsetPercentInv());
	}

	if(the_time >= next_turrent_time)
	{
		next_turrent_time = the_time + turrent_time_int;
		
		t_direction++;
		if(t_direction >= MAX_ANGLE_TYPES) t_direction = 0;
	}

	if(anim_on && the_time >= next_hook_time)
	{
		next_hook_time = the_time + 0.01;
		
		hook_i++;
		if(hook_i >= 16) hook_i = 0;
	}

	if(conco_anim)
	{
		if(conco_anim->KillMe())
		{
			delete conco_anim;
			conco_anim = NULL;
		}
		else
			conco_anim->Process();
	}
	
	return 1;
}

void VCrane::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
//	SDL_Surface *base_surface;
//	SDL_Rect from_rect, to_rect;
//	int lx, ly;

	if(IsDestroyed())
	{
		the_map.RenderZSurface(&wasted[owner], x, y);
		//if(the_map.GetBlitInfo(wasted[owner], x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( wasted[owner], &from_rect, dest, &to_rect);
		//}

		return;
	}

	if(conco_anim && !conco_anim->KillMe())
		conco_anim->DoRender(the_map, dest);
	
	the_map.RenderZSurface(&base[owner][direction][move_i], x, y, do_hit_effect);
	//base_surface = base[owner][direction][move_i];
	//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	//SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
	//	ZSDL_BlitHitSurface(base_surface, &from_rect, dest, &to_rect, do_hit_effect);
	//}

	if(owner != NULL_TEAM)
	{
		RenderCrane(the_map, dest, shift_x, shift_y);
	}

	do_hit_effect = false;
}

void VCrane::RenderCrane(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	const int crane_x[8] = {-6, -3,  0, 3,  6,  1,  0, -2};
	const int crane_y[8] = {-6, -4, -5, -4, -6, -8, -9, -8};
	const int hook_x[8] = {0,   4,  14,  23,  25,  21,  14, 5};
	const int hook_y[8] = {14, 20,  23,  20,  14,  8,  5, 8};
	//SDL_Surface *base_surface;
//	SDL_Rect from_rect, to_rect;
	int lx, ly;

	lx = x + crane_x[direction] + hook_x[direction];
	ly = y + crane_y[direction] + hook_y[direction];

	the_map.RenderZSurface(&hook[hook_i], lx, ly, do_hit_effect);
	//if(the_map.GetBlitInfo(hook[hook_i], lx, ly, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	//SDL_BlitSurface( hook[hook_i], &from_rect, dest, &to_rect);
	//	ZSDL_BlitHitSurface( hook[hook_i], &from_rect, dest, &to_rect, do_hit_effect);
	//}
	
	lx = x + crane_x[direction];
	ly = y + crane_y[direction];

	the_map.RenderZSurface(&crane[direction], lx, ly, do_hit_effect);
	//if(the_map.GetBlitInfo(crane[direction], lx, ly, from_rect, to_rect))
	//{
	//	to_rect.x += shift_x;
	//	to_rect.y += shift_y;

	//	//SDL_BlitSurface( crane[direction], &from_rect, dest, &to_rect);
	//	ZSDL_BlitHitSurface( crane[direction], &from_rect, dest, &to_rect, do_hit_effect);
	//}
}

void VCrane::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	//int &x = loc.x;
	//int &y = loc.y;
	//SDL_Rect from_rect, to_rect;
	//int lx, ly;
}

void VCrane::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	if(effect_list) effect_list->insert(effect_list->begin(), (ZEffect*)(new EDeath(ztime, loc.x, loc.y, EDEATH_CRANE)));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}

void VCrane::DoCraneAnim(bool on_, ZObject *rep_obj)
{
	if(on_)
	{
		//spawn extra animations
		next_hook_time = 0;

		if(rep_obj)
		{
			bool is_bridge;
			unsigned char ot, oid;

			if(conco_anim)
			{
				delete conco_anim;
				conco_anim = NULL;
			}

			rep_obj->GetObjectID(ot, oid);

			if(ot == BUILDING_OBJECT)
			{
				int bx, by, bw, bh;

				if(oid == BRIDGE_VERT || oid == BRIDGE_HORZ)
					is_bridge = true;
				else
					is_bridge = false;

				rep_obj->GetCords(bx, by);
				rep_obj->GetDimensionsPixel(bw, bh);

				conco_anim = new ECraneConco(ztime, owner, loc.x, loc.y, bx, by, bw, bh, is_bridge);
			}
		}
	}
	else
	{
		//kill extra animations
		hook_i = 0;

		if(conco_anim)
			conco_anim->BeginDeath(loc.x, loc.y);
	}

	anim_on = on_;
}
