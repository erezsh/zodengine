#include "zfont.h"

ZFont::ZFont()
{
	finished_init = false;
}

void ZFont::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<MAX_CHARACTERS;i++)
		char_img[i] = NULL;

	for(i=0;i<MAX_CHARACTERS;i++)
	{
		sprintf(filename_c, "assets/fonts/%s/char_%03d.png", font_type_string[type].c_str(), i);
		char_img[i] = IMG_Load(filename_c);
	}

	finished_init = true;
}

SDL_Surface *ZFont::Render(const char *message)
{
	int i;
	int total_width, max_height;
	SDL_Rect to_rect;
	SDL_Surface *surface = NULL;

	if(!finished_init) return NULL;

	//get what the width of the img will be
	total_width = 0;
	max_height = 0;
	for(i=0;message[i];i++)
	{
		char &c = ((char*)message)[i];

		if(char_img[c]) 
		{
			total_width += char_img[c]->w;
			if(char_img[c]->h > max_height)
				max_height = char_img[c]->h;
		}
	}

	if(!total_width) return NULL;
	if(!max_height) return NULL;

	//make our surface
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, total_width, max_height, 32, 0xFF000000, 0x0000FF00, 0x00FF0000, 0x000000FF);

	//render to it
	to_rect.x = 0;
	to_rect.y = 0;
	for(i=0;message[i];i++)
	{
		char &c = ((char*)message)[i];

		if(char_img[c]) 
		{
			SDL_BlitSurface(char_img[c], NULL, surface, &to_rect);
			to_rect.x += char_img[c]->w;
		}
	}

	//return it
	return surface;
}

void ZFont::SetType(int type_)
{
	type = type_;
}
