#!/usr/bin/env bash
git pull
rm -rf ./build
rm -rf ./build_arm

# mkdir -p build
mkdir -p build_arm

#cd ./build
#cmake ..
#make

#cd ..
cd ./build_arm
cmake -DCMAKE_TOOLCHAIN_FILE=../PSCtoolchain.cmake ..
make -j4
cd ..



