#!/usr/bin/env bash
rm -rf ./build_arm
mkdir -p build_arm

cd ./build_arm
cmake -DCMAKE_TOOLCHAIN_FILE=../PSCtoolchainV7.cmake ..
make -j4
cd ..

