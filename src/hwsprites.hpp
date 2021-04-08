#pragma once

#include "stdint.hpp"
#include "video.hpp"

class video;

class hwsprites
{
public:
    int src_width, src_height;

    // Y Offset to draw sprites at
    int32_t display_y_off;

    hwsprites();
    ~hwsprites();
    void init(const uint8_t*, int format, int length);
    //void render();
    void (hwsprites::*render)();

private:
    uint32_t sprites_length;
    uint32_t* sprdata; // Converted sprites

    void render4();
    void render8();
    void render16();
    void draw_pixel(const int32_t x, const int32_t y, const uint16_t pix);
};

