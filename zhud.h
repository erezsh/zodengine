#ifndef _ZHUD_H_
#define _ZHUD_H_

#include <string>
#include <time.h>
#include "zsdl.h"
#include "zobject.h"
#include "zmini_map.h"
#include "zfont_engine.h"
#include "zportrait.h"
#include "ztime.h"

using namespace std;

enum hud_buttons
{
	A_BUTTON, B_BUTTON, D_BUTTON, G_BUTTON, MENU_BUTTON, R_BUTTON, T_BUTTON, V_BUTTON, Z_BUTTON, MAX_HUD_BUTTONS
};

const string hub_buttons_string[MAX_HUD_BUTTONS] =
{
	"a_button", "b_button", "d_button", "g_button", "menu_button", "r_button", "t_button", "v_button", "z_button"
};

enum hud_button_state
{
	B_ACTIVE, B_INACTIVE, B_PRESSED, MAX_HUD_BUTTON_STATES
};

enum hud_reponse_type
{
	HUD_BUTTON, HUD_MINI_MAP, HUD_JUMP_TO_UNIT
};

class hud_click_response
{
public:
	hud_click_response() { clear(); }

	bool used;
	int type;
	int button;
	int mini_x, mini_y;
	int jump_ref_id;

	void clear()
	{
		used = false;
		type = -1;
		jump_ref_id = -1;
	}
};

class HubButton
{
	public:
		HubButton();
		void SetType(hud_buttons type_);
		void Init();
		void Render(SDL_Surface *dest, int off_x, int off_y);
		bool WithinCords(int x, int y);
		hud_button_state CurrentState();
		void SetState(hud_button_state new_state);
		void SetShift(int shift_x_ = 0, int shift_y_ = 0);
		hud_buttons GetType();
		
	private:
		string name;
		int type;
		int state;
		int x, y;
		int shift_x, shift_y;
		
		ZSDL_Surface button_img[MAX_HUD_BUTTON_STATES];
};

class zhud_end_unit
{
public:
	zhud_end_unit() {}
	zhud_end_unit(unsigned char ot_, unsigned char oid_, unsigned char roid_)
	{
		ot = ot_;
		oid = oid_;
		roid = roid_;
	}

	unsigned char ot, oid, roid;
};

class ZHud
{
	public:
		ZHud();
		
		void Init();
		void Process(double the_time, vector<ZObject*> &object_list);
		void DoRender(SDL_Surface *dest, int screen_w, int screen_h);
		void SetZTime(ZTime *ztime_);
		void ReRenderAll();
		void SetTerrainType(planet_type terrain_);
		void LeftClick(int x, int y, int screen_w, int screen_h, hud_click_response &response);
		void LeftUnclick(int x, int y, int screen_w, int screen_h, hud_click_response &response);
		void MouseMotion(int x, int y, int screen_w, int screen_h, hud_click_response &response);
		bool OverMiniMap(int x, int y, int screen_w, int screen_h, int &mini_x, int &mini_y);
		bool OverPortrait(int rx, int ry);
		ZMiniMap &GetMiniMap();
		HubButton &GetButton(int button_num);
		ZObject *GetSelectedObject();
		void DeleteObject(ZObject *obj);
		void ResetGame();
		void SetMaxUnits(int max_units_);
		void SetUnitAmount(int unit_amount_);
		void SetTeam(int team_);
		void ShowChatMessage(bool show_chat_);
		void SetChatMessage(string message);
		void StartEndAnimations(vector<zhud_end_unit> &end_animations_, bool won);

		void SetSelectedObject(ZObject *selection);
		void GiveSelectedCommand(bool no_way = false);
		void SetARefID(int a_ref_id_);
		int GetARefID() { return a_ref_id; }
		ZPortrait &GetPortrait() { return portrait; }
		ZPortrait &GetAPortrait() { return aportrait; }
	private:
		void ProcessA(double the_time, vector<ZObject*> &object_list);
		void RenderBackdrop(SDL_Surface *dest, int off_x, int off_y);
		void RenderTime(SDL_Surface *dest, int off_x, int off_y);
		void RenderHealth(SDL_Surface *dest, int off_x, int off_y);
		void RenderUnitAmountBar(SDL_Surface *dest, int off_x, int off_y);
		void RenderChatMessage(SDL_Surface *dest, int off_x, int off_y);
		HubButton button[MAX_HUD_BUTTONS];
		
		planet_type terrain;
		int team;
		bool rerender_main;
		bool rerender_time;
		bool rerender_icon;
		bool rerender_backdrop;
		int rerender_button;
		bool rerender_health;
		bool rerender_unit_amount;
		bool rerender_chat;

		bool show_chat;
		string chat_message;
		ZSDL_Surface chat_message_img;
		int chat_start_x, chat_end_x;

		int max_units;
		int unit_amount;
		ZSDL_Surface unit_amount_text;
		
		ZObject *selected_object;
		ZPortrait portrait;
		ZPortrait aportrait;
		ZMiniMap zminimap;
		ZTime *ztime;
		int a_ref_id;

		double next_a_check_time;
		double next_a_flash_time;
		double next_a_anim_time;
		int last_a_anim;
		int a_times_not_under_attack;

		bool do_end_animations;
		bool do_end_animations_won;
		double next_end_anim_time;
		vector<zhud_end_unit> end_animations;

		ZSDL_Surface *render_icon;
		ZSDL_Surface *render_label;
		ZSDL_Surface *render_unit_label;
		//ZSDL_Surface *render_backdrop;

		//SDL_Surface *main_hud;
		ZSDL_Surface main_hud_side[MAX_TEAM_TYPES];
		ZSDL_Surface main_hud_bottom;
		ZSDL_Surface main_hud_bottom_left;
		ZSDL_Surface main_hud_bottom_center;
		ZSDL_Surface main_hud_bottom_right;
		ZSDL_Surface bottom_filler;
		ZSDL_Surface side_filler;
		ZSDL_Surface health_full_img;
		ZSDL_Surface health_empty_img;
		ZSDL_Surface health_lost_img;
		//ZSDL_Surface backdrop[MAX_PLANET_TYPES];
		//ZSDL_Surface backdrop_vehicle;
		ZSDL_Surface robot_icon[MAX_ROBOT_TYPES][MAX_TEAM_TYPES];
		ZSDL_Surface cannon_icon[MAX_CANNON_TYPES][MAX_TEAM_TYPES];
		ZSDL_Surface vehicle_icon[MAX_VEHICLE_TYPES][MAX_TEAM_TYPES];
		ZSDL_Surface robot_label[MAX_ROBOT_TYPES];
		ZSDL_Surface cannon_label[MAX_CANNON_TYPES];
		ZSDL_Surface vehicle_label[MAX_VEHICLE_TYPES];
		ZSDL_Surface robot_unit_label[MAX_ROBOT_TYPES][MAX_TEAM_TYPES];
		ZSDL_Surface unit_amount_bar[MAX_TEAM_TYPES];
		ZSDL_Surface grenade_img[MAX_TEAM_TYPES];

		int icon_shift_y;

};

#endif
