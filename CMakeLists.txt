cmake_minimum_required(VERSION 3.10)
project(cbleemsync)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/include)

find_package(Threads REQUIRED)

add_executable(cbleemsync
        src/code/sqlite3.c
        src/include/main.h
        src/code/database.cpp
        src/code/main.cpp
        src/include/database.h
        src/include/sqlite3.h
        src/include/util.h
        src/code/util.cpp src/code/game.cpp src/include/game.h src/code/scanner.cpp src/include/scanner.h src/code/unecm.c)

target_link_libraries(cbleemsync 
         Threads::Threads
         ${CMAKE_DL_LIBS}
)

configure_file(src/resources/default.lic default.lic COPYONLY)
configure_file(src/resources/default.png default.png COPYONLY)
configure_file(src/resources/pcsx.cfg pcsx.cfg COPYONLY)
configure_file(src/resources/run.sh run.sh COPYONLY)