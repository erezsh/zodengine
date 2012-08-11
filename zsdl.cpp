#include "zsdl.h"
#include "common.h"

using namespace COMMON;

bool zsdl_play_music = true;

SDL_RotoZoomSurface::SDL_RotoZoomSurface()
{
	base_surface = NULL;
	for(int i=0;i<360;i++)
		for(int j=0;j<100;j++)
			the_surface[i][j] = NULL;
}

void SDL_RotoZoomSurface::LoadBaseImage(string filename)
{
	base_surface = IMG_Load_Error(filename);
}

SDL_Surface *SDL_RotoZoomSurface::GetImage(int angle, double zoom)
{
	int zoom_int;

	zoom_int = (int)(zoom * 10);

	if(zoom_int >= 100)
	{
		printf("SDL_ZoomSurface::Requesting zoom of surface too large\n");
		zoom_int = 99;
	}
	else if(zoom_int < 0)
		zoom_int = 0;

	if(angle >= 360 || angle < 0)
	{
		printf("tried to get invalid rotation:%d\n", angle);
		return NULL;
	}

	if(the_surface[angle][zoom_int]) return the_surface[angle][zoom_int];

	the_surface[angle][zoom_int] = rotozoomSurface(base_surface, (double)angle, zoom_int / 10.0, 1);

	return the_surface[angle][zoom_int];
}

SDL_Surface *SDL_RotoZoomSurface::CreateImage(int angle, double zoom)
{
	int zoom_int;

	zoom_int = (int)(zoom * 10);

	if(zoom_int >= 100)
	{
		printf("SDL_ZoomSurface::Requesting zoom of surface too large\n");
		zoom_int = 99;
	}
	else if(zoom_int < 0)
		zoom_int = 0;

	if(angle >= 360 || angle < 0)
	{
		printf("tried to get invalid rotation:%d\n", angle);
		return NULL;
	}

	return rotozoomSurface(base_surface, (double)angle, zoom_int / 10.0, 1);
}

SDL_ZoomSurface::SDL_ZoomSurface()
{
	base_surface = NULL;
	for(int i=0;i<100;i++)
		zoom_surface[i] = NULL;
}

void SDL_ZoomSurface::LoadBaseImage(string filename)
{
	base_surface = IMG_Load_Error(filename);
}

SDL_Surface *SDL_ZoomSurface::GetImage(double zoom)
{
	int zoom_int;

	zoom_int = (int)(zoom * 10);

	if(zoom_int >= 100)
	{
		printf("SDL_ZoomSurface::Requesting zoom of surface too large\n");
		zoom_int = 99;
	}
	else if(zoom_int < 0)
		zoom_int = 0;

	if(zoom_surface[zoom_int]) return zoom_surface[zoom_int];

	zoom_surface[zoom_int] = rotozoomSurface(base_surface, 0.0, zoom_int / 10.0, 1);

	return zoom_surface[zoom_int];
}

SDL_Surface *SDL_ZoomSurface::CreateImage(double zoom)
{
	int zoom_int;

	zoom_int = (int)(zoom * 10);

	if(zoom_int >= 100)
	{
		printf("SDL_ZoomSurface::Requesting zoom of surface too large\n");
		zoom_int = 99;
	}
	else if(zoom_int < 0)
		zoom_int = 0;

	return rotozoomSurface(base_surface, 0.0, zoom_int / 10.0, 1);
}

SDL_RotateSurface::SDL_RotateSurface()
{
	base_surface = NULL;
	for(int i=0;i<360;i++)
		rotated_surface[i] = NULL;
}

void SDL_RotateSurface::LoadBaseImage(string filename)
{
	base_surface = IMG_Load_Error(filename);
}

SDL_Surface *SDL_RotateSurface::GetImage(int angle)
{
	if(!base_surface) return NULL;

	//printf("in angle:%d\n", angle);
	//angle = abs(angle % 360);
	//printf("out angle:%d\n", angle);

	if(angle >= 360 || angle < 0)
	{
		printf("tried to get invalid rotation:%d\n", angle);
		return NULL;
	}

	if(rotated_surface[angle]) return rotated_surface[angle];

	rotated_surface[angle] = rotozoomSurface(base_surface, (double)angle, 1, 1);

	return rotated_surface[angle];
}

int AngleFromLoc(float dx, float dy)
{
	//const float z = 0.000001;
	float a;
	int ret;

	//are we going anywhere?
	//if((dx > -z && dx < z) && (dy > -z && dy < z))
	if(isz(dx) && isz(dy))
		return -1;

	a = atan2(dy,dx);

	//atan2 is kind of funky
	if(a < 0) a += 3.14159f + 3.14159f;

	//printf("a:%f\n", a);

	ret = (int)(180 * a / 3.14159f);

	while(ret >= 360) ret = ret - 360;
	while(ret < 0) ret = ret + 360;

	return ret;
}

void draw_selection_box(SDL_Surface *surface, SDL_Rect dim, SDL_Color color, int max_x, int max_y)
{
	const int padding = 3;
	const int the_len = 5;
	int /* x_i, y_i,*/ x_end, y_end;
	SDL_Rect line_box;
	//int sdlmap;

	//sdlmap = SDL_MapRGB(surface->format, color.r, color.g, color.b);

	dim.x -= padding;
	dim.y -= padding;
	dim.w += padding + padding;
	dim.h += padding + padding;

	if(dim.x >= max_x) return;
	if(dim.y >= max_y) return;
	if(dim.x + dim.w < 0) return;
	if(dim.y + dim.h < 0) return;

	x_end = dim.x+dim.w;
	y_end = dim.y+dim.h;

	//top left corner
	line_box.x = dim.x;
	line_box.y = dim.y;
	line_box.w = the_len;
	line_box.h = 1;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	line_box.x = dim.x;
	line_box.y = dim.y;
	line_box.w = 1;
	line_box.h = the_len;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	//top right corner
	line_box.x = (dim.x + dim.w) - the_len;
	line_box.y = dim.y;
	line_box.w = the_len;
	line_box.h = 1;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	line_box.x = (dim.x + dim.w);
	line_box.y = dim.y;
	line_box.w = 1;
	line_box.h = the_len;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	//bottom left corner
	line_box.x = dim.x;
	line_box.y = (dim.y + dim.h);
	line_box.w = the_len;
	line_box.h = 1;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	line_box.x = dim.x;
	line_box.y = (dim.y + dim.h) - the_len;
	line_box.w = 1;
	line_box.h = the_len;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	//bottom right corner
	line_box.x = (dim.x + dim.w) - the_len;
	line_box.y = (dim.y + dim.h);
	line_box.w = the_len;
	line_box.h = 1;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	line_box.x = (dim.x + dim.w);
	line_box.y = (dim.y + dim.h) - the_len;
	line_box.w = 1;
	line_box.h = the_len;

	if(line_box.x + line_box.w >= max_x) line_box.w = max_x - line_box.x;
	if(line_box.y + line_box.h >= max_y) line_box.h = max_y - line_box.y;
	if(max_x - line_box.x > 0 && max_y - line_box.y > 0)
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);


	//y_i = dim.y+(dim.h);
	//for(int i=0;i<the_len;i++)
	//{
	//	x_i = dim.x+i;

	//	if(x_i < max_x && x_i >= 0)
	//	{
	//		put32pixel(surface, x_i, dim.y, color);
	//		if(y_i < max_y && y_i >= 0) put32pixel(surface, x_i, y_i, color);
	//	}

	//	x_i = (x_end)-i;

	//	if(x_i < max_x && x_i >= 0)
	//	{
	//		put32pixel(surface, x_i, dim.y, color);
	//		if(y_i < max_y && y_i >= 0) put32pixel(surface, x_i, y_i, color);
	//	}
	//}

	//x_i = dim.x+(dim.w);
	//for(int i=0;i<the_len;i++) 
	//{
	//	y_i = dim.y+i;
	//	
	//	if(y_i < max_y && y_i >= 0)
	//	{
	//		put32pixel(surface, dim.x, y_i, color);
	//		if(x_i < max_x && x_i >= 0) put32pixel(surface, x_i, y_i, color);
	//	}

	//	y_i = y_end-i;
	//	
	//	if(y_i < max_y && y_i >= 0)
	//	{
	//		put32pixel(surface, dim.x, y_i, color);
	//		if(x_i < max_x && x_i >= 0) put32pixel(surface, x_i, y_i, color);
	//	}
	//}
}

void draw_box(SDL_Surface *surface, SDL_Rect dim, SDL_Color color, int max_x, int max_y)
{
//	int x_i, y_i;
	SDL_Rect line_box;
	//int sdlmap;

	//sdlmap = SDL_MapRGB(surface->format, color.r, color.g, color.b);

	if(dim.x >= max_x) return;
	if(dim.y >= max_y) return;

	//top line
	line_box.x = dim.x;
	line_box.y = dim.y;
	line_box.w = dim.w;
	line_box.h = 1;
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	//bottom line
	line_box.x = dim.x;
	line_box.y = dim.y + dim.h;
	line_box.w = dim.w;
	line_box.h = 1;
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	//left line
	line_box.x = dim.x;
	line_box.y = dim.y;
	line_box.w = 1;
	line_box.h = dim.h;
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	//right line
	line_box.x = dim.x + dim.w;
	line_box.y = dim.y;
	line_box.w = 1;
	line_box.h = dim.h;
	//SDL_FillRect(surface, &line_box, sdlmap);
	ZSDL_FillRect(&line_box, color.r, color.g, color.b);

	//printf("color:%d %d %d\n", color.r, color.g, color.b);

	//y_i = dim.y+(dim.h);
	//for(int i=0;i<dim.w+1;i++)
	//{
	//	x_i = dim.x+i;

	//	if(x_i >= max_x) continue;

	//	put32pixel(surface, x_i, dim.y, color);
	//	if(y_i >= max_y) continue;
	//	put32pixel(surface, x_i, y_i, color);
	//}
	//x_i = dim.x+(dim.w);
	//for(int i=0;i<dim.h;i++) 
	//{
	//	y_i = dim.y+i;
	//	
	//	if(y_i >= max_y) continue;

	//	put32pixel(surface, dim.x, y_i, color);
	//	if(x_i >= max_x) continue;
	//	put32pixel(surface, x_i, y_i, color);
	//}
}

void ZSDL_SetMusicOn(bool iton)
{
	zsdl_play_music = iton;
}

int ZSDL_PlayMusic(Mix_Music *music, int eh)
{
	if(!zsdl_play_music) return 0;
	if(!music) return 0;

	return Mix_PlayMusic(music, eh);
}

int ZMix_PlayChannel(int ch, Mix_Chunk *wav, int repeat)
{
	int ret;
	if(!zsdl_play_music) return 0;

	ret = Mix_PlayChannel(ch, wav, repeat);

	if(ret == -1) printf("unable to play wav\n");

	return ret;
}

SDL_Surface *ZSDL_ConvertImage(SDL_Surface *src)
{
	if(src)
	{
		SDL_Surface *new_ret;

		new_ret = SDL_DisplayFormatAlpha(src);
		SDL_FreeSurface( src );
		src = new_ret;
	}

	return src;
}

SDL_Surface *ZSDL_IMG_Load(string filename)
{
	SDL_Surface *ret;
	
	ret = IMG_Load(filename.c_str());

	if(!ret) printf("could not load:%s\n", filename.c_str()); 

	ret = ZSDL_ConvertImage(ret);
	
	//SDL_DisplayFormat
	
	return ret;
}

SDL_Surface *IMG_Load_Error(string filename)
{
	SDL_Surface *ret;
	
	if(!(ret = ZSDL_IMG_Load(filename.c_str()))) printf("could not load:%s\n", filename.c_str());

	//SDL_DisplayFormat
	
	return ret;
}

Mix_Music *MUS_Load_Error(string filename)
{
	Mix_Music *ret;
	
	if(!(ret = Mix_LoadMUS(filename.c_str()))) printf("could not load:%s\n", filename.c_str());
	
	return ret;
}

Mix_Chunk *MIX_Load_Error(string filename)
{
	Mix_Chunk *ret;
	
	if(!(ret = Mix_LoadWAV(filename.c_str()))) printf("could not load:%s\n", filename.c_str());
	
	return ret;
}

SDL_Surface *CopyImage(SDL_Surface *original)
{
	SDL_Surface *copy;
	
	if(!original) return NULL;
	
	copy = SDL_DisplayFormatAlpha(original);//SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, original->w, original->h, 32, 0xFF000000, 0x0000FF00, 0x00FF0000, 0x000000FF);
	//copy = ZSDL_ConvertImage(copy);
	
	SDL_BlitSurface(original, NULL, copy, NULL);
	
	return copy;
}

SDL_Surface *CopyImageShifted(SDL_Surface *original, int x, int y)
{
	SDL_Surface *copy;
	SDL_Rect to_rect;
	
	if(!original) return NULL;
	if(x < 0) return CopyImage(original);
	if(y < 0) return CopyImage(original);
	
	copy = SDL_DisplayFormatAlpha(original);//SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, original->w + x, original->h + y, 32, 0xFF000000, 0x0000FF00, 0x00FF0000, 0x000000FF);
	//copy = ZSDL_ConvertImage(copy);
	
	to_rect.x = x;
	to_rect.y = y;
	
	SDL_BlitSurface(original, NULL, copy, &to_rect);
	
	return copy;
}

void ZSDL_ModifyBlack(SDL_Surface *surface)
{
	SDL_Rect White_Pix_Rect;
	int rgb_map;

	rgb_map = SDL_MapRGB(surface->format, 1, 0, 0);

	for(int i=0;i<surface->w;i++)
		for(int j=0;j<surface->h;j++)
		{
			Uint8 r, g, b, a;
			Uint32 pixel;

			pixel = *(Uint32*)((Uint8*)surface->pixels + j * surface->pitch + i * surface->format->BytesPerPixel);
			SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

			if(!r && !g && !b && a)
			{
				White_Pix_Rect.x = i;
				White_Pix_Rect.y = j;
				White_Pix_Rect.w = 1;
				White_Pix_Rect.h = 1;
				//SDL_FillRect(&White_Pix_Rect, 1, 0, 0, surface);
				SDL_FillRect(surface, &White_Pix_Rect, rgb_map);
			}
		}
}

void ZSDL_BlitSurface(SDL_Surface *src, int fx, int fy, int fw, int fh, SDL_Surface *dest, int x, int y)
{
	SDL_Rect from_rect, to_rect;

	from_rect.x = fx;
	from_rect.y = fy;
	from_rect.w = fw;
	from_rect.h = fh;

	to_rect.x = x;
	to_rect.y = y;
	to_rect.w = fw;
	to_rect.h = fh;

	SDL_BlitSurface( src, &from_rect, dest, &to_rect);
}

void ZSDL_BlitTileSurface(SDL_Surface *src, int fx, int fy, SDL_Surface *dest, int x, int y)
{
	ZSDL_BlitSurface(src, fx * 16, fy * 16, 16, 16, dest, x * 16, y * 16);
}

void ZSDL_BlitHitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect, bool render_hit)
{
	SDL_Rect White_Pix_Rect;
	//Uint32 White_Pix, Red_Pix;
	int i,j;

	if(!render_hit)
	{
		SDL_BlitSurface(src, srcrect, dst, dstrect);
		return;
	}

	//render the hit effect
	if(!src) return;
	if(!dst) return;

	//Red_Pix = SDL_MapRGB(dst->format, 255, 0, 0);

	//White_Pix = SDL_MapRGB(dst->format, 255, 255, 255);
	White_Pix_Rect.w = 1;
	White_Pix_Rect.h = 1;

	//SDL_GetRGB
	//SDL_MapRGB

	int start_x, width_x;
	int start_y, height_y;
	int to_x, to_y;

	if(srcrect)
	{
		start_x = srcrect->x;
		width_x = srcrect->w;

		start_y = srcrect->y;
		height_y = srcrect->h;
	}
	else
	{
		start_x = 0;
		start_y = 0;
		width_x = src->w;
		height_y = src->h;
	}

	if(width_x > src->w) width_x = src->w;
	if(height_y > src->h) height_y = src->h;

	if(dstrect)
	{
		to_x = dstrect->x;
		to_y = dstrect->y;
	}
	else
	{
		to_x = 0;
		to_y = 0;
	}

	for(i=start_x;i<width_x;i++)
		for(j=start_y;j<height_y;j++)
		{
			Uint8 r, g, b, a;
			Uint32 pixel;

			pixel = *(Uint32*)((Uint8*)src->pixels + j * src->pitch + i * src->format->BytesPerPixel);
			SDL_GetRGBA(pixel, src->format, &r, &g, &b, &a);

			if(a)
			{
				White_Pix_Rect.x = dstrect->x + i;
				White_Pix_Rect.y = dstrect->y + j;
				White_Pix_Rect.w = 1;
				White_Pix_Rect.h = 1;
				//SDL_FillRect(dst, &White_Pix_Rect, White_Pix);
				ZSDL_FillRect(&White_Pix_Rect, 0, 0, 0);
			}
			//else
			//{
			//	White_Pix_Rect.x = dstrect->x + i;
			//	White_Pix_Rect.y = dstrect->y + j;
			//	White_Pix_Rect.w = 1;
			//	White_Pix_Rect.h = 1;
			//	SDL_FillRect(dst, &White_Pix_Rect, Red_Pix);
			//}
		}
}

SDL_Surface *ZSDL_NewSurface(int w, int h)
{
	if(w <= 0) return NULL;
	if(h <= 0) return NULL;

	SDL_Surface *copy;

	copy = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, w, h, 32, 0, 0, 0, 0);
		//SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, w, h, 32, 0xFF000000, 0x0000FF00, 0x00FF0000, 0x000000FF);
	//copy = ZSDL_ConvertImage(copy);

	return copy;
}

void put32pixel(SDL_Surface *surface, int x, int y, SDL_Color color)
{
	SDL_PixelFormat *fmt;
	//SDL_Surface *surface;
//	SDL_Color return_color;
	Uint32 /* temp,*/ *pixel;
//	Uint8 red, green, blue, alpha;
	
	if(x<0) return;
	if(y<0) return;
	if(x>=surface->w) return;
	if(y>=surface->h) return;

	fmt = surface->format;
	//SDL_LockSurface(surface);
	//pixel = *((Uint32*)surface->pixels);
	//pixel = *((Uint32*)((Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel));
	//SDL_UnlockSurface(surface);
	
	pixel = (Uint32*)((Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
	
	//clear red
	((Uint8*)pixel)[2] = color.r;
	((Uint8*)pixel)[1] = color.g;
	((Uint8*)pixel)[0] = color.b;
	((Uint8*)pixel)[3] = color.unused;
}

SDL_Color get32pixel(SDL_Surface *surface, int x, int y)
{
	SDL_PixelFormat *fmt;
	//SDL_Surface *surface;
	SDL_Color return_color;
	Uint32 temp, pixel;
	Uint8 red, green, blue, alpha;

	fmt = surface->format;
	//SDL_LockSurface(surface);
	//pixel = *((Uint32*)surface->pixels);
	pixel = *((Uint32*)((Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel));
	//SDL_UnlockSurface(surface);
	 
	/* Get Red component */
	temp = pixel & fmt->Rmask;  /* Isolate red component */
	temp = temp >> fmt->Rshift; /* Shift it down to 8-bit */
	temp = temp << fmt->Rloss;  /* Expand to a full 8-bit number */
	red = (Uint8)temp;
	 
	/* Get Green component */
	temp = pixel & fmt->Gmask;  /* Isolate green component */
	temp = temp >> fmt->Gshift; /* Shift it down to 8-bit */
	temp = temp << fmt->Gloss;  /* Expand to a full 8-bit number */
	green = (Uint8)temp;
	
	/* Get Blue component */
	temp = pixel & fmt->Bmask;  /* Isolate blue component */
	temp = temp >> fmt->Bshift; /* Shift it down to 8-bit */
	temp = temp << fmt->Bloss;  /* Expand to a full 8-bit number */
	blue = (Uint8)temp;
	 
	/* Get Alpha component */
	temp = pixel & fmt->Amask;  /* Isolate alpha component */
	temp = temp >> fmt->Ashift; /* Shift it down to 8-bit */
	temp = temp << fmt->Aloss;  /* Expand to a full 8-bit number */
	alpha = (Uint8)temp;

	return_color.r = red;
	return_color.g = green;
	return_color.b = blue;
	return_color.unused = alpha;
	
	return return_color;
}

void ZSDL_FreeSurface(SDL_Surface *&surface)
{
	if(surface)
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}
}

void ZSDL_Quit()
{
	Mix_CloseAudio();
	SDL_Quit();
}
