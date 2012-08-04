#ifndef _ETANKSPARK_H_
#define _ETANKSPARK_H_

#include "zeffect.h"

class ETankSpark : ZEffect
{
public:
	ETankSpark(ZTime *ztime_, int cx_, int cy_, int direction_);

	static void Init();

	void Process();
	void DoPreRender(ZMap &zmap, SDL_Surface *dest);
private:
	static bool finished_init;
	static ZSDL_Surface tank_spark[6];

	void SetCoords(int cx_, int cy_, int dir_);

	int cx, cy;
	int direction;

	int ni_max;
	int ni_i;
	int ni;

	double next_ni_time;
};

#endif
