#include "common.h"
#include "save_menu.h"

void init_save_menu(SaveMenu *menu, SDL_Renderer *renderer)
{
    SDL_Color yellow = {255, 204, 0};

    /* Background */
    init_background(&menu->bg, "./assets_load/bg2.png", "./assets_load/bg2_music.mp3", renderer);

    /* Question text */
    init_text(&menu->question,
              "./assets_load/font.ttf",
              40,
              "DO YOU WANT TO SAVE YOUR GAME?",
              yellow,
              0, 120,
              renderer);

    /* Center the question text */
    int window_width = 1100;
    menu->question.pos.x = (window_width - menu->question.pos.w) / 2;

    /* YES/NO buttons */
    int btn_spacing = 50;

    init_button(&menu->yes_btn,
                "./assets_load/yes.png",
                "./assets_load/yes_hover.png",
                "./assets_load/yes_click.png",
                0, 0,
                renderer);
    init_button(&menu->no_btn,
                "./assets_load/no.png",
                "./assets_load/no_hover.png",
                "./assets_load/no_click.png",
                0, 0,
                renderer);

    int total_width = menu->yes_btn.pos.w + menu->no_btn.pos.w + btn_spacing;
    int start_x     = (window_width - total_width) / 2;
    int btn_y       = 300;

    menu->yes_btn.pos.x = start_x;
    menu->yes_btn.pos.y = btn_y;
    menu->no_btn.pos.x  = start_x + menu->yes_btn.pos.w + btn_spacing;
    menu->no_btn.pos.y  = btn_y;

    /* ADDED: Store pointers to both buttons in the navigation array */
    menu->buttons[0]   = &menu->yes_btn;
    menu->buttons[1]   = &menu->no_btn;
    menu->button_count = 2;

    /* ADDED: Pre-select the first button (YES) using the keyboard_selected flag
       so update_button() won't reset it to idle every frame */
    menu->selected_index                      = 0;
    menu->buttons[0]->keyboard_selected = 1;
    menu->prev_interface = 0;
}

void handle_save_menu_input(SaveMenu *menu, SDL_Event event, int *state)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int x = event.button.x;
        int y = event.button.y;

        /* YES button */
        if (x > menu->yes_btn.pos.x &&
            x < menu->yes_btn.pos.x + menu->yes_btn.pos.w &&
            y > menu->yes_btn.pos.y &&
            y < menu->yes_btn.pos.y + menu->yes_btn.pos.h)
        {
            if (menu->prev_interface == 7 || menu->prev_interface == 8)
                *state = menu->prev_interface;
            else
                *state = 3;   /* go to LOAD MENU */
        }

        /* NO button */
        if (x > menu->no_btn.pos.x &&
            x < menu->no_btn.pos.x + menu->no_btn.pos.w &&
            y > menu->no_btn.pos.y &&
            y < menu->no_btn.pos.y + menu->no_btn.pos.h)
        {
            if (menu->prev_interface == 7 || menu->prev_interface == 8)
                *state = menu->prev_interface;
            else
                *state = 0;   /* back to MAIN MENU */
        }
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                *state = 0;
                break;

            case SDLK_v:
                if (menu->prev_interface == 7 || menu->prev_interface == 8)
                    *state = menu->prev_interface;
                break;

            /* ADDED: Left / Up arrow - move highlight one step to the left */
            case SDLK_LEFT:
            case SDLK_UP:
                /* ADDED: Deselect current button */
                menu->buttons[menu->selected_index]->keyboard_selected = 0;

                /* ADDED: Decrement index, wrap around to last button if needed */
                menu->selected_index--;
                if (menu->selected_index < 0)
                    menu->selected_index = menu->button_count - 1;

                /* ADDED: Select new button */
                menu->buttons[menu->selected_index]->keyboard_selected = 1;
                break;

            /* ADDED: Right / Down arrow - move highlight one step to the right */
            case SDLK_RIGHT:
            case SDLK_DOWN:
                /* ADDED: Deselect current button */
                menu->buttons[menu->selected_index]->keyboard_selected = 0;

                /* ADDED: Increment index, wrap around to first button if needed */
                menu->selected_index++;
                if (menu->selected_index >= menu->button_count)
                    menu->selected_index = 0;

                /* ADDED: Select new button */
                menu->buttons[menu->selected_index]->keyboard_selected = 1;
                break;

            /* ADDED: X key - confirm the currently highlighted button */
            case SDLK_SPACE:
                /* ADDED: Fire the action that matches the selected button */
                if (menu->selected_index == 0)
                    *state = (menu->prev_interface == 7 || menu->prev_interface == 8) ? menu->prev_interface : 3;   /* YES -> go to LOAD MENU */
                else if (menu->selected_index == 1)
                    *state = (menu->prev_interface == 7 || menu->prev_interface == 8) ? menu->prev_interface : 0;   /* NO  -> back to MAIN MENU */
                break;
        }
    }
}

void update_save_menu(SaveMenu *menu)
{
    update_button(&menu->yes_btn);
    update_button(&menu->no_btn);
}

void display_save_menu(SaveMenu menu, SDL_Renderer *renderer)
{
    display_background(menu.bg, renderer);
    display_text(menu.question, renderer);
    display_button(menu.yes_btn, renderer);
    display_button(menu.no_btn, renderer);
}

void free_save_menu(SaveMenu *menu)
{
    free_background(&menu->bg);
    free_text(&menu->question);
    free_button(&menu->yes_btn);
    free_button(&menu->no_btn);
}
