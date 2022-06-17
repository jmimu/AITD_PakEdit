#! /bin/bash

set -e
set -x
umask 0000
export PATH=/usr/lib/mxe/usr/bin/:$PATH

version=`cat /pakedit/src/version.h | grep "VERSION" | sed 's/.*VERSION "//' | sed 's/[^0-9.a-zA-Z_].*//'`

NBRP=$(cat /proc/cpuinfo | grep processor | wc -l)
GREEN='\033[0;32m'
STOP='\033[0m'


echo -e "${GREEN}Remove all${STOP}"
rm -Rf /pakedit/autobuild/ /pakedit/autobuild-mxe/

echo -e "${GREEN}Linux compilation${STOP}"
cd /pakedit/
lrelease AITD_PakEdit.pro
mkdir -p autobuild/
cd autobuild/
qmake ../AITD_PakEdit.pro CONFIG+=release
make clean
make -j$NBRP

echo -e "${GREEN}Copy AppImage tools...${STOP}"
cp /linuxdeploy-x86_64.AppImage /pakedit/
cp /linuxdeploy-plugin-qt-x86_64.AppImage /pakedit/

echo -e "${GREEN}Creating AppImage...${STOP}"
cd /pakedit/
distrib/make_appimage.sh  autobuild/
distrib/make_ziplinux.sh AITD_PakEdit_$version-x86_64.AppImage

echo -e "${GREEN}Windows cross-compilation${STOP}"
cd /pakedit/
mkdir -p autobuild-mxe/
cd autobuild-mxe/
x86_64-w64-mingw32.static-qmake-qt5 ../AITD_PakEdit.pro
make clean
make -j$NBRP

echo -e "${GREEN}Creating windows zip...${STOP}"
cd /pakedit/
distrib/make_zipwin.sh autobuild-mxe/

