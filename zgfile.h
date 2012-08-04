#ifndef _ZGFILE_H_
#define _ZGFILE_H_

#include "zsdl.h"
#include <vector>

using namespace std;

#define ZGFILE_NAME "assets/sprites.zgfx"

struct ZGFileEntry
{
	char filename[512];
	int w, h;
	int f_offset;
};

class ZGFile
{
public:
	ZGFile();

	static void Init();
	static void AddFile(string filename, SDL_Surface *img);
	static SDL_Surface *LoadFile(string filename);
	static int FindEntry(string filename);

private:
	static void LoadEntryList();

	static vector<ZGFileEntry> entry_list;
};

#endif
