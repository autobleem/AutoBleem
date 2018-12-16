cmake_minimum_required(VERSION 3.10)
project(cbleemsync)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/include)

find_package(Threads REQUIRED)

add_executable(cbleemsync
        src/code/unecm.c
        src/code/sqlite3.c
        src/code/main.h
        src/code/database.cpp
        src/code/main.cpp
        src/code/database.h
        src/code/sqlite3.h
        src/code/util.h
        src/code/util.cpp
        src/code/game.cpp
        src/code/game.h
        src/code/scanner.cpp
        src/code/scanner.h
        src/code/metadata.cpp
        src/code/metadata.h
        src/code/ecmhelper.cpp
        src/code/ecmhelper.h
        )

target_link_libraries(cbleemsync 
         Threads::Threads
         ${CMAKE_DL_LIBS}
)

add_custom_command(TARGET cbleemsync PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:cbleemsync>)


configure_file(src/resources/default.lic default.lic COPYONLY)
configure_file(src/resources/default.png default.png COPYONLY)
configure_file(src/resources/pcsx.cfg pcsx.cfg COPYONLY)
configure_file(src/resources/coversU.db coversU.db COPYONLY)
configure_file(src/resources/coversP.db coversP.db COPYONLY)

