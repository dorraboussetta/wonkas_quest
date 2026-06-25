#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>

typedef struct {
    /* Textures (created from the original surfaces, rendered each frame) */
    SDL_Texture *thumbnail_tex;
    SDL_Texture *man_tex;

    /* On-screen position of the minimap panel */
    SDL_Rect pos;

    /* On-screen position of the player dot */
    SDL_Rect posMan;

    /* World-to-minimap scale */
    float scaleX;
    float scaleY;
} minimap;

/* Pass renderer so we can convert surfaces → textures at init time.
   thumbnailPath : small BMP of the full level background (~20% width).
   manPath       : small BMP for the player dot, or NULL for a gold dot. */
int  InitMinimap  (minimap *m, SDL_Renderer *renderer,
                   const char *thumbnailPath, const char *manPath,
                   int screenW, int screenH, int bgW, int bgH);

/* Update the dot position given the player's world-space rect and camera. */
void MAJMinimap   (SDL_Rect posJoueur, minimap *m, SDL_Rect camera);

/* Draw the minimap directly to the renderer — no surface needed. */
void DisplayMinimap(minimap *m, SDL_Renderer *renderer);

/* Free GPU textures. */
void FreeMinimap  (minimap *m);

#endif /* MINIMAP_H */
