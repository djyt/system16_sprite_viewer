/***************************************************************************
    Sega System 16 Sprite Viewer

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#include <sstream>  //include this to use string streams
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

#include "globals.hpp"
#include "config.hpp"
#include "input.hpp"
#include "video.hpp"
#include "romloader.hpp"

static void quit_func(int code)
{
    TTF_Quit();
    SDL_Quit();
    exit(code);
}

static void process_events(void)
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            // Resize window. Create new area to render onto
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    video.resize_window(event.window.data1, event.window.data2);
                    video.draw_frame();
                }
                else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                    video.close();
                break;

            case SDL_KEYDOWN:
                input.handle_key_down(&event.key.keysym);
                break;

            case SDL_KEYUP:
                input.handle_key_up(&event.key.keysym);
                break;

            case SDL_QUIT:
                /* Handle quit requests (like Ctrl-c). */
                quit_func(0);
                break;
        }
    }
    return;
}

// Increment each time we save
std::string save_name;
static int save_counter = 0;

static void save_screenshot()
{
    std::string filename = save_name + std::to_string(save_counter) + ".bmp";

    SDL_SaveBMP(video.get_surface(), filename.c_str());
    save_counter++;

    std::cout << "Saved file: " << filename.c_str() << std::endl;
}

static bool process_user_input()
{
    if (input.has_pressed(Input::SAVE_BMP))         { save_screenshot(); return false; }
    else if (input.is_pressed(Input::DOWN))         video.scroll(5);
    else if (input.is_pressed(Input::UP))           video.scroll(-5);
    else if (input.has_pressed(Input::PAGEDOWN))    video.scroll(video.win_height / video.scale);
    else if (input.has_pressed(Input::PAGEUP))      video.scroll(-(video.win_height / video.scale));
    else if (input.has_pressed(Input::HOME))        video.sprite_layer->display_y_off = 0;
    else if (input.has_pressed(Input::END))         video.sprite_layer->display_y_off = video.sprite_layer->y_max;
    else if (input.has_pressed(Input::LEFT))        video.prev_palette();
    else if (input.has_pressed(Input::RIGHT))       video.next_palette();
    else if (input.has_pressed(Input::ZOOM_IN))     video.set_scale(++video.scale);
    else if (input.has_pressed(Input::ZOOM_OUT))    { if (video.scale > 1) video.set_scale(--video.scale); }
    else if (input.has_pressed(Input::CYCLE_BG))    video.cycle_background();
    else if (input.has_pressed(Input::SHOW_HUD))    video.toggle_hud();
    else return false;

    return true;
}


static void tick()
{
    process_events();

    if (process_user_input())
    {
        video.draw_frame();
    }
    input.frame_done();
}

static void main_loop()
{
    video.draw_frame();

    while (true)
    {
        tick();
        SDL_Delay( 10 );
    }
}

int main(int argc, char* argv[])
{
    // Initialize timer and video systems
    if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) == -1 ) 
    { 
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        return 1; 
    }

    if (TTF_Init() == -1)
    {
        std::cerr << "SDL TTF Initialization Failed." << std::endl;
        return 1; 
    }

    std::cout << "\n" << settings::PROGRAM_NAME << " " << settings::VERSION << std::endl;
    std::cout << "http://reassembler.blogspot.com\n" << std::endl;

    if (argc < 2)
    {
        std::cout << "Please specify an Sprite XML config file to load. Example:" << std::endl;
        std::cout << argv[0] << " outrun.xml" << std::endl;
        quit_func(1);
        return 1;
    }

    config.set_config_file(argv[1]);
    if (!config.load())
    {
        quit_func(1);
        return 1;
    }

    // Load Roms
    RomLoader romloader;
    if (!romloader.init(config.sprites))
    {
        quit_func(1);
        return 1;
    }

    // Initialize SDL Video
    if (!video.init(romloader.pal_data, romloader.pal_data_len,
                    config.sprites.pal_bytes_per_entry, config.sprites.pal_offset))
    {
        quit_func(1);
        return 1;
    }

    // Convert to Sprite Format
    video.sprite_layer->init(romloader.spr_data, romloader.format, romloader.spr_data_len, config.sprites.swap_endian);

    // Delete original data
    romloader.unload();

    std::cout << "Cursor Up / Down    : Scroll" << std::endl;
    std::cout << "Page Up / Down      : Scroll Page" << std::endl;
    std::cout << "Home / End          : Scroll To Start / End" << std::endl;
    if (romloader.pal_data_len > 0)
        std::cout << "Cursor Left / Right : Change Palette" << std::endl;
    std::cout << "+ / -               : Zoom" << std::endl;
    std::cout << "B                   : Change Background Colour" << std::endl;
    std::cout << "S                   : Save Screenshot" << std::endl;
    std::cout << "Space               : Toggle HUD " << std::endl;


    save_name = argv[1];
    size_t pos = save_name.find(".xml");
    if (pos != std::string::npos) save_name.erase(pos);

    main_loop();

    // Never Reached
    quit_func(0);

    return 0;
}
