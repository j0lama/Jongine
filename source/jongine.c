#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>
#include <dirent.h>
#include <stdio.h>

#include "map.h"

/* Debug */
void drawPoint(int x, int y)
{
	glColor3f(0,1,0);
	glPointSize(4);
	glLineWidth(2);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


/* Generic functions */
float degToRad(int a)
{
	return a*M_PI/180.0;
}

int truncateAngle(int a)
{
	if(a > 359)
		a -= 360;
	if(a < 0)
		a += 360;
	return a;
}

float getDistance(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}


/* Player */
#define PLAYER_SPEED 8
#define PLAYER_SENSIBILITY 5

typedef struct _Player
{
	float x;
	float y;
	float angle;
	float dx;
	float dy;
}Player;

Player player;

void drawPlayer2D()
{
	/* Draw player */
	glColor3f(1,0,0);
	glPointSize(8);
	glLineWidth(4);
	glBegin(GL_POINTS);
	glVertex2i(player.x, player.y);
	glEnd();
	/* Draw direction */
	glBegin(GL_LINES);
	glVertex2i(player.x, player.y);
	glVertex2i(player.x + player.dx*20, player.y + player.dy*20);
	glEnd();
}


/* Map */

Map ** maps;
int current_map;
#define MAX_SIGHT_DEPTH 8

void drawMap2D()
{
	int x, y, cellx, celly, width, height;

	/* Get map dimensions */
	width = get_map_width(maps[current_map]);
	height = get_map_height(maps[current_map]);

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			if(get_map_cell(maps[current_map], y, x) == 0)
				glColor3f(1,1,1);
			else if(get_map_cell(maps[current_map], y, x) > 1)
				glColor3f(0,1,0);
			else
				glColor3f(0,0,0);
			/* Draw cell */
			cellx = x * CELL_SIZE;
			celly = y * CELL_SIZE;
			glBegin(GL_QUADS);
			glVertex2i(cellx+1, celly+1);
			glVertex2i(cellx+1, CELL_SIZE+celly-1);
			glVertex2i(CELL_SIZE+cellx-1, CELL_SIZE+celly-1);
			glVertex2i(CELL_SIZE+cellx-1, celly+1);
			glEnd();
		}
	}
}

void renderRayMap(float angle, float ray)
{
	float Tan = tan(degToRad(angle));
	float ray_x, ray_y, nray_x, nray_y;
	float h_distance = 1000000, v_distance = 1000000, hit_distance;
	float h_hit_x, h_hit_y, v_hit_x, v_hit_y, hit_x, hit_y;
	int sight_depth, hcellx, hcelly, vcellx, vcelly, cellx, celly;
	float * color = get_map_color(maps[current_map]);
	float colorMult = 1;


	/********************/
	/* Horizontal walls */
	/********************/
	sight_depth = 0;
	/* Looking down */
	if(sin(degToRad(angle)) < - 0.0001)
	{
		ray_y = (((int)player.y / CELL_SIZE) * CELL_SIZE) + CELL_SIZE;
		ray_x = (player.y - ray_y) / Tan + player.x;
		nray_y = CELL_SIZE;
		nray_x = -nray_y / Tan;
	}
	/* Looking up */
	else if(sin(degToRad(angle)) > 0.0001)
	{
		ray_y = ((int)player.y / CELL_SIZE) * CELL_SIZE - 0.0001; /* To fix the MAP hit indexation */
		ray_x = (player.y - ray_y) / Tan + player.x;
		nray_y = -CELL_SIZE;
		nray_x = -nray_y / Tan;
	}
	else
	{
		ray_x = player.x;
		ray_y = player.y;
		sight_depth = MAX_SIGHT_DEPTH;
	}

	while(sight_depth < MAX_SIGHT_DEPTH)
	{
		hcellx = ray_x / CELL_SIZE;
		hcelly = ray_y / CELL_SIZE;
		/* Check ray hit */
		if(get_map_cell(maps[current_map], hcelly, hcellx) == 1)
		{
			/* Get distance to the hit */
			h_distance = getDistance(player.x, player.y, ray_x, ray_y);
			h_hit_x = ray_x;
			h_hit_y = ray_y;
			break;
		}
		ray_x += nray_x;
		ray_y += nray_y;
		sight_depth += 1;
	}

	/******************/
	/* Vertical walls */
	/******************/
	sight_depth = 0;
	/* Looking right */
	if(cos(degToRad(angle)) > 0.0001)
	{
		ray_x = (((int)player.x / CELL_SIZE) * CELL_SIZE) + CELL_SIZE;
		ray_y = (player.x - ray_x) * Tan + player.y;
		nray_x = CELL_SIZE;
		nray_y = -nray_x * Tan;
	}
	/* Looking left */
	else if(cos(degToRad(angle)) < -0.0001)
	{
		ray_x = ((int)player.x / CELL_SIZE) * CELL_SIZE - 0.0001; /* To fix the MAP hit indexation */
		ray_y = (player.x - ray_x) * Tan + player.y;
		nray_x = -CELL_SIZE;
		nray_y = -nray_x * Tan;
	}
	else
	{
		ray_x = player.x;
		ray_y = player.y;
		sight_depth = MAX_SIGHT_DEPTH;
	}

	while(sight_depth < MAX_SIGHT_DEPTH)
	{
		vcellx = ray_x / CELL_SIZE;
		vcelly = ray_y / CELL_SIZE;
		/* Check ray hit */
		if(get_map_cell(maps[current_map], vcelly, vcellx) == 1)
		{
			/* Get distance to the hit */
			v_distance = getDistance(player.x, player.y, ray_x, ray_y);
			v_hit_x = ray_x;
			v_hit_y = ray_y;
			break;
		}
		ray_x += nray_x;
		ray_y += nray_y;
		sight_depth += 1;
	}

	/* Get shortest distance */
	if(h_distance < v_distance)
	{
		hit_x = h_hit_x;
		hit_y = h_hit_y;
		hit_distance = h_distance;
		cellx = hcellx;
		celly = hcelly;
		colorMult = 0.6;
	}
	else
	{
		hit_x = v_hit_x;
		hit_y = v_hit_y;
		hit_distance = v_distance;
		cellx = vcellx;
		celly = vcelly;
		colorMult = 0.9;
	}


	glColor3f(color[0]*colorMult, color[1]*colorMult, color[2]*colorMult);
	glLineWidth(2); glBegin(GL_LINES); glVertex2i(player.x,player.y); glVertex2i(hit_x,hit_y); glEnd();

	hit_distance = hit_distance*cos(degToRad(truncateAngle(player.angle - angle))); /* Fix fisheye */
	int lineH = (CELL_SIZE*320)/(hit_distance);
	if(lineH>320)
	{
		lineH=320; /* Line height and limit */
	}
	int lineOff = 160 - (lineH>>1); /* Line offset */
	glLineWidth(2);glBegin(GL_LINES);glVertex2i(-ray*8+530+CELL_SIZE*8,lineOff);glVertex2i(-ray*8+530+CELL_SIZE*8,lineOff+lineH);glEnd();
}


void renderMap()
{
	float ray = 0;

	/* Render ceiling */
	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_QUADS);
	glVertex2i(50+CELL_SIZE*8, 0);
	glVertex2i(50+CELL_SIZE*8, 160);
	glVertex2i(530+CELL_SIZE*8, 160);
	glVertex2i(530+CELL_SIZE*8, 0);
	glEnd();

	/* Render floor */
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_QUADS);
	glVertex2i(50+CELL_SIZE*8, 160);
	glVertex2i(50+CELL_SIZE*8, 320);
	glVertex2i(530+CELL_SIZE*8, 320);
	glVertex2i(530+CELL_SIZE*8, 160);
	glEnd();

	/* Render walls */
	for(ray = 0; ray < 60; ray += 0.2)
		renderRayMap(truncateAngle(player.angle - 30 + ray), ray);
}



/* Main Functions */

void buildMap()
{
	int count = 0;
	DIR * dirp;
	struct dirent * entry;
	char map_path[256];
	Map * temp_map;

	/* Count map subfiles */
	dirp = opendir("./Maps/");
	while ((entry = readdir(dirp)) != NULL)
	{
		if (entry->d_type == DT_REG)
			count++;
	}
	closedir(dirp);
	
	/* Allocate maps array */
	maps = malloc(count*sizeof(Map*));

	/* Load each map */
	dirp = opendir("./Maps/");
	while ((entry = readdir(dirp)) != NULL)
	{
		if (entry->d_type == DT_REG)
		{
			sprintf(map_path, "./Maps/%s", entry->d_name);
			printf("Loading %s\n", map_path);
			temp_map = loadMap(map_path);
			printMap(temp_map);
			maps[get_map_id(temp_map)] = temp_map;
		}
	}
	closedir(dirp);
	current_map = 0;
}

void init()
{
	/* Background color */
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,1024,510,0);

	/* Initialize Player */
	player.x = get_map_entrance_x(maps[current_map])*CELL_SIZE + 25;
	player.y = get_map_entrance_y(maps[current_map])*CELL_SIZE + 25;
	player.angle = 0;
	player.dx = cos(degToRad(player.angle));
	player.dy = -sin(degToRad(player.angle));
}

void display()
{
	/* Clear buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer2D();
	renderMap();
	glutSwapBuffers();
}

void collisionDetector(float dx, float dy)
{
	int cellx, celly, cell_id;
	cellx = (player.x + dx) / CELL_SIZE;
	celly = (player.y + dy) / CELL_SIZE;
	/* Check hotizontal */
	if(get_map_cell(maps[current_map], (int)(player.y/CELL_SIZE), cellx) != 1)
		player.x += dx;
	if(get_map_cell(maps[current_map], celly, (int)(player.x/CELL_SIZE)) != 1)
		player.y += dy;

	/* Detect doors */
	cell_id = get_map_cell(maps[current_map], (int)(player.y/CELL_SIZE), (int)(player.x/CELL_SIZE));
	if(cell_id > 1)
	{
		/* Change map */
		current_map = cell_id-1;
		/* Update player position */
		player.x = get_map_entrance_x(maps[current_map])*CELL_SIZE + 25;
		player.y = get_map_entrance_y(maps[current_map])*CELL_SIZE + 25;
	}
}

void keyboard_input(unsigned char key,int x,int y)
{
	if(key=='a')
	{
		player.angle = truncateAngle(player.angle + PLAYER_SENSIBILITY);
		player.dx = cos(degToRad(player.angle));
		player.dy = -sin(degToRad(player.angle));
	} 	
	if(key=='d')
	{
		player.angle = truncateAngle(player.angle - PLAYER_SENSIBILITY);
		player.dx = cos(degToRad(player.angle));
		player.dy = -sin(degToRad(player.angle));
	} 
	if(key=='w')
	{
		collisionDetector(player.dx*PLAYER_SPEED, player.dy*PLAYER_SPEED);
	}
	if(key=='s')
	{
		collisionDetector(-(player.dx*PLAYER_SPEED), -(player.dy*PLAYER_SPEED));
	}
	glutPostRedisplay();
}

void startEngine(int argc, char const *argv[])
{
	/* Load map */
	buildMap();
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1024, 510);
	glutCreateWindow("Jongine");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard_input);
	glutMainLoop();
}