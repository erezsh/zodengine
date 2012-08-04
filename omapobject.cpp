#include "omapobject.h"

ZSDL_Surface OMapObject::render_img[MAP_ITEMS_AMOUNT];

OMapObject::OMapObject(ZTime *ztime_, ZSettings *zsettings_, int object_i_) : ZObject(ztime_, zsettings_)
{
	SetType(object_i_);

	object_type = MAP_ITEM_OBJECT;
	width = 1;
	height = 1;
	width_pix = width * 16;
	height_pix = height * 16;
	attacked_by_explosives = true;

	InitTypeId(object_type, object_id);

	//max_health = MAP_OBJECT_MAX_HEALTH;
	//health = max_health;
}

void OMapObject::SetType(int object_i_)
{
	object_i = object_i_ - MAP0_ITEM;

	if(object_i < 0) object_i = 0;
	else if(object_i >= MAP_ITEMS_AMOUNT) object_i = MAP_ITEMS_AMOUNT-1;

	char message[500];

	sprintf(message, "map_object%d", object_i);

	object_name = message;
	object_id = MAP0_ITEM + object_i;
}
		
void OMapObject::Init()
{
	char filename_c[500];
	int i;

	for(i=0;i<MAP_ITEMS_AMOUNT;i++)
	{
		sprintf(filename_c, "assets/other/map_items/map_object%d.png", i);
		render_img[i].LoadBaseImage(filename_c);// = ZSDL_IMG_Load(filename_c);
	}
}

void OMapObject::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;

	if(!render_img[object_i].GetBaseSurface()) return;

	if(the_map.GetBlitInfo( render_img[object_i].GetBaseSurface(), x, y + (16 - render_img[object_i].GetBaseSurface()->h), from_rect, to_rect))
	{
		to_rect.x += shift_x;
		to_rect.y += shift_y;

		render_img[object_i].BlitSurface(&from_rect, &to_rect);
		//SDL_BlitSurface( render_img[object_i], &from_rect, dest, &to_rect);
	}
}

int OMapObject::Process()
{
	return 0;
}

bool OMapObject::CausesImpassAtCoord(int x, int y)
{
	return x == loc.x && y == loc.y;
}

void OMapObject::SetMapImpassables(ZMap &tmap)
{
	int tx, ty;

	tx = loc.x / 16;
	ty = loc.y / 16;

	tmap.SetImpassable(tx, ty, true, true);
}

void OMapObject::UnSetMapImpassables(ZMap &tmap)
{
	int tx, ty;

	tx = loc.x / 16;
	ty = loc.y / 16;

	tmap.SetImpassable(tx, ty, false, true);
}

//bool OMapObject::ServerFireTurrentMissile(int &x_, int &y_, int &damage, int &radius, double &offset_time)
//{
//	x_ = (loc.x + 16) + (130 - (rand() % 260));
//	y_ = (loc.y + 16) + (130 - (rand() % 260));
//	damage = MAP_OBJECT_DAMAGE;
//	radius = 40;
//	offset_time = 7 + (0.01 * (rand() % 300));
//	return true;
//}

vector<fire_missile_info> OMapObject::ServerFireTurrentMissile(int &damage, int &radius)
{
	vector<fire_missile_info> ret;
	fire_missile_info a_missile;
	int &max_horz = zsettings->max_turrent_horizontal_distance;
	int &max_vert = zsettings->max_turrent_vertical_distance;

	a_missile.missile_offset_time = 3 + (0.01 * (rand() % 100));
	a_missile.missile_x = (loc.x + 16) + (max_horz - (rand() % (max_horz + max_horz)));
	a_missile.missile_y = (loc.y + 16) + (max_vert - (rand() % (max_vert + max_vert)));

	//damage = MAP_OBJECT_DAMAGE;
	damage = zsettings->map_item_turrent_damage * MAX_UNIT_HEALTH;
	radius = 40;

	ret.push_back(a_missile);

	return ret;
}

void OMapObject::FireTurrentMissile(int x_, int y_, double offset_time)
{
	if(!render_img[object_i].GetBaseSurface()) return;

	if(effect_list) effect_list->push_back((ZEffect*)(new EMapObjectTurrent(ztime, loc.x, loc.y, x_, y_, offset_time, object_i)));	
}

void OMapObject::DoDeathEffect(bool do_fire_death, bool do_missile_death)
{
	int &x = loc.x;
	int &y = loc.y;
	int sparks_amt;
	int spark_x, spark_y;
	int i;
	int particles;

	//particles
	particles = 10 + (rand() % 8);
	if(effect_list) 
	for(int i=0;i<particles;i++)
		effect_list->push_back((ZEffect*)(new EUnitParticle(ztime, x, y, 65, 55)));

	//sparks
	{
		sparks_amt = 20 + (rand() % 20);

		spark_x = x + 16;
		spark_y = y + 16;

		if(effect_list) 
		for(i=0;i<sparks_amt;i++)
			effect_list->push_back((ZEffect*)(new EDeathSparks(ztime, spark_x, spark_y)));
	}

	//effect_list->insert(effect_list->begin(), (ZEffect*)(new EDeath(loc.x, loc.y, EDEATH_TANK, base_damaged[owner][direction][move_i])));
	//effect_list->push_back((ZEffect*)(new ERobotDeath(loc.x, loc.y, owner)));
}

void OMapObject::SetOwner(team_type owner_)
{
	//do nothing, should always be the NULL_TEAM
}
