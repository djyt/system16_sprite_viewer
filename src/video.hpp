#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include "stdint.hpp"
#include "romloader.hpp"
#include "hwsprites.hpp"

class hwsprites;

class Video
{
public:
    // Window Width & Height
    int win_width, win_height;

    // Scale/Zoom value
    int scale;

	hwsprites* sprite_layer;

	Video();
	void draw_frame();
	int init(uint8_t* pal, const int pal_data_len, const int bytes_per_entry, const int pal_offset);
    void close();
	~Video();

    void sdl_screen_size();
    void set_scale(int s);
    bool resize_window(int w, int h);
    void set_pixel(uint32_t x, uint32_t y, uint8_t index);
    void clear_screen();
    void cycle_background(bool cycle = true);
    void toggle_hud();
    SDL_Surface* get_surface();
    void next_palette();
    void prev_palette();
    void scroll(int y_scr);

private:
    static const int BPP = 32;
    static const int PAL_ENTRIES = PAL_LENGTH / 2;

    uint32_t rgb[PAL_ENTRIES];

    // Source: Palette bytes per entry
    int bytes_per_entry;

    // Destination: Palette offset to match original game setup
    int pal_offset;

    uint32_t *screen_pixels;
    uint8_t* palette;

	SDL_Surface *surface;

    // SDL2 window
    SDL_Window *window;

    // SDL2 renderer
    SDL_Renderer *renderer;

    // SDL2 texture
    SDL_Texture *texture;

    // Font
    TTF_Font* font;
    SDL_Color font_color;

    // Palette
    const static int PAL_COLS = PAL_ENTRIES / 2;
    const static int PAL_ROWS = PAL_ENTRIES / 8;
    SDL_Surface* pal_surface;
    SDL_Texture* pal_texture;
    uint32_t *pal_surface_p;

    // SDL2 blitting rects for hw scaling 
    // ratio correction using SDL_RenderCopy()
    SDL_Rect src_rect;
    SDL_Rect dst_rect;

    // SDL Pixel Format Codes. These differ between platforms.
    uint8_t  Rshift, Gshift, Bshift;

    // Original Screen Width & Height
    uint16_t scn_width, scn_height;

    // Current palette selected
    int32_t selected_palette;

    // Number of palette entries
    int max_pal_entries;
    const static int PAL_GREYSCALE = -1;

    // Background colour cycling
    uint32_t bg_colour;
    int bg_index;

    // Toggle HUD display
    bool show_hud;

    enum
    {
        ANCHOR_LEFT = 0,
        ANCHOR_RIGHT = 1,
    };

    int draw_text(std::string, int x, int y, int anchor);
    void draw_palette(int x, int y, int anchor);
    void refresh_palette();
};

extern Video video;

