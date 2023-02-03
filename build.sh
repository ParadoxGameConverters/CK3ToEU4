#!/bin/bash

#rm -rf build &&
#rm -rf Release-Linux &&
#rm -rf test &&
#rm -rf lib &&
cmake -H. -Bbuild &&
cmake --build build -- -j40
