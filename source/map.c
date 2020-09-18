#include <stdio.h>
#include <stdlib.h>
#include "map.h"

#define MAP_NAME 32

struct _Map
{
	char name[MAP_NAME];
	int map_id;
	int ** map;
	int height;
	int width;
	int entrance[2];
	float color[3];
};

Map * loadMap(char * map_path)
{
	FILE * fp;
	Map * map;
	int i, j;

	map = malloc(sizeof(Map));

	fp = fopen(map_path, "r");
	if(fp == NULL)
		return NULL;

	/* Get map name */
	fscanf(fp, "%s\n", map->name);
	/* Get map ID */
	fscanf(fp, "%d\n", &map->map_id);
	fscanf(fp, "%f %f %f", &map->color[0], &map->color[1], &map->color[2]);
	/* Get map dimensions */
	fscanf(fp, "%d %d", &map->width, &map->height);
	/* Get map entrance */
	fscanf(fp, "%d %d", &map->entrance[0], &map->entrance[1]);

	/* Allocate the map */
	map->map = malloc(map->height*sizeof(int*));
	for(j = 0; j < map->height; j++)
	{
		map->map[j] = malloc(map->width*sizeof(int));
		for(i = 0; i < map->width; i++)
		{
			fscanf(fp, "%d", &map->map[j][i]);
		}
	}

	/* Close file */
	fclose(fp);

	return map;
}

void printMap(Map * map)
{
	int i, j;
	printf("Name: %s\n", map->name);
	printf("Map ID: %d\n", map->map_id);
	printf("Map Color: (%f, %f, %f)\n", map->color[0], map->color[1], map->color[2]);
	printf("Dimensions: (%d, %d)\n", map->width, map->height);
	printf("Entrance: (%d, %d)\n", map->entrance[0], map->entrance[1]);
	printf("Map:\n");
	for(j = 0; j < map->height; j++)
	{
		for(i = 0; i < map->width; i++)
			printf("%d ", map->map[j][i]);
		printf("\n");
	}
}

char * get_map_name(Map * map)
{
	return map->name;
}

int get_map_id(Map * map)
{
	return map->map_id;
}

int get_map_width(Map * map)
{
	return map->width;
}

int get_map_height(Map * map)
{
	return map->height;
}

int get_map_entrance_x(Map * map)
{
	return map->entrance[0];
}

int get_map_entrance_y(Map * map)
{
	return map->entrance[1];
}

int get_map_cell(Map * map, int row, int column)
{
	if(column < 0 || column >= map->width || row < 0 || row >= map->height)
		return -1;
	return map->map[row][column];
}

float * get_map_color(Map * map)
{
	return map->color;
}
