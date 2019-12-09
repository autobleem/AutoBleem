set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)


set(CMAKE_C_COMPILER armv8-sony-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER armv8-sony-linux-gnueabihf-g++)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

SET(CMAKE_C_FLAGS "-mfloat-abi=hard -march=armv8-a+simd  -Os ")
SET(CMAKE_CXX_FLAGS " -mfloat-abi=hard -march=armv8-a+simd  -Os ")
set(CMAKE_SYSROOT /opt/toolchain/armv8-sony-linux-gnueabihf/sysroot/)
