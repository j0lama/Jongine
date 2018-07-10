#ifndef JONGINE_H
#define JONGINE_H

#define PI 3.14159265358979323846
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MINI_MAP_HEIGHT 160
#define MINI_MAP_WIDTH 160
#define HFOV 75
#define VFOV 2

typedef struct _Map Map;
typedef struct _Wall Wall;


void * initSDL(char * windowName, int width, int height);
Map * readMap(char * mapfile);
void runGame(void * renderer, Map * map);
void destroySDL(void * renderer);
void destroyMap(Map * map);


#endif
