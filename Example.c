#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "jongine.h"

#define MINI_MAP_HEIGHT 160
#define MINI_MAP_WIDTH 160

#define WALL_HEIGHT 100

int main(int argc, char const *argv[])
{
	SDL_Event e;
	SDL_Renderer * renderer = NULL;
	int x = MINI_MAP_WIDTH/2;
	int y = MINI_MAP_HEIGHT/2;
	int dx = 0;
	int dy = 0;
	int x_aux0, y_aux0, x_aux1, y_aux1;
	double alpha = 0.0;
	int d_alpha = 0;
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
		/*Alpha increment to 0 to avoid angle movements*/
		d_alpha = 0;

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
						case SDLK_a: d_alpha = -4; break;
						case SDLK_d: d_alpha = 4; break;
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
				case SDL_MOUSEMOTION:
					d_alpha = e.motion.xrel;
					break;
			}
		}

		/*Draw in the screen*/
		/*Black background*/
		setBackgroundColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

		x += dx;
		if(x < 0) x = 0;
		if(x > MINI_MAP_WIDTH) x = MINI_MAP_WIDTH;
		y += dy;
		if(y < 0) y = 0;
		if(y > MINI_MAP_HEIGHT) y = MINI_MAP_HEIGHT;

		alpha += d_alpha;
		rads = alpha * (PI/180);

		x_aux0 = 30*cos(rads + PI/5);
		y_aux0 = 30*sin(rads + PI/5);
		x_aux1 = 30*cos(rads - PI/5);
		y_aux1 = 30*sin(rads - PI/5);

		/****Draw****/

		/*MiniMap*/
		drawRect(renderer, 0, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);

		/*Player in MiniMap*/
		drawLine(renderer, x, y, x + x_aux0, y + y_aux0, 0, 0, 0, SDL_ALPHA_OPAQUE);
		drawLine(renderer, x, y, x + x_aux1, y + y_aux1, 0, 0, 0, SDL_ALPHA_OPAQUE);
		drawRect(renderer, x-5, y-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);

		/*Wall in MiniMap*/
		drawLine(renderer, 80, 40, 130, 90, 0, 0, 255, SDL_ALPHA_OPAQUE);

		/*Update screen*/
		updateRender(renderer);
	}

	quit:
	destroySDL(renderer);

	return 0;
}
