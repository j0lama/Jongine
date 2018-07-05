#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#define PI 3.14159265358979323846

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

  	return renderer;
}

int main(int argc, char const *argv[])
{
	SDL_Event e;
	SDL_Renderer * renderer = NULL;
	int x = 640/2;
	int y = 480/2;
	int dx = 0;
	int dy = 0;
	int x_aux = 0;
	int y_aux = 20;
	double alpha = 0.0;
	double d_alpha = 0.0;
	double rads;

	/*Creating the renderer*/
	renderer = initSDL("Jongine", 640, 480);
	if(renderer == NULL)
	{
		fprintf(stderr, "Error in initSDL");
		return 1;
	}

	while(1)
	{
		/*Detect buttons*/
		while(SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
					goto quit;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
						case SDLK_ESCAPE: goto quit;
						case SDLK_UP: dy = -1; break;
						case SDLK_RIGHT: dx = 1; break;
						case SDLK_DOWN: dy = 1; break;
						case SDLK_LEFT: dx = -1; break;
						case SDLK_a: d_alpha = -2; break;
						case SDLK_d: d_alpha = 2; break;
					}
					break;
				case SDL_KEYUP:
					switch (e.key.keysym.sym)
					{
						case SDLK_UP: dy = 0; break;
						case SDLK_RIGHT: dx = 0; break;
						case SDLK_DOWN: dy = 0; break;
						case SDLK_LEFT: dx = 0; break;
						case SDLK_a: d_alpha = 0; break;
						case SDLK_d: d_alpha = 0; break;
					}
					break;
			}
		}

		/*Draw in the screen*/
		/*Black background*/
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

		SDL_RenderClear(renderer);

		x += dx;
		if(x < 0) x = 0;
		if(x > 640) x = 640;
		y += dy;
		if(y < 0) y = 0;
		if(y > 480) x = 480;

		alpha += d_alpha;
		rads = alpha * (PI/180);
		x_aux = 50*cos(rads);
		y_aux = 50*sin(rads);

		/*Draw*/

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, x, y, x + x_aux, y + y_aux);

		drawRect(renderer, x-5, y-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);

		/*Update screen*/
		SDL_RenderPresent(renderer);
	}

	quit:
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	return 0;
}