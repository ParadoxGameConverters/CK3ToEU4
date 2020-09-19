#!/bin/bash

# Copy converter globals
cp "Data_Files/ReadMe.txt" "Release-Linux/"
cp "Data_Files/FAQ.txt" "Release-Linux/"
cp "Data_Files/after_converting.txt" "Release-Linux/"
cp "Data_Files/license.txt" "Release-Linux/"

cp ../imageMagick/lib64/lib*8 "Release-Linux/"
cp ../imageMagick/lib64/lib*6 "Release-Linux/"
cp "Resources/librakaly.so" "Release-Linux/"

# Copy configurables
rm -rf "Release-Linux/configurables"
cp -r "Data_Files/configurables" "Release-Linux/"

# Copy blankmod
rm -rf "Release-Linux/blankMod"
cp -r "Data_Files/blankMod" "Release-Linux/"
