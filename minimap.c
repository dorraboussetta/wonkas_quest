#include "minimap.h"
#include <stdio.h>

/* ── helper: solid-colour texture ─────────────────────────────────── */
static SDL_Texture *CreateColoredDotTex(SDL_Renderer *renderer,
                                        int w, int h,
                                        Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Surface *surf = SDL_CreateRGBSurface(0, w, h, 32,
                                             0x00FF0000,
                                             0x0000FF00,
                                             0x000000FF,
                                             0xFF000000);
    if (!surf) {
        fprintf(stderr, "[Minimap] CreateRGBSurface: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, r, g, b));
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex)
        fprintf(stderr, "[Minimap] CreateTextureFromSurface (dot): %s\n", SDL_GetError());
    return tex;
}

/* ── InitMinimap ───────────────────────────────────────────────────── */
int InitMinimap(minimap *m, SDL_Renderer *renderer,
                const char *thumbnailPath, const char *manPath,
                int screenW, int screenH, int bgW, int bgH)
{
    if (!m || !renderer) return -1;

    m->thumbnail_tex = NULL;
    m->man_tex       = NULL;
    m->scaleX        = 1.0f;
    m->scaleY        = 1.0f;
    SDL_memset(&m->pos,    0, sizeof(SDL_Rect));
    SDL_memset(&m->posMan, 0, sizeof(SDL_Rect));

    /* Load thumbnail BMP → texture */
    SDL_Surface *thumb = SDL_LoadBMP(thumbnailPath);
    if (!thumb) {
        fprintf(stderr,
            "[Minimap] Cannot load '%s': %s\n"
            "          Create it in GIMP: open the full background,\n"
            "          Image > Scale to ~20%% of %dpx wide, File > Export As .bmp\n",
            thumbnailPath, SDL_GetError(), bgW);
        return -1;
    }

    m->scaleX = (float)thumb->w / (float)bgW;
    m->scaleY = (float)thumb->h / (float)bgH;

    m->pos.x = screenW - thumb->w - 10;
    m->pos.y = 10;
    m->pos.w = thumb->w;
    m->pos.h = thumb->h;

    m->thumbnail_tex = SDL_CreateTextureFromSurface(renderer, thumb);
    SDL_FreeSurface(thumb);
    if (!m->thumbnail_tex) {
        fprintf(stderr, "[Minimap] CreateTextureFromSurface (thumbnail): %s\n", SDL_GetError());
        return -1;
    }

    /* Load or generate player-dot texture */
    if (manPath) {
        SDL_Surface *dot = SDL_LoadBMP(manPath);
        if (!dot) {
            fprintf(stderr, "[Minimap] Cannot load dot '%s': %s — using gold dot.\n",
                    manPath, SDL_GetError());
            m->man_tex = CreateColoredDotTex(renderer, 6, 6, 255, 215, 0);
        } else {
            m->man_tex = SDL_CreateTextureFromSurface(renderer, dot);
            SDL_FreeSurface(dot);
        }
    } else {
        m->man_tex = CreateColoredDotTex(renderer, 6, 6, 255, 215, 0);
    }

    if (!m->man_tex) return -1;

    /* Query dot size */
    SDL_QueryTexture(m->man_tex, NULL, NULL, &m->posMan.w, &m->posMan.h);
    m->posMan.x = m->pos.x;
    m->posMan.y = m->pos.y;

    printf("[Minimap] Ready — thumbnail %dx%d covers %dx%d world "
           "(scaleX=%.3f scaleY=%.3f) panel at (%d,%d)\n",
           m->pos.w, m->pos.h, bgW, bgH,
           m->scaleX, m->scaleY, m->pos.x, m->pos.y);
    return 0;
}

/* ── MAJMinimap ────────────────────────────────────────────────────── */
void MAJMinimap(SDL_Rect posJoueur, minimap *m, SDL_Rect camera)
{
    if (!m || !m->thumbnail_tex) return;

    /* Convert player screen position → world position → minimap position */
    int worldX = posJoueur.x + camera.x;
    int worldY = posJoueur.y + camera.y;

    int dotX = (int)(worldX * m->scaleX);
    int dotY = (int)(worldY * m->scaleY);

    /* Clamp inside panel */
    if (dotX < 0)                       dotX = 0;
    if (dotY < 0)                       dotY = 0;
    if (dotX > m->pos.w - m->posMan.w)  dotX = m->pos.w - m->posMan.w;
    if (dotY > m->pos.h - m->posMan.h)  dotY = m->pos.h - m->posMan.h;

    m->posMan.x = m->pos.x + dotX;
    m->posMan.y = m->pos.y + dotY;
}

/* ── DisplayMinimap ────────────────────────────────────────────────── */
void DisplayMinimap(minimap *m, SDL_Renderer *renderer)
{
    if (!m || !renderer || !m->thumbnail_tex) return;
    if (m->pos.w == 0 || m->pos.h == 0) return;

    /* Remove any clip rect so the minimap always renders on top */
    SDL_RenderSetClipRect(renderer, NULL);

    /* Dark border */
    SDL_SetRenderDrawColor(renderer, 30, 20, 10, 220);
    SDL_Rect border = { m->pos.x - 2, m->pos.y - 2,
                        m->pos.w + 4, m->pos.h + 4 };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &border);

    /* Thumbnail */
    SDL_RenderCopy(renderer, m->thumbnail_tex, NULL, &m->pos);

    /* Thin white outline */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_RenderDrawRect(renderer, &m->pos);

    /* Player dot */
    if (m->man_tex)
        SDL_RenderCopy(renderer, m->man_tex, NULL, &m->posMan);
}

/* ── FreeMinimap ───────────────────────────────────────────────────── */
void FreeMinimap(minimap *m)
{
    if (!m) return;
    if (m->thumbnail_tex) { SDL_DestroyTexture(m->thumbnail_tex); m->thumbnail_tex = NULL; }
    if (m->man_tex)       { SDL_DestroyTexture(m->man_tex);       m->man_tex       = NULL; }
    printf("[Minimap] Resources freed.\n");
}
