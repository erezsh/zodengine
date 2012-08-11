#include "zvehicle.h"

ZSDL_Surface ZVehicle::lid[MAX_ANGLE_TYPES][3];
ZSDL_Surface ZVehicle::tank_robot[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][2];

ZVehicle::ZVehicle(ZTime *ztime_, ZSettings *zsettings_) : ZObject(ztime_, zsettings_)
{
	object_name = "vehicle";
	object_type = VEHICLE_OBJECT;
	selectable = true;
	moving = false;
	next_move_time = 0;
	lid_open = false;
	show_robot = false;
	do_open_lid = false;
	do_close_lid = false;
	next_lid_time = 0;
	track_type = ET_TANK;
	
	lid_i = 0;
	robot_i = 0;
}

void ZVehicle::Init()
{
	int i, j, k;
	char filename_c[500];
	
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		for(k=0;k<3;k++)
	{
		sprintf(filename_c, "assets/units/vehicles/tank_lid_r%03d_n%02d.png", ROTATION[j], k);
		lid[j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
	}

	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<MAX_ANGLE_TYPES;j++)
			for(k=0;k<2;k++)
	{
		sprintf(filename_c, "assets/units/robots/tank_fire_%s_r%03d_n%02d.png", team_type_string[i].c_str(), ROTATION[j], k);
		//tank_robot[i][j][k].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, tank_robot[ZTEAM_BASE_TEAM][j][k], tank_robot[i][j][k], filename_c);
	}

	//set the null team
	for(j=0;j<MAX_ANGLE_TYPES;j++)
		for(k=0;k<2;k++)
			tank_robot[0][j][k] = tank_robot[RED_TEAM][j][k];
}

void ZVehicle::RecalcDirection()
{
	int new_dir;

	new_dir = DirectionFromLoc(loc.dx, loc.dy);

	if(new_dir != -1) 
	{
		moving = true;
		direction = new_dir;
		move_i = 0;
		//printf("mode:walking\n");
	}
	else
	{
		moving = false;
		//printf("mode:standing\n");
	}
}

bool ZVehicle::ShowDamaged()
{
	return (0.4 > 1.0 * health / max_health);
}

bool ZVehicle::ShowPartiallyDamaged()
{
	double health_ratio;

	health_ratio = 1.0 * health / max_health;

	return (health_ratio < 0.7 && health_ratio > 0.4);
}

void ZVehicle::TryDropTracks()
{
	double &the_time = ztime->ztime;

	if(!effect_list) return;
	if(!zmap) return;

	if(!IsMoving()) return;

	if(the_time >= next_drop_track_time)
	{
		int x[2], y[2];
		bool lay[2];

		//get where it would place the tracks
		ETrack::SetTrackCoords(center_x, center_y, direction, x, y);

		//check if these are on a road
		for(int i=0;i<2;i++)
		{
			lay[i] = false;

			if(zmap->CoordIsRoad(x[i], y[i])) continue;
			if(zmap->CoordIsRoad(x[i], y[i]-16)) continue; //up
			if(zmap->CoordIsRoad(x[i], y[i]+16)) continue; //down
			if(zmap->CoordIsRoad(x[i]-16, y[i])) continue; //left
			if(zmap->CoordIsRoad(x[i]+16, y[i])) continue; //right

			if(rand()%5) lay[i] = true;
		}

		if(lay[0] || lay[1]) 
		{
			effect_list->insert(effect_list->begin(), (ZEffect*)(new ETrack(ztime, center_x, center_y, direction, track_type, zmap->GetMapBasics().terrain_type, lay[0], lay[1])));

			for(int i=0;i<2;i++)
				if(lay[i] && rand() % 4) 
					effect_list->insert(effect_list->begin(), (ZEffect*)(new ETankDirt(ztime, x[i], y[i], zmap->GetMapBasics().terrain_type, direction == 0 || direction >= 4)));
		}

		//partially damaged smoke
		if(ShowPartiallyDamaged() && !(rand() % 3)) effect_list->insert(effect_list->begin(), (ZEffect*)(new ETankSmoke(ztime, center_x, center_y, direction, false)));

		//fully damaged fire smoke, sparks and oil
		if(ShowDamaged()) 
		{
			if(!(rand() % 3)) effect_list->insert(effect_list->begin(), (ZEffect*)(new ETankSmoke(ztime, center_x, center_y, direction, true)));
			if(!(rand() % 16)) effect_list->insert(effect_list->begin(), (ZEffect*)(new ETankOil(ztime, center_x, center_y, direction)));
			if(!(rand() % 48)) effect_list->insert(effect_list->begin(), (ZEffect*)(new ETankSpark(ztime, center_x, center_y, direction)));
		}

		next_drop_track_time = the_time + 0.2;
	}
}

void ZVehicle::SetAttackObject(ZObject *obj)
{
	attack_object = obj;

	if(attack_object)
	{
		int tx, ty;
		int new_dir;

		attack_object->GetCenterCords(tx, ty);
		new_dir = DirectionFromLoc((float)(tx - loc.x), (float)(ty - loc.y));
		if(new_dir != -1) t_direction = new_dir;
	}
}

void ZVehicle::SetInitialDrivers()
{
	if(owner != NULL_TEAM)
	{
		driver_type = GRUNT;
		//AddDriver(GRUNT_MAX_HEALTH);
		AddDriver((int)zsettings->GetUnitSettings(ROBOT_OBJECT, GRUNT).health * MAX_UNIT_HEALTH);
	}
	else
	{
		driver_type = GRUNT;
		ClearDrivers();
	}
}

bool ZVehicle::CanBeRepaired()
{
	if(IsDestroyed()) return false;
	if(health >= max_health) return false;

	return true;
}

bool ZVehicle::CanBeSniped()
{
	if(has_lid)
	{
		return can_be_sniped && lid_open && driver_info.size();
	}
	else
		return can_be_sniped && driver_info.size();
}

void ZVehicle::SetLidState(bool lid_open_)
{
	lid_open = lid_open_;
}

bool ZVehicle::GetLidState()
{
	return lid_open;
}

void ZVehicle::SignalLidShouldOpen()
{
	if(!has_lid) return;

	//1 in 3 chance we don't care
	if(!(rand() % 5)) return;

	sflags.updated_open_lid = true;
	lid_open = true;
}

void ZVehicle::SignalLidShouldClose()
{
	double &the_time = ztime->ztime;

	if(!has_lid) return;

	if(lid_open && !do_close_lid)
	{
		do_close_lid = true;
		next_close_lid_time = the_time + (0.1 * (rand() % 15));
	}
}

void ZVehicle::ProcessServerLid()
{
	double &the_time = ztime->ztime;

	if(do_open_lid && the_time >= next_open_lid_time)
	{
		do_open_lid = false;

		sflags.updated_open_lid = true;
		lid_open = true;
	}
	else if(do_close_lid && the_time >= next_close_lid_time)
	{
		do_close_lid = false;

		sflags.updated_open_lid = true;
		lid_open = false;
	}
}

void ZVehicle::ProcessLid()
{
	double &the_time = ztime->ztime;

	if(the_time >= next_lid_time)
	{
		next_lid_time = the_time + 0.2;

		if(lid_open)
		{
			if(lid_i>=2)
				show_robot = true;
			else
				lid_i++;
		}
		else
		{
			show_robot = false;

			if(lid_i > 0)
				lid_i--;
		}

	}
}

void ZVehicle::RenderLid(ZMap &the_map, SDL_Surface *dest, int &lx, int &ly, int &shift_x, int &shift_y)
{
	const int robot_shift_x[8] = {3, -1, -3, -7, -10, -7, -4, 0};
	const int robot_shift_y[8] = {0, -4, -6, -4,   0,  1,  1, 1};
	int rx, ry;
	ZSDL_Surface *lid_surface;
	ZSDL_Surface *robot_surface;
//	SDL_Rect from_rect, to_rect;

	lid_surface = &lid[t_direction][lid_i];

	if(show_robot)
	{
		robot_surface = &tank_robot[owner][t_direction][robot_i];

		rx = lx + robot_shift_x[t_direction];
		ry = ly + robot_shift_y[t_direction];

		//render order is different depending on the direction
		if(t_direction > 3 || !t_direction)
		{
			the_map.RenderZSurface(lid_surface, lx, ly, do_hit_effect);
			//if(the_map.GetBlitInfo(lid_surface, lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	ZSDL_BlitHitSurface(lid_surface, &from_rect, dest, &to_rect, do_hit_effect);
			//}

			the_map.RenderZSurface(robot_surface, rx, ry, do_driver_hit_effect);
			//if(the_map.GetBlitInfo(robot_surface, rx, ry, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	ZSDL_BlitHitSurface(robot_surface, &from_rect, dest, &to_rect, do_driver_hit_effect);
			//}
		}
		else
		{
			the_map.RenderZSurface(robot_surface, rx, ry, do_driver_hit_effect);
			//if(the_map.GetBlitInfo(robot_surface, rx, ry, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	ZSDL_BlitHitSurface(robot_surface, &from_rect, dest, &to_rect, do_driver_hit_effect);
			//}

			the_map.RenderZSurface(lid_surface, lx, ly, do_hit_effect);
			//if(the_map.GetBlitInfo(lid_surface, lx, ly, from_rect, to_rect))
			//{
			//	to_rect.x += shift_x;
			//	to_rect.y += shift_y;

			//	ZSDL_BlitHitSurface(lid_surface, &from_rect, dest, &to_rect, do_hit_effect);
			//}
		}
	}
	else
	{
		//just the lid
		the_map.RenderZSurface(lid_surface, lx, ly, do_hit_effect);
		//if(the_map.GetBlitInfo(lid_surface, lx, ly, from_rect, to_rect))
		//{
		//	to_rect.x += shift_x;
		//	to_rect.y += shift_y;

		//	ZSDL_BlitHitSurface(lid_surface, &from_rect, dest, &to_rect, do_hit_effect);
		//}
	}

	do_driver_hit_effect = false;
}
