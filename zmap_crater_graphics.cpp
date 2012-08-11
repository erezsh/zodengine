#include "zmap_crater_graphics.h"

bool ZMapCraterGraphics::finished_init = false;

ZSDL_Surface ZMapCraterGraphics::crater_small[MAX_PLANET_TYPES][MAX_KNOWN_CRATER_TYPES][MAX_KNOWN_CRATER_N];
ZSDL_Surface ZMapCraterGraphics::crater_large[MAX_PLANET_TYPES][MAX_KNOWN_CRATER_TYPES][MAX_KNOWN_CRATER_N];

int ZMapCraterGraphics::crater_types[MAX_PLANET_TYPES];
int ZMapCraterGraphics::crater_small_n[MAX_PLANET_TYPES][MAX_KNOWN_CRATER_TYPES];
int ZMapCraterGraphics::crater_large_n[MAX_PLANET_TYPES][MAX_KNOWN_CRATER_TYPES];

void ZMapCraterGraphics::Init()
{
	for(int p=0;p<MAX_PLANET_TYPES;p++) crater_types[p] = 0;

	crater_types[DESERT] = 7;
	crater_types[VOLCANIC] = 3;
	crater_types[ARCTIC] = 2;
	crater_types[JUNGLE] = 3;
	crater_types[CITY] = 3;

	for(int p=0;p<MAX_PLANET_TYPES;p++)
		for(int t=0;t<crater_types[p] && t<MAX_KNOWN_CRATER_TYPES;t++)
		{
			char filename[1500];
			int n;

			//load smalls
			{
				for(n=0;1;n++)
				{
					SDL_Surface *surface;

					sprintf(filename, "assets/planets/craters/crater_small_%s_t%02d_n%02d.png", planet_type_string[p].c_str(), t, n);

					surface = IMG_Load(filename);

					//not loaded?
					if(!surface) break;

					//n too high?
					if(n>=MAX_KNOWN_CRATER_N)
					{
						SDL_FreeSurface(surface);
						printf("ZMapCraterGraphics::Init:n:%d for %s small type:%d too high (%d)\n", n, planet_type_string[p].c_str(), t, MAX_KNOWN_CRATER_N);
						break;
					}

					crater_small[p][t][n].LoadBaseImage(surface);
				}
				crater_small_n[p][t] = n;
				//printf("crater_small_n[%d][%d]:%d\n", p, t, n);
			}

			//load larges
			{
				for(n=0;1;n++)
				{
					SDL_Surface *surface;

					sprintf(filename, "assets/planets/craters/crater_large_%s_t%02d_n%02d.png", planet_type_string[p].c_str(), t, n);

					surface = IMG_Load(filename);

					//not loaded?
					if(!surface) break;

					//n too high?
					if(n>=MAX_KNOWN_CRATER_N)
					{
						SDL_FreeSurface(surface);
						printf("ZMapCraterGraphics::Init:n:%d for %s large type:%d too high (%d)\n", n, planet_type_string[p].c_str(), t, MAX_KNOWN_CRATER_N);
						break;
					}

					crater_large[p][t][n].LoadBaseImage(surface);
				}
				crater_large_n[p][t] = n;
				//printf("crater_large_n[%d][%d]:%d\n", p, t, n);
			}

		}

	finished_init = true;
}

bool ZMapCraterGraphics::CraterExists(bool is_small, int palette, int crater_type)
{
	if(palette < 0) return false;
	if(palette >= MAX_PLANET_TYPES) return false;
	if(crater_type < 0) return false;
	if(crater_type >= MAX_KNOWN_CRATER_TYPES) return false;

	if(is_small)
		return crater_small_n[palette][crater_type] != 0;
	else
		return crater_large_n[palette][crater_type] != 0;
}

ZSDL_Surface &ZMapCraterGraphics::RandomCrater(bool is_small, int palette, int crater_type)
{
	static ZSDL_Surface nothing;
	int choice, n;

	if(palette < 0) return nothing;
	if(palette >= MAX_PLANET_TYPES) return nothing;
	if(crater_type < 0) return nothing;
	if(crater_type >= MAX_KNOWN_CRATER_TYPES) return nothing;

	if(is_small)
	{
		n = crater_small_n[palette][crater_type];

		if(n<=0) return nothing;

		choice = rand() % n;

		return crater_small[palette][crater_type][choice];
	}
	else
	{
		n = crater_large_n[palette][crater_type];

		if(n<=0) return nothing;

		choice = rand() % n;

		return crater_large[palette][crater_type][choice];
	}
}
