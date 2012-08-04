#ifndef _ETANKSMOKE_H_
#define _ETANKSMOKE_H_

#include "zeffect.h"

class ETankSmoke : ZEffect
{
public:
	ETankSmoke(ZTime *ztime_, int cx_, int cy_, int direction_, bool do_spark_);

	static void Init();

	void Process();
	void DoPreRender(ZMap &zmap, SDL_Surface *dest);

	static void SetCoords(int cx_, int cy_, int dir_, int &x_, int &y_);
private:
	static bool finished_init;
	static ZSDL_Surface tank_smoke[MAX_ANGLE_TYPES][7];
	static ZSDL_Surface tank_spark[MAX_ANGLE_TYPES][4];

	int cx, cy;
	int ni;
	int direction;
	int x, y;
	bool do_spark;

	double next_ni_time;
};

#endif
