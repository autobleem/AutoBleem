#!/usr/bin/env bash
rm -rf ./build_arm
mkdir -p build_arm

cd ./build_arm
cmake -DCMAKE_SYSTEM_PROCESSOR="Arm" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../PSCtoolchainV8.cmake ../
cmake --build .
#make -j4
cd ..

