#include <iostream>
#include <sstream>  //include this to use string streams
#include "globals.hpp"
#include "video.hpp"

Video video;

using namespace settings;

// See: SDL_PixelFormat
#define CURRENT_RGB() (r << Rshift) | (g << Gshift) | (b << Bshift);

Video::Video(void)
{
    font          = NULL;
    texture       = NULL;
    renderer      = NULL;
    surface       = NULL;
    screen_pixels = NULL;
    pal_surface   = NULL;
    pal_texture   = NULL;
    sprite_layer  = new hwsprites();
    scn_width     = 0;
    scn_height    = 0;
    bg_index      = 0;
    cycle_background(false);
    show_hud = true;
    scale = 2;
}

Video::~Video(void)
{
    close();
    delete sprite_layer;
}

int Video::init(uint8_t* pal, const int pal_data_len, const int bytes_per_entry, const int pal_offset)
{
    // Setup Custom Palette
    this->palette = pal;
    this->bytes_per_entry = bytes_per_entry;
    this->pal_offset = pal_offset;

    if (pal == nullptr || pal_data_len == -1)
        max_pal_entries = -1;
    else
        max_pal_entries = (pal_data_len / bytes_per_entry) - 1;

    // Setup SDL Screen size
    sdl_screen_size();
    
    // Base this on actual screen size
    win_width  = S16_SCREEN_WIDTH * scale;
    win_height = scn_height / 2;
    
    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    window = SDL_CreateWindow(
        settings::PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height,
        flags);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

    if (!resize_window(win_width, win_height))
        return 0;

    // SDL Pixel Format Information
    Rshift = surface->format->Rshift;
    Gshift = surface->format->Gshift;
    Bshift = surface->format->Bshift;

    // Font stuff
    font = TTF_OpenFont("cousine-regular.ttf", FONT_SIZE);
    if (!font)
    {
        std::cerr << "Error opening font: " << TTF_GetError() << std::endl;
        return 0;
    }

    // Palette Widget
    pal_surface = SDL_CreateRGBSurface(0, PAL_COLS, PAL_ROWS, BPP, 0, 0, 0, 0);
    if (!pal_surface)
    {
        std::cerr << "Palette Surface creation failed: " << SDL_GetError() << std::endl;
        return 0;
    }
    pal_surface_p = (uint32_t*)pal_surface->pixels;
    pal_texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, PAL_COLS, PAL_ROWS);
    if (!pal_texture)
    {
        std::cerr << "Palette Texture creation failed: " << SDL_GetError() << std::endl;
        return 0;
    }

    selected_palette = PAL_GREYSCALE;
    refresh_palette();

    return 1;
}

void Video::close()
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    if (font != NULL)
    {
        TTF_CloseFont(font);
        font = NULL;
    }
    if (pal_surface != NULL)
    {
        SDL_FreeSurface(pal_surface);
        pal_surface_p = NULL;
        pal_surface = NULL;
    }
    if (pal_texture != NULL)
    {
        SDL_DestroyTexture(pal_texture);
        pal_texture = NULL;
    }
}

void Video::sdl_screen_size()
{
    if (scn_width == 0 || scn_height == 0)
    {
        SDL_DisplayMode info;
        SDL_GetCurrentDisplayMode(0, &info);

        scn_width  = info.w; 
        scn_height = info.h;
    }
}

void Video::set_scale(int s)
{
    scale = s;
    resize_window(win_width, win_height);
}

bool Video::resize_window(int w, int h)
{
    win_width  = std::max(S16_SCREEN_WIDTH + X_PADDING, w);
    win_height = h;

    src_rect.w = win_width;
    src_rect.h = win_height;
    src_rect.x = 0;
    src_rect.y = 0;
    dst_rect.w = win_width * scale;
    dst_rect.h = win_height * scale;
    dst_rect.x = 0;
    dst_rect.y = 0;

    // Max Width and Height Sprite Layer Renders To
    sprite_layer->src_width = win_width;
    sprite_layer->src_height = win_height;

    // Set the video mode
    if (surface != NULL)
    {
        screen_pixels = NULL;
        SDL_FreeSurface(surface);
        surface = NULL;
    }

    surface = SDL_CreateRGBSurface(0,
                                   win_width,
                                   win_height,
                                   BPP,
                                   0,
                                   0,
                                   0,
                                   0);

    if (!surface)
    {
        std::cerr << "Surface creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Convert the SDL pixel surface to 32 bit.
    // This is potentially a larger surface area than the internal pixel array.
    screen_pixels = (uint32_t*)surface->pixels;

    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                win_width, win_height);

    if (!texture)
    {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Video::draw_frame(void)
{
    clear_screen();
    (sprite_layer->*(sprite_layer->render))();
    SDL_UpdateTexture(texture, NULL, screen_pixels, win_width * sizeof(Uint32));
    SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);

    if (show_hud)
    {
        int h = 0;
        h += draw_text("Palette: " + std::to_string(selected_palette), X_PADDING, Y_PADDING, ANCHOR_RIGHT);
        h += draw_text("Y Pos: " + std::to_string(sprite_layer->display_y_off), X_PADDING, Y_PADDING + h, ANCHOR_RIGHT);
        h += draw_text("Scale: " + std::to_string(scale) + "x", X_PADDING, Y_PADDING + h, ANCHOR_RIGHT);
        draw_palette(X_PADDING, Y_PADDING + h, ANCHOR_RIGHT);
    }

    SDL_RenderPresent(renderer);
}

int Video::draw_text(std::string text, int x, int y, int anchor)
{
    if (font != NULL)
    {
        int tex_w = 0;
        int tex_h = 0;
        SDL_Surface* font_surface = TTF_RenderText_Solid(font, text.c_str(), font_color);
        SDL_Texture* font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
        SDL_QueryTexture(font_texture, NULL, NULL, &tex_w, &tex_h);

        if (anchor == ANCHOR_RIGHT) 
            x = win_width - tex_w - x;

        SDL_Rect font_rect = { x, y, tex_w, tex_h };
        SDL_RenderCopy(renderer, font_texture, NULL, &font_rect);
        SDL_DestroyTexture(font_texture);
        SDL_FreeSurface(font_surface);
        return tex_h;
    }
    return 0;
}

void Video::draw_palette(int x, int y, int anchor)
{
    int tex_w = 0;
    int tex_h = 0;
    SDL_QueryTexture(pal_texture, NULL, NULL, &tex_w, &tex_h);
    if (anchor == ANCHOR_RIGHT) 
        x = win_width - (tex_w * PAL_BLOCK_SIZE) - x;
    SDL_UpdateTexture(pal_texture, NULL, pal_surface_p, PAL_COLS * sizeof(Uint32));
    SDL_Rect dst_rect = { x, y, tex_w * PAL_BLOCK_SIZE, tex_h * PAL_BLOCK_SIZE };
    SDL_RenderCopy(renderer, pal_texture, NULL, &dst_rect);
}

void Video::set_pixel(uint32_t x, uint32_t y, uint8_t index)
{
    screen_pixels[(y * win_width) + x] = rgb[index];
}

void Video::clear_screen()
{
    const int len = win_width * win_height;
    for (int i = 0; i < len; i++)
        screen_pixels[i] = bg_colour;
}

SDL_Surface* Video::get_surface()
{
    return surface;
}

void Video::next_palette()
{
    if (max_pal_entries < 0) return;

    if (++video.selected_palette > max_pal_entries)
        video.selected_palette = PAL_GREYSCALE;

    refresh_palette();
}

void Video::prev_palette()
{
    if (max_pal_entries < 0) return;

    if (--video.selected_palette < PAL_GREYSCALE)
        video.selected_palette = max_pal_entries;

    refresh_palette();
}

void Video::scroll(int y_scr)
{
    sprite_layer->display_y_off += y_scr;
    
    if (sprite_layer->display_y_off < 0)
        sprite_layer->display_y_off = 0;

    else if (sprite_layer->display_y_off >= sprite_layer->y_max)
        sprite_layer->display_y_off = sprite_layer->y_max;
}

// Cycle background colours used
void Video::cycle_background(bool cycle)
{
    const static uint32_t BG_COLOURS[] = { 0, 0xFFFFFF, 0x7F0000, 0x4800FF };
    int size = sizeof(BG_COLOURS)/sizeof(BG_COLOURS[0]);

    if (cycle && ++bg_index >= size)
        bg_index = 0;

    bg_colour = BG_COLOURS[bg_index];

    font_color.r = font_color.g = font_color.b = bg_colour == 0xFFFFFF ? 0 : 0xff;
}

void Video::toggle_hud()
{
    show_hud = !show_hud;
}

void Video::refresh_palette()
{
    uint32_t pal, r, g, b;

    // Default: 32 Bytes, 16 Entries Per Palette. Although some input formats vary
    for (int i = 0; i < (bytes_per_entry / 2); i++)
    {
        if (selected_palette == PAL_GREYSCALE)
        {
            pal = PAL_LENGTH - 1 - (i * 2);
            r = g = b = pal;
        }
        else
        {
            pal = selected_palette * bytes_per_entry;

            uint32_t a = (palette[(i * 2) + pal] << 8) | palette[(i * 2) + pal + 1];
            r = (a & 0x000f) << 1; // r rrr0
            g = (a & 0x00f0) >> 3; // g ggg0
            b = (a & 0x0f00) >> 7; // b bbb0
            if ((a & 0x1000) != 0)
                r |= 1; // r rrrr
            if ((a & 0x2000) != 0)
                g |= 1; // g gggg
            if ((a & 0x4000) != 0)
                b |= 1; // b bbbb
        }

        r = r * 255 / 31;
        g = g * 255 / 31;
        b = b * 255 / 31;

        pal_surface_p[i+pal_offset] = rgb[i+pal_offset] = CURRENT_RGB();
    }
}