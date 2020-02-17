#!/usr/bin/env bash
rm -rf ./build_arm
mkdir -p build_arm

if [ ! -d "/opt/toolchain/armv8-sony-linux-gnueabihf/" ]; then
  echo "/opt/toolchain/armv8-sony-linux-gnueabihf/ NOT FOUND!!"
  exit 1
fi

cd ./build_arm
PATH="${PATH}:/opt/toolchain/armv8-sony-linux-gnueabihf/bin" cmake -DCMAKE_SYSTEM_PROCESSOR="Arm" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../PSCtoolchainV8.cmake ../
#cmake --build .
make -j 4
cd ..

