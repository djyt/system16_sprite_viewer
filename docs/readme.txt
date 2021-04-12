-------------------------------------------------------------------------------
                              Sega System 16 Sprite Viewer
                                reassembler.blogspot.com
                                      
                                    (c) 2021 dj_yt
...............................................................................

-------------------------------------------------------------------------------
  What is this? 
-------------------------------------------------------------------------------

This application is a quick and dirty Sega sprite viewer.

This sprite viewer supports sprites from the following arcade hardware:

1/ Sega Hang-On / Space Harrier
2/ Sega System 16
3/ Sega System 18
4/ Sega OutRun / Super Hang-On 
5/ Sega X-Board (AfterBurner, Thunderblade etc.)
6/ Sega Y-Board (Power Drift, G-Loc etc.)


-------------------------------------------------------------------------------
 Getting Started
-------------------------------------------------------------------------------

Try the application with one of the example configuration files. 

You will find out the box support for OutRun, AfterBurner, Space Harrier, 
Golden Axe and others.

1/ Copy the relevant sprite roms to a directory.
2/ Edit the appropriate xml file to reflect the location of the roms and 
   palette data.
3/ From the command line, run 's16_viewer <config.xml>' 
   Where config.xml is replaced with the name of the configuration file.
   
It's possible to create your own configuration files for your favourite titles.
I'll add more in due course.

It helps to be familar with reading the MAME source code to set a game up 
correctly. 


-------------------------------------------------------------------------------
 Controls 
-------------------------------------------------------------------------------

Cursor Up / Down    : Scroll
Page Up / Down      : Scroll Page
Home / End          : Scroll To Start / End
Cursor Left / Right : Change Palette
+ / -               : Zoom
B                   : Change Background Colour
S                   : Save Screenshot
Space               : Toggle HUD


-------------------------------------------------------------------------------
 Limitations
-------------------------------------------------------------------------------

The sprite rom files do not contain any information aside from the raw pixel
data. There's no palette information for example. This is always setup by the 
main 68000 program code.

Therefore, there's no easy way to automate extracting palette data or any 
useful information regarding the sprites beyond what is displayed. 


-------------------------------------------------------------------------------
 Extracting Palettes
-------------------------------------------------------------------------------

Each palette entry is 32 bytes (16 colours per sprite @ 2 bytes each).

Different hardware allows a varying number of sprite palettes to be in memory
at any time:

1/ System 16 Hardware: 64 Palette Entries  [Length: 0x0800]
2/ OutRun Hardware:    128 Palette Entries [Length: 0x1000]
3/ X-Board Hardware:   256 Palette Entries [Length: 0x2000]

Palettes are not stored with the sprite data. Instead they are located in the
68k program code. This unfortunately means that there isn't an automatic or
easy way to rip palette data.

You do not have to extract the palette. A default greyscale palette is provided
as a fallback.

To extract the palette data, there are two approaches:

1/ Identify its location in the 68K program code. This will require a small
   amount of reverse engineering/debugging. It's often easiest to identify
   where the palette RAM is located and then trace the code back to where it
   is populated from.
2/ Extract the 'live palette' direct from palette RAM in MAME.

Extracting from palette RAM whilst the game is running is possibly the easiest
way to grab the palette. However, it will only result in the palettes being
extracted in memory at that moment in time. Therefore, anything not in memory
will be missing. This is why the first approach is more thorough. 

Here is an example to extract the palette from rom location 0x14ed8 to a 
file named 'outrun.pal' using the MAME debugger:

 save outrun.pal,0x14ed8,0x2000,0
 
This palette is partially copied to palette RAM at runtime. But as mentioned,
this would not contain every available palette entry. Example:

 save outrun.pal,0x121000,0x1000,0

...............................................................................

For System 16 games, palette RAM is allocated dynamically at run-time. It can
be derived from the MAME debugger by following these steps:

1/ View the Sega 315-5195 Memory Mapper m_regs configuration.
2/ Grab the value at offset $1d. Left shift by $10. 
   This is the location of palette ram in memory.
3/ Often the sprite palette is at offset +$800 within the palette RAM. 

An example of the above for Altered Beast:

1/ The offset $1d in the memory mapper has a value of $84. 

2/ Left shift by $10 gets you memory location $840000.
   Sprite Palette RAM is therefore located at $840800.

3/ We can set a memory watch point at $840800. This informs us which part of 
   the game-code is writing to the sprite colour palette.
   
4/ We can then determine a routine at $2dc0 is copying to this location from 
   address $24ccc in the game code. (Using the 'altbeast' set in MAME). 
   
5/ If we set breakpoints at appropriate places in this routine we find the
   copies from the following locations take place for the first few cycles.
   
   $242bc - $242d8
   $242f4 - $24310   
   $243b8 - $243d4   
   $243d4 - $243f0
   $24818 - $24834
   $24ad4 - $24af0
   $24ccc - $24ce8
   
   Note that this routine copies 28 bytes at a time. 
   This is useful info as it tells us the length of each palette entry.
   
6/ This gives us a good idea of where the palette data is stored. We can trace
   back through the code further to discover that the start pointer is setup
   at the assembled code location $3c2c. 
   This routine specifies $242a0 as the first sprite palette entry, so close 
   to our first rough value we found above.
   A byte is used (0-255) as an index into the table of entries. So we can 
   assume that the table has 255 entries max.
   At 28 bytes per entry, this means the table is $1be4 entries long.
   
7/ We can now rip the palette as follows using the MAME debugger:
   save altbeast.pal,0x242a0,0x1be4,0
   
8/ Note that it is configured as follows in the XML:
  <pal name="roms/altbeast/altbeast.pal" bytes_per_entry="28" offset="1"/>
  
  We specify the number of bytes per entry, and the fact that the palette
  data is offset by 1 word when displayed by hardware. 

You'll notice that this isn't a particularly easy or quick experience. :-D

-------------------------------------------------------------------------------