#ifndef BACKGROUND_H
#define BACKGROUND_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#define SCREEN_W 1920
#define SCREEN_H 1080
#define MAX_PLATFORMS 32
#define HALF_W        (SCREEN_W / 2)

/* platform type values */
#define PLATFORM_FIXED        0
#define PLATFORM_MOVING       1
#define PLATFORM_DESTRUCTIBLE 2

typedef struct {
    SDL_Texture *texture;
    SDL_Rect     pos;
    int          prev_x;
    int          prev_y;
    int          type;     /* 0=fixed, 1=moving, 2=destructible */
    int          status;
    float        vel_x;
    float        vel_y;
    int          dir;
} Platform;

typedef struct {
    SDL_Texture *texture;
    int          current_level;
    int          map_w;
    int          map_h;
    Mix_Music   *music;
    Mix_Chunk   *sound_effect;
    SDL_Rect     camera1;
    SDL_Rect     pos_screen1;
    SDL_Rect     camera2;
    SDL_Rect     pos_screen2;
} Background_Level;

typedef struct {
    int       start_ticks;
    TTF_Font *font;
    int       r, g, b, a;
} Timer;

typedef struct {
    SDL_Rect     pos;
    SDL_Texture *texture;
    const char  *hint_text;
    int          active;
} Sign;

#define MAX_SIGNS 16

/* texture helper */
SDL_Texture *load_texture(SDL_Renderer *r, const char *path);

/* platform helper */
void make_platform(Platform *p, SDL_Renderer *r, const char *img,
                   int x, int y, int w, int h,
                   int type, float vx, float vy);

/* background */
int  load_background(Background_Level *bg, SDL_Renderer *r,
                     const char *path, int level);
void update_camera(Background_Level *bg,
                   int p1x, int p1y, int p1w, int p1h,
                   int p2x, int p2y, int p2w, int p2h);
void render_background(Background_Level *bg, SDL_Renderer *r);
void free_background_level(Background_Level *bg);
int  load_all_assets(SDL_Renderer *r, Background_Level *bg1, Background_Level *bg2,
                     Platform *lvl1, int *count1,
                     Platform *lvl2, int *count2);

/* platforms */
int  init_level(Platform *platforms, int *count,
                SDL_Renderer *r, int level);
void render_platforms_split(Platform *platforms, int count,
                            SDL_Renderer *r,
                            SDL_Rect camera1, SDL_Rect screen1,
                            SDL_Rect camera2, SDL_Rect screen2);
void update_mobile(Platform *platforms, int count, float delta_t, int map_w);
void destroy_platform(Platform *p);
void free_platforms(Platform *platforms, int count);

/* timer */
Timer *create_timer(const char *font_path, int font_size);
void   render_timer(Timer *t, SDL_Renderer *r, int x, int y);
void   free_timer(Timer *t);

/* signs */
void init_signs(Sign *signs, int *count, SDL_Renderer *r, int level);
void update_signs(Sign *signs, int count, int px, int py, int pw, int ph);
void render_signs(Sign *signs, int count, SDL_Renderer *r,
                  SDL_Rect camera, SDL_Rect screen_area, TTF_Font *font);
void free_signs(Sign *signs, int count);

#endif
