#ifndef COLLISION_PERFECT_H
#define COLLISION_PERFECT_H

#include <SDL2/SDL.h>

#define SHAKE_DURATION 10

/* Mask colour that marks solid/obstacle pixels */
#define MASK_R 255
#define MASK_G 0
#define MASK_B 0

/* Renamed from GetPixel → GetPixelXY to avoid conflict with
   enemy.h's  SDL_Color GetPixel(SDL_Surface*, SDL_Rect).      */
SDL_Color GetPixelXY    (SDL_Surface *surface, int x, int y);

int IsObstacle          (SDL_Surface *mask, int x, int y);

int CollisionBottom     (SDL_Rect r, SDL_Surface *mask, int *shakeTimer);
int CollisionTop        (SDL_Rect r, SDL_Surface *mask, int *shakeTimer);
int CollisionLeft       (SDL_Rect r, SDL_Surface *mask, int *shakeTimer);
int CollisionRight      (SDL_Rect r, SDL_Surface *mask, int *shakeTimer);
int CollisionPerfect    (SDL_Rect posJoueur, SDL_Surface *mask, int *shakeTimer);

#endif /* COLLISION_PERFECT_H */
