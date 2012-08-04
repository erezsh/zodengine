#ifndef _ZDAMGEMISSILE_H_
#define _ZDAMGEMISSILE_H_

#include <math.h>

#include "common.h"

using namespace COMMON;

class damage_missile
{
public:
	damage_missile(){};
	damage_missile(int x_, int y_, int damage_, int radius_, double explode_time_)
	{
		x = x_;
		y = y_;
		damage = damage_;
		radius = radius_;
		explode_time = explode_time_;
	}

	void CalcExplodeTimeTo(int sx, int sy, int missile_speed, double the_time)
	{
		float mag;
		int dx, dy;

		dx = x - sx;
		dy = y - sy;
		mag = sqrt((float)((dx * dx) + (dy * dy)));

		explode_time = the_time + (mag / missile_speed);
	}

	int x, y;
	int damage;
	int team;
	int radius;
	double explode_time;
	int attacker_ref_id;
	bool attack_player_given;
	int target_ref_id;
};

#endif
