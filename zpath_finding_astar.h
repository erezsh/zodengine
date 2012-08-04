#ifndef _ZPATH_FINDING_ASTAR_H_
#define _ZPATH_FINDING_ASTAR_H_

#include "zpath_finding.h"

#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//from zpath_finding.h
struct map_pathfinding_info_tile;
class ZPath_Finding_Response;

namespace ZPath_Finding_AStar
{
	class pf_point
	{
	public:
		pf_point() { x = y = 0; }
		pf_point(int x_, int y_) {x=x_; y=y_; f=0; g=0; h=0;}

		int x, y;
		int f, g, h;
		int px, py;
	};

	class pf_point_array
	{
	public:
		pf_point_array()
		{
			list = NULL;
			point_index = NULL;
			size = 0;
			alloc_size = 0;
		}

		inline void InitPointIndex(int w_, int h_)
		{
			int ipi_size;

			w = w_;
			h = h_;

			ipi_size = w * sizeof(int *);
			point_index = (int **)malloc(ipi_size);
			ipi_size = h * sizeof(int);
			for(int i=0;i<w;i++)
			{
				point_index[i] = (int *)malloc(ipi_size);

				for(int j=0;j<h;j++)
					point_index[i][j] = -1;
			}
		}

		inline void FreePointIndex()
		{
			for(int i=0;i<w;i++) free(point_index[i]);
			free(point_index);
		}

		inline void FreeList()
		{
			free(list);
		}

		inline void AddPoint(pf_point &np)
		{
			if(size >= alloc_size)
			{
				alloc_size += 1000;
				list = (pf_point*)realloc(list, alloc_size * sizeof(pf_point));
			}

			list[size] = np;
			point_index[np.x][np.y] = size;

			size++;
		}

		inline void RemovePoint(int x, int y)
		{
			int i = point_index[x][y];

			if(i != -1)
			{
				point_index[x][y] = -1;

				size--;
				list[i] = list[size];

				point_index[list[i].x][list[i].y] = i;
			}
		}

		pf_point* list;
		int **point_index;
		int w, h;
		int size;
		int alloc_size;
	};

	void Do_Astar(ZPath_Finding_Response *response);
};

#endif
