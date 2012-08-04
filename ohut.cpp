#include "ohut.h"

ZSDL_Surface OHut::render_img[MAX_PLANET_TYPES];

OHut::OHut(ZTime *ztime_, ZSettings *zsettings_, int palette_) : ZObject(ztime_, zsettings_)
{
	palette = palette_;

	object_name = "hut";
	object_type = MAP_ITEM_OBJECT;
	object_id = HUT_ITEM;
	width = 1;
	height = 1;
	width_pix = width * 16;
	height_pix = height * 16;
	attacked_by_explosives = true;

	InitTypeId(object_type, object_id);

	//max_health = HUT_MAX_HEALTH;
	//health = max_health;

	next_hut_animal_time = 0;
	next_max_hut_animal_time = 0;

	SetMaxHutAnimals();
}

OHut::~OHut()
{
	//delete all hut animals
	for(vector<AHutAnimal*>::iterator i=hut_animals.begin(); i!=hut_animals.end(); ++i) delete *i;

	hut_animals.clear();
}
		
void OHut::Init()
{
	string filename;
	int i;

	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		filename = "assets/other/map_items/hut_" + planet_type_string[i] + ".png";
		render_img[i].LoadBaseImage(filename);// = ZSDL_IMG_Load(filename);
	}
}

void OHut::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int &x = loc.x;
	int &y = loc.y;
	SDL_Rect from_rect, to_rect;

	if(the_map.GetBlitInfo( render_img[palette].GetBaseSurface(), x, y, from_rect, to_rect))
	{
		to_rect.x += shift_x;
		to_rect.y += shift_y;

		render_img[palette].BlitSurface(&from_rect, &to_rect);
		//SDL_BlitSurface( render_img[palette], &from_rect, dest, &to_rect);
	}
}

void OHut::DoPreRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	//only for rendering the animals
	for(vector<AHutAnimal*>::iterator i=hut_animals.begin(); i!=hut_animals.end(); ++i)
		(*i)->DoRender(the_map, dest, shift_x, shift_y);
}

int OHut::Process()
{
	double &the_time = ztime->ztime;

	//only a player client will create objects with an effect list
	//and we only want this hut processing if it is a player with a visual
	if(!effect_list) return 0;

	//process animals
	{
		//kill some?
		for(vector<AHutAnimal*>::iterator i=hut_animals.begin(); i!=hut_animals.end();)
		{
			if((*i)->KillMe(the_time))
			{
				delete *i;
				i = hut_animals.erase(i);
			}
			else
				i++;
		}

		//process
		for(vector<AHutAnimal*>::iterator i=hut_animals.begin(); i!=hut_animals.end();i++)
			(*i)->Process();
	}

	//make or run home animals?
	if(the_time >= next_hut_animal_time)
	{
		int displacement_amount;
		int rand_amount;

		next_hut_animal_time = the_time + 1.0;

		displacement_amount = max_hut_animals - hut_animals.size();

		//need to add some?
		if(displacement_amount > 0)
		{
			rand_amount = rand() % (displacement_amount+1);

			CreateAnimals(rand_amount);
		}
		else if(displacement_amount < 0)
		{
			//send some home?
			rand_amount = rand() % ((displacement_amount*-1)+1);

			SendAnimalsHome(rand_amount);
		}
	}

	//change max animals we manage
	if(the_time >= next_max_hut_animal_time)
	{
		//next_max_hut_animal_time = the_time + 45.0 + (rand() % 45);
		next_max_hut_animal_time = the_time + 10;

		SetMaxHutAnimals();
	}

	return 0;
}

void OHut::SendAnimalsHome(int amount)
{
	int amount_going_home = 0;

	//find amount already going
	for(vector<AHutAnimal*>::iterator i=hut_animals.begin(); i!=hut_animals.end(); ++i)
		if((*i)->IsGoingHome())
			amount_going_home++;

	amount -= amount_going_home;

	if(amount <= 0) return;

	//send some home
	for(vector<AHutAnimal*>::iterator i=hut_animals.begin(); i!=hut_animals.end(); ++i)
		if(!(*i)->IsGoingHome())
		{
			(*i)->GoHome();

			amount--;
			if(amount <= 0) return;
		}
}

bool OHut::GetExitToTile(int &tx, int &ty)
{
	vector<xy_struct> possible_list;
	int itx, ity;
	int ctx, cty;
	int rand_choice;

	ctx = loc.x>>4;
	cty = loc.y>>4;

	tx = ctx;
	ty = cty + 1;

	if(!zmap) return true;

	//test
	if(zmap->GetPathFinder().TilePassable(tx, ty, false)) return true;

	for(itx=ctx-1;itx<=ctx+1;itx++)
		for(ity=cty-1;ity<=cty+1;ity++)
		{	
			if(ity==cty && itx==ctx) continue;

			if(!zmap->GetPathFinder().TilePassable(itx, ity, false)) continue;

			xy_struct new_xy;
			new_xy.x = itx;
			new_xy.y = ity;
			possible_list.push_back(new_xy);
		}

	if(possible_list.size())
	{
		rand_choice = rand() % possible_list.size();
		tx = possible_list[rand_choice].x;
		ty = possible_list[rand_choice].y;
		return true;
	}

	return false;
}

void OHut::CreateAnimals(int amount)
{
	for(int i=0;i<amount;i++)
	{
		int tx, ty;

		//create only if there is a place to move to
		if(GetExitToTile(tx, ty))
		{
			AHutAnimal *new_animal;

			new_animal = new AHutAnimal(ztime, zsettings, palette);

			new_animal->SetMap(zmap);
			new_animal->SetEffectList(effect_list);
			new_animal->SetHomeCoords(center_x, center_y);
			new_animal->SetCords(center_x, center_y);
			new_animal->GotoTile(tx, ty);

			hut_animals.push_back(new_animal);
		}
	}
}

void OHut::SetMaxHutAnimals()
{
	int rand_diff;

	rand_diff = zsettings->hut_animal_max - zsettings->hut_animal_min;

	max_hut_animals = zsettings->hut_animal_min;
	if(rand_diff > 0) max_hut_animals += rand() % rand_diff;
}

void OHut::ChangePalette(int palette_)
{
	palette = palette_;
}

bool OHut::CausesImpassAtCoord(int x, int y)
{
	return x == loc.x && y == loc.y;
}

void OHut::SetMapImpassables(ZMap &tmap)
{
	int tx, ty;

	tx = loc.x / 16;
	ty = loc.y / 16;

	tmap.SetImpassable(tx, ty, true, true);
}

void OHut::UnSetMapImpassables(ZMap &tmap)
{
	int tx, ty;

	tx = loc.x / 16;
	ty = loc.y / 16;

	tmap.SetImpassable(tx, ty, false, true);
}

void OHut::SetOwner(team_type owner_)
{
	//do nothing
}
