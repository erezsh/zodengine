#ifndef _ETRACK_H_
#define _ETRACK_H_

#include "zeffect.h"

enum ETRACK_TYPE
{
	ET_TANK, ET_JEEP, MAX_ETRACK_TYPES
};

const string etrack_type_string[MAX_ETRACK_TYPES] = 
{
	"tank", "jeep"
};

class ETrack : ZEffect
{
public:
	ETrack(ZTime *ztime_, int cx_, int cy_, int dir_, int type_, int p_, bool lay1 = true, bool lay2 = true);

	static void Init();

	void Process();
	void DoPreRender(ZMap &zmap, SDL_Surface *dest);

	static void SetTrackCoords(int cx_, int cy_, int dir_, int *x_, int *y_);
private:
	static bool finished_init;

	static ZSDL_Surface track_img[MAX_ETRACK_TYPES][MAX_PLANET_TYPES][MAX_ANGLE_TYPES][3];

	int cx, cy;
	int dir;
	int type;
	int p;
	int ti;

	bool lay_track[2];
	int x[2], y[2];

	double start_time;
};

#endif
