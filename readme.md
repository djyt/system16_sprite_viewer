# Sega System 16 Sprite Viewer


A quick and dirty Sega arcade hardware sprite viewer, with optional palette support. 

Sprites from the following arcade hardware are supported:

- Sega Hang-On / Space Harrier
- Sega System 16
- Sega System 18
- Sega OutRun / Super Hang-On 
- Sega X-Board (AfterBurner, Thunderblade etc.)
- Sega Y-Board (Power Drift, G-Loc etc.)

## Screenshots

![shot1](https://user-images.githubusercontent.com/2414449/114319863-34efac00-9b0b-11eb-9310-2ca82dfd572f.png)
![shot4](https://user-images.githubusercontent.com/2414449/114319868-35884280-9b0b-11eb-8f78-7069c93fcd13.png)
![shot2](https://user-images.githubusercontent.com/2414449/114319864-35884280-9b0b-11eb-802e-49b360ffb0a5.png)
![shot3](https://user-images.githubusercontent.com/2414449/114319865-35884280-9b0b-11eb-9df1-7757d3d7e78a.png)
![shot5](https://user-images.githubusercontent.com/2414449/114319869-3620d900-9b0b-11eb-9b90-9a9577aeed60.png)


## Getting Started

Try the application with one of the example configuration files. You will find out the box support with palettes for:

- Space Harrier
- Hang On
- Super Hang On (Original & Limited Edition)
- OutRun
- AfterBurner
- Power Drift
- Shinobi
- Golden Axe
- Altered Beast


1. Copy the relevant sprite roms to a directory.
2. Edit the appropriate xml file to reflect the location of the roms and 
   palette data.
3. From the command line: *s16_viewer <config.xml>*

   Where config.xml is replaced with the name of the configuration file.
   
   
It's possible to create your own configuration files for your favourite titles. Please consult the [readme.txt](https://github.com/djyt/system16_sprite_viewer/blob/main/docs/readme.txt) for a more detailed guide. 

The latest set of configuration files can be found [here](https://github.com/djyt/system16_sprite_viewer/tree/main/res/config), which may be newer than those bundled with the release.

## Contributing

- Found something cool? Feel free to start a thread in the [Discussions](https://github.com/djyt/system16_sprite_viewer/discussions) area.
- If you have an XML and Palette to contribute, feel free to do a Push Request or send me a link to your files. 

## Advanced Reading

- [Ripping Sega System 16 Sprites & Palettes: A Brief Guide](http://reassembler.blogspot.com/2021/04/ripping-sega-system-16-sprites-palettes.html)
