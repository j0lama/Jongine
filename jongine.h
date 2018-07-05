#ifndef JONGINE_H
#define JONGINE_H

#define PI 3.14159265358979323846


SDL_Renderer * initSDL(char * windowName, int width, int height);
void drawRect(SDL_Renderer * renderer, int x, int y, int height, int width, int r, int g, int b, int a);
void drawCircle(SDL_Renderer * renderer, int x, int y, double radius, int r, int g, int b, int a);
void drawLine(SDL_Renderer * renderer, int x0, int y0, int x1, int y1, int r, int g, int b, int a);
void setBackgroundColor(SDL_Renderer * renderer, int r, int g, int b, int a);
void updateRender(SDL_Renderer * renderer);
void destroySDL(SDL_Renderer * renderer);


#endif
