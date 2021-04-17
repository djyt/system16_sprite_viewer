/***************************************************************************
    XML Configuration File Handling.

    Load Settings.

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#pragma once

#include <set>
#include <string>
#include <vector>
#include "globals.hpp"

struct rom_t
{
    std::string filename;
    int src_offset = 0;
    int dst_offset = 0;
    int length = 0;
};

struct sprites_t
{
    // Sprite Data Pixel Format
    int format = format::PIX4;

    // Information About Individual Rom Files
    std::vector <rom_t> rom;

    // Palette Information
    std::string palname;
    int pal_bytes_per_entry = PAL_LENGTH;
    int pal_offset = 0;

    // Interleave override
    int interleave = 0;
    bool swap_endian = false;
};


class Config
{
public:
    sprites_t sprites;

    Config(void);
    ~Config(void);

    void set_config_file(const std::string& filename);
    bool load();

private:
    std::string cfg_file;
};

extern Config config;
