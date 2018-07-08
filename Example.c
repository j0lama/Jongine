#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "jongine.h"

#define MINI_MAP_HEIGHT 160
#define MINI_MAP_WIDTH 160

#define WALL_HEIGHT 100

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

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


int main(int argc, char const *argv[])
{
	SDL_Event e;
	SDL_Renderer * renderer = NULL;
	int x = MINI_MAP_WIDTH/2;
	int y = MINI_MAP_HEIGHT/2;
	int dx = 0;
	int dy = 0;
	int x_aux, y_aux;
	int wx0 = 40, wy0 = 40, wx1 = 130, wy1 = 40, wx0_aux, wy0_aux, wx1_aux, wy1_aux;
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
					d_alpha = -e.motion.xrel;
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

		/****Draw****/

		/****MiniMap 1****/
		drawRect(renderer, 0, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);

		/*Player in MiniMap*/
		drawLine(renderer, x, y, x + x_aux, y + y_aux, 0, 0, 0, SDL_ALPHA_OPAQUE);
		drawRect(renderer, x-5, y-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);

		/*Wall in MiniMap*/
		drawLine(renderer, wx0, wy0, wx1, wy1, 0, 0, 255, SDL_ALPHA_OPAQUE);


		/****MiniMap 2****/
		drawRect(renderer, MINI_MAP_WIDTH, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);

		wx0_aux = wx0 - x;
		wy0_aux = wy0 - y;
		wx1_aux = wx1 - x;
		wy1_aux = wy1 - y;

		int tz1, tz2;

		tz1 = (wx0 - x)*cos(rads) + (wy0 - y)*sin(rads);
		tz2 = (wx1 - x)*cos(rads) + (wy1 - y)*sin(rads);
		wx0_aux = (wx0 - x)*sin(rads) - (wy0 - y)*cos(rads);
		wx1_aux = (wx1 - x)*sin(rads) - (wy1 - y)*cos(rads);

		/*Wall in MiniMap*/
		drawLine(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2 - wx0_aux, MINI_MAP_HEIGHT/2 - tz1, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2 - wx1_aux, MINI_MAP_HEIGHT/2 - tz2, 0, 0, 255, SDL_ALPHA_OPAQUE);
		/*PLayer in MiniMap*/
		drawLine(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2, MINI_MAP_HEIGHT/2, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2, MINI_MAP_HEIGHT/2 - 30, 0, 0, 0, SDL_ALPHA_OPAQUE);
		drawRect(renderer, MINI_MAP_WIDTH + MINI_MAP_WIDTH/2-5, MINI_MAP_HEIGHT/2-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);


		/****MiniMap 3****/
		int x1, x2, y1a, y2a, y1b, y2b;
		int ix1, ix2, iz1, iz2;

		if(tz1 > 0 || tz2 > 0)
		{
			intersec(wx0_aux, tz1, wx1_aux, tz2, -0.0001, 0.0001, -100, 5, &ix1, &iz1);
			intersec(wx0_aux, tz1, wx1_aux, tz2, 0.0001, 0.0001, 100, 5, &ix2, &iz2);
			if(tz1 <= 0)
			{
				if(iz1 > 0)
				{
					wx0_aux = ix1;
					tz1 = iz1;
				}
				else
				{
					wx0_aux = ix2;
					tz1 = iz2;
				}
			}
			if(tz2 <= 0)
			{
				if(iz1 > 0)
				{
					wx1_aux = ix1;
					tz2 = iz1;
				}
				else
				{
					wx1_aux = ix2;
					tz2 = iz2;
				}
			}

			if(tz1 != 0)
			{
				x1 = -wx0_aux*16/tz1;
				y1a = -(WINDOW_HEIGHT/2)/tz1;
				y1b = (WINDOW_HEIGHT/2)/tz1;
			}
			if(tz2 != 0)
			{
				x2 = -wx1_aux*16/tz2;
				y2a = -(WINDOW_HEIGHT/2)/tz2;
				y2b = (WINDOW_HEIGHT/2)/tz2;
			}

			drawLine(renderer, WINDOW_WIDTH/2 + x1, WINDOW_HEIGHT/2 + y1a, WINDOW_WIDTH/2 + x2, WINDOW_HEIGHT/2 + y2a, 0, 0, 255, SDL_ALPHA_OPAQUE); //top
			drawLine(renderer, WINDOW_WIDTH/2 + x1, WINDOW_HEIGHT/2 + y1b, WINDOW_WIDTH/2 + x2, WINDOW_HEIGHT/2 + y2b, 0, 0, 255, SDL_ALPHA_OPAQUE); //bottom
			drawLine(renderer, WINDOW_WIDTH/2 + x1, WINDOW_HEIGHT/2 + y1a, WINDOW_WIDTH/2 + x1, WINDOW_HEIGHT/2 + y1b, 255, 0, 0, SDL_ALPHA_OPAQUE); //left
			drawLine(renderer, WINDOW_WIDTH/2 + x2, WINDOW_HEIGHT/2 + y2a, WINDOW_WIDTH/2 + x2, WINDOW_HEIGHT/2 + y2b, 255, 0, 0, SDL_ALPHA_OPAQUE); //right

		}


		/*MiniMap Separator 1*/
		drawLine(renderer, MINI_MAP_WIDTH, 0, MINI_MAP_WIDTH, MINI_MAP_HEIGHT, 0, 0, 0, SDL_ALPHA_OPAQUE);



		/*Update screen*/
		updateRender(renderer);
	}

	quit:
	destroySDL(renderer);

	return 0;
}
