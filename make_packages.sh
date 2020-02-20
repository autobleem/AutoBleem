#!/bin/bash
#
#  Copyright 2020
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

ARMv8STRIP="/opt/toolchain/armv8-sony-linux-gnueabihf/bin/armv8-sony-linux-gnueabihf-strip"
TARGET="$(pwd)/build_arm/autobleem-gui"
TARGET_PLATFORMS=( "psc_eris" )

# Grab toolchain if req'd
if [ ! -d "/opt/toolchain/armv8-sony-linux-gnueabihf/" ]; then
  echo "/opt/toolchain/armv8-sony-linux-gnueabihf/ NOT FOUND!!"
  mkdir -p "/opt/toolchain/"
  git clone https://github.com/autobleem/PSC-CrossCompile-Toolchain "/opt/toolchain/"
fi

for PLATFORM in "${TARGET_PLATFORMS[@]}"; do
  TARGET="$(pwd)/build_arm_${PLATFORM}/autobleem-gui"
  # Clean dist bin
  rm -rf "./build_arm_${PLATFORM}" && mkdir -p "./build_arm_${PLATFORM}"
  # CMAKE the AB binary
  cd "./build_arm_${PLATFORM}"
  PATH="${PATH}:/opt/toolchain/armv8-sony-linux-gnueabihf/bin" \
  cmake -DCMAKE_SYSTEM_PROCESSOR="Arm" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_TOOLCHAIN_FILE="../PSCtoolchainV8.cmake" -DTARGET="${PLATFORM}" \
        -DCONSOLIDATE="TRUE" \
  ../
  make -j
  ${ARMv8STRIP} "${TARGET}"
  chmod +x "${TARGET}"
  cd ..
  # Sense check before proceeding to packaging
  touch "${TARGET}" || exit 1
done

# Build the packages
for PLATFORM in "${TARGET_PLATFORMS[@]}"; do
  TARGET="$(pwd)/build_arm_${PLATFORM}/autobleem-gui"
  cd "./new_payload"
  cp -f "${TARGET}" "./${PLATFORM}/media/Autobleem/bin/autobleem/"
  make -f "Makefile.pack_${PLATFORM}"
  cd "../"
done

# Echo results
echo "Built the following mod packages:"
ls -l "./new_payload/"*".mod"
echo
exit 0