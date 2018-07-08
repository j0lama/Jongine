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
	int x_aux, y_aux, x_aux0, y_aux0, x_aux1, y_aux1;
	int wx0 = 80, wy0 = 40, wx1 = 130, wy1 = 90, wx0_aux, wy0_aux, wx1_aux, wy1_aux;
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

		x_aux = 30*cos(rads);
		y_aux = 30*sin(rads);
		x_aux0 = 30*cos(rads + PI/5);
		y_aux0 = 30*sin(rads + PI/5);
		x_aux1 = 30*cos(rads - PI/5);
		y_aux1 = 30*sin(rads - PI/5);

		/****Draw****/

		/****MiniMap 1****/
		drawRect(renderer, 0, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);

		/*Player in MiniMap*/
		#ifdef _DEBUG
		drawLine(renderer, x, y, x + x_aux0, y + y_aux0, 0, 0, 0, SDL_ALPHA_OPAQUE);
		drawLine(renderer, x, y, x + x_aux1, y + y_aux1, 0, 0, 0, SDL_ALPHA_OPAQUE);
		#else
		drawLine(renderer, x, y, x + x_aux, y + y_aux, 0, 0, 0, SDL_ALPHA_OPAQUE);
		#endif
		drawRect(renderer, x-5, y-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);

		/*Wall in MiniMap*/
		drawLine(renderer, wx0, wy0, wx1, wy1, 0, 0, 255, SDL_ALPHA_OPAQUE);


		/****MiniMap 2****/
		drawRect(renderer, MINI_MAP_HEIGHT, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);

		wx0_aux = wx0 - x;
		wy0_aux = wy0 - y;
		wx1_aux = wx1 - x;
		wy1_aux = wy1 - y;

		int tz1, tz2;

		tz1 = wx0_aux*cos(rads) + wy0_aux*sin(rads);
		tz2 = wx1_aux*cos(rads) + wy1_aux*sin(rads);
		wx0_aux = wx0_aux*sin(rads) - wy0_aux*cos(rads);
		wx1_aux = wx1_aux*sin(rads) - wy1_aux*cos(rads);

		/*Wall in MiniMap*/
		drawLine(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2 - wx0_aux, MINI_MAP_HEIGHT/2 - tz1, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2 - wx1_aux, MINI_MAP_HEIGHT/2 - tz2, 0, 0, 255, SDL_ALPHA_OPAQUE);
		/*PLayer in MiniMap*/
		drawLine(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2, MINI_MAP_HEIGHT/2, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2, MINI_MAP_HEIGHT/2 - 30, 0, 0, 0, SDL_ALPHA_OPAQUE);
		drawRect(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2-5, MINI_MAP_HEIGHT/2-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);


		/****MiniMap 3****/
		//drawRect(renderer, MINI_MAP_HEIGHT, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);


		/*MiniMap Separator 1*/
		drawLine(renderer, MINI_MAP_WIDTH, 0, MINI_MAP_WIDTH, MINI_MAP_HEIGHT, 0, 0, 0, SDL_ALPHA_OPAQUE);

		/*MiniMap Separator 2*/
		drawLine(renderer, 2*MINI_MAP_WIDTH, 0, 2*MINI_MAP_WIDTH, MINI_MAP_HEIGHT, 0, 0, 0, SDL_ALPHA_OPAQUE);



		/*Update screen*/
		updateRender(renderer);
	}

	quit:
	destroySDL(renderer);

	return 0;
}
