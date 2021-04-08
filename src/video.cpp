#include "globals.hpp"
#include "video.hpp"

Video video;

using namespace settings;

// See: SDL_PixelFormat
#define CURRENT_RGB() (r << Rshift) | (g << Gshift) | (b << Bshift);

Video::Video(void)
{
    texture       = NULL;
    renderer      = NULL;
    surface       = NULL;
    screen_pixels = NULL;
    sprite_layer  = new hwsprites();
    scn_width     = 0;
    scn_height    = 0;
    bg_index      = 0;
    cycle_background(false);
    scale = 2;
}

Video::~Video(void)
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    delete sprite_layer;
}

int Video::init(uint8_t* pal, int pal_data_len)
{
    // Setup Custom Palette
    this->palette = pal;
    if (pal == nullptr || pal_data_len == -1)
        max_pal_entries = -1;
    else
        max_pal_entries = (pal_data_len / BYTES_PER_ENTRY) - 1;

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

    selected_palette = PAL_GREYSCALE;

    return 1;
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
    win_width  = w;
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

    const int bpp = 32;
    surface = SDL_CreateRGBSurface(0,
                                   win_width,
                                   win_height,
                                   bpp,
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
    //(sprite_layer.*(sprite_layer.render))();
    (sprite_layer->*(sprite_layer->render))();
    SDL_UpdateTexture(texture, NULL, screen_pixels, win_width * sizeof (Uint32));
    SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);
    SDL_RenderPresent(renderer);
}

void Video::set_pixel(uint32_t x, uint32_t y, uint8_t index)
{
    // Greyscale Palette
    if (selected_palette == PAL_GREYSCALE)
    {
        index = 0xFF - (index << 4);
        uint32_t r = index;
        uint32_t g = index;
        uint32_t b = index;
        screen_pixels[(y * win_width) + x] = CURRENT_RGB();
    }
    // Colour Palette
    else
    {
        screen_pixels[(y * win_width) + x] = rgb[index];
    }
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

// Cycle background colours used
void Video::cycle_background(bool cycle)
{
    const static uint32_t BG_COLOURS[] = { 0, 0xFFFFFF, 0x7F0000, 0x4800FF };
    int size = sizeof(BG_COLOURS)/sizeof(BG_COLOURS[0]);

    if (cycle && ++bg_index >= size)
        bg_index = 0;

    bg_colour = BG_COLOURS[bg_index];
}

void Video::refresh_palette()
{
    if (selected_palette == PAL_GREYSCALE)
        return;

    // 32 Bytes, 16 Entries Per Palette
    for (int i = 0; i < (BYTES_PER_ENTRY / 2); i++)
    {
        uint32_t pal = selected_palette * BYTES_PER_ENTRY;

        uint32_t a = (palette[(i * 2) + pal] << 8) | palette[(i * 2) + pal + 1];
        uint32_t r = (a & 0x000f) << 1; // r rrr0
        uint32_t g = (a & 0x00f0) >> 3; // g ggg0
        uint32_t b = (a & 0x0f00) >> 7; // b bbb0
        if ((a & 0x1000) != 0)
            r |= 1; // r rrrr
        if ((a & 0x2000) != 0)
            g |= 1; // g gggg
        if ((a & 0x4000) != 0)
            b |= 1; // b bbbb

        r = r * 255 / 31;
        g = g * 255 / 31;
        b = b * 255 / 31;

        rgb[i] = CURRENT_RGB();
    }
}
