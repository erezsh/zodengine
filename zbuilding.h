#ifndef _ZBUILDING_H_
#define _ZBUILDING_H_

#include <algorithm>
#include "zobject.h"
#include "estandard.h"

#define MAX_QUEUE_ITEMS 5

enum building_state
{
	BUILDING_PLACE, BUILDING_SELECT, BUILDING_BUILDING, BUILDING_PAUSED, MAX_BUILDING_STATES
};

//used in transfer so make uniform makeup / size
#pragma pack(1)

class ZBProductionUnit
{
public:
	ZBProductionUnit() { ot=oid=0; }
	ZBProductionUnit(unsigned char ot_, unsigned char oid_) { ot=ot_; oid=oid_; }

	unsigned char ot, oid;
};

//undo the packet standardize command
#pragma pack()

class ZBuilding : public ZObject
{
	public:
		ZBuilding(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT);
		~ZBuilding();
		
		static void Init();
		virtual void ChangePalette(planet_type palette_);
		void ReRenderBase();
		int GetBuildState();
		int GetLevel();
		void SetLevel(int level_);
		virtual bool SetBuildingDefaultProduction();
		virtual bool SetBuildingProduction(unsigned char ot, unsigned char oid);
		virtual bool AddBuildingQueue(unsigned char ot, unsigned char oid, bool push_to_front = true);
		virtual bool CancelBuildingQueue(int list_i, unsigned char ot, unsigned char oid);
		virtual void CreateBuildingStateData(char *&data, int &size);
		virtual void ProcessSetBuildingStateData(char *data, int size);
		virtual bool StopBuildingProduction(bool clear_queue_list = true);
		double PercentageProduced(double &the_time);
		double ProductionTimeLeft(double &the_time);
		virtual bool GetBuildingCreationPoint(int &x, int &y);
		virtual bool GetBuildingCreationMovePoint(int &x, int &y);
		virtual bool BuildUnit(double &the_time, unsigned char &ot, unsigned char &oid);
		virtual bool StoreBuiltCannon(unsigned char oid);
		virtual void ResetProduction();
		bool GetBuildUnit(unsigned char &ot, unsigned char &oid);
		virtual void CreateBuiltCannonData(char *&data, int &size);
		virtual void ProcessSetBuiltCannonData(char *data, int size);
		virtual bool RemoveStoredCannon(unsigned char oid);
		virtual bool HaveStoredCannon(unsigned char oid);
		virtual void SetOwner(team_type owner_);
		vector<unsigned char> &GetBuiltCannonList();
		virtual void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		virtual void DoReviveEffect();
		int CannonsInZone(ZOLists &ols);
		virtual void ProcessBuildingsEffects(double &the_time);
		virtual bool ResetBuildTime(float zone_ownage);
		virtual void SetZoneOwnage(float zone_ownage_) { zone_ownage = zone_ownage_; }
		virtual bool RecalcBuildTime();
		double BuildTimeModified(double base_build_time);
		void CreateBuildingQueueData(char *&data, int &size);
		void ProcessBuildingQueueData(char *data, int size);
		vector<ZBProductionUnit> &GetQueueList() { return queue_list; }
	protected:
		void ResetShowTime(int new_time);

		planet_type palette;
		int level;
		bool do_base_rerender;
		int build_state;
		unsigned char bot, boid;
		double binit_time, bfinal_time;
		int unit_create_x, unit_create_y;
		int unit_move_x, unit_move_y;
		vector<unsigned char> built_cannon_list;
		float zone_ownage;
		vector<ZBProductionUnit> queue_list;

		int show_time;
		ZSDL_Surface show_time_img;

		SDL_Rect effects_box;
		int max_effects;
		vector<EStandard*> extra_effects;
		
		static ZSDL_Surface level_img[MAX_BUILDING_LEVELS];
		static ZSDL_Surface exhaust[13];
		static ZSDL_Surface little_exhaust[4];
	private:
};

#endif
