#ifndef ENTITIES_H
#define ENTITIES_H

#include <SDL2/SDL.h>

#define MAX_MOVING_PLATFORMS      8
#define MAX_DESTRUCTIBLE_PLATFORMS 8
#define DESTRUCT_TIMER            90

#define PLATFORM_HORIZONTAL 0
#define PLATFORM_VERTICAL   1

typedef struct {
    SDL_Rect     pos;
    SDL_Surface *sprite;
    int          dir;
    int          speed;
    int          min;
    int          max;
    int          active;
} MovingPlatform;

typedef struct {
    SDL_Rect     pos;
    SDL_Surface *sprite;
    int          active;
    int          hit;
    int          timer;
} DestructiblePlatform;

#endif
