#ifndef _ZOLISTS_H_
#define _ZOLISTS_H_

#include <vector>

using namespace std;

class ZObject;

class ZOLists
{
public:
	ZOLists();

	void Init(vector<ZObject*> *object_list_) { object_list = object_list_; }

	void AddObject(ZObject *obj);
	void DeleteObject(ZObject *obj);
	void RemoveObject(ZObject *obj);
	static void DeleteObjectFromList(ZObject *obj, vector<ZObject*> *olist);
	static void RemoveObjectFromList(ZObject *obj, vector<ZObject*> *olist);
	void DeleteAllObjects();

	void SetupFlagList();

	vector<ZObject*> *object_list;
	vector<ZObject*> flag_olist;
	vector<ZObject*> cannon_olist;
	vector<ZObject*> building_olist;
	vector<ZObject*> rock_olist;
	vector<ZObject*> passive_engagable_olist;
	vector<ZObject*> mobile_olist;
	vector<ZObject*> prender_olist;
	vector<ZObject*> non_mapitem_olist;
	vector<ZObject*> grenades_olist;
};

#endif
