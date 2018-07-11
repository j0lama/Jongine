#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "jongine.h"

#define PI 3.14159265358979323846
#define HFOV 75
#define VFOV 2

#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define max(a,b)	(((a) > (b)) ? (a) : (b))

struct _Map
{
	int x, y;
	int px, py;
	double alpha;
	int wallsNumber;
	Wall ** walls;
};

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
	wall->r = r;
	wall->g = g;
	wall->b = b;
	wall->a = a;
	return wall;
}

void destryWall(Wall * wall)
{
	if(wall == NULL)
		return;
	free(wall);
}

Map * readMap(char * mapfile)
{
	FILE * file;
	Map * map;
	int x0 = 0, y0 = 0, x1 = 0, y1 = 0, r = 0, g = 0, b = 0, a = 0;
	int wallsNumber = 0;
	char line[128];

	file = fopen(mapfile, "r");
	if(file == NULL)
		return NULL;
	map = (Map *) malloc(sizeof(Map));
	#ifdef _DEBUG
	printf("Map debug info:\n");
	#endif
	while(fgets(line, 128, file) != NULL)
	{
		if (sscanf(line,"mapsize\t%d\t%d",&map->x, &map->y))
		{
			#ifdef _DEBUG
			printf("\tmapsize x:%d y:%d\n", map->x, map->y);
			#endif
		}
		if (sscanf(line,"player\t%d\t%d",&map->px, &map->py))
		{
			#ifdef _DEBUG
			printf("\tplayer px:%d py:%d\n", map->px, map->py);
			#endif
		}
		if (sscanf(line,"wallsnumber\t%d",&map->wallsNumber))
		{
			#ifdef _DEBUG
			printf("\twallsnumber %d\n", map->wallsNumber);
			#endif
			map->walls = (Wall **)malloc(map->wallsNumber*sizeof(Wall *));
		}
		if (sscanf(line,"wall\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",&x0, &y0, &x1, &y1, &r, &g, &b, &a))
		{
			if(wallsNumber >= map->wallsNumber)
				break;
			#ifdef _DEBUG
			printf("\twall x0:%d y0:%d x1:%d y1:%d r:%d g:%d b:%d a:%d\n", x0, y0, x1, y1, r, g, b, a);
			#endif
			map->walls[wallsNumber] = newWall(x0, y0, x1, y1, r, g, b, a);
			wallsNumber++;
		}
		bzero(line, 128);
	}
	map->alpha = 0;
	return map;
}

void destroyMap(Map * map)
{
	int i = 0;
	if(map == NULL)
		return;
	for(i = 0; i < map->wallsNumber; i++)
	{
		destryWall(map->walls[i]);
	}
	free(map->walls);
	free(map);
}

void * initSDL(char * windowName, int width, int height)
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

  	/*Casting to void*/
  	return (void *) renderer;
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

void drawFilledWall(SDL_Renderer * renderer, int x1, int x2, int y1a, int y2a, int y1b, int y2b)
{
	int i = 0;
	float m1 = 0.0, m2 = 0.0;
	float b1 = 0, b2 = 0;

	m1 = (float)(y2a-y1a)/(x2-x1);
	b1 = y1a - x1*m1;

	m2 = (float)(y2b-y1b)/(x2-x1);
	b2 = y1b - x1*m2;

	if(x1 < x2)
	{
		for(i = x1; i <= x2; i++)
		{
			SDL_RenderDrawLine(renderer, i, (int)i*m1+b1, i, (int)i*m2+b2);
		}
	}
	else
	{
		for(i = x2; i <= x1; i++)
		{
			SDL_RenderDrawLine(renderer, i, (int)i*m1+b1, i, (int)i*m2+b2);
		}
	}
	
}

void draw3DWall(SDL_Renderer * renderer, int px, int py, double alpha, Wall * wall, Map * map)
{
	int x0, y0, x1, y1;
	int tz1 = 0, tz2 = 0, wx0_aux = 0, wx1_aux = 0, x_1 = 0, x_2 = 0, y1a = 0, y2a = 0, y1b = 0, y2b = 0, ix1 = 0, ix2 = 0, iz1 = 0, iz2 = 0;
	double rads = 0.0;

	if(wall == NULL)
		return;

	x0 = wall->x0;
	y0 = wall->y0;
	x1 = wall->x1;
	y1 = wall->y1;

	rads = alpha;

	/*Draw MiniMap 1 and Player*/
	#ifdef _DEBUG
	/*Player in MiniMap*/
	drawLine(renderer, px, py, px + 30*cos(rads), py + 30*sin(rads), 0, 0, 0, SDL_ALPHA_OPAQUE);
	drawRect(renderer, px-5, py-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);
	/*Wall in MiniMap*/
	drawLine(renderer, x0, y0, x1, y1, 0, 0, 0, SDL_ALPHA_OPAQUE);
	#endif

	tz1 = (x0 - px)*cos(rads) + (y0 - py)*sin(rads);
	tz2 = (x1 - px)*cos(rads) + (y1 - py)*sin(rads);
	wx0_aux = (x0 - px)*sin(rads) - (y0 - py)*cos(rads);
	wx1_aux = (x1 - px)*sin(rads) - (y1 - py)*cos(rads);

	/*Draw MiniMap 2 and Player*/
	#ifdef _DEBUG
	/*Wall in MiniMap*/
	drawLine(renderer, map->x + map->x/2 - wx0_aux, map->y/2 - tz1, map->x + map->x/2 - wx1_aux, map->y/2 - tz2, 0, 0, 0, SDL_ALPHA_OPAQUE);
	/*Player in MiniMap*/
	drawLine(renderer, map->x + map->x/2, map->y/2, map->x + map->x/2, map->y/2 - 30, 0, 0, 0, SDL_ALPHA_OPAQUE);
	drawRect(renderer, map->x + map->x/2-5, map->y/2-5, 10, 10, 0, 255, 0, SDL_ALPHA_OPAQUE);
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

		/*Setting wall color*/
		SDL_SetRenderDrawColor(renderer, wall->r, wall->g, wall->b, wall->a);

		drawFilledWall(renderer, WINDOW_WIDTH/2 + x_1, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y1a, WINDOW_HEIGHT/2 + y2a, WINDOW_HEIGHT/2 + y1b, WINDOW_HEIGHT/2 + y2b);

		/*Setting to black the wall margin*/
		//SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1a, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2a); //top
		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1b, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2b); //bottom
		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1a, WINDOW_WIDTH/2 + x_1, WINDOW_HEIGHT/2 + y1b); //left
		SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2a, WINDOW_WIDTH/2 + x_2, WINDOW_HEIGHT/2 + y2b); //right
	}
	return;
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

double getDistance(Map * map, int x, int y)
{
	return sqrt((map->px - x)*(map->px - x) + (map->py - y)*(map->py - y));
}

void drawMap(SDL_Renderer * renderer, Map * map)
{
	int i = 0;
	if(map == NULL)
		return;
	Wall * walls[map->wallsNumber];
	int iter = 0;
	int permutation = 1;
	Wall * aux;
	/*3D rendering algorithm*/
	/*Painter's algorithm, Raycasting or binary space partition*/

	for(i = 0; i < map->wallsNumber; i++)
	{
		walls[i] = map->walls[i];
	}

	while ( permutation == 1) {
		permutation = 0;
		iter++;
		for (i=0;i<map->wallsNumber-iter;i++) {
			if (min(getDistance(map, walls[i]->x0, walls[i]->y0), getDistance(map, walls[i]->x1, walls[i]->y1)) < max(getDistance(map, walls[i+1]->x0, walls[i+1]->y0), getDistance(map, walls[i+1]->x1, walls[i+1]->y1))){
				permutation = 1;
				aux = walls[i];
				walls[i] = walls[i+1];
				walls[i+1] = aux;
			}
        }
    }

	/*Draw all walls*/
	for(i = 0; i < map->wallsNumber; i++)
	{
		draw3DWall(renderer, map->px, map->py, map->alpha, walls[i], map);
	}
}

void runGame(void * renderer, Map * map)
{
	SDL_Event e;
	double dx = 0;
	double dy = 0;
	double alpha = 0.0, rads = 0.0;
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
						case SDLK_UP: dy += 2*sin(rads); dx += 2*cos(rads); break;
						case SDLK_DOWN: dy += -2*sin(rads); dx += -2*cos(rads); break;
						case SDLK_RIGHT: dx += -2*cos(rads - PI/2); dy += -2*sin(rads - PI/2); break;
						case SDLK_LEFT: dx += -2*cos(rads + PI/2); dy += -2*sin(rads + PI/2); break;
					}
					break;
				case SDL_KEYUP:
					switch (e.key.keysym.sym)
					{
						case SDLK_UP: dy = 0; break;
						case SDLK_RIGHT: dx = 0; break;
						case SDLK_DOWN: dy = 0; break;
						case SDLK_LEFT: dx = 0; break;
					}
					break;
				case SDL_MOUSEMOTION:
					dx = dy = 0;
					alpha += e.motion.xrel/5;
					break;
			}
		}

		/*Draw in the screen*/
		/*Black background*/
		setBackgroundColor((SDL_Renderer *) renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		if(dx >= 1) {
			map->px += 1;
			dx = 0;
		}
		if(dx <= -1) {
			map->px -= 1;
			dx = 0;
		}
		if(map->px < 0) map->px = 0;
		if(map->px > map->x) map->px = map->x;
		if(dy >= 1) {
			map->py += 1;
			dy = 0;
		}
		if(dy <= -1) {
			map->py -= 1;
			dy = 0;
		}
		if(map->py < 0) map->py = 0;
		if(map->py > map->y) map->py = map->y;

		/*Convert alpha to radians*/
		rads = alpha * (PI/180);
		map->alpha = rads;
		drawMap((SDL_Renderer *) renderer, map);

		/*Update screen*/
		updateRender((SDL_Renderer *) renderer);
	}

	quit:
	destroySDL((SDL_Renderer *) renderer);
	return;
}
