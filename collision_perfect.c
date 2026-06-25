#include "collision_perfect.h"
#include <string.h>

/* Renamed GetPixel → GetPixelXY to avoid conflicting declaration with
   enemy.h which declares:  SDL_Color GetPixel(SDL_Surface*, SDL_Rect). */
SDL_Color GetPixelXY(SDL_Surface *surface, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255};
    if (!surface) return color;
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return color;

    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

    Uint8 *p = (Uint8 *)surface->pixels
        + y * surface->pitch
        + x * surface->format->BytesPerPixel;

    Uint32 pixel = 0;
    memcpy(&pixel, p, surface->format->BytesPerPixel);
    SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);

    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    return color;
}

int IsObstacle(SDL_Surface *mask, int x, int y)
{
    SDL_Color c = GetPixelXY(mask, x, y);
    return (c.r == MASK_R && c.g == MASK_G && c.b == MASK_B);
}

int CollisionBottom(SDL_Rect r, SDL_Surface *mask, int *shakeTimer)
{
    int hit;
    if (!mask) return 0;

    hit = (
        IsObstacle(mask, r.x,         r.y + r.h - 1) ||
        IsObstacle(mask, r.x + r.w/2, r.y + r.h - 1) ||
        IsObstacle(mask, r.x + r.w-1, r.y + r.h - 1)
    );
    if (hit && shakeTimer && *shakeTimer == 0)
        *shakeTimer = SHAKE_DURATION;
    return hit;
}

int CollisionTop(SDL_Rect r, SDL_Surface *mask, int *shakeTimer)
{
    int hit;
    if (!mask) return 0;

    hit = (
        IsObstacle(mask, r.x,         r.y) ||
        IsObstacle(mask, r.x + r.w/2, r.y) ||
        IsObstacle(mask, r.x + r.w-1, r.y)
    );
    if (hit && shakeTimer && *shakeTimer == 0)
        *shakeTimer = SHAKE_DURATION;
    return hit;
}

int CollisionLeft(SDL_Rect r, SDL_Surface *mask, int *shakeTimer)
{
    int hit;
    if (!mask) return 0;

    hit = (
        IsObstacle(mask, r.x, r.y) ||
        IsObstacle(mask, r.x, r.y + r.h/2) ||
        IsObstacle(mask, r.x, r.y + r.h-1)
    );
    if (hit && shakeTimer && *shakeTimer == 0)
        *shakeTimer = SHAKE_DURATION;
    return hit;
}

int CollisionRight(SDL_Rect r, SDL_Surface *mask, int *shakeTimer)
{
    int hit;
    if (!mask) return 0;

    hit = (
        IsObstacle(mask, r.x + r.w-1, r.y) ||
        IsObstacle(mask, r.x + r.w-1, r.y + r.h/2) ||
        IsObstacle(mask, r.x + r.w-1, r.y + r.h-1)
    );
    if (hit && shakeTimer && *shakeTimer == 0)
        *shakeTimer = SHAKE_DURATION;
    return hit;
}

int CollisionPerfect(SDL_Rect posJoueur, SDL_Surface *mask, int *shakeTimer)
{
    if (!mask) return 0;
    if (CollisionBottom(posJoueur, mask, shakeTimer)) return 1;
    if (CollisionTop   (posJoueur, mask, shakeTimer)) return 1;
    if (CollisionLeft  (posJoueur, mask, shakeTimer)) return 1;
    if (CollisionRight (posJoueur, mask, shakeTimer)) return 1;
    return 0;
}
