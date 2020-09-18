#ifndef __map__
#define __map__

#define CELL_SIZE 50

typedef struct _Map Map;

Map * loadMap(char * map_path);
void printMap(Map * map);
char * get_map_name(Map * map);
int get_map_id(Map * map);
int get_map_width(Map * map);
int get_map_height(Map * map);
int get_map_entrance_x(Map * map);
int get_map_entrance_y(Map * map);
int get_map_cell(Map * map, int row, int column);
float * get_map_color(Map * map);

#endif