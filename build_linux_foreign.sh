#!/bin/bash

cd imageMagick &&
cat im7.1.1-47.tar.* > im7.1.1-47.tar &&
tar xvf im7.1.1-47.tar &&
cd ImageMagick-7.1.1-47  &&
./configure --with-quantum-depth=8 --enable-hdri=no --with-x=no --with-utilities=no LDFLAGS='-L/usr/local/lib -R/usr/local/lib' &&
sudo make install &&
sudo ldconfig /usr/local/lib &&
cd ../../ &&

cmake -H. -Bbuild &&
cmake --build build -- -j5
