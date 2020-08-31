#!/bin/bash

export CC=/usr/bin/gcc-9 &&
export CXX=/usr/bin/g++-9 &&

rm -rf Release/ &&
cd Fronter &&
./build_linux.sh &&
mv Release ../ &&
cd ../CK3ToEU4 &&
rm -rf build &&
rm -rf Release-Linux &&
cmake -H. -Bbuild &&
cmake --build build -- -j3 &&
mv Release-Linux ../Release/CK3ToEU4 &&
cd .. &&

cp CK3ToEU4/Data_Files/*yml Release/Configuration/ &&
cp CK3ToEU4/Data_Files/fronter*txt Release/Configuration/ &&

tar -cjf CK3ToEU4-dev-release.tar.bz2 Release
