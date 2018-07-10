#ifndef JONGINE_H
#define JONGINE_H

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

typedef struct _Map Map;
typedef struct _Wall Wall;


void * initSDL(char * windowName, int width, int height);
Map * readMap(char * mapfile);
void runGame(void * renderer, Map * map);
void destroyMap(Map * map);


#endif
