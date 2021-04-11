# Sega System 16 Sprite Viewer


This application is a quick and dirty Sega sprite viewer.


This sprite viewer supports sprites from the following arcade hardware:
```
1. Sega Hang-On / Space Harrier
2. Sega System 16
3. Sega System 18
4. Sega OutRun / Super Hang-On 
5. Sega X-Board (AfterBurner, Thunderblade etc.)
6. Sega Y-Board (Power Drift, G-Loc etc.)
```
![shot1](https://user-images.githubusercontent.com/2414449/114319863-34efac00-9b0b-11eb-9310-2ca82dfd572f.png)
![shot2](https://user-images.githubusercontent.com/2414449/114319864-35884280-9b0b-11eb-802e-49b360ffb0a5.png)
![shot3](https://user-images.githubusercontent.com/2414449/114319865-35884280-9b0b-11eb-9df1-7757d3d7e78a.png)
![shot4](https://user-images.githubusercontent.com/2414449/114319868-35884280-9b0b-11eb-8f78-7069c93fcd13.png)
![shot5](https://user-images.githubusercontent.com/2414449/114319869-3620d900-9b0b-11eb-9b90-9a9577aeed60.png)



## Getting Started


Try the application with one of the example configuration files. 

You will find out the box support for OutRun, AfterBurner, Space Harrier, 
Golden Axe and others.

1. Copy the relevant sprite roms to a directory.
2. Edit the appropriate xml file to reflect the location of the roms and 
   palette data.
3. From the command line, run 's16_viewer <config.xml>' 
   Where config.xml is replaced with the name of the configuration file.
   
It's possible to create your own configuration files for your favourite titles.
I'll add more in due course.

It helps to be familar with reading the MAME source code to set a game up 
correctly. 
