#ifndef _ZPORTRAIT_H_
#define _ZPORTRAIT_H_

#include "zsdl.h"
#include "zmap.h"
#include "constants.h"
#include "zsound_engine.h"
#include "zobject.h"

class ZObject;

#include <vector>

using namespace std;

#define ZPORTRAIT_BASE_WIDTH 86
#define ZPORTRAIT_BASE_HEIGHT 74

#define MAX_EYES 11
#define MAX_HANDS 9
#define MAX_MOUTHS 16

enum look_direction
{
	LOOK_STRAIGHT, LOOK_RIGHT, LOOK_LEFT, MAX_LOOK_DIRECTIONS
};

enum portrait_anim
{
	YES_SIR_ANIM, YES_SIR3_ANIM, UNIT_REPORTING1_ANIM,
	UNIT_REPORTING2_ANIM, GRUNTS_REPORTING_ANIM,
	PSYCHOS_REPORTING_ANIM, SNIPERS_REPORTING_ANIM,
	TOUGHS_REPORTING_ANIM, LASERS_REPORTING_ANIM,
	PYROS_REPORTING_ANIM, WERE_ON_OUR_WAY_ANIM,
	HERE_WE_GO_ANIM, YOUVE_GOT_IT_ANIM, MOVING_IN_ANIM,
	OKAY_ANIM, ALRIGHT_ANIM, NO_PROBLEM_ANIM, OVER_N_OUT_ANIM,
	AFFIRMATIVE_ANIM, GOING_IN_ANIM, LETS_DO_IT_ANIM,
	LETS_GET_EM_ANIM, WERE_UNDER_ATTACK_ANIM,
	I_SAID_WERE_UNDER_ATTACK_ANIM, HELP_HELP_ANIM,
	THEYRE_ALL_OVER_US_ANIM, WERE_LOSEING_IT_ANIM,
	AAAHHH_ANIM, FOR_CHRIST_SAKE_ANIM,
	YOURE_JOKING_ANIM, TARGET_DESTROYED_ANIM, BLINK_ANIM,
	WINK_ANIM, SURPRISE_ANIM, ANGER_ANIM, GRIN_ANIM,
	SCARED_ANIM, EYES_LEFT_ANIM, EYES_RIGHT_ANIM,
	EYES_UP_ANIM, EYES_DOWN_ANIM, WHISTLE_ANIM,
	LOOK_LEFT_ANIM, LOOK_RIGHT_ANIM, SALUTE_ANIM,
	THUMBS_UP_ANIM, YES_SIR_SALUTE_ANIM, GOING_IN_THUMBS_UP_ANIM,
	FORGET_IT_ANIM, GET_OUTTA_HERE_ANIM, NO_WAY_ANIM, 
	GOOD_HIT_ANIM, NICE_ONE_ANIM, OH_YEAH_ANIM, GOTCHA_ANIM,
	SMOKIN_ANIM, COOL_ANIM, WIPE_OUT_ANIM, TERRITORY_TAKEN_ANIM,
	FIRE_EXTINGUISHED_ANIM, GUN_CAPTURED_ANIM, VEHICLE_CAPTURED_ANIM,
	GRENADES_COLLECTED_ANIM, ENDW1_ANIM, ENDW2_ANIM, ENDW3_ANIM,
	ENDL1_ANIM, ENDL2_ANIM, ENDL3_ANIM,
	MAX_PORTRAIT_ANIMS
};

const string portrait_anim_string[MAX_PORTRAIT_ANIMS] = 
{
	"YES_SIR_ANIM", "YES_SIR3_ANIM", "UNIT_REPORTING1_ANIM",
	"UNIT_REPORTING2_ANIM", "GRUNTS_REPORTING_ANIM",
	"PSYCHOS_REPORTING_ANIM", "SNIPERS_REPORTING_ANIM",
	"TOUGHS_REPORTING_ANIM", "LASERS_REPORTING_ANIM",
	"PYROS_REPORTING_ANIM", "WERE_ON_OUR_WAY_ANIM",
	"HERE_WE_GO_ANIM", "YOUVE_GOT_IT_ANIM", "MOVING_IN_ANIM",
	"OKAY_ANIM", "ALRIGHT_ANIM", "NO_PROBLEM_ANIM", "OVER_N_OUT_ANIM",
	"AFFIRMATIVE_ANIM", "GOING_IN_ANIM", "LETS_DO_IT_ANIM",
	"LETS_GET_EM_ANIM", "WERE_UNDER_ATTACK_ANIM",
	"I_SAID_WERE_UNDER_ATTACK_ANIM", "HELP_HELP_ANIM",
	"THEYRE_ALL_OVER_US_ANIM", "WERE_LOSEING_IT_ANIM",
	"AAAHHH_ANIM", "FOR_CHRIST_SAKE_ANIM",
	"YOURE_JOKING_ANIM", "TARGET_DESTROYED_ANIM", "BLINK_ANIM",
	"WINK_ANIM", "SURPRISE_ANIM", "ANGER_ANIM", "GRIN_ANIM",
	"SCARED_ANIM", "EYES_LEFT_ANIM", "EYES_RIGHT_ANIM",
	"EYES_UP_ANIM", "EYES_DOWN_ANIM", "WHISTLE_ANIM",
	"LOOK_LEFT_ANIM", "LOOK_RIGHT_ANIM", "SALUTE_ANIM",
	"THUMBS_UP_ANIM", "YES_SIR_SALUTE_ANIM", "GOING_IN_THUMBS_UP_ANIM",
	"FORGET_IT_ANIM", "GET_OUTTA_HERE_ANIM", "GOOD_HIT_ANIM",
	"NO_WAY_ANIM", "NICE_ONE_ANIM", "OH_YEAH_ANIM", "GOTCHA_ANIM",
	"SMOOKIN_ANIM", "COOL_ANIM", "WIPE_OUT_ANIM", "TERRITORY_TAKEN_ANIM",
	"FIRE_EXTINGUISHED_ANIM", "GUN_CAPTURED_ANIM", "VEHICLE_CAPTURED_ANIM",
	"GRENADES_COLLECTED_ANIM", "ENDW1_ANIM", "ENDW2_ANIM", "ENDW3_ANIM",
	"ENDL1_ANIM", "ENDL2_ANIM", "ENDL3_ANIM",
};

class ZPortrait_Frame
{
public:
	ZPortrait_Frame()
	{
		look_direction = LOOK_STRAIGHT;
		mouth = 0;
		eyes = 0;
		hand_do_render = false;
		head_x = 4;
		head_y = 2;
	}

	char look_direction;
	char mouth;
	char eyes;
	char hand;
	char hand_x;
	char hand_y;
	bool hand_do_render;
	char head_x;
	char head_y;
	double duration;
};

class ZPortrait_Anim
{
public:
	ZPortrait_Anim();

	void AddFrame(ZPortrait_Frame &new_frame);

	vector<ZPortrait_Frame> frame_list;
	double total_duration;

	ZPortrait_Anim& operator=(const ZPortrait_Anim &rhs)
	{
		if(this == &rhs) return *this;

		frame_list = rhs.frame_list;
		total_duration = rhs.total_duration;

		return *this;
	}
};

class ZPortrait_Unit_Graphics
{
public:
	void Load(robot_type oid, team_type team, ZPortrait_Unit_Graphics &bc);

	ZSDL_Surface head[MAX_LOOK_DIRECTIONS];
	ZSDL_Surface eyes[MAX_EYES];
	ZSDL_Surface hand[MAX_HANDS];
	ZSDL_Surface mouth[MAX_MOUTHS];
	ZSDL_Surface shoulders;
};

class ZPortrait
{
public:
	ZPortrait();

	static void Init();

	void SetCords(int x_, int y_);
	void SetOverMap(bool over_map_);
	void SetTerrainType(planet_type terrain_);
	void SetInVehicle(bool in_vehicle_);
	void SetRobotID(unsigned char oid_);
	void SetTeam(int team_);
	void SetRefID(int ref_id_) { ref_id = ref_id_; }
	int GetRefID() { return ref_id; }
	void SetObject(ZObject *obj);
	void SetDoRandomAnims(bool do_random_anims_) { do_random_anims = do_random_anims_; }
	void ClearRobotID();

	void StartAnim(int anim_);
	bool DoingAnim();

	int Process();
	void DoRender();

private:
	static void SetupFrames();

	int GetBlitInfo(SDL_Surface *src, int x, int y, SDL_Rect &from_rect, SDL_Rect &to_rect);

	void StartRandomAnim();
	void RenderFace();
	void PlayAnimSound();

	static bool finished_init;

	static ZSDL_Surface backdrop[MAX_PLANET_TYPES];
	static ZSDL_Surface backdrop_vehicle;
	static ZPortrait_Unit_Graphics unit_graphics[MAX_ROBOT_TYPES][MAX_TEAM_TYPES];
	static ZPortrait_Anim anim_info[MAX_PORTRAIT_ANIMS];

	//frames
	static ZPortrait_Frame still_frame;

	int x, y;
	bool over_map;
	bool in_vehicle;
	bool do_render;
	unsigned char oid;
	int team;
	planet_type terrain;
	int ref_id;
	bool do_random_anims;

	int cur_anim;
	double anim_start_time;
	ZPortrait_Frame *render_frame;
	double next_random_anim_time;
};

#endif
