## Export as map files and import them into your world (Creative mode)

In order to export files to `.dat` map files, follow these steps.

### 1. Create a folder to store the maps

Create folder in your computer to store the files you are creating.

A map can only store 128x128 pixels, so, for larger images, multiple maps will be created.

### 2. Export

Go to the menu **File > Export > Export as map files**

 - Choose the folder you created to save the maps.
 - Input the starting number for your maps, in case you already have  more maps, to prevent them from overlapping. If you only have a map, leave it to 0.
 - Click Export

### 3. Add the files to your world

You can add the map files into your world by copying them into the `data` folder.

You may also want to change the map count number. In order to do that, you have to modify the `idcounts.dat` file with an NBT Editor of your choice.

### 4. Give yourself the map items in game

To give yourself the map items you have to use commands.

For the version 1.13 and later, you can use the following command in chat:

```
/give @p minecraft:filled_map[minecraft:map_id=0]
```

You have to use it as many times as maps files you created, depending on the size of the image, increasing the `map` attribute in the command.

The maps are sorted left to right, top to bottom.
