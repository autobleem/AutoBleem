cmake_minimum_required(VERSION 3.6)
project(autobleem-gui)

set(CMAKE_CXX_STANDARD 11)


include_directories(src/include /usr/local/cross-tools/arm-linux-gnueabihf/include/)
link_directories(/usr/local/cross-tools/arm-linux-gnueabihf/lib/)

find_package(Threads REQUIRED)

add_executable(autobleem-gui
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
        src/code/gui.h
        src/code/gui.cpp
		src/code/inifile.cpp
		src/code/inifile.h
		src/code/isodir.cpp
		src/code/isodir.h
		src/code/config.cpp
		src/code/config.h
		src/code/cfgprocessor.cpp
		src/code/cfgprocessor.h
        src/code/memcard.cpp
		src/code/memcard.h
		src/code/gui_about.cpp
		src/code/gui_about.h
		src/code/gui_screen.cpp
		src/code/gui_screen.h
		src/code/gui_splash.cpp
		src/code/gui_splash.h
		src/code/gui_options.cpp
		src/code/gui_options.h
		src/code/gui_memcards.cpp
		src/code/gui_memcards.h
		src/code/gui_confirm.cpp
		src/code/gui_confirm.h
		src/code/gui_keyboard.cpp
		src/code/gui_keyboard.h
		src/code/gui_manager.cpp
		src/code/gui_manager.h
		src/code/gui_editor.cpp
		src/code/gui_editor.h
		src/code/gui_selectmemcard.cpp
		src/code/gui_selectmemcard.h
		src/code/ver_migration.cpp
		src/code/ver_migration.h
		src/code/coverdb.cpp
		src/code/coverdb.h
		src/code/serialscanner.cpp
		src/code/serialscanner.h
		src/code/md5.c
        )

add_executable(starter
		src/code/starter.cpp
		src/code/util.h
		src/code/util.cpp
		src/code/inifile.cpp
		src/code/inifile.cpp
		src/code/memcard.cpp src/code/memcard.h
		)

target_link_libraries(autobleem-gui
         Threads::Threads
         SDL2
         SDL2_image
         SDL2_mixer
         SDL2_ttf
         ${CMAKE_DL_LIBS}
)

add_custom_command(TARGET autobleem-gui PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:autobleem-gui>)


configure_file(src/resources/default.lic default.lic COPYONLY)
configure_file(src/resources/default.png default.png COPYONLY)
configure_file(src/resources/pcsx.cfg pcsx.cfg COPYONLY)
configure_file(db/coversU.db coversU.db COPYONLY)
configure_file(db/coversP.db coversP.db COPYONLY)

