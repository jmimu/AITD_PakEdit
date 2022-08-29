AITD PakEdit
============

https://github.com/jmimu/AITD_PakEdit

Alone in the Dark Pak Edit allows to modify PAK file for AITD and FITD.
It is based on yaz0r's FITD, and using original PKZip 1.1 compressor.

License
-------

AITD PakEdit is distributed under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version. A copy of this license an be found in
the file LICENSE.txt included with the source code of this program.

Files in `fitd` folder are Yaz0r's FITD source (https://github.com/yaz0r/FITD)
with minimal modifications from commit eafabb8cffb09.

Uses jsoncpp (github.com/open-source-parsers/jsoncpp).

Compiling
---------

Windows:

get qtCreator for mingw
get zlib source and complied there http://www.winimage.com/zLibDll/
Copy zlib.h and zlib_bor.lib in C:\libs\
Open .pro file
compile

Linux:

    sudo apt install qtcreator zlib
    qmake
    make

Automatic cross compilation from linux
--------------------------------------

Create docker image from: https://github.com/jmimu/crossqt

Then, from distrib directory: 

    docker run --rm --device /dev/fuse --privileged -v $(pwd)/..:/pakedit crossqt1804 /pakedit/distrib/compile_all.sh

Getting Started
---------------
Copy io_alone/ to bender/2.9x/scripts/addons/

Make a copy of your original PAK files.

Run AITD_PakEdit.
Open a database corresponding to the game (AITD1_PAK_DB.json for AITD1).
Open a PAK.

You see every file in the PAK, with there description and type from the database.
You can modify them and overwrite database (and send it to me for next release!).
When selecting a file, you can export it depending of its type.
For now you can export raw files (just decompressed), pictures, texts,
palettes+pictures, sound samples and floors (not finalized).
For now only pictures, texts, sound samples and raw files can be re-imported.
You can then modify the exported files, and re-import them.
Overwrite the PAK file (a .PAK.BAK file will be created to save previous version).
Run the game with the new PAK.

To be able to re-import compressed files (mandatory for bodies), you have
to install Dosbox emulator and set dosbox executable path (if it is not in system's PATH) with `Settings>Set Dosbox Path` menu.
You also have to get PKZip 1.1 executable, and give its path with `Settings>Set PKZip Path` menu.

Floors are exported in Collada format.
You can open it with meshlab or blender.
To have a correct superposition with the background images, you must
stretch them into 320*240 or set blender's output Y aspect to 1.2.
Objects names are : type_rX_tY_pZ, where X is room number, Y type and Z parameter.
You have to change those names in order to change objects metadata.



TODO
----
  * make an import body blender plugin
  * what is the link between scezone meta and step sample?
  * make a scene for each room in collada
  * text export: remove last char, convert to iso-8859-1 with dos2unix -850
  * fix file name encoding "caméra"
  * text import: add last char, convert to dos page 850
  * import pal+pic
  * export ADL music from LISTMUS.PAK
  * export and import other types of files

CD VS Floppy version
--------------------
  * new presentation picture (with correct head)
  * 4 sounds removed
  * audio CD music (FM seems to be still present in PAK)
  * voice acting (VOC files not copied to hard disk)
  * pictures names in CAMERA PAKs
  * languages modifications (TODO: investigate)
  * ETAGE modifications (TODO: investigate)
  * animation modifications (TODO: investigate)

Changelog
---------

v0.09
  * fix cameras in etage export
v0.08
  * fix body ply export when multiple groups
  * update blender body exporter for blender 3.x
v0.07
  * fix PKZip path setting
  * fix body interpretation
  * fix number of rooms
v0.06
  * uses 2021 version of yaz0r's FITD
  * fix floor camera interpretation
  * fix body interpretation

Older versions, found in https://github.com/jmimu/FITD/tree/master/AITD_PakEdit

v0.05
  * Fix SceZone export/import y
  * cross compilation script
  * able to compress at import
v0.04:
  * collada export with some metadata
  * begin collada import (hardcol and scezones)
  * fix collada export crash
  * fix PAK export
v0.03:
  * fix windows collada export
  * collada camera export ok
v0.02:
  * split database between floppy and cd versions
  * import/export VOX
  * export pal+pic
  * start export floors
v0.01:
  * database
  * import/export pictures
  * import/export text
  * decompress data
