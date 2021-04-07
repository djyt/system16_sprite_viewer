#include <sstream>  //include this to use string streams
#include <iostream>
#include <SDL.h>

#include "globals.hpp"
#include "input.hpp"
#include "video.hpp"
#include "roms.hpp"
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
    // Golden Axe Test
    /*argc = 3;
    argv[1] = "mpr-12379.ic12";
    argv[2] = "mpr-12378.ic9";
    argv[1] = "mpr-12381.ic13";
    argv[2] = "mpr-12380.ic10";
    argv[1] = "mpr-12383.ic14";
    argv[2] = "mpr-12382.ic11";*/

    // Shinobi Test
    /*argc = 3;
    argv[1] = "epr-11294.11";
    argv[2] = "epr-11290.10";*/

    // OutRun Test
    /*argc = 5;
    argv[1] = "mpr-10371.9";
    argv[2] = "mpr-10373.10";
    argv[3] = "mpr-10375.11";
    argv[4] = "mpr-10377.12";*/

    // PowerDrift Test
    /*argc = 9;
    argv[1] = "epr-11757.67";
    argv[2] = "epr-11758.75";
    argv[3] = "epr-11773.63";
    argv[4] = "epr-11774.71";
    argv[5] = "epr-11759.86";
    argv[6] = "epr-11760.114";
    argv[7] = "epr-11775.82";
    argv[8] = "epr-11776.110";*/

    // Initialize timer and video systems
    if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) == -1 ) 
    { 
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        return 1; 
    } 

    std::cout << "\n" << settings::PROGRAM_NAME << " " << settings::VERSION << std::endl;
    std::cout << "http://reassembler.blogspot.com\n" << std::endl;

    if (argc < 2)
    {
        std::cout << "Invalid number of filenames provided.\nPlease specify a bank of System 16 sprite files." << std::endl;
        std::cout << "This should be 2 files for System 16 titles, 4 for X-Board and 8 for Y-Board." << std::endl;
        std::cout << "\nExamples..." << std::endl;
        std::cout << "OutRun:     " << argv[0] << " mpr-10371.9 mpr-10373.10 mpr-10375.11 mpr-10377.12" << std::endl;
        std::cout << "Golden Axe: " << argv[0] << " mpr-12379.ic12 mpr-12378.ic9" << std::endl;
        return 1;
    }

    // Load Roms
    if (roms.init(argc, argv))
    {
        std::cout << "Page Up / Page Down & Cursors Up & Down to scroll" << std::endl;
        std::cout << "Space to save a screenshot" << std::endl;
        std::cout << "Cursors Left & Right to set OutRun palette" << std::endl;

        // Initialize SDL Video
        if (!video.init(roms.sprites.rom))
            quit_func(1);

        main_loop();
    }

    // Never Reached
    return 0;
}
