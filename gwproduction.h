#ifndef _ZGWPRODUCTION_H_
#define _ZGWPRODUCTION_H_

#include "zgui_window.h"
#include "zbuilding.h"

enum gwprod_type
{
	GWPROD_ROBOT, GWPROD_VEHICLE, GWPROD_FORT, GWPROD_TYPES_MAX
};

//enum gwprod_state
//{
//	GWPROD_PLACE, GWPROD_SELECT, GWPROD_BUILDING, GWPROD_PAUSED, MAX_GWPROD_STATES
//};

class ZObject;
class ZBuilding;

class GWPFullUnitSelectorButton
{
public:
	GWPFullUnitSelectorButton() { ot=oid=0; object_button.SetType(OBJECT_BUTTON); }

//private:
	ZGuiButton object_button;
	unsigned char ot, oid;
};

class GWPFullUnitSelector
{
public:
	GWPFullUnitSelector();
	~GWPFullUnitSelector();

	static void Init();

	void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
	void Process();

	bool Click(int x_, int y_);
	bool UnClick(int x_, int y_);

	void SetZTime(ZTime *ztime_) { ztime = ztime_; }
	void SetMap(ZMap *zmap_) { zmap = zmap_; }
	void SetBuildList(ZBuildList *buildlist_) { buildlist = buildlist_; }
	void SetBuildingObj(ZBuilding *building_obj_) { building_obj = building_obj_; }
	void SetBuildingType(int building_type_) { building_type = building_type_; }
	void SetCoords(int x_, int y_) { x=x_; y=y_; }
	void SetCenterCoords(int cx_, int cy_) { cx=cx_; cy=cy_; }
	void SetUnitSelectorRefID(int us_ref_id_) { us_ref_id = us_ref_id_; }
	void SetActive(bool is_active_) { is_active = is_active_; }
	void ClearSelected() { sot = 0; soid = 0; object_selected = false; }
	bool GetSelected(unsigned char &ot, unsigned char &oid) { ot=sot; oid=soid; return object_selected; }
	int GetUnitSelectorRefID() { return us_ref_id; }
	bool IsActive() { return is_active; }

private:
	static bool finished_init;
	static ZSDL_Surface fus_top_left_img;
	static ZSDL_Surface fus_top_right_img;
	static ZSDL_Surface fus_bottom_left_img;
	static ZSDL_Surface fus_bottom_right_img;
	static ZSDL_Surface fus_top_img;
	static ZSDL_Surface fus_bottom_img;
	static ZSDL_Surface fus_left_img;
	static ZSDL_Surface fus_right_img;
	static ZSDL_Surface object_back_img;

	void ClearLists();
	void LoadLists();
	void LoadUnitToLists(unsigned char ot, unsigned char oid);
	void DoRenderList(ZMap &the_map, SDL_Surface *dest, int lx, int ly, vector<ZObject*> &the_list);
	void LoadButtonList();
	void AppendButtonList(int lx, int ly, vector<ZObject*> &the_list);
	void CalculateWH();
	void CalculateXY();

	vector<ZObject*> robot_list;
	vector<ZObject*> vehicle_list;
	vector<ZObject*> cannon_list;
	vector<GWPFullUnitSelectorButton> button_list;
	int lists_building_type;
	int lists_building_level;

	ZTime *ztime;
	ZMap *zmap;
	ZBuildList *buildlist;
	ZBuilding *building_obj;

	int cx,cy;
	int x,y;
	int w,h;
	int building_type;
	int us_ref_id;
	bool is_active;

	bool object_selected;
	unsigned char sot, soid;
};

#define GWP_SELECTOR_CENTER_X (2 + (45/2))
#define GWP_SELECTOR_CENTER_Y (2 + (39/2))

class GWPUnitSelector
{
public:
	GWPUnitSelector();
	~GWPUnitSelector();

	static void Init();

	void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
	void Process();

	bool Click(int x_, int y_);
	bool UnClick(int x_, int y_);
	bool WheelUpButton();
	bool WheelDownButton();

	void SetZTime(ZTime *ztime_) { ztime = ztime_; }
	void SetCoords(int x_, int y_) { x=x_; y=y_; }
	void GetCoords(int &x_, int &y_) { x_=x; y_=y; }
	void SetActive(bool is_active_) { is_active = is_active_; }
	void SetBuildingObj(ZBuilding *building_obj_) { building_obj = building_obj_; }
	void SetBuildList(ZBuildList *buildlist_) { buildlist = buildlist_; }
	void SetBuildingType(int building_type_) { building_type = building_type_; }
	void SetIsOnlySelector(bool is_only_selector_) { is_only_selector = is_only_selector_; }
	bool GetSelectedID(unsigned char &ot, unsigned char &oid);
	bool LoadFullSelector() { return load_fus; }
	int GetRefID() { return ref_id; }
	void SetSelection(unsigned char ot, unsigned char oid);
private:
	static bool finished_init;
	static ZSDL_Surface selector_back_img;
	static ZSDL_Surface p_bar;
	static ZSDL_Surface p_bar_yellow;

	static int next_ref_id;

	void DoUpButton();
	void DoDownButton();

	void DeleteDrawObject();
	void SetDrawObject();
	void ResetDrawObjectTo(unsigned char ot, unsigned char oid);
	void DrawPercentageBar(ZMap &the_map, SDL_Surface *dest, int tx, int ty);
	bool WithinPortrait(int x, int y);

	ZTime *ztime;
	ZMap *zmap;
	ZBuilding *building_obj;
	ZBuildList *buildlist;
	ZObject *draw_obj;

	ZGuiButton up_button;
	ZGuiButton down_button;

	int ref_id;
	int px, py;
	int x, y;
	int w, h;
	bool is_active;
	bool is_only_selector;
	bool load_fus;

	int build_state;
	int building_type;
	int select_i;
	double percentage_produced;
};

class GWPQueueItem
{
public:
	GWPQueueItem() { ot=oid=0; x_off=y_off=0; obj_name_button.SetType(OBJECT_NAME_BUTTON); }
	GWPQueueItem(int x_off_, int y_off_, unsigned char ot_, unsigned char oid_) 
	{ 
		ot=ot_;
		oid=oid_;
		obj_name_button.SetType(OBJECT_NAME_BUTTON);
		obj_name_button.SetOffsets(x_off_, y_off_);
		x_off=x_off_;
		y_off=y_off_;
	}

	ZGuiButton obj_name_button;
	unsigned char ot, oid;
	int x_off, y_off;
};

class GWProduction : public ZGuiWindow
{
	public:
		GWProduction(ZTime *ztime_);

		static void Init();

		void Process();
		void DoRender(ZMap &the_map, SDL_Surface *dest);

		void SetType(int type_);
		void SetRefID(int ref_id_) { ref_id = ref_id_; }
		void SetBuildingObj(ZBuilding *building_obj_);
		int GetRefID() { return ref_id; }
		void SetCords(int center_x, int center_y);
		bool Click(int x_, int y_);
		bool UnClick(int x_, int y_);
		bool WheelUpButton();
		bool WheelDownButton();
		ZObject *GetBuildingObj() { return building_obj; }
		void SetState(int state_);
		void ToggleExpanded() { is_expanded = !is_expanded; ProcessSetExpanded(); }
		void SetIsExpanded(bool is_expanded_) { is_expanded = is_expanded_; ProcessSetExpanded(); }
		void SetBuildList(ZBuildList *buildlist_);
	protected:
		//void DrawPercentageBar(ZMap &the_map, SDL_Surface *dest);
		//void DrawObject(ZMap &the_map, SDL_Surface *dest);
		//void SetDrawObject();
		//void ResetDrawObjectTo(unsigned char &ot, unsigned char &oid);
		//void DeleteDrawObject();
		void ResetShowTime(int new_time);
		void RecalcShowTime();
		void ProcessSetExpanded();
		void CheckQueueButtonList();
		void MakeQueueButtonList();
		void RenderQueueButtonList(ZMap &the_map, SDL_Surface *dest);

		//void DoUpButton();
		//void DoDownButton();
		void DoOkButton();
		void DoCancelButton();
		void DoPlaceButton();
		void DoPlusButton() { SetIsExpanded(true); }
		void DoMinusButton() { SetIsExpanded(false); }
		void DoQueueButton();
		void DoCancelQueueItem(int i);
		void LoadFullSelector(int us_ref_id);

		static ZSDL_Surface base_img;
		static ZSDL_Surface base_expanded_img;
		static ZSDL_Surface name_label[GWPROD_TYPES_MAX];
		static ZSDL_Surface state_label[MAX_BUILDING_STATES][2];
		static ZSDL_Surface p_bar;
		static ZSDL_Surface p_bar_yellow;

		//ZGuiButton up_button;
		//ZGuiButton down_button;
		ZGuiButton ok_button;
		ZGuiButton cancel_button;
		ZGuiButton place_button;
		ZGuiButton small_plus_button;
		ZGuiButton small_minus_button;
		ZGuiButton queue_button;

		GWPUnitSelector unit_selector;
		GWPUnitSelector queue_selector;
		GWPFullUnitSelector full_selector;

		int is_expanded;

		int type;
		int building_type;
		int ref_id;
		int state;
		//double percentage_produced;
		bool rerender_show_time;
		ZBuilding *building_obj;
		//ZObject *draw_obj;
		vector<GWPQueueItem> queue_button_list;

		int show_time;
		ZSDL_Surface show_time_img;

		int health_percent;
		ZSDL_Surface health_percent_img;

		int state_i;
		double next_state_time;

		//int select_i;
};

#endif
