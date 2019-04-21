#!/usr/bin/env bash
rm -rf ./build_sys
mkdir -p build_sys

cd ./build_sys
cmake ..
make
cd ..

