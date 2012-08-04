#ifndef _ETANKOIL_H_
#define _ETANKOIL_H_

#include "zeffect.h"

class ETankOil : ZEffect
{
public:
	ETankOil(ZTime *ztime_, int cx_, int cy_, int direction_);

	static void Init();

	void Process();
	void DoPreRender(ZMap &zmap, SDL_Surface *dest);
private:
	static bool finished_init;
	static ZSDL_Surface tank_oil[3][3];

	void SetCoords(int cx_, int cy_, int dir_);

	int cx, cy;
	int direction;

	int oil_i;
	int ni;

	double next_ni_time;
};

#endif
