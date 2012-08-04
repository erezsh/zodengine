#ifndef _OROCKETS_H_
#define _OROCKETS_H_

#include "zobject.h"

class ORockets : public ZObject
{
	public:
		ORockets(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();

		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		int Process();

		void SetOwner(team_type owner_);
	private:
		static ZSDL_Surface render_img;
};

#endif
