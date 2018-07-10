#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "jongine.h"

struct _Wall 
{
	int x0;
	int y0;
	int x1;
	int y1;
	int z;
	int r, g, b, a;
};

Wall * newWall(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	Wall * wall;
	wall = (Wall *) malloc(sizeof(Wall));
	wall->x0 = x0;
	wall->y0 = y0;
	wall->x1 = x1;
	wall->y1 = y1;
}

SDL_Renderer * initSDL(char * windowName, int width, int height)
{
	SDL_Window * window = NULL;
	SDL_Renderer * renderer = NULL;

	/*Init SDL*/
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		/*Error in SDL initialization*/
		fprintf(stderr, "Cannot initialise SDL: %s\n", SDL_GetError());
		return NULL;
	}

	/*Creating a window*/
	window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if(window == NULL)
	{
		fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
		return NULL;
	}

	/*Creating a renderer*/
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(renderer == NULL)
	{
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
		return NULL;
	}

	/*Setting the BlendMode*/
	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0)
	{
    	fprintf(stderr, "Error setting BlendMode: %s\n", SDL_GetError());
    	return NULL;
  	}

  	/*Trapping mouse in the screen*/
  	SDL_SetRelativeMouseMode(SDL_TRUE);

  	return renderer;
}

void drawCircle(SDL_Renderer * renderer, int x, int y, double radius, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r,g,b,a);

	SDL_RenderDrawPoint(renderer, x, y);
}

void drawRect(SDL_Renderer * renderer, int x, int y, int height, int width, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r,g,b,a);
	SDL_Rect rect = { x, y, width, height};
	SDL_RenderFillRect(renderer, &rect);
}

void drawLine(SDL_Renderer * renderer, int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
}

int cross_product(int x1, int y1, int x2, int y2)
{
	return (x1*y2) - (x2 * y1);
}

void intersec(int x1, int y1, int x2, int y2, float nearside, float nearz, float farside, float farz, int * x, int * y)
{
	int det, x_aux, y_aux;
	x_aux = cross_product(x1, y1, x2, y2);
	y_aux = cross_product(nearside, nearz, farside, farz);
	det = cross_product(x1-x2, y1-y2, nearside-farside, nearz-farz);
	if(det == 0)
		return;
	*x = cross_product(x_aux, x1-x2, y_aux, nearside-farside) / det;
	*y = cross_product(x_aux, y1-y2, y_aux, nearz-farz) / det;
}

void draw3DWall(SDL_Renderer * renderer, int px, int py, double alpha, int x0, int y0, int x1, int y1)
{
	int tz1 = 0, tz2 = 0, wx0_aux = 0, wx1_aux = 0, x_1 = 0, x_2 = 0, y1a = 0, y2a = 0, y1b = 0, y2b = 0, ix1 = 0, ix2 = 0, iz1 = 0, iz2 = 0;
	double rads = 0.0;

	/*Convert angle to radians*/
	rads = alpha * (PI/180);

	/*Draw MiniMap 1 and Player*/
	#ifdef _MAP
	/*Player in MiniMap*/
	drawLine(renderer, px, py, px + 30*cos(rads), py + 30*sin(rads), 0, 0, 0, SDL_ALPHA_OPAQUE);
	drawRect(renderer, px-5, py-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);
	/*Wall in MiniMap*/
	drawLine(renderer, x0, y0, x1, y1, 0, 0, 255, SDL_ALPHA_OPAQUE);
	#endif

	tz1 = (x0 - px)*cos(rads) + (y0 - py)*sin(rads);
	tz2 = (x1 - px)*cos(rads) + (y1 - py)*sin(rads);
	wx0_aux = (x0 - px)*sin(rads) - (y0 - py)*cos(rads);
	wx1_aux = (x1 - px)*sin(rads) - (y1 - py)*cos(rads);

	/*Draw MiniMap 2 and Player*/
	#ifdef _MAP
	/*Wall in MiniMap*/
	drawLine(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2 - wx0_aux, MINI_MAP_HEIGHT/2 - tz1, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2 - wx1_aux, MINI_MAP_HEIGHT/2 - tz2, 0, 0, 255, SDL_ALPHA_OPAQUE);
	/*Player in MiniMap*/
	drawLine(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2, MINI_MAP_HEIGHT/2, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2, MINI_MAP_HEIGHT/2 - 30, 0, 0, 0, SDL_ALPHA_OPAQUE);
	drawRect(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2-5, MINI_MAP_HEIGHT/2-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);
	#endif

	if(tz1 > 0 || tz2 > 0) {
		intersec(wx0_aux, tz1, wx1_aux, tz2, -0.0001, 0.0001, -100, 20, &ix1, &iz1);
		intersec(wx0_aux, tz1, wx1_aux, tz2, 0.0001, 0.0001, 100, 20, &ix2, &iz2);
		if(tz1 <= 0) {
			if(iz1 > 0) {
				wx0_aux = ix1;
				tz1 = iz1;
			}
			else {
				wx0_aux = ix2;
				tz1 = iz2;
			}
		}
		if(tz2 <= 0) {
			if(iz1 > 0) {
				wx1_aux = ix1;
				tz2 = iz1;
			}
			else {
				wx1_aux = ix2;
				tz2 = iz2;
			}
		}

		if(tz1 != 0)
		{
			x_1 = -wx0_aux*HFOV/tz1;
			y1a = -(WINDOW_HEIGHT*VFOV)/tz1;
			y1b = (WINDOW_HEIGHT*VFOV)/tz1;
		}
		if(tz2 != 0)
		{
			x_2 = -wx1_aux*HFOV/tz2;
			y2a = -(WINDOW_HEIGHT*VFOV)/tz2;
			y2b = (WINDOW_HEIGHT*VFOV)/tz2;
		}

		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1a, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2a); //top
		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1b, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2b); //bottom
		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1a, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1b); //left
		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2a, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2b); //right
	}
	return;
}

void setColor(SDL_Renderer * renderer, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r,g,b,a);
}

void setBackgroundColor(SDL_Renderer * renderer, int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderClear(renderer);
}

void updateRender(SDL_Renderer * renderer)
{
	SDL_RenderPresent(renderer);
}

void destroySDL(SDL_Renderer * renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}
