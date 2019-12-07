set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)


set(CMAKE_C_COMPILER armv7-psc-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER armv7-psc-linux-gnueabihf-g++)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_C_FLAGS "-mfloat-abi=hard -march=armv7ve -Os -s")
SET(CMAKE_CXX_FLAGS " -mfloat-abi=hard -march=armv7ve -Os -s")

