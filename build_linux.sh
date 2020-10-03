#!/bin/bash

export CC=/usr/bin/gcc-9 &&
export CXX=/usr/bin/g++-9 &&

cd imageMagick &&
cat im7.10.tar.* > im7.10-linux-source.tar &&
tar xvf im7.10-linux-source.tar &&
cd ImageMagick-7.0.10 &&
./configure --with-quantum-depth=8 --enable-hdri=no --with-x=no --with-utilities=no &&
sudo make install &&
cd ../../ &&
rm -rf Release/ &&
cd Fronter &&
./build_linux.sh &&
mv Release ../ &&
cd ../CK3toEU4 &&
rm -rf build &&
rm -rf Release-Linux &&
cmake -H. -Bbuild &&
cmake --build build -- -j3 &&
mv Release-Linux ../Release/CK3toEU4 &&
cd .. &&

cp CK3toEU4/Data_Files/*yml Release/Configuration/ &&
cp CK3toEU4/Data_Files/fronter*txt Release/Configuration/ &&

tar -cjf CK3toEU4-dev-release.tar.bz2 Release
