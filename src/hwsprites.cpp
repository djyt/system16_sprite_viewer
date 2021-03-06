#include "globals.hpp"
#include "hwsprites.hpp"

/***************************************************************************
    Sprite Rendering Code.
    Based on MAME source code.

    Copyright Aaron Giles.
    All rights reserved.
***************************************************************************/
/*
     Sprite data

     Sprite pattern data is arranged as groups of words, within each word every
     four bits defines one pixel, like so:

     MSB          LSB
     aaaabbbbccccdddd

     a = Pixel 0
     b = Pixel 1
     c = Pixel 2
     d = Pixel 3

     Some sprite pixel values have special meanings. Zero is transparent; such
     pixels are not displayed. A value of 15 indicates an end marker - this is
     also transparent, but additionally tells the sprite generator to stop
     drawing the current line of a sprite. It's the end markers that define the
     width of the sprite, which can vary on every line. I'll discuss the end
     markers later on.
*/

hwsprites::hwsprites()
{
    src_width      = 0;
    src_height     = 0;
    display_y_off  = 0;
    sprites_length = 0;
    y_max          = 0;
    render         = nullptr;
    sprdata        = nullptr;
}

hwsprites::~hwsprites()
{
    if (sprdata != nullptr)
        delete[] sprdata;
}

void hwsprites::init(const uint8_t* src_sprites, const int format, const int length, const bool swap_endian)
{
    // Convert S16 tiles to a more useable format
    if (format == format::PIX8 || format == format::PIX8_SH || format == format::PIX16)
    {
        const uint8_t *spr = src_sprites;
        sprites_length = length >> 2;
        sprdata = new uint32_t[sprites_length];

        for (uint32_t i = 0; i < sprites_length; i++)
        {
            uint8_t d3 = *spr++;
            uint8_t d2 = *spr++;
            uint8_t d1 = *spr++;
            uint8_t d0 = *spr++;

            if (format == format::PIX8 || format == format::PIX8_SH)
                sprdata[i] = (d0 << 24) | (d1 << 16) | (d2 << 8) | d3;
            else if (format == format::PIX16)
                sprdata[i] = (d3 << 24) | (d2 << 16) | (d1 << 8) | d0;
        }

        if (format == format::PIX8)
        {
            y_max = count8();
            render = &hwsprites::render8;
        }
        else if (format == format::PIX8_SH)
        {
            y_max = count4();
            render = &hwsprites::render8_sh;
        }
        else if (format == format::PIX16)
        {
            y_max = count16();
            render = &hwsprites::render16;
        }
    }
    else if (format == format::PIX4)
    {
        const uint8_t *spr = src_sprites;
        sprites_length = length >> 1;
        sprdata = new uint32_t[sprites_length];

        for (uint32_t i = 0; i < sprites_length; i++)
        {
            uint8_t d1 = *spr++;
            uint8_t d0 = *spr++;

            sprdata[i] = !swap_endian ? (d1 << 8) | d0 : (d0 << 8) | d1;
        }
        y_max = count4();
        render = &hwsprites::render4;
    }
    y_max += settings::Y_PADDING;
}

// System 16B Rendering
void hwsprites::render4()
{
    uint8_t pix;
    uint16_t pixels = 0;
    int32_t y = settings::Y_PADDING;

    // Screen X-Coordinates
    int32_t sx = settings::X_PADDING;
 
    for (uint32_t counter = 0; counter < sprites_length; counter++)
    {
        pixels = sprdata[counter];

        if (sx == settings::X_PADDING && (!pixels || (pixels & 0xf) == 0xf))
            continue;
        
        if (y >= display_y_off && y < display_y_off + src_height)
        {        
            // Screen Y-Coordinates
            uint32_t sy = y - display_y_off;

            // draw four pixels
            pix = (pixels >> 12) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  8) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  4) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  0) & 0xf; draw_pixel(sx, sy, pix); sx++;
        }
        else
            sx += 4;

        // stop if the last pixel in the group was 0xf
        if ((pixels & 0xF) == 0xF)
        {
            sx = settings::X_PADDING;
            y++;
        }
    }
}

// Space Harrier Hardware
void hwsprites::render8_sh()
{
    uint8_t pix;
    uint32_t pixels = 0;
    int32_t y = settings::Y_PADDING;

    // Screen X-Coordinates
    int32_t sx = settings::X_PADDING;

    for (uint32_t counter = 0; counter < sprites_length; counter++)
    {
        pixels = sprdata[counter];

        if (sx == settings::X_PADDING && (!pixels || (pixels & 0xf) == 0xf))
            continue;

        if (y >= display_y_off && y < display_y_off + src_height)
        {        
            // Screen Y-Coordinates
            uint32_t sy = y - display_y_off;

            // draw eight pixels
            pix = (pixels >> 28) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 24) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 20) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 16) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 12) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  8) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  4) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  0) & 0xf; draw_pixel(sx, sy, pix); sx++;
        }
        else
            sx += 8;

        // stop if the second-to-last pixel in the group was 0xf
        if ((pixels & 0xf) == 0xf)
        {
            sx = settings::X_PADDING;
            y++;
        }
    }
}

// OutRun & X-Board Rendering
void hwsprites::render8()
{
    uint8_t pix;
    uint32_t pixels = 0;
    int32_t y = settings::Y_PADDING;

    // Screen X-Coordinates
    int32_t sx = settings::X_PADDING;
   
    for (uint32_t counter = 0; counter < sprites_length; counter++)
    {
        pixels = sprdata[counter];

        if (sx == settings::X_PADDING && pixels == 0x000000F0)
            continue;
        
        if (y >= display_y_off && y < display_y_off + src_height)
        {        
            // Screen Y-Coordinates
            uint32_t sy = y - display_y_off;

            // draw eight pixels
            pix = (pixels >> 28) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 24) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 20) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 16) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >> 12) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  8) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  4) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixels >>  0) & 0xf; draw_pixel(sx, sy, pix); sx++;
        }
        else
            sx += 8;

        // stop if the second-to-last pixel in the group was 0xf
        if ((pixels & 0x000000f0) == 0x000000f0)
        {
            sx = settings::X_PADDING;
            y++;
        }
    }
}

// Y-Board Rendering
void hwsprites::render16()
{
    uint8_t pix;
    uint32_t pixelsL = 0;
    uint32_t pixelsH = 0;
    int32_t y = settings::Y_PADDING;

    // Screen X-Coordinates
    int32_t sx = settings::X_PADDING;
   
    for (uint32_t counter = 0; counter < sprites_length; counter+=2)
    {
        pixelsH = sprdata[counter];
        pixelsL = sprdata[counter + 1];

        if (sx == settings::X_PADDING && pixelsH == 0 && pixelsL == 0xF)
            continue;
        
        if (y >= display_y_off && y < display_y_off + src_height)
        {        
            // Screen Y-Coordinates
            uint32_t sy = y - display_y_off;

            // draw sixteen pixels
            pix = (pixelsH >> 28) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsH >> 24) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsH >> 20) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsH >> 16) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsH >> 12) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsH >>  8) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsH >>  4) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsH >>  0) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >> 28) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >> 24) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >> 20) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >> 16) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >> 12) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >>  8) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >>  4) & 0xf; draw_pixel(sx, sy, pix); sx++;
            pix = (pixelsL >>  0) & 0xf; draw_pixel(sx, sy, pix); sx++;
        }
        else
            sx += 16;

        // stop if the last pixel in the group was 0xf
        if ((pixelsL & 0xF) == 0xF)
        {
            sx = settings::X_PADDING;
            y++;
        }
    }
}

// Count number of lines in file
int hwsprites::count4()
{
    uint32_t pixels = 0;
    int32_t y = 0;

    // Screen X-Coordinates
    int32_t sx = 0;

    for (uint32_t counter = 0; counter < sprites_length; counter++)
    {
        pixels = sprdata[counter];

        if (sx == 0 && (!pixels || (pixels & 0xf) == 0xf))
            continue;

        sx += 4;

        if ((pixels & 0xF) == 0xF)
        {
            sx = 0;
            y++;
        }
    }
    return y;
}

int hwsprites::count8()
{
    uint32_t pixels = 0;
    int32_t y = 0;
    
    // Screen X-Coordinates
    int32_t sx = 0;

    for (uint32_t counter = 0; counter < sprites_length; counter++)
    {
        pixels = sprdata[counter];

        if (sx == 0 && pixels == 0x000000F0)
            continue;

        sx += 8;

        // stop if the second-to-last pixel in the group was 0xf
        if ((pixels & 0x000000f0) == 0x000000f0)
        {
            sx = 0;
            y++;
        }
    }
    return y;
}

int hwsprites::count16()
{
    uint32_t pixelsL = 0;
    uint32_t pixelsH = 0;
    int32_t y = 0;

    // Screen X-Coordinates
    int32_t sx = 0;

    for (uint32_t counter = 0; counter < sprites_length; counter+=2)
    {
        pixelsH = sprdata[counter];
        pixelsL = sprdata[counter + 1];

        if (sx == 0 && pixelsH == 0 && pixelsL == 0xF)
            continue;

        sx += 16;

        // stop if the last pixel in the group was 0xf
        if ((pixelsL & 0xF) == 0xF)
        {
            sx = 0;
            y++;
        }
    }
    return y;
}

inline void hwsprites::draw_pixel(const int32_t x, const int32_t y, const uint8_t pix)
{
    if (x >= settings::X_PADDING && x < S16_SCREEN_WIDTH + settings::X_PADDING && pix != 0 && pix != 15)
    {
        video.set_pixel(x, y, pix);
    }   
}
