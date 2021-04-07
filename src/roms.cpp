#include "stdint.hpp"
#include "roms.hpp"

/***************************************************************************
    Load OutRun ROM Set.

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

Roms roms;

Roms::Roms()
{
}

Roms::~Roms(void)
{
}

bool Roms::init(int argc, char* argv[])
{
    uint8_t length = (argc - 1);
    type = UNKNOWN; // Default to unknown type
  
    if (length == 8)
    {
        type = PIX16;

        sprites.init(0x80000 * 2);
        if (!sprites.load(argv[1],  0x000000, 0x20000, RomLoader::INTERLEAVE8)) return false;
        if (!sprites.load(argv[2],  0x000001, 0x20000, RomLoader::INTERLEAVE8)) return false;
        if (!sprites.load(argv[3],  0x000002, 0x20000, RomLoader::INTERLEAVE8)) return false;
        if (!sprites.load(argv[4],  0x000003, 0x20000, RomLoader::INTERLEAVE8)) return false;
        if (!sprites.load(argv[5],  0x000004, 0x20000, RomLoader::INTERLEAVE8)) return false;
        if (!sprites.load(argv[6],  0x000005, 0x20000, RomLoader::INTERLEAVE8)) return false;
        if (!sprites.load(argv[7],  0x000006, 0x20000, RomLoader::INTERLEAVE8)) return false;
        if (!sprites.load(argv[8],  0x000007, 0x20000, RomLoader::INTERLEAVE8)) return false;
    }
    if (length == 4)
    {
        type = PIX8;

        sprites.init(0x80000);
        if (!sprites.load(argv[1],  0x000000, 0x20000, RomLoader::INTERLEAVE4)) return false;
        if (!sprites.load(argv[2],  0x000001, 0x20000, RomLoader::INTERLEAVE4)) return false;
        if (!sprites.load(argv[3],  0x000002, 0x20000, RomLoader::INTERLEAVE4)) return false;
        if (!sprites.load(argv[4],  0x000003, 0x20000, RomLoader::INTERLEAVE4)) return false;
    }
    else if (length == 2)
    {
        type = PIX4;

        uint32_t file_size = sprites.get_filesize(argv[1]); // 0x40000 / 2

        sprites.init(file_size * 2);
        if (!sprites.load(         argv[1],                 0x000000,      file_size / 2, RomLoader::INTERLEAVE2)) return false;
        if (!sprites.load_continue(argv[1],  file_size / 2, file_size + 0, file_size / 2, RomLoader::INTERLEAVE2)) return false;
        if (!sprites.load(         argv[2],                 0x000001,      file_size / 2, RomLoader::INTERLEAVE2)) return false;
        if (!sprites.load_continue(argv[2],  file_size / 2, file_size + 1, file_size / 2, RomLoader::INTERLEAVE2)) return false;
    }

    return true;
    // AfterBurner 2
    /*else if (game == 1)
    {
        sprites.init(0x200000);
        sprites.load( "mpr-10932.90",  0x000000, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "mpr-10934.94",  0x000001, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "mpr-10936.98",  0x000002, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "mpr-10938.102", 0x000003, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "mpr-10933.91",  0x080000, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "mpr-10935.95",  0x080001, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "mpr-10937.99",  0x080002, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "mpr-10939.103", 0x080003, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10942.92",  0x100000, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10943.96",  0x100001, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10944.100", 0x100002, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10945.104", 0x100003, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10946.93",  0x180000, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10947.97",  0x180001, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10948.101", 0x180002, 0x20000, RomLoader::INTERLEAVE4);
        sprites.load( "epr-10949.105", 0x180003, 0x20000, RomLoader::INTERLEAVE4);
    }*/
}