#!/bin/bash

export CC=/usr/bin/gcc-11 &&
export CXX=/usr/bin/g++-11 &&

cd CK3toEU4 &&
rm -rf build &&
rm -rf Release-Linux &&
cmake -H. -Bbuild &&
cmake --build build -- -j40 &&
mv Release-Linux ../Release/CK3toEU4 &&
cd .. &&

cd Fronter.NET &&
dotnet publish -p:PublishProfile=linux-x64 --output:"../Release" &&
cd .. &&

cp CK3toEU4/Data_Files/*yml Release/Configuration/ &&
cp CK3toEU4/Data_Files/fronter*txt Release/Configuration/ &&

tar -cjf CK3toEU4-dev-release.tar.bz2 Release
