#pragma once

#include "stdint.hpp"

namespace settings
{
    const char PROGRAM_NAME[] = "System 16 Sprite Viewer";
    const char VERSION[] = "0.3";
};

namespace format
{
    enum
    {
        UNKNOWN = -1,
        PIX4  = 0,   // System 16 (Golden Axe, Shinobi etc.)
        PIX8  = 1,   // OutRun / X-Board (AfterBurner etc.)
        PIX16 = 2,   // Y-Board (Power Drift etc.)
    };
};

// Max width of screen on original hardware
const static int S16_SCREEN_WIDTH = 320;