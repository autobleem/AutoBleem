cmake_minimum_required(VERSION 3.13)
project(cbleemsync)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/include)

add_executable(cbleemsync
        src/code/database.c
        src/code/filesystem.c
        src/code/main.c
        src/code/sqlite3.c
        src/include/database.h
        src/include/filesystem.h
        src/include/sqlite3.h
        src/include/util.h
        src/resources/default.lic
        src/resources/default.png
        src/resources/pcsx.cfg
        src/resources/run.sh)
