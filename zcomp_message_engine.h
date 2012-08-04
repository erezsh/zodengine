#ifndef _ZCOMP_MESSAGE_ENGINE_H_
#define _ZCOMP_MESSAGE_ENGINE_H_

#include "constants.h"
#include "zsdl.h"
#include "zmap.h"
#include "ztime.h"

#define MAX_RENDERABLE_STORED_GUNS 8

enum comp_message
{
	ROBOT_MANUFACTURED_MSG, VEHICLE_MANUFACTURED_MSG, GUN_MANUFACTURED_MSG,
	FORT_MSG
};

class ZObject;

class comp_msg_flags
{
public:
	comp_msg_flags() { Clear(); };

	void Clear()
	{
		ref_id = -1;
		open_gui = false;
		select_obj = false;
		resume_game = false;
	}

	bool open_gui;
	bool select_obj;
	bool resume_game;
	int ref_id;
};

class ZCompMessageEngine
{
public:
	ZCompMessageEngine();

	static void Init();

	void SetZTime(ZTime *ztime_);
	void Process(double the_time);
	void DoRender(ZMap &the_map, SDL_Surface *dest);
	void SetObjectList(vector<ZObject*> *object_list_);
	void SetTeam(int our_team_);
	void DisplayMessage(int comp_message_, int ref_id_);
	bool AbsorbedLClick(int x, int y, ZMap &the_map);
	bool AbsorbedLUnClick(int x, int y, ZMap &the_map);
	comp_msg_flags &GetFlags();

private:
	void RenderGuns(ZMap &the_map, SDL_Surface *dest);
	void RenderResume(ZMap &the_map, SDL_Surface *dest);

	static ZSDL_Surface robot_manufactured;
	static ZSDL_Surface vehicle_manufactured;
	static ZSDL_Surface gun_manufactured;
	static ZSDL_Surface fort_under_attacked;
	static ZSDL_Surface gun_img;
	static ZSDL_Surface paused_img;
	static ZSDL_Surface click_to_resume_img;
	static ZSDL_Surface x_img[MAX_STORED_CANNONS];

	ZTime *ztime;

	int show_message;
	double final_time;
	double next_flip_time;
	bool show_the_message;
	ZSDL_Surface *show_message_img;
	int flips_done;
	int ref_id;
	SDL_Rect render_box;
	comp_msg_flags cmflags;
	vector<ZObject*> *object_list;
	int our_team;
	int rendered_gun_ref_id[MAX_RENDERABLE_STORED_GUNS];
	int rendered_guns;
};

#endif
