#include <sstream>  //include this to use string streams
#include <iostream>
#include <SDL.h>

#include "globals.hpp"
#include "config.hpp"
#include "input.hpp"
#include "video.hpp"
#include "romloader.hpp"

static void quit_func(int code)
{
    SDL_Quit();
    exit(code);
}

static void handle_key_down(SDL_Keysym* keysym)
{
    switch(keysym->sym)
    {
        case SDLK_ESCAPE:
            quit_func(0);
            break;

        case SDLK_SPACE:
            //should_rotate = !should_rotate;
            break;
        
        default:
            break;
    }
    return;
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
static int save_counter = 0;

static bool process_user_input()
{
    // Save screen to a bitmap
    if (input.has_pressed(Input::SAVE_BMP))
    {
        std::string filename = "save" + std::to_string(save_counter);
        filename += ".bmp";

        SDL_SaveBMP(video.get_surface(), filename.c_str());
        save_counter++;

        std::cout << "Saved file: " << filename.c_str() << std::endl;
    }

    // Move About
    if (input.is_pressed(Input::DOWN))
    {
        video.sprite_layer->display_y_off += 5;
        return true;
    }
    else if (input.is_pressed(Input::UP))
    {
        video.sprite_layer->display_y_off -= 5;

        if (video.sprite_layer->display_y_off < 0)
            video.sprite_layer->display_y_off = 0;

        return true;
    }
    if (input.has_pressed(Input::PAGEDOWN))
    {
        video.sprite_layer->display_y_off += video.win_height / video.scale;
        return true;
    }
    else if (input.has_pressed(Input::PAGEUP))
    {
        video.sprite_layer->display_y_off -= video.win_height / video.scale;

        if (video.sprite_layer->display_y_off < 0)
            video.sprite_layer->display_y_off = 0;
        return true;
    }

    // Change Palette
    else if (input.has_pressed(Input::LEFT))
    {
        video.prev_palette();
        std::cout << "Palette:" << video.selected_palette << " Y:" << video.sprite_layer->display_y_off << std::endl;
        return true;
    }
    else if (input.has_pressed(Input::RIGHT))
    {
        video.next_palette();
        std::cout << "Palette:" << video.selected_palette << " Y:" << video.sprite_layer->display_y_off << std::endl;
        return true;
    }
    else if (input.has_pressed(Input::ZOOM_IN))
    {
        video.set_scale(++video.scale);
        return true;
    }
    else if (input.has_pressed(Input::ZOOM_OUT))
    {
        if (video.scale > 1) video.set_scale(--video.scale);
        return true;
    }
    else if (input.has_pressed(Input::CYCLE_BG))
    {
        video.cycle_background();
        return true;
    }
    return false;
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

    std::cout << "\n" << settings::PROGRAM_NAME << " " << settings::VERSION << std::endl;
    std::cout << "http://reassembler.blogspot.com\n" << std::endl;

    if (argc < 1)
    {
        std::cout << "Please specify an Sprite XML config file to load. Example:" << std::endl;
        std::cout << "OutRun:     " << argv[0] << " outrun.xml" << std::endl;
        return 1;
    }

    config.set_config_file(argv[1]);
    if (!config.load())
        return 1;

    // Load Roms
    RomLoader romloader;
    romloader.init(config.sprites);

    // Initialize SDL Video
    if (!video.init(romloader.pal_data, romloader.pal_data_len, 
                    config.sprites.pal_bytes_per_entry, config.sprites.pal_offset))
        quit_func(1);

    // Convert to Sprite Format
    video.sprite_layer->init(romloader.spr_data, romloader.format, romloader.spr_data_len);

    // Delete original data
    romloader.unload();

    std::cout << "Page Up / Page Down & Cursors Up & Down to scroll" << std::endl;
    std::cout << "Space to save a screenshot" << std::endl;

    if (romloader.pal_data_len > 0)
        std::cout << "Cursors Left & Right to set a palette" << std::endl;

    main_loop();

    // Never Reached
    return 0;
}
