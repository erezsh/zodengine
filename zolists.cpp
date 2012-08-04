#include "zolists.h"

#include "zmap.h"
#include "constants.h"
#include "zobject.h"

ZOLists::ZOLists()
{
	object_list = NULL;
}

void ZOLists::DeleteAllObjects()
{
	for(vector<ZObject*>::iterator o=object_list->begin(); o!=object_list->end(); o++)
		delete *o;

	//clear the lists
	object_list->clear();
	flag_olist.clear();
	cannon_olist.clear();
	building_olist.clear();
	rock_olist.clear();
	passive_engagable_olist.clear();
	mobile_olist.clear();
	prender_olist.clear();
	non_mapitem_olist.clear();
	grenades_olist.clear();
}

void ZOLists::RemoveObject(ZObject *obj)
{
	RemoveObjectFromList(obj, object_list);
	RemoveObjectFromList(obj, &flag_olist);
	RemoveObjectFromList(obj, &cannon_olist);
	RemoveObjectFromList(obj, &building_olist);
	RemoveObjectFromList(obj, &rock_olist);
	RemoveObjectFromList(obj, &passive_engagable_olist);
	RemoveObjectFromList(obj, &mobile_olist);
	RemoveObjectFromList(obj, &prender_olist);
	RemoveObjectFromList(obj, &non_mapitem_olist);
	RemoveObjectFromList(obj, &grenades_olist);
}

void ZOLists::DeleteObject(ZObject *obj)
{
	delete obj;

	RemoveObject(obj);
}

void ZOLists::AddObject(ZObject *obj)
{
	//already exist?
	for(vector<ZObject*>::iterator o=object_list->begin(); o!=object_list->end(); o++)
		if(*o == obj)
			return;

	//push back
	object_list->push_back(obj);

	unsigned char ot, oid;

	obj->GetObjectID(ot, oid);

	if(ot == CANNON_OBJECT) cannon_olist.push_back(obj);
	if(ot == BUILDING_OBJECT) building_olist.push_back(obj);
	if(ot == MAP_ITEM_OBJECT && oid == ROCK_OBJECT) rock_olist.push_back(obj);
	if(ot == CANNON_OBJECT) cannon_olist.push_back(obj);
	if(ot == CANNON_OBJECT || ot == ROBOT_OBJECT || ot == VEHICLE_OBJECT) passive_engagable_olist.push_back(obj);
	if(ot == ROBOT_OBJECT || ot == VEHICLE_OBJECT) mobile_olist.push_back(obj);
	prender_olist.push_back(obj);
	if(ot != MAP_ITEM_OBJECT) non_mapitem_olist.push_back(obj);
	if(ot == MAP_ITEM_OBJECT && oid == FLAG_ITEM) flag_olist.push_back(obj);
	if(ot == MAP_ITEM_OBJECT && oid == GRENADES_ITEM) grenades_olist.push_back(obj);
}

void ZOLists::DeleteObjectFromList(ZObject *obj, vector<ZObject*> *olist)
{
	delete obj;

	RemoveObjectFromList(obj, olist);
}

void ZOLists::RemoveObjectFromList(ZObject *obj, vector<ZObject*> *olist)
{
	for(vector<ZObject*>::iterator o=olist->begin(); o!=olist->end();)
	{
		if(*o == obj)
			o = olist->erase(o);
		else
			o++;
	}
}

void ZOLists::SetupFlagList()
{
	flag_olist.clear();

	for(vector<ZObject*>::iterator i=object_list->begin(); i!=object_list->end(); i++)
	{
		unsigned char ot, oid;
		(*i)->GetObjectID(ot, oid);

		if(ot == MAP_ITEM_OBJECT && oid == FLAG_ITEM)
			flag_olist.push_back(*i);
	}
}
