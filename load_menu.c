#include "load_menu.h"
#include "save_menu.h"

void init_load_menu(LoadMenu *menu, SDL_Renderer *renderer)
{
    SDL_Color yellow = {255, 204, 0};

    /* Background */
    init_background(&menu->bg,
                    "./assets_load/bg2.png",
                    "./assets_load/bg2_music.mp3",
                    renderer);

    /* Title text (optional, you can remove if using image text in buttons) */
    init_text(&menu->title,
              "./assets_load/font.ttf",
              40,
              "Choose an option",
              yellow,
              430, 120,   // centered for 1100 width
              renderer);

    /* Buttons — centered horizontally */

    init_button(&menu->load_btn,
                "./assets_load/load.png",
                "./assets_load/load_hover.png",
                "./assets_load/load_click.png",
                399, 260,   // centered
                renderer);

    init_button(&menu->new_btn,
                "./assets_load/new.png",
                "./assets_load/new_hover.png",
                "./assets_load/new_click.png",
                399, 350,
                renderer);
}

void handle_load_menu_input(LoadMenu *menu, SDL_Event event, int *state)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int x = event.button.x;
        int y = event.button.y;

        /* LOAD GAME button */
        if (x > menu->load_btn.pos.x &&
            x < menu->load_btn.pos.x + menu->load_btn.pos.w &&
            y > menu->load_btn.pos.y &&
            y < menu->load_btn.pos.y + menu->load_btn.pos.h)
        {
            *state = 4;   /* go to GAME (loaded) */
        }

        /* NEW PART button */
        if (x > menu->new_btn.pos.x &&
            x < menu->new_btn.pos.x + menu->new_btn.pos.w &&
            y > menu->new_btn.pos.y &&
            y < menu->new_btn.pos.y + menu->new_btn.pos.h)
        {
            *state = 4;   /* go to GAME (new) */
        }
    }

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    {
        *state = 2;   /* back to SAVE MENU */
    }
}

void update_load_menu(LoadMenu *menu)
{
    update_button(&menu->load_btn);
    update_button(&menu->new_btn);
}

void display_load_menu(LoadMenu menu, SDL_Renderer *renderer)
{
    display_background(menu.bg, renderer);

    /* If you want the title text */
    display_text(menu.title, renderer);

    display_button(menu.load_btn, renderer);
    display_button(menu.new_btn, renderer);
}

void free_load_menu(LoadMenu *menu)
{
    free_background(&menu->bg);
    free_text(&menu->title);

    free_button(&menu->load_btn);
    free_button(&menu->new_btn);
}
