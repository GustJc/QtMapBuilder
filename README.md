QtMapBuilder
============

Tool for creating roguelike maps

As of now, this tool have the ability to create visually perceptive maps, manage Entitys(be it an item or enemy) and add them to maps.

The tool also has the capability to generate maps from .lua scripts from the mapped lua functions to manipulate the map and entitys.


Build
------------

To build the project change the QtMapBuilder.pro to set the libraries includes and linkers to those of your system. 

The project depends on the following libraries:

-Lua 5.1
-Luabind 0.9 or greater (on ubuntu, used luabind from apt-get)
-QScintilla2 2.8 (needs to compile from source... it's easy, see bellow)


The tool was tested using Qtcreator 5.2 64-bits, but should work fine with any version of Qt 5. 

Building QScintilla2
--------------

Building QScintilla2 from source is easy both on linux and on windows

Download QScintilla2 from: http://www.riverbankcomputing.com/software/qscintilla/download
Used the latest version as of now: QScintilla-gpl-2.8

Descompact the file in your system and go to Qt4Qt5 in your terminal and use qmake to compile qscintilla.pro:

`qmake qscintilla.pro`

Note that you need to use qmake from Qt5, and it may not be the default, you can check your version with:

`qmake -v`

The qmake from qt5 is inside qtcreator installation folder, on ubuntu I used:

`/opt/Qt5.2.1/5.2.1/gcc_64/bin/qmake qscintilla.pro`

After its done, run make and make install:

`make`

`sudo make install `

Its done! Now just make sure the folders set in QtMapBuilder.pro at the correct ones for your system

The proccess for windows is simillar, if might need to add the Qt's bin folders to your PATH so that you can use qmake and make, but it'll work and generate the .dll and .a files inside the folder. 

Scripting maps
-------------

The tool has a capability to use .lua files to create procedural maps. 

The lua scripts have 2 global variables: 'map' and 'res

**map** has two fuctions to handle the drawing and creation of the map
* map:createMap(int w, int h, int defaultType, int defaultGfx)
* map:setTile(int x, int y, int gfx, int type)

The type represents the block type and can be one of those:
* Tile.PASS
* Tile.BLOCK
* Tile.END
* Tile.START

Note that a map should only have one Start block.

**res** has several functions to handle adding Entitys to the map. Entitys can be a Gold, an Item or an Enemy
* res:addGold(int x, int y, int gold)             
* res:addEntityByIndex(int x, int y, int ID)      --Add saved Entity at position x,y with ID sorted by creation
* res:addItemByIndex(int x, int y, int ID)
* res:addEntity(int x, int y, string name)        --Add saved Entity with name at position x,y
* res:addItem(int x, int y, string name)
* res:addCustomEntity(int x, int y, Entity ent)
* res:clearEntityMap()                            -- Erase all entitys saved
* res:createEntity(string name)                   -- Creates a entity and returns its reference
