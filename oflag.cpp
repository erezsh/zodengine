#include "oflag.h"

ZSDL_Surface OFlag::flag_img[MAX_TEAM_TYPES][4];

OFlag::OFlag(ZTime *ztime_, ZSettings *zsettings_) : ZObject(ztime_, zsettings_)
{
	width = 1;
	height = 1;
	width_pix = width * 16;
	height_pix = height * 16;
	object_name = "flag";
	object_type = MAP_ITEM_OBJECT;
	object_id = FLAG_ITEM;
	can_be_destroyed = false;
	
	connected_zone = NULL;
	
	flag_i = rand() % 4;
}

void OFlag::Init()
{
	int i, j;
	char filename_c[500];
	
	//load colors
	for(i=0;i<MAX_TEAM_TYPES;i++)
		for(j=0;j<4;j++)
	{
		sprintf(filename_c, "assets/other/flag_%s_%d.png", team_type_string[i].c_str(), j);
		//flag_img[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, flag_img[ZTEAM_BASE_TEAM][j], flag_img[i][j], filename_c);
	}
}

int OFlag::Process()
{
	double &the_time = ztime->ztime;
	const double int_time = 0.2;
	
	if(the_time - last_process_time >= int_time)
	{
		last_process_time = the_time;
		
		flag_i++;
		if(flag_i > 3) flag_i = 0;
	}

	return 1;
}

void OFlag::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	ZSDL_Surface *base_surface;
	SDL_Rect from_rect, to_rect;
//	int lx, ly;
	
	base_surface = &flag_img[owner][flag_i];
	
	if(the_map.GetBlitInfo(base_surface->GetBaseSurface(), x, y, from_rect, to_rect))
	{
		to_rect.x += shift_x;
		to_rect.y += shift_y;

		base_surface->BlitSurface(&from_rect, &to_rect);
		//SDL_BlitSurface( base_surface, &from_rect, dest, &to_rect);
	}
}

void OFlag::SetZone(map_zone_info *the_zone, ZMap &zmap, vector<ZObject*> &oobject_list)
{
   connected_zone = the_zone;
   
   the_zone->owner = owner;
   
   connected_object_list.clear();
   
   //connect to buildings
   //and set their initial owner
   for(vector<ZObject*>::iterator i=oobject_list.begin(); i!=oobject_list.end(); i++)
   {
      unsigned char object_type;
      unsigned char object_id;
      int ox, oy;
      
      (*i)->GetObjectID(object_type, object_id);
      (*i)->GetCords(ox, oy);
      
      //is this a fort, then just set the zone to the forts owner
      if(object_type == BUILDING_OBJECT)
         if(connected_zone == zmap.GetZone(ox,oy))
         {
            connected_object_list.push_back(*i);
            (*i)->SetOwner(owner);
         }
   }
}

vector<ZObject*> &OFlag::GetLinkedObjects()
{
	return connected_object_list;
}

map_zone_info *OFlag::GetLinkedZone()
{
	return connected_zone;
}

bool OFlag::HasRadar()
{
	for(vector<ZObject*>::iterator i=connected_object_list.begin(); i!=connected_object_list.end(); i++)
   {
      unsigned char ot, oid;
      
      (*i)->GetObjectID(ot, oid);

	  if(ot == BUILDING_OBJECT && oid == RADAR)
		  return true;
	}

	return false;
}

// SDL_Surface *OFlag::GetRender()
// {
// 	return flag_img[owner][flag_i];
// }
