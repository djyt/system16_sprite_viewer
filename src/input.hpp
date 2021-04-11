#pragma once

#include <SDL.h>

class Input
{
public:
    enum presses
    {
        LEFT = 0,
        RIGHT = 1,
        UP = 2,
        DOWN = 3,
        PAGEDOWN = 4,
        PAGEUP = 5,
        SAVE_BMP = 6,

        ZOOM_IN = 7,
        ZOOM_OUT  = 8,
        
        CYCLE_BG = 9,
        SHOW_HUD  = 10,
        HOME = 11,
        END = 12,
    };

    bool keys[13];
    bool keys_old[13];

    Input(void);
    ~Input(void);

    void handle_key_up(SDL_Keysym*);
    void handle_key_down(SDL_Keysym*);
    void frame_done();
    bool is_pressed(presses p);
    bool has_pressed(presses p);

private:
    void handle_key(SDL_Keysym*, bool);
};

extern Input input;

