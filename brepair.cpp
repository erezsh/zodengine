#include "brepair.h"

ZSDL_Surface BRepair::base[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
ZSDL_Surface BRepair::base_destroyed[MAX_PLANET_TYPES];
ZSDL_Surface BRepair::base_color[MAX_TEAM_TYPES];

ZSDL_Surface BRepair::smoke_stack[5];
ZSDL_Surface BRepair::text_box[3];
ZSDL_Surface BRepair::side_light[2];
ZSDL_Surface BRepair::front_light[2];
ZSDL_Surface BRepair::bulb[2];

BRepair::BRepair(ZTime *ztime_, ZSettings *zsettings_, planet_type palette_) : ZBuilding(ztime_, zsettings_, palette_)
{
	width = 5;
	height = 4;
	width_pix = width * 16;
	height_pix = height * 16;
	object_name = "repair";
	object_id = REPAIR;
	palette = palette_;

	InitTypeId(object_type, object_id);

	//max_health = REPAIR_BUILDING_HEALTH;
	//health = max_health;
	
	smoke_stack_i = 0;
	text_box_i = 0;
	side_light_i = 0;
	front_light_i = 0;
	bulb_i = 0;

	//fire effects
	effects_box.x = 8;
	effects_box.y = 8;
	effects_box.w = width_pix - (16 + 8);
	effects_box.h = height_pix - (16 + 8);
	max_effects = 6 + (rand() % 4);

	repairing_unit = false;
}

// SDL_Surface *BRepair::GetRender()
// {
// 	if(destroyed)
// 	{
// 		return base_destroyed[palette];
// 	}
// 	else
// 	{
// 		return base[palette][owner];
// 	}
// }

void BRepair::Init()
{
	int i, j;
	string filename;
	char filename_c[500];
	
	//load colors
	//base_color[0] = NULL;
	for(j=1;j<MAX_TEAM_TYPES;j++)
	{
		filename = "assets/buildings/repair/" + team_type_string[j] + ".png";
		//base_color[j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		ZTeam::LoadZSurface(j, base_color[ZTEAM_BASE_TEAM], base_color[j], filename);
	}
	
	//load images pertaining to a fort
	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/buildings/repair/base_" + planet_type_string[i] + ".png";
		base[i][0].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		filename = "assets/buildings/repair/base_destroyed_" + planet_type_string[i] + ".png";
		base_destroyed[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		
		//make the other colors
		for(j=1;j<MAX_TEAM_TYPES;j++)
		{
			SDL_Rect dest;
			
			//base[i][j] = CopyImage(base[i][0]);
			base[i][j] = base[i][0].GetBaseSurface();
			
			dest.x = 0;
			dest.y = 48;
			
			//SDL_BlitSurface(base_color[j], NULL, base[i][j], &dest);
			base_color[j].BlitSurface(NULL, &dest, &base[i][j]);
		}
	}
	
	//now load effects
	for(i=0;i<5;i++)
	{
		sprintf(filename_c, "assets/buildings/repair/smoke_stack_%d.png", i);
		smoke_stack[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(i=0;i<3;i++)
	{
		sprintf(filename_c, "assets/buildings/repair/text_box_%d.png", i);
		text_box[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
	
	for(i=0;i<2;i++)
	{
		sprintf(filename_c, "assets/buildings/repair/bulb_%d.png", i);
		bulb[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		
		sprintf(filename_c, "assets/buildings/repair/side_light_%d.png", i);
		side_light[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		
		sprintf(filename_c, "assets/buildings/repair/front_light_%d.png", i);
		front_light[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}
}

int BRepair::Process()
{
	double &the_time = ztime->ztime;
	const double min_interval_time = 0.35;

	ProcessBuildingsEffects(the_time);
	
	if(the_time - last_process_time >= min_interval_time)
	{
		last_process_time = the_time;
		
		smoke_stack_i++;
		if(smoke_stack_i >= 5) smoke_stack_i = 0;
		
		text_box_i++;
		if(text_box_i >= 3) text_box_i = 0;
		
		side_light_i++;
		if(side_light_i >= 2) side_light_i = 0;
		
		front_light_i++;
		if(front_light_i >= 2) front_light_i = 0;
		
		bulb_i++;
		if(bulb_i >= 2) bulb_i = 0;
	}

	if(repairing_unit)
		ResetShowTime((int)(repair_time - the_time));
	else
		ResetShowTime(-1);

	return 1;
}

void BRepair::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
	SDL_Rect from_rect, to_rect;
	int lx, ly;
	
	if(!dont_stamp)
	{
		if(do_base_rerender)
		{
			if(IsDestroyed())
				base_surface = &base_destroyed[palette];
			else
				base_surface = &base[palette][owner];
			
			if(the_map.PermStamp(x, y, base_surface))
				do_base_rerender = false;

			//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
			//}
		}
	}
	else
	{
		if(IsDestroyed())
			base_surface = &base_destroyed[palette];
		else
			base_surface = &base[palette][owner];

		the_map.RenderZSurface(base_surface, x, y);
		//if(the_map.GetBlitInfo(base_surface, x, y, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
		//}
	}
}

void BRepair::DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	const int front_light_x = 6;
	const int front_light_y = 16;
	const int side_light_x = 18;
	const int side_light_y = 6;
	const int bulb_x = 32;
	const int bulb_y = 0;
	const int smoke_stack_x = 61;
	const int smoke_stack_y = 0;
	const int text_box_x = 16;
	const int text_box_y = 32;
	ZSDL_Surface *base_surface;
	
	
	SDL_Rect from_rect, to_rect;
	int lx, ly;

	//do this right when this building can repair
	if(!repairing_unit)
	{
		bulb_i = 0;
		smoke_stack_i = 0;
	}

	//these are always on
	front_light_i = 1;
	side_light_i = 1;
	
	if(!IsDestroyed())
	{
		if(owner != NULL_TEAM)
		{
			const int x_plus = 26 - 16;
			const int y_plus = 30 - 24;

			if(the_map.GetBlitInfo(x + x_plus, y + y_plus, 44, 50 - (y_plus), from_rect, to_rect))
			{
				from_rect.x += x_plus;
				from_rect.y += y_plus;

				to_rect.x += shift_x;
				to_rect.y += shift_y;

				if(IsDestroyed())
					base_surface = &base_destroyed[palette];
				else
					base_surface = &base[palette][owner];

				base_surface->BlitSurface(&from_rect, &to_rect);
				//SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
			}

			lx = x + text_box_x;
			ly = y + text_box_y;
			
			the_map.RenderZSurface(&text_box[text_box_i], lx, ly);
			//if(the_map.GetBlitInfo(text_box[text_box_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( text_box[text_box_i], &from_rect, dest, &to_rect);
			//}
			
			lx = x + bulb_x;
			ly = y + bulb_y;

			the_map.RenderZSurface(&bulb[bulb_i], lx, ly);
			//if(the_map.GetBlitInfo(bulb[bulb_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( bulb[bulb_i], &from_rect, dest, &to_rect);
			//}
			
			if(repairing_unit)
			{
				lx = x + smoke_stack_x;
				ly = y + smoke_stack_y;

				the_map.RenderZSurface(&smoke_stack[smoke_stack_i], lx, ly);
				//if(the_map.GetBlitInfo(smoke_stack[smoke_stack_i], lx, ly, from_rect, to_rect))
				//{
				//	to_rect.x += shift_x;
				//	to_rect.y += shift_y;

				//	SDL_BlitSurface( smoke_stack[smoke_stack_i], &from_rect, dest, &to_rect);
				//}
			}
			
			lx = x + front_light_x;
			ly = y + front_light_y;

			the_map.RenderZSurface(&front_light[front_light_i], lx, ly);
			//if(the_map.GetBlitInfo(front_light[front_light_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( front_light[front_light_i], &from_rect, dest, &to_rect);
			//}
			
			lx = x + side_light_x;
			ly = y + side_light_y;

			the_map.RenderZSurface(&side_light[side_light_i], lx, ly);
			//if(the_map.GetBlitInfo(side_light[side_light_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( side_light[side_light_i], &from_rect, dest, &to_rect);
			//}

			//render time
			lx = x + 25;
			ly = y + 41;

			the_map.RenderZSurface(&show_time_img, lx, ly);
			//if(show_time_img && the_map.GetBlitInfo(show_time_img, x + 25, y + 41, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( show_time_img, &from_rect, dest, &to_rect);
			//}
		}
		else //not owned
		{
			lx = x + smoke_stack_x;
			ly = y + smoke_stack_y;
			
			the_map.RenderZSurface(&smoke_stack[smoke_stack_i], lx, ly);
			//if(the_map.GetBlitInfo(smoke_stack[smoke_stack_i], lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	SDL_BlitSurface( smoke_stack[smoke_stack_i], &from_rect, dest, &to_rect);
			//}
		}
	}
	else
	{
		//IsDestroyed()
		//const int x_plus = 32 - 16;
		//const int y_plus = 32 - 24;

		//if(the_map.GetBlitInfo(x + x_plus, y + y_plus, 32, 50 - (y_plus), from_rect, to_rect))
		const int x_plus = 26 - 16;
		const int y_plus = 30 - 24;

		if(the_map.GetBlitInfo(x + x_plus, y + y_plus, 44, 50 - (y_plus), from_rect, to_rect))
		{
			from_rect.x += x_plus;
			from_rect.y += y_plus;

			to_rect.x += shift_x;
			to_rect.y += shift_y;

			base_destroyed[palette].BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface( base_destroyed[palette], &from_rect, dest, &to_rect);
		}
	}

	//render effects
	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->DoRender(the_map, dest);
}

void BRepair::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	int sx, sy;
	int ex, ey;
	double offset_time;
	int peices;
	int fireballs;
	int i;

	//fireballs
	{
		fireballs = 6 + (rand() % 3);

		for(i=0;i<fireballs;i++)
		{
			sx = loc.x + effects_box.x + (rand() % effects_box.w);
			sy = loc.y + effects_box.y + (rand() % effects_box.h);

			if(effect_list) effect_list->push_back((ZEffect*)(new ESideExplosion(ztime, sx, sy, 1.3)));
		}
	}

	peices = 4 + (rand() % 3);

	for(i=0;i<peices;i++)
	{
		sx = loc.x + effects_box.x + (rand() % effects_box.w);
		sy = loc.y + effects_box.y + (rand() % effects_box.h);
		ex = loc.x + (width_pix >> 1) + (200 - (rand() % 400));
		ey = loc.y + (height_pix >> 1) + (200 - (rand() % 400));

		offset_time = 1.5 + (0.01 * (rand() % 200));

		switch(rand()%2)
		{
		case 0:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_BUILDING_PEICE0)));
			break;
		case 1:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, sx, sy, ex, ey, offset_time, ETURRENTMISSILE_BUILDING_PEICE1)));
			break;
		}
	}

	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);
	ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, loc.x, loc.y, width_pix, height_pix);
	do_base_rerender = true;
}

void BRepair::SetMapImpassables(ZMap &tmap)
{
	int tx, ty, ex, ey;
	int i, j;

	tx = loc.x / 16;
	ty = loc.y / 16;
	ex = tx + width;
	ey = ty + height;

	for(i=tx;i<ex;i++)
		for(j=ty;j<ey;j++)
			tmap.SetImpassable(i,j);

	//unset
	//tmap.SetImpassable(tx+3,ty+2, false);
}

bool BRepair::GetCraneEntrance(int &x, int &y, int &x2, int &y2)
{
	x = x2 = loc.x + 32;
	y = y2 = loc.y + height_pix + 32;
	return true;
}

bool BRepair::GetCraneCenter(int &x, int &y)
{
	x = loc.x + 32;
	y = loc.y + 32;
	return true;
}

bool BRepair::GetRepairEntrance(int &x, int &y)
{
	x = loc.x + 32;
	y = loc.y + height_pix + 32;
	return true;
}

bool BRepair::GetRepairCenter(int &x, int &y)
{
	x = loc.x + 32;
	y = loc.y + 32;
	return true;
}

bool BRepair::CanRepairUnit(int units_team)
{
	if(owner == NULL_TEAM) return false;
	if(units_team != owner) return false;
	if(IsDestroyed()) return false;

	return true;
}

bool BRepair::RepairingAUnit()
{
	return repairing_unit;
}

bool BRepair::SetRepairUnit(ZObject *unit_obj)
{
	double &the_time = ztime->ztime;
	unsigned char ot, oid;

	//checks
	if(!unit_obj) return false;
	if(repairing_unit) return false;
	if(!CanRepairUnit(unit_obj->GetOwner())) return false;

	//get info
	unit_obj->GetObjectID(ot, oid);

	//store info
	rot = ot;
	roid = oid;
	driver_type = unit_obj->GetDriverType();
	driver_info = unit_obj->GetDrivers();
	rwaypoint_list = unit_obj->GetWayPointList();

	//set time
	repairing_unit = true;
	repair_time = the_time + 5;

	return true;
}

void BRepair::CreateRepairAnimData(char *&data, int &size, bool play_sound)
{
	double &the_time = ztime->ztime;

	repair_building_anim_packet send_data;

	size = sizeof(repair_building_anim_packet);
	data = (char*)malloc(size);

	send_data.ref_id = ref_id;
	send_data.on = repairing_unit;
	send_data.play_sound = play_sound;
	if(repairing_unit)
		send_data.remaining_time = repair_time - the_time;
	else
		send_data.remaining_time = 0;

	memcpy(data, &send_data, sizeof(repair_building_anim_packet));
}

void BRepair::DoRepairBuildingAnim(bool on_, double remaining_time_)
{
	double &the_time = ztime->ztime;

	repairing_unit = on_;
	repair_time = the_time + remaining_time_;

	if(repairing_unit)
	{
		bulb_i = 0;
		smoke_stack_i = 0;
	}
}

bool BRepair::RepairUnit(double &the_time, unsigned char &ot, unsigned char &oid, int &driver_type_, vector<driver_info_s> &driver_info_, vector<waypoint> &rwaypoint_list_)
{
	if(!repairing_unit) return false;
	if(the_time < repair_time) return false;

	//ok lets "repair"
	ot = rot;
	oid = roid;
	driver_type_ = driver_type;
	driver_info_ = driver_info;
	rwaypoint_list_ = rwaypoint_list;

	repairing_unit = false;

	return true;
}
