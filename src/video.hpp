#pragma once

// For reporting errors :)
#include <iostream>

#include <SDL.h>
#include "stdint.hpp"
#include "romloader.hpp"
#include "hwsprites.hpp"

class hwsprites;

class Video
{
public:
	static const uint16_t PALETTE_ENTRIES = 32;

    // Window Width & Height
    int win_width, win_height;

    // Scale/Zoom value
    int scale;

    int32_t selected_palette;

	hwsprites* sprite_layer;

	Video();
	void draw_frame();
	int init(uint8_t*);
	~Video();

    void sdl_screen_size();
    void set_scale(int s);
    bool resize_window(int w, int h);
    void set_pixel(uint32_t x, uint32_t y, uint8_t index);
    void clear_screen();
    void cycle_background(bool cycle = true);
    void refresh_palette();
    SDL_Surface* get_surface();
    void next_palette();
    void prev_palette();

private:
    uint32_t rgb[PALETTE_ENTRIES];

    uint32_t *screen_pixels;

	SDL_Surface *surface;

    // SDL2 window
    SDL_Window *window;

    // SDL2 renderer
    SDL_Renderer *renderer;

    // SDL2 texture
    SDL_Texture *texture;

    // SDL2 blitting rects for hw scaling 
    // ratio correction using SDL_RenderCopy()
    SDL_Rect src_rect;
    SDL_Rect dst_rect;

    // SDL Pixel Format Codes. These differ between platforms.
    uint8_t  Rshift, Gshift, Bshift;

    // Original Screen Width & Height
    uint16_t scn_width, scn_height;

    const static int PAL_GREYSCALE = -1;

    // Background colour cycling
    uint32_t bg_colour;
    int bg_index;
};

extern Video video;

