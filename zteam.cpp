#include "zteam.h"
//#include "zgfile.h"

ZTeam_Palette ZTeam::team_palette[MAX_TEAM_TYPES];

SDL_Color team_color[MAX_TEAM_TYPES];

bool ZTeam_Palette::LoadSurfacePalette(SDL_Surface *src)
{
	//dimensions ok?
	if(src->w != 2)
	{
		printf("ZTeam_Palette::LoadSurfacePalette:palette width not 2\n");
		return false;
	}

	if(src->h != ZTEAM_PALETTE_MAX)
	{
		printf("ZTeam_Palette::LoadSurfacePalette:palette height not %d\n", ZTEAM_PALETTE_MAX);
	}

	//clear out
	//base_color.clear();
	//replace_color.clear();

	//load in
	for(int j=0;j<src->h;j++)
	{
		Uint8 r, g, b, a;
		Uint32 pixel;
		SDL_Color nc;

		//base color
		{
			pixel = *(Uint32*)((Uint8*)src->pixels + j * src->pitch + 0 * src->format->BytesPerPixel);
			SDL_GetRGBA(pixel, src->format, &r, &g, &b, &a);

			nc.r = r;
			nc.g = g;
			nc.b = b;

			//base_color.push_back(nc);
			if(j<ZTEAM_PALETTE_MAX) base_color[j] = nc;
		}

		//replace color
		{
			pixel = *(Uint32*)((Uint8*)src->pixels + j * src->pitch + 1 * src->format->BytesPerPixel);
			SDL_GetRGBA(pixel, src->format, &r, &g, &b, &a);

			nc.r = r;
			nc.g = g;
			nc.b = b;

			//replace_color.push_back(nc);
			if(j<ZTEAM_PALETTE_MAX) replace_color[j] = nc;
		}
	}
	
	return true;
}

bool ZTeam_Palette::SaveSurfacePalette(string filename)
{
	printf("ZTeam_Palette::SaveSurfacePalette: this function requires color arrays be vectors\n");
	return false;
	/*
	SDL_Surface *src;

	if(!base_color.size())
	{
		printf("ZTeam_Palette::SaveSurfacePalette:palette not loaded to save to '%s'\n", filename.c_str());
		return false;
	}

	src = ZSDL_NewSurface(2, base_color.size());

	if(!src)
	{
		printf("ZTeam_Palette::SaveSurfacePalette:could not make surface to save to '%s'\n", filename.c_str());
		return false;
	}

	//load surface
	for(int i=0;i<src->h;i++)
	{
		SDL_Rect point_rect;
		SDL_Color c;

		//base color
		{
			c = base_color[i];

			point_rect.x = 0;
			point_rect.y = i;
			point_rect.w = 1;
			point_rect.h = 1;

			SDL_FillRect(src, &point_rect, SDL_MapRGB(src->format, c.r, c.g, c.b));
		}

		//replace color
		{
			c = replace_color[i];

			point_rect.x = 1;
			point_rect.y = i;
			point_rect.w = 1;
			point_rect.h = 1;

			SDL_FillRect(src, &point_rect, SDL_MapRGB(src->format, c.r, c.g, c.b));
		}
	}

	//save surface
	SDL_SaveBMP(src, filename.c_str());

	//free surface
	SDL_FreeSurface(src);

	//good
	return true;
	*/
}

bool ZTeam_Palette::AddColor(SDL_Color &bc, SDL_Color &rc)
{
	printf("ZTeam_Palette::AddColor: this function requires color arrays be vectors\n");
	return false;
	/*
	//this color already in the list?
	for(vector<SDL_Color>::iterator i=base_color.begin(); i!=base_color.end(); i++)
		if(bc.r == i->r && bc.g == i->g && bc.b == i->b)
			return false;

	//add it
	base_color.push_back(bc);
	replace_color.push_back(rc);

	return true;
	*/
}

bool ZTeam_Palette::GetReplacement(Uint8 &r, Uint8 &g, Uint8 &b, SDL_Color &c)
{
	for(int i=0;i<ZTEAM_PALETTE_MAX;i++)
	{
		SDL_Color &bi = base_color[i];

		if(r == bi.r && g == bi.g && b == bi.b)
		{
			SDL_Color &ri = replace_color[i];

			c.r = ri.r;
			c.g = ri.g;
			c.b = ri.b;

			return true;
		}
	}

	return false;

	/*
	vector<SDL_Color>::iterator bi;
	vector<SDL_Color>::iterator ri;

	if(!r && !g && !b) return false;

	bi=base_color.begin();
	ri=replace_color.begin();

	for(; bi!=base_color.end(); bi++, ri++)
		if(r == bi->r && g == bi->g && b == bi->b)
		{
			c.r = ri->r;
			c.g = ri->g;
			c.b = ri->b;

			return true;
		}

	return false;
	*/
}

void ZTeam::Init()
{
	int i;

	for(i=0;i<MAX_TEAM_TYPES;i++)
		LoadPalette(i);

	Setup_team_color();
}

/*
{
	{115, 115, 115, 0},
	{233, 0, 0, 0},
	{19, 55, 251, 0},
	{23, 143, 19, 0},
	{203, 99, 47, 0}
};*/

void ZTeam::Setup_team_color()
{
	int i;

	for(i=0;i<MAX_TEAM_TYPES;i++)
	{
		team_color[i].r = 115;
		team_color[i].g = 115;
		team_color[i].b = 115;
	}

	team_color[NULL_TEAM].r = 115;
	team_color[NULL_TEAM].g = 115;
	team_color[NULL_TEAM].b = 115;

	team_color[RED_TEAM].r = 223;
	team_color[RED_TEAM].g = 0;
	team_color[RED_TEAM].b = 0;

	team_color[BLUE_TEAM].r = 19;
	team_color[BLUE_TEAM].g = 55;
	team_color[BLUE_TEAM].b = 251;

#ifdef GREEN_TEAM
	team_color[GREEN_TEAM].r = 23;
	team_color[GREEN_TEAM].g = 143;
	team_color[GREEN_TEAM].b = 19;
#endif

#ifdef YELLOW_TEAM
	team_color[YELLOW_TEAM].r = 203;
	team_color[YELLOW_TEAM].g = 99;
	team_color[YELLOW_TEAM].b = 47;
#endif

	for(i=BLUE_TEAM;i<MAX_TEAM_TYPES;i++)
	{
		SDL_Color c;
		Uint8 r, g, b;

		r = 223;
		g = 0;
		b = 0;

		if(team_palette[i].GetReplacement(r, g, b, c))
		{
			team_color[i].r = c.r;
			team_color[i].g = c.g;
			team_color[i].b = c.b;
		}
	}
}

void ZTeam::LoadPalette(int team)
{
	string filename;
	SDL_Surface *surface;

	if(team == ZTEAM_BASE_TEAM) return;

	filename = "assets/teams/" + team_type_string[team] + "_palette.bmp";
	surface = IMG_Load(filename.c_str());

	if(!surface)
	{
		printf("ZTeam::Could not load palette for the %s team:'%s'\n", team_type_string[team].c_str(), filename.c_str());
		return;
	}

	//convert it to 32 bits
	surface = ZSDL_ConvertImage(surface);

	//load it and free it
	team_palette[team].LoadSurfacePalette(surface);
	SDL_FreeSurface(surface);
}

void ZTeam::SavePalette(int team)
{
	if(team == ZTEAM_BASE_TEAM)
	{
		printf("ZTeam::SavePalette:You can not save the base palette (%s)\n", team_type_string[ZTEAM_BASE_TEAM].c_str());
		return;
	}

	string filename;

	filename = "assets/teams/" + team_type_string[team] + "_palette.bmp";

	team_palette[team].SaveSurfacePalette(filename);

	//SDL_SaveBMP(team_palette[team], filename.c_str());
}

void ZTeam::SaveAllPalettes()
{
	int i;

	for(i=0;i<MAX_TEAM_TYPES;i++)
		SavePalette(i);
}

void ZTeam::AppendPalette(int team, SDL_Surface *bv, SDL_Surface *rv)
{
	if(team == ZTEAM_BASE_TEAM)
	{
		printf("ZTeam::AppendPalette:You can not append to the base palette (%s)\n", team_type_string[ZTEAM_BASE_TEAM].c_str());
		return;
	}

	//surfaces ok?
	if(!bv || !rv) return;

	if(!bv->w || !bv->h) return;
	if(!rv->w || !rv->h) return;

	if(bv->w != rv->w) return;
	if(bv->h != rv->h) return;

	//get list of differences
	int i, j;

	for(i=0;i<bv->w;i++)
		for(j=0;j<bv->h;j++)
		{
			Uint8 br, bg, bb, ba;
			Uint8 rr, rg, rb, ra;
			Uint32 pixel;

			pixel = *(Uint32*)((Uint8*)bv->pixels + j * bv->pitch + i * bv->format->BytesPerPixel);
			SDL_GetRGBA(pixel, bv->format, &br, &bg, &bb, &ba);

			pixel = *(Uint32*)((Uint8*)rv->pixels + j * rv->pitch + i * rv->format->BytesPerPixel);
			SDL_GetRGBA(pixel, rv->format, &rr, &rg, &rb, &ra);

			//add to the list?
			if(br != rr || bg != rg || bb != rb)
			{
				SDL_Color bc, rc;

				bc.r = br;
				bc.g = bg;
				bc.b = bb;

				rc.r = rr;
				rc.g = rg;
				rc.b = rb;

				team_palette[team].AddColor(bc, rc);
			}
		}
}

//#define COLLECT_TEAM_COLORS

void ZTeam::LoadZSurface(int team, ZSDL_Surface &bv, ZSDL_Surface &rv, string filename)
{
#ifdef USE_TEAM_COLORS
	#ifdef COLLECT_TEAM_COLORS
			rv.LoadBaseImage(filename);
			if(team != NULL_TEAM && team != ZTEAM_BASE_TEAM)
				ZTeam::AppendPalette(team, bv.GetBaseSurface(), rv.GetBaseSurface());
	#else
			if(team == NULL_TEAM || team == ZTEAM_BASE_TEAM) rv.LoadBaseImage(filename);
			else rv.LoadBaseImage(ZTeam::Make(team, bv.GetBaseSurface())); 
	#endif
#else
	rv.LoadBaseImage(filename);
#endif
}

SDL_Surface *ZTeam::Make(int team, SDL_Surface *base_version)
{
	if(team == ZTEAM_BASE_TEAM)
	{
		printf("ZTeam::Make:You can not make from the base palette (%s)\n", team_type_string[ZTEAM_BASE_TEAM].c_str());
		return NULL;
	}

	if(!base_version) return NULL;

	SDL_Surface *nw;

	nw = CopyImage(base_version);

	if(!nw) return NULL;

	Uint32* pixel_ptr;
	pixel_ptr = (Uint32*)nw->pixels;

	for(int j=0;j<nw->h;j++)
		for(int i=0;i<nw->w;i++)
		{
			Uint8 r, g, b, a;
			Uint32 pixel;
			SDL_Color c;

			//pixel = *(Uint32*)((Uint8*)nw->pixels + j * nw->pitch + i * nw->format->BytesPerPixel);
			pixel = *pixel_ptr;
			pixel_ptr++;
			SDL_GetRGBA(pixel, nw->format, &r, &g, &b, &a);

			//bypass invisible
			if(!a) continue;

			if(team_palette[team].GetReplacement(r,g,b,c))
			{
				SDL_Rect point_rect;

				point_rect.x = i;
				point_rect.y = j;
				point_rect.w = 1;
				point_rect.h = 1;

				SDL_FillRect(nw, &point_rect, SDL_MapRGB(nw->format, c.r, c.g, c.b));
			}
		}

	return nw;
}
