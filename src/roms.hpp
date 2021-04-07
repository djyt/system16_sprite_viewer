#pragma once

#include "romloader.hpp"

class Roms
{
public:
    RomLoader sprites;

    // Autodetected game type
    uint8_t type;

    enum
    {
        UNKNOWN, // Failed to detect type
        PIX4,    // System 16
        PIX8,    // OutRun & X-Board
        PIX16,   // Y-Board
    };

    Roms();
    ~Roms();
    bool init(int argc, char* argv[]);
};

extern Roms roms;