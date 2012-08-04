#ifndef _BBRIDGE_H_
#define _BBRIDGE_H_

#include "zbuilding.h"

class BBridge : public ZBuilding
{
public:
	BBridge(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT, bool is_vertical_ = true, unsigned short extra_links_ = 0);
	~BBridge();
		
	static void Init();
	void ChangePalette(planet_type palette_);
	void SetIsVertical(bool is_vertical_);
	void SetExtraLinks(unsigned short extra_links_);
	unsigned short GetExtraLinks();
	void SetOwner(team_type owner_);

	void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
	int Process();
	void DoDeathEffect(bool do_fire_death, bool do_missile_death);
	void DoReviveEffect();
	void DoTurrentEffect(bool is_reversed = false);
	void SetMapImpassables(ZMap &tmap);
	void SetDestroyMapImpassables(ZMap &tmap);
	void UnSetDestroyMapImpassables(ZMap &tmap);
	bool UnderCursorCanAttack(int &map_x, int &map_y);

	bool GetCraneEntrance(int &x, int &y, int &x2, int &y2);
	bool GetCraneCenter(int &x, int &y);
private:
	void ResetStats();
	void UnRenderImages();
	void ReRenderImages();
	void IndividualReRender(ZSDL_Surface &surface);
	void DoExplosions();
	void ImpassCenter(ZMap &tmap, bool impassable);

	static ZSDL_Surface planet_template[MAX_PLANET_TYPES];
	ZSDL_Surface render_img;
	ZSDL_Surface render_damaged_img;
	ZSDL_Surface render_destroyed_img;

	bool is_vertical;
	unsigned short extra_links;
	bool do_rerender;

	int last_process_health;

	bool do_revive_rerender;
	double next_revive_rerender_time;
};

#endif
