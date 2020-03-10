#!/bin/bash
rm -rf ./build_sys
mkdir -p build_sys

cd ./build_sys
cmake -DCMAKE_BUILD_TYPE=Release ../
#cmake --build .
make -j 4
cd ..