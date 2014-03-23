#! /bin/bash

rm -r build skane.opk
make -f Makefile.gcw0 clean
make -f Makefile.gcw0
mkdir build
mkdir build/res
cp skane build
cp icon.png build
cp default.gcw0.desktop build
cp -r res/* build/res
mksquashfs build skane.opk -all-root -noappend -no-exports -no-xattrs
