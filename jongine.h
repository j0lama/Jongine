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


SDL_Renderer * initSDL(char * windowName, int width, int height);
void drawRect(SDL_Renderer * renderer, int x, int y, int height, int width, int r, int g, int b, int a);
void drawCircle(SDL_Renderer * renderer, int x, int y, double radius, int r, int g, int b, int a);
void drawLine(SDL_Renderer * renderer, int x0, int y0, int x1, int y1, int r, int g, int b, int a);
void draw3DWall(SDL_Renderer * renderer, int px, int py, double alpha, Wall * wall);
void setBackgroundColor(SDL_Renderer * renderer, int r, int g, int b, int a);
void updateRender(SDL_Renderer * renderer);
void destroySDL(SDL_Renderer * renderer);


#endif
