#!/bin/bash

rm -rf build &&
rm -rf Release-Linux &&
rm -rf test &&
cmake -H. -Bbuild &&
cmake --build build -- -j40
