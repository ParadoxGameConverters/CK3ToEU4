#!/bin/bash

# Copy converter globals
cp "Data_Files/ReadMe.txt" "Release-Linux/"
cp "Data_Files/FAQ.txt" "Release-Linux/"
cp "Data_Files/after_converting.txt" "Release-Linux/"
cp "Data_Files/license.txt" "Release-Linux/"

# Copy configurables
rm -rf "Release-Linux/configurables"
cp -r "Data_Files/configurables" "Release-Linux/"

# Copy blankmod
rm -rf "Release-Linux/blankMod"
cp -r "Data_Files/blankMod" "Release-Linux/"
