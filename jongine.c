#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


//FRANUTE ESTUVO AQUI
void filledRect(void *r_in,
                int x, int y, int w, int h,
                int r, int g, int b, int a)
{
    SDL_Renderer* renderer = (SDL_Renderer*)r_in;
    int rc1 = SDL_SetRenderDrawColor(renderer, r, g, b, a);
    if (rc1 != 0) {
      fprintf(stderr, "SDL_SetRenderDrawColor failed: %s\n", SDL_GetError());
      exit(1);
    }
    SDL_Rect rect = { x, y, w, h };
    int rc2 = SDL_RenderFillRect(renderer, &rect);
    if (rc2 != 0) {
      fprintf(stderr, "SDL_RenderFillRect failed: %s\n", SDL_GetError());
      exit(1);
    }
}

SDL_Renderer *init(int w, int h) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Cannot initialise SDL: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_Window *window = SDL_CreateWindow(
    "moves", 
    SDL_WINDOWPOS_CENTERED, 
    SDL_WINDOWPOS_CENTERED, 
    w, h,
    SDL_WINDOW_SHOWN
  );
  if (window == NULL) {
    fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
    window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );
  if (renderer == NULL) {
    fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
    exit(1);
  }

  if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0) {
    fprintf(stderr, "SDL_BlendMode failed: %s\n", SDL_GetError());
    exit(1);
  };

  return renderer;
}

int pollEventsForQuit() {
  return 0;
}

int main(int argc, char* args[]) {
  SDL_Renderer *renderer = init(640, 480);

  int x = 640/2 - 50/2;
  int y = 480/2 - 50/2;
  int dx = 0;
  int dy = 0;

  for (;;) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          goto quit;
        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE: goto quit;
            case SDLK_UP: dy = -1; break;
            case SDLK_RIGHT: dx = 1; break;
            case SDLK_DOWN: dy = 1; break;
            case SDLK_LEFT: dx = -1; break;
          }
          break;
        case SDL_KEYUP:
          switch (e.key.keysym.sym) {
            case SDLK_UP: dy = 0; break;
            case SDLK_RIGHT: dx = 0; break;
            case SDLK_DOWN: dy = 0; break;
            case SDLK_LEFT: dx = 0; break;
          }
          break;
      }
    }

    int rc1 = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if (rc1 != 0) {
      fprintf(stderr, "SDL_SetRenderDrawColor failed: %s\n", SDL_GetError());
      exit(1);
    }
    SDL_RenderClear(renderer);
    x += dx;
    y += dy;
    filledRect(renderer, x, y, 50, 50, 255, 0, 0, 128);
    SDL_RenderPresent(renderer); // Update screen.
  }
quit:
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}