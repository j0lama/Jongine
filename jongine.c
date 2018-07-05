#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "jongine.h"

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

void getScreenDimensions(int * height, int * width)
{
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	*height = DM.h;
	*width = DM.w;
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
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
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
