#pragma once

class RomLoader
{

public:
    enum {NORMAL = 1, INTERLEAVE2 = 2, INTERLEAVE4 = 4, INTERLEAVE8 = 8};

    uint8_t* rom;

    // Size of rom
    uint32_t length;

    RomLoader();
    ~RomLoader();
    void init(uint32_t);
    bool load(const char* filename, const int offset, const int length, const uint8_t mode = NORMAL);
    bool load_continue(const char* filename, const int src_offset, const int dst_offset, const int length, const uint8_t mode = NORMAL);
    void unload();
    uint32_t get_filesize(const char* filename);
    uint32_t read32(uint32_t*);
    uint16_t read16(uint32_t*);
    uint8_t read8(uint32_t*);

    uint32_t read32(uint32_t);
    uint16_t read16(uint32_t);
    uint8_t read8(uint32_t);

private:    
    void error(const char* p, const char* p2 = "");
};