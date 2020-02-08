#!/usr/bin/env bash
rm -rf ./build_sys
mkdir -p build_sys

cd ./build_sys
cmake -DCMAKE_BUILD_TYPE=Release ../
cmake --build .
#make -j4
cd ..

