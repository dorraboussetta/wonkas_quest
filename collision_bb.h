#ifndef COLLISION_BB_H
#define COLLISION_BB_H

#include <SDL2/SDL.h>
#include "entities.h"
#include "collision_perfect.h"

int CheckAABB(SDL_Rect a, SDL_Rect b);
int CollisionPlayerMovingPlatform(SDL_Rect playerWorld, MovingPlatform *p, int *shakeTimer);
int CollisionPlayerDestructible(SDL_Rect playerWorld, DestructiblePlatform *p, int *shakeTimer);

#endif
