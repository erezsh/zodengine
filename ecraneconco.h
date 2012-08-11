#ifndef _ECRANECONCO_H_
#define _ECRANECONCO_H_

#include "zeffect.h"
#include <list>

enum ecraneconco_render_item
{
	ECC_CONCO, ECC_CONE0, ECC_CONE1, ECC_JACK, ECC_PAPER, ECC_SIGN, ECC_RENDER_ITEM_TYPES
};

class ECraneConcoItem
{
public:
	ECraneConcoItem()
	{
		x = y = 0;
		start_x = start_y = 0;
		dest_x = dest_y = 0;
		w = h = 0;
		type = -1;
	}

	void Init(int type_, int c_cx, int c_cy, int w_ = 0, int h_ = 0)
	{
		type = type_;
		start_x = dest_x = x = c_cx;
		start_y = dest_y = y = c_cy;
		w = w_;
		h = h_;
		SetTravelDistances();
	}

	void SetStart(int x_, int y_)
	{
		start_x = x = x_ - (w >> 1);
		start_y = y = y_ - (w >> 1);
	}

	void SetReturn(int c_cx, int c_cy)
	{
		start_x = x;
		start_y = y;
		dest_x = c_cx - (w >> 1);
		dest_y = c_cy - (h >> 1);
		SetTravelDistances();
	}

	void SetTravelDistances()
	{
		w_dist = dest_x - start_x;
		h_dist = dest_y - start_y;
	}

	void Move(double percentage)
	{
		x = start_x + (int)((w_dist) * percentage);
		y = start_y + (int)((h_dist) * percentage);
	}

	void MoveToDest()
	{
		x = dest_x;
		y = dest_y;
	}

	int type;
	int x, y;
	int start_x, start_y;
	int dest_x, dest_y;
	int w, h;
	int w_dist, h_dist;
};
 
extern bool ecc_render_item_comp (ECraneConcoItem* a, ECraneConcoItem* b);

class ECraneConco : public ZEffect
{
public:
	ECraneConco(ZTime *ztime_, int team_, int c_x, int c_y, int b_x, int b_y, int b_w, int b_h, bool is_bridge = false);

	static void Init();

	void Process();
	void DoRender(ZMap &zmap, SDL_Surface *dest);

	void BeginDeath(int c_x, int c_y);

private:
	void SetBotInitCords(bool is_bridge, int c_cx, int c_cy, int b_x, int b_y, int b_w, int b_h, int &dest_x, int &dest_y);

	static bool finished_init;
	static ZSDL_Surface conco[MAX_TEAM_TYPES][8];
	static ZSDL_Surface cone_no_shadow[MAX_TEAM_TYPES];
	static ZSDL_Surface cone[MAX_TEAM_TYPES];
	static ZSDL_Surface robot_jackhammer[MAX_TEAM_TYPES][2];
	static ZSDL_Surface robot_paper[MAX_TEAM_TYPES][2];
	static ZSDL_Surface robot_point[MAX_TEAM_TYPES][3];
	static ZSDL_Surface robot_travel_right[MAX_TEAM_TYPES];
	static ZSDL_Surface robot_travel_left[MAX_TEAM_TYPES];
	static ZSDL_Surface robot_travel_updown[MAX_TEAM_TYPES];
	static ZSDL_Surface sign_flip[MAX_TEAM_TYPES][8];
	static ZSDL_Surface sign[MAX_TEAM_TYPES];
	
	static int conco_w, conco_h;
	static int cone_w, cone_h;
	static int sign_w, sign_h;

	int our_team;

	ECraneConcoItem render_item[ECC_RENDER_ITEM_TYPES];
	list<ECraneConcoItem*> render_item_list;

	int conco_i;

	int sign_i;

	int jackbot_i;
	double next_jackbot_time;

	int pbot_i;
	bool pbot_pointing;
	double next_pbot_time;

	static const double travel_time_width;
	double travel_time_start, travel_time_end;
	bool travel_to, travel_back;
};

#endif
