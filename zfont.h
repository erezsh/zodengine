#ifndef _ZFONT_H_
#define _ZFONT_H_

#include "zsdl.h"

#define MAX_CHARACTERS 255

enum font_type
{
	BIG_WHITE_FONT, SMALL_WHITE_FONT, GREEN_BUILDING_FONT, 
	LOADING_WHITE_FONT, YELLOW_MENU_FONT,
	MAX_FONT_TYPES
};

const string font_type_string[MAX_FONT_TYPES] = 
{
	"big_white", "small_white", "green_building", "loading_white", 
	"yellow_menu"
};

class ZFont
{
	public:
		ZFont();

		void Init();
		void SetType(int type_);
		SDL_Surface *Render(const char *message);
	private:
		int finished_init;
		int type;

		SDL_Surface *char_img[MAX_CHARACTERS];
};

#endif
