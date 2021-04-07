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
    void init(const uint8_t*);
    void render();
    void render4();
    void render8();
    void render16();

private:
    uint32_t sprites_length;
    uint32_t* sprites; // Converted sprites

    void draw_pixel(const int32_t x, const int32_t y, const uint16_t pix);
};

