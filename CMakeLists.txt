cmake_minimum_required(VERSION 3.10)
project(cbleemsync)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/include)

find_package(Threads REQUIRED)

add_executable(cbleemsync
        src/code/sqlite3.c
        src/code/database.cpp
        src/code/filesystem.cpp
        src/code/main.cpp
        src/include/database.h
        src/include/filesystem.h
        src/include/sqlite3.h
        src/include/util.h
        src/code/util.cpp)

target_link_libraries(cbleemsync 
         Threads::Threads
         ${CMAKE_DL_LIBS}
)