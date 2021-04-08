#pragma once

struct sprites_t;

class RomLoader
{

public:
    enum {UNKNOWN = 0, NORMAL = 1, INTERLEAVE2 = 2, INTERLEAVE4 = 4, INTERLEAVE8 = 8};

    // Interleaved Sprite Data
    uint8_t* spr_data;
    int spr_data_len;

    // Palette Data
    uint8_t* pal_data;
    int pal_data_len;

    int format;

    RomLoader();
    ~RomLoader();
    void init(sprites_t sprites);
    bool load(std::string filename, const int src_offset, const int dst_offset, const int length, const uint8_t mode = NORMAL);
    void unload();
    uint32_t get_filesize(std::string filename);
    uint32_t read32(uint32_t*);
    uint16_t read16(uint32_t*);
    uint8_t read8(uint32_t*);

    uint32_t read32(uint32_t);
    uint16_t read16(uint32_t);
    uint8_t read8(uint32_t);

private:    
    int load_binary(std::string filename, uint8_t** dest);
    void error(const char* p, const char* p2 = "");
};