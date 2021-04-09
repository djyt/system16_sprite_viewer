#pragma once

#include "globals.hpp"
#include "video.hpp"

class hwsprites
{
public:
    int src_width, src_height;

    // Y Offset to draw sprites at
    int32_t display_y_off;

    hwsprites();
    ~hwsprites();
    void init(const uint8_t*, const int format, const int length);
    void (hwsprites::*render)();

private:
    uint32_t sprites_length;
    uint32_t* sprdata; // Converted sprites

    void render4();
    void render8();
    void render16();
    inline void draw_pixel(const int32_t x, const int32_t y, const uint16_t pix);
};

