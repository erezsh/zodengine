#include "zgfile.h"

vector<ZGFileEntry> ZGFile::entry_list;

ZGFile::ZGFile()
{

}

void ZGFile::Init()
{
	LoadEntryList();
}

void ZGFile::LoadEntryList()
{
	FILE *fp;

	fp = fopen(ZGFILE_NAME, "rb");

	if(!fp)
	{
		printf("ZGFile::LoadEntryList: could not load file '%s'\n", ZGFILE_NAME);
		return;
	}

	//clear out current list
	entry_list.clear();

	//load in new list
	while(1)
	{
		int ret;
		ZGFileEntry temp_entry;
		long int entry_size;

		ret = fread(&temp_entry, sizeof(ZGFileEntry), 1,  fp);

		if(!ret) break;

		temp_entry.f_offset = ftell(fp);

		entry_list.push_back(temp_entry);

		//move ahead
		entry_size = temp_entry.w * temp_entry.h * 4;
		ret = fseek(fp, entry_size, SEEK_CUR);

		if(ret)
		{
			printf("ZGFile::LoadEntryList: fseek error\n");
			break;
		}
	}

	printf("ZGFile::LoadEntryList: entries loaded:%d\n", entry_list.size());

	fclose(fp);

	//sort entry_list for binary searching
	;
}

void ZGFile::AddFile(string filename, SDL_Surface *img)
{
	FILE *fp;
	int ret;
	ZGFileEntry temp_entry;
	int entry_size;
	SDL_Surface *write_img;

	//surface ok?
	if(!img) 
	{
		printf("ZGFile::AddFile: null image for '%s'\n", filename.c_str());
		return;
	}

	//it already in the file?
	if(FindEntry(filename) != -1)
	{
		printf("ZGFile::AddFile: filename already in added '%s'\n", filename.c_str());
		return;
	}

	//filename too long?
	if(filename.length() >= sizeof(temp_entry.filename))
	{
		printf("ZGFile::AddFile: filename too long [size:%d] for '%s'\n", filename.length(), filename.c_str());
		return;
	}

	fp = fopen(ZGFILE_NAME, "ab");

	if(!fp)
	{
		printf("ZGFile::AddFile: could not open file '%s'\n", ZGFILE_NAME);
		return;
	}

	//create our write img
	write_img = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, img->w, img->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_Rect to_rect;
	to_rect.x = 0;
	to_rect.y = 0;
	to_rect.w = img->w;
	to_rect.h = img->h;
	//SDL_FillRect(write_img, &to_rect, SDL_MapRGB(write_img->format, 0, 0, 0));
	SDL_BlitSurface(img, &to_rect, write_img, &to_rect);

	//needed to make ftell give us the right value
	fseek (fp, 0, SEEK_END);

	//setup entry
	strcpy(temp_entry.filename, filename.c_str());
	temp_entry.w = img->w;
	temp_entry.h = img->h;
	temp_entry.f_offset = sizeof(ZGFileEntry) + ftell(fp);

	//add entry to list
	entry_list.push_back(temp_entry);

	//write in entry
	ret = fwrite(&temp_entry, sizeof(ZGFileEntry), 1, fp);

	if(!ret)
	{
		printf("ZGFile::AddFile: could not write entry\n");
		SDL_FreeSurface(write_img);
		fclose(fp);
		return;
	}

	//write in data
	entry_size = temp_entry.w * temp_entry.h * 4;
	ret = fwrite(write_img->pixels, entry_size, 1, fp);

	if(!ret)
	{
		printf("ZGFile::AddFile: could not write data\n");
		SDL_FreeSurface(write_img);
		fclose(fp);
		return;
	}

	fclose(fp);

	//free our write img
	SDL_FreeSurface(write_img);
}

SDL_Surface *ZGFile::LoadFile(string filename)
{
	FILE *fp;
	int ret;
	int entry_size;
	SDL_Surface *read_img;
	int e_num;

	//it found?
	e_num = FindEntry(filename);
	
	if(e_num == -1)
	{
		printf("ZGFile::LoadFile::entry not found '%s'\n", filename.c_str());
		return NULL;
	}

	fp = fopen(ZGFILE_NAME, "rb");

	if(!fp)
	{
		printf("ZGFile::LoadFile: could not open file '%s'\n", ZGFILE_NAME);
		return NULL;
	}

	//go there
	ret = fseek (fp, entry_list[e_num].f_offset, SEEK_SET);

	if(ret)
	{
		printf("ZGFile::LoadFile: could not fseek to %d\n", entry_list[e_num].f_offset);
		fclose(fp);
		return NULL;
	}

	//make our image
	read_img = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, entry_list[e_num].w, entry_list[e_num].h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	//SDL_Rect to_rect;
	//to_rect.x = 0;
	//to_rect.y = 0;
	//to_rect.w = entry_list[e_num].w;
	//to_rect.h = entry_list[e_num].h;
	//SDL_FillRect(read_img, &to_rect, SDL_MapRGB(read_img->format, 0, 0, 0));

	//was the image made?
	if(!read_img)
	{
		printf("ZGFile::LoadFile: could not create sdl surface\n");
		fclose(fp);
		return NULL;
	}

	//read into the image
	entry_size = entry_list[e_num].w * entry_list[e_num].h * 4;
	ret = fread(read_img->pixels, entry_size, 1,  fp);
	//ret = 1;

	if(!ret)
	{
		printf("ZGFile::LoadFile: could not read in sdl surface\n");
		SDL_FreeSurface(read_img);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	return read_img;
}

int ZGFile::FindEntry(string filename)
{
	for(int i=0; i<entry_list.size(); i++)
		if(filename == entry_list[i].filename)
			return i;

	return -1;

}
