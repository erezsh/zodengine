#include "zmap.h"
#include <stdio.h>

enum direction
{
	VERTICAL, HORIZONTAL
};

#undef main
int main(int argc, char **argv)
{
	//got the right inputs?
	if(argc < 5)
	{
		printf("proper usage:%s output_map merge_direction map1 map2\n", argv[0]);
		return 0;
	}

	printf("argc:%d\n", argc);
	printf("output_map:'%s'\n", argv[1]);
	printf("direction:'%s'\n", argv[2]);
	printf("map1:'%s'\n", argv[3]);
	printf("map2:'%s'\n", argv[4]);

	ZMap output_map;
	ZMap map1;
	ZMap map2;
	int direction;

	if(!strcmp("vertical", argv[2]))
		direction = VERTICAL;
	else if(!strcmp("horizontal", argv[2]))
		direction = HORIZONTAL;
	else
	{
		printf("acceptable directions are 'vertical' or 'horizontal'\n");
		system("PAUSE");
		return 1;
	}

	//load maps
	if(!map1.Read(argv[3]))
	{
		printf("could not load '%s'\n", argv[3]);
		system("PAUSE");
		return 1;
	}

	if(!map2.Read(argv[4]))
	{
		printf("could not load '%s'\n", argv[4]);
		system("PAUSE");
		return 1;
	}

	int new_w, new_h;

	//create output
	if(direction == VERTICAL)
	{
		new_h = map1.GetMapBasics().height + map2.GetMapBasics().height;
		new_w = map1.GetMapBasics().width;
		if(new_w < map2.GetMapBasics().width) new_w = map2.GetMapBasics().width;
	}
	else
	{
		new_w = map1.GetMapBasics().width + map2.GetMapBasics().width;
		new_h = map1.GetMapBasics().height;
		if(new_h < map2.GetMapBasics().height) new_h = map2.GetMapBasics().height;
	}
	output_map.MakeNewMap("merged_map", (planet_type)map1.GetMapBasics().terrain_type, new_w, new_h);

	//load output
	//tiles
	{
		//map1
		for(int i=0;i<map1.GetMapBasics().width;i++)
			for(int j=0;j<map1.GetMapBasics().height;j++)
			{
				int out_index;
				int m_index;

				//map 1
				m_index = (j * map1.GetMapBasics().width) + i;
				out_index = (j * output_map.GetMapBasics().width) + i;
				output_map.ChangeTile(out_index, map1.GetTile(m_index));
			}

		//map2
		for(int i=0;i<map2.GetMapBasics().width;i++)
			for(int j=0;j<map2.GetMapBasics().height;j++)
			{
				int out_index;
				int m_index;

				//map 1
				m_index = (j * map2.GetMapBasics().width) + i;
				if(direction == VERTICAL)
					out_index = ((j + map1.GetMapBasics().height) * output_map.GetMapBasics().width) + i;
				else
					out_index = (j * output_map.GetMapBasics().width) + (i + map1.GetMapBasics().width);
				output_map.ChangeTile(out_index, map2.GetTile(m_index));
			}
	}

	//objects
	{
		//map1
		for(vector<map_object>::iterator i=map1.GetObjectList().begin(); i!=map1.GetObjectList().end(); i++)
			output_map.PlaceObject(*i);

		//map2
		for(vector<map_object>::iterator i=map2.GetObjectList().begin(); i!=map2.GetObjectList().end(); i++)
		{
			map_object new_object;

			new_object = *i;
			if(direction == VERTICAL)
				new_object.y += map1.GetMapBasics().height;
			else
				new_object.x += map1.GetMapBasics().width;

			output_map.PlaceObject(new_object);
		}
	}

	//zones
	{
		for(vector<map_zone>::iterator i=map1.GetZoneList().begin(); i!=map1.GetZoneList().end(); i++)
			output_map.AddZone(*i);

		for(vector<map_zone>::iterator i=map2.GetZoneList().begin(); i!=map2.GetZoneList().end(); i++)
		{
			map_zone new_zone;

			new_zone = *i;
			if(direction == VERTICAL)
				new_zone.y += map1.GetMapBasics().height;
			else
				new_zone.x += map1.GetMapBasics().width;

			output_map.AddZone(new_zone);
		}
	}

	//save output
	output_map.Write(argv[1]);

	system("PAUSE");

	return 1;
}
