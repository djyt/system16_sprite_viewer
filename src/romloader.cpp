#include <iostream>
#include <fstream>

#include "globals.hpp"
#include "romloader.hpp"
#include "config.hpp"

/***************************************************************************
    Binary File Loader. 
    
    Handles loading an individual binary file to memory.
    Supports reading bytes, words and longs from this area of memory.

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

RomLoader::RomLoader()
{
    spr_data = nullptr;
    pal_data = nullptr;
}

RomLoader::~RomLoader()
{
    if (spr_data != nullptr) delete[] spr_data;
    if (pal_data != nullptr) delete[] pal_data;
}

int RomLoader::init(sprites_t sprites)
{
    format = sprites.format;

    // Calculate total length
    spr_data_len = 0;
    for (rom_t& rom : sprites.rom)
        spr_data_len += rom.length;
    spr_data = new uint8_t[spr_data_len];

    // Use correct interleave value
    int interleave = UNKNOWN;
    switch (format)
    {
        case format::PIX4:  interleave = 2; break;
        case format::PIX8:  interleave = 4; break;
        case format::PIX16: interleave = 8; break;
    }

    if (interleave == UNKNOWN)
    {
        std::cerr << "Unknown ROM format specified" << std::endl;
        return 0;
    }

    // Interleave roms
    for (rom_t& rom : sprites.rom)
        load(rom.filename, rom.src_offset, rom.dst_offset, rom.length, interleave);

    // Load Palette
    pal_data_len = -1;
    if (!sprites.palname.empty())
        pal_data_len = load_binary(sprites.palname, &pal_data);

    return 1;
}

void RomLoader::unload(void)
{
    if (spr_data != nullptr) delete[] spr_data;
}

bool RomLoader::load(std::string filename, const int src_offset, const int dst_offset, const int length, const uint8_t interleave)
{
    // Open rom file
    std::ifstream src(filename, std::ios::in | std::ios::binary);
    if (!src)
    {
        std::cerr << "cannot open rom: " << filename << std::endl;
        return false; // fail
    }
 
    if (src_offset > 0) 
        src.seekg(src_offset, std::ios::beg);

    char ch;
    int adr;
    for (int i = 0; i < length; i++)
    {
        adr = (i * interleave) + dst_offset;
        if (adr >= spr_data_len)
        {
            std::cerr << "Config Error: attempt to map " << filename << 
                         " to 0x" << std::hex << adr << 
                         " beyond total length of sprite roms 0x" << spr_data_len << std::endl;
            src.close();
            return false;
        }
        src.get(ch); // Get from source
        spr_data[adr] = ch;
    }

    src.close();
    return true; // success
}

int RomLoader::load_binary(std::string filename, uint8_t** dest)
{
    std::ifstream src(filename, std::ios::in | std::ios::binary);
    if (!src)
    {
        std::cout << "cannot open file: " << filename << std::endl;
        return -1;
    }

    int fs = get_filesize(filename);

    // Read file
    char* buffer = new char[fs];
    src.read(buffer, fs);

    // Update pointer to pointer
    *dest = (uint8_t*) buffer;

    // Clean Up
    src.close();

    return fs; // return file size on success
}

uint32_t RomLoader::get_filesize(std::string filename)
{
    std::ifstream src(filename, std::ios::ate | std::ios::binary);
    return (uint32_t) src.tellg();
}

uint32_t RomLoader::read32(uint32_t* addr)
{    
    uint32_t r = (spr_data[*addr] << 24) | (spr_data[*addr+1] << 16) | (spr_data[*addr+2] << 8) | (spr_data[*addr+3]);
    *addr += 4;
    return r;
}

uint16_t RomLoader::read16(uint32_t* addr)
{
    uint16_t r = (spr_data[*addr] << 8) | (spr_data[*addr+1]);
    *addr += 2;
    return r;
}

uint8_t RomLoader::read8(uint32_t* addr)
{
    return spr_data[(*addr)++]; 
}

uint32_t RomLoader::read32(uint32_t addr)
{    
    return (spr_data[addr] << 24) | (spr_data[addr+1] << 16) | (spr_data[addr+2] << 8) | spr_data[addr+3];
}

uint16_t RomLoader::read16(uint32_t addr)
{
    return (spr_data[addr] << 8) | spr_data[addr+1];
}

uint8_t RomLoader::read8(uint32_t addr)
{
    return spr_data[addr];
}
