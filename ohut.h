#ifndef _OHUT_H_
#define _OHUT_H_

#include "zobject.h"
#include "ahutanimal.h"

class OHut : public ZObject
{
	public:
		OHut(ZTime *ztime_, ZSettings *zsettings_ = NULL, int palette_ = DESERT);
		~OHut();
		
		static void Init();

		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoPreRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		int Process();

		void SetOwner(team_type owner_);
		void SetMapImpassables(ZMap &tmap);
		void UnSetMapImpassables(ZMap &tmap);

		void ChangePalette(int palette_);

		bool IsDestroyableImpass() { return true; }
		bool CausesImpassAtCoord(int x, int y);
	private:
		static ZSDL_Surface render_img[MAX_PLANET_TYPES];

		void SetMaxHutAnimals();
		void CreateAnimals(int amount = 1);
		void SendAnimalsHome(int amount);
		bool GetExitToTile(int &tx, int &ty);

		int palette;

		vector<AHutAnimal*> hut_animals;
		double next_hut_animal_time;
		double next_max_hut_animal_time;
		int max_hut_animals;
};

#endif
