#include <stdio.h>
#include "jongine.h"

int main(int argc, char const *argv[])
{
	void * renderer = NULL;
	Map * map;

	/*Creating the renderer*/
	renderer = initSDL("Jongine", 640, 480);
	if(renderer == NULL)
	{
		fprintf(stderr, "Error in initSDL");
		return 1;
	}

	map = readMap("map.txt");

	runGame(renderer, map);

	destroyMap(map);

	return 0;
}
