#include "ogrenades.h"
#include "eturrentmissile.h"

ZSDL_Surface OGrenades::render_img;

OGrenades::OGrenades(ZTime *ztime_, ZSettings *zsettings_) : ZObject(ztime_, zsettings_)
{
	object_name = "grenades";
	object_type = MAP_ITEM_OBJECT;
	object_id = GRENADES_ITEM;
	width = 1;
	height = 1;
	width_pix = width * 16;
	height_pix = height * 16;
	attacked_by_explosives = true;

	InitTypeId(object_type, object_id);

	//max_health = GRENADES_MAX_HEALTH;
	//health = max_health;

	grenade_amount = zsettings->grenades_per_box;
}
		
void OGrenades::Init()
{
	render_img.LoadBaseImage("assets/other/map_items/grenades.png");// = ZSDL_IMG_Load("assets/other/map_items/grenades.png");
}

void OGrenades::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;

	if(the_map.GetBlitInfo( render_img.GetBaseSurface(), x, y, from_rect, to_rect))
	{
		to_rect.x += shift_x;
		to_rect.y += shift_y;

		render_img.BlitSurface(&from_rect, &to_rect);
		//SDL_BlitSurface( render_img, &from_rect, dest, &to_rect);
	}
}

int OGrenades::Process()
{
	return 0;
}

void OGrenades::FireTurrentMissile(int x_, int y_, double offset_time)
{
	if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, loc.x + 2, loc.y + 2, x_, y_, offset_time, ETURRENTMISSILE_GRENADE)));	
}

vector<fire_missile_info> OGrenades::ServerFireTurrentMissile(int &damage, int &radius)
{
	vector<fire_missile_info> ret;
	fire_missile_info a_missile;
	const int max_horz = 130;
	const int max_vert = 130;

	for(int i=0;i<grenade_amount;i++)
	{
		a_missile.missile_x = (loc.x + 16) + (max_horz - (rand() % (max_horz + max_horz)));
		a_missile.missile_y = (loc.y + 16) + (max_vert - (rand() % (max_vert + max_vert)));
		a_missile.missile_offset_time = 3 + (0.01 * (rand() % 100));

		//damage = GRENADE_DAMAGE;
		damage = zsettings->grenade_damage * MAX_UNIT_HEALTH;
		radius = 40;

		ret.push_back(a_missile);
	}

	return ret;
}

void OGrenades::SetOwner(team_type owner_)
{
	//do nothing
}
