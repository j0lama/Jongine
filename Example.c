#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "jongine.h"

int main(int argc, char const *argv[])
{
	SDL_Event e;
	SDL_Renderer * renderer = NULL;
	int x = MINI_MAP_WIDTH/2;
	int y = MINI_MAP_HEIGHT/2;
	double dx = 0;
	double dy = 0;
	double alpha = 0.0, rads = 0.0;
	int d_alpha = 0;

	/*Wall 1*/
	int wx0 = 40, wy0 = 40, wx1 = 130, wy1 = 40;
	/*Wall 2*/
	int wx0_ = 40, wy0_ = 40, wx1_ = 40, wy1_ = 130;
	/*Wall 3*/
	int wx0__ = 40, wy0__ = 130, wx1__ = 130, wy1__ = 130;

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
						case SDLK_UP: dy += 2*sin(rads); dx += 2*cos(rads); break;
						case SDLK_DOWN: dy += -2*sin(rads); dx += -2*cos(rads); break;
						case SDLK_RIGHT: dx += -2*cos(rads - PI/2); dy += -2*sin(rads - PI/2); break;
						case SDLK_LEFT: dx += -2*cos(rads + PI/2); dy += -2*sin(rads + PI/2); break;
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
					dx = dy = 0;
					d_alpha = e.motion.xrel/2;
					break;
			}
		}

		/*Draw in the screen*/
		/*Black background*/
		setBackgroundColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		if(dx >= 1) {
			x += 1;
			dx = 0;
		}
		if(dx <= -1) {
			x -= 1;
			dx = 0;
		}
		if(x < 0) x = 0;
		if(x > MINI_MAP_WIDTH) x = MINI_MAP_WIDTH;
		if(dy >= 1) {
			y += 1;
			dy = 0;
		}
		if(dy <= -1) {
			y -= 1;
			dy = 0;
		}
		if(y < 0) y = 0;
		if(y > MINI_MAP_HEIGHT) y = MINI_MAP_HEIGHT;

		alpha += d_alpha;
		rads = alpha * (PI/180);
		#ifdef _MAP
		drawRect(renderer, 0, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);
		drawRect(renderer, MINI_MAP_WIDTH, 0, MINI_MAP_HEIGHT, MINI_MAP_WIDTH, 255, 255, 255, SDL_ALPHA_OPAQUE);
		#endif

		setColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		draw3DWall(renderer, x, y, alpha, wx0, wy0, wx1, wy1);
		setColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		draw3DWall(renderer, x, y, alpha, wx0_, wy0_, wx1_, wy1_);
		setColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
		draw3DWall(renderer, x, y, alpha, wx0__, wy0__, wx1__, wy1__);

		#ifdef _MAP
		drawLine(renderer, MINI_MAP_WIDTH, 0, MINI_MAP_WIDTH, MINI_MAP_HEIGHT, 0, 0, 0, SDL_ALPHA_OPAQUE);
		#endif

		/*Update screen*/
		updateRender(renderer);
	}

	quit:
	destroySDL(renderer);

	return 0;
}
