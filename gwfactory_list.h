#ifndef _ZGWFACTORY_LIST_H_
#define _ZGWFACTORY_LIST_H_

#include "zgui_window.h"
#include "zolists.h"

#include <vector>
#include <string>

using namespace std;

class gwfl_render_entry
{
public:
	gwfl_render_entry() { clear(); }

	void clear() 
	{ 
		for(int i=0;i<3;i++) colored[i] = false; 
		ref_id = -1;
		x=y=0;
		w=h=0;
	}

	string message_left[3];
	string message_right[3];
	bool colored[3];
	double percent[3];
	int ref_id;

	int x, y;
	int w, h;
};

class GWFactoryList : public ZGuiWindow
{
public:
	GWFactoryList(ZTime *ztime_);

	static void Init();

	void SetOList(ZOLists *ols_) { ols = ols_; }
	void SetTeam(int our_team_) { our_team = our_team_; }

	void Process();
	void DoRender(ZMap &the_map, SDL_Surface *dest);
	bool Click(int x_, int y_);
	bool UnClick(int x_, int y_);
	bool WheelUpButton();
	bool WheelDownButton();

private:
	static bool finished_init;

	ZOLists *ols;
	int our_team;

	ZGuiButton up_button;
	ZGuiButton down_button;
	ZGuiScrollBar scroll_bar;

	static ZSDL_Surface main_top_img;
	static ZSDL_Surface main_right_img;
	static ZSDL_Surface main_entry_img;
	static ZSDL_Surface entry_bar_green_img;
	static ZSDL_Surface entry_bar_red_img;
	static ZSDL_Surface entry_bar_grey_img;
	static ZSDL_Surface entry_bar_white_i_img;

	vector<gwfl_render_entry> entry_list;
	int show_able_entries;
	int show_start_entry;

	void CollectEntries();
	void AddEntry(ZObject *obj);
	void DetermineHeight(int max_h);

	void DoUpButton();
	void DoDownButton();

	void DoRenderEntries(ZMap &the_map, SDL_Surface *dest);
};

#endif
