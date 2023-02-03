#!/bin/bash

cd imageMagick &&
cat im7.10.tar.* > im7.10-linux-source.tar &&
tar xvf im7.10-linux-source.tar &&
cd ImageMagick-7.0.10 &&
./configure --with-quantum-depth=8 --enable-hdri=no --with-x=no --with-utilities=no &&
sudo make install &&
sudo ldconfig /usr/local/lib &&
cd ../../ &&

rm -rf build &&
rm -rf Release-Linux &&
rm -rf test &&
cmake -H. -Bbuild &&
cmake --build build -- -j3
