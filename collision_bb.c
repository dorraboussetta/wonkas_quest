#include "collision_bb.h"

int CheckAABB(SDL_Rect a, SDL_Rect b)
{
    if (a.x + a.w < b.x) return 0;
    if (a.x > b.x + b.w) return 0;
    if (a.y + a.h < b.y) return 0;
    if (a.y > b.y + b.h) return 0;
    return 1;
}

int CollisionPlayerMovingPlatform(SDL_Rect playerWorld, MovingPlatform *p, int *shakeTimer)
{
    if (!p || !p->active) return 0;
    if (CheckAABB(playerWorld, p->pos)) {
        if (shakeTimer && *shakeTimer == 0) *shakeTimer = SHAKE_DURATION;
        return 1;
    }
    return 0;
}

int CollisionPlayerDestructible(SDL_Rect playerWorld, DestructiblePlatform *p, int *shakeTimer)
{
    if (!p || !p->active) return 0;
    if (CheckAABB(playerWorld, p->pos)) {
        if (shakeTimer && *shakeTimer == 0) *shakeTimer = SHAKE_DURATION;
        return 1;
    }
    return 0;
}
