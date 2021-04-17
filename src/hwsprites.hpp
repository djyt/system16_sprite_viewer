#pragma once

#include "globals.hpp"
#include "video.hpp"

class hwsprites
{
public:
    int src_width, src_height;

    // Y Offset to draw sprites at
    int32_t display_y_off;

    // Number of lines in sprite file
    int32_t y_max;

    hwsprites();
    ~hwsprites();
    void init(const uint8_t*, const int format, const int length, const bool swap_endian);
    void (hwsprites::*render)();

private:
    uint32_t sprites_length;
    uint32_t* sprdata; // Converted sprites

    void render4();
    void render8_sh();
    void render8();
    void render16();
    int count4();
    int count8();
    int count16();
    inline void draw_pixel(const int32_t x, const int32_t y, const uint8_t pix);
};

