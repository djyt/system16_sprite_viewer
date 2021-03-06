#pragma once

#include "stdint.hpp"

namespace settings
{
    const char PROGRAM_NAME[] = "System 16 Sprite Viewer";
    const char VERSION[] = "0.31";

    // Padding around window edge
    const static int X_PADDING = 3;
    const static int Y_PADDING = 3;

    const static int FONT_SIZE = 24;
    const static int PAL_BLOCK_SIZE = 24;
};

namespace format
{
    enum
    {
        UNKNOWN = -1,
        PIX4 = 0,       // System 16 (Golden Axe, Shinobi etc.)
        PIX8 = 1,       // OutRun / X-Board (AfterBurner etc.)
        PIX8_SH = 2,    // Space Harrier & Hang-On Hardware
        PIX16 = 3,      // Y-Board (Power Drift etc.)
    };
};

// Max width of screen on original hardware
const static int S16_SCREEN_WIDTH = 320;

// Default palette length in bytes
const static int PAL_LENGTH = 32;