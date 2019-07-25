cmake_minimum_required(VERSION 3.6)
project(autobleem-gui)

set(CMAKE_CXX_STANDARD 11)


if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm" )
	SET(CMAKE_C_FLAGS "-mfloat-abi=hard -march=armv8-a+simd -s -Os -O3")
	SET(CMAKE_CXX_FLAGS "-mfloat-abi=hard -march=armv8-a+simd -Os -O3")
	include_directories(src/include /home/screemer/x-tools/armv8-sony-linux-gnueabihf/armv8-sony-linux-gnueabihf/sysroot/usr/include)
	link_directories(/home/screemer/x-tools/armv8-sony-linux-gnueabihf/armv8-sony-linux-gnueabihf/sysroot/usr/lib$)
else()
	if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
		if ( "$ENV{MEM_DEBUG}" STREQUAL "1" )
			message("Enabling additional memory debugging")
			SET(CMAKE_C_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
			SET(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
		endif()
	endif()
endif()

find_package(Threads REQUIRED)

add_executable(
        autobleem-gui
        src/code/unecm.c
        src/code/sqlite3.c
        src/code/main.h
        src/code/main.cpp
        src/code/DebugTimer.h
        src/code/DebugTimer.cpp
        src/code/engine/database.cpp
        src/code/engine/database.h
        src/code/sqlite3.h
        src/code/util.h
        src/code/util.cpp
        src/code/engine/game.cpp
        src/code/engine/game.h
        src/code/engine/scanner.cpp
        src/code/engine/scanner.h
        src/code/engine/metadata.cpp
        src/code/engine/metadata.h
        src/code/engine/ecmhelper.cpp
        src/code/engine/ecmhelper.h
        src/code/gui/gui.h
        src/code/gui/gui.cpp
        src/code/engine/inifile.cpp
        src/code/engine/inifile.h
        src/code/engine/isodir.cpp
        src/code/engine/isodir.h
        src/code/engine/config.cpp
        src/code/engine/config.h
        src/code/engine/cfgprocessor.cpp
        src/code/engine/cfgprocessor.h
        src/code/engine/memcard.cpp
        src/code/engine/memcard.h
        src/code/gui/gui_about.cpp
        src/code/gui/gui_about.h
        src/code/gui/gui_screen.cpp
        src/code/gui/gui_screen.h
        src/code/gui/gui_splash.cpp
        src/code/gui/gui_splash.h
        src/code/gui/gui_options.cpp
        src/code/gui/gui_options.h
        src/code/gui/gui_memcards.cpp
        src/code/gui/gui_memcards.h
        src/code/gui/gui_confirm.cpp
        src/code/gui/gui_confirm.h
        src/code/gui/gui_keyboard.cpp
        src/code/gui/gui_keyboard.h
        src/code/gui/gui_manager.cpp
        src/code/gui/gui_manager.h
        src/code/gui/gui_editor.cpp
        src/code/gui/gui_editor.h
		src/code/gui/gui_selectmemcard.cpp
        src/code/gui/gui_selectmemcard.h
        src/code/gui/gui_sdl_wrapper.h
        src/code/gui/gui_sdl_wrapper.cpp
        src/code/gui/gui_font.h
        src/code/gui/gui_font.cpp
        src/code/gui/gui_font_wrapper.h
		src/code/ver_migration.cpp
		src/code/ver_migration.h
        src/code/engine/coverdb.cpp
        src/code/engine/coverdb.h
        src/code/engine/serialscanner.cpp
        src/code/engine/serialscanner.h
        src/code/lang.cpp
        src/code/lang.h 
        src/code/launcher/gui_launcher.cpp
        src/code/launcher/gui_launcher.h 
		src/code/launcher/gui_NotificationLine.h
		src/code/launcher/gui_NotificationLine.cpp
        src/code/launcher/ps_obj.cpp
        src/code/launcher/ps_obj.h
        src/code/launcher/ps_static.cpp
        src/code/launcher/ps_static.h
        src/code/launcher/ps_settings_back.cpp 
        src/code/launcher/ps_settings_back.h
        src/code/launcher/ps_zoom_btn.cpp
        src/code/launcher/ps_zoom_btn.h
        src/code/launcher/ps_meta.cpp
        src/code/launcher/ps_meta.h
        src/code/launcher/ps_game.cpp 
        src/code/launcher/ps_game.h 
        src/code/launcher/ps_carousel.cpp
        src/code/launcher/ps_carousel.h
        src/code/launcher/pcsx_interceptor.cpp
        src/code/launcher/pcsx_interceptor.h
        src/code/launcher/ps_move_bnt.cpp
        src/code/launcher/ps_move_bnt.h 
        src/code/launcher/ps_menu.cpp
        src/code/launcher/ps_menu.h
        src/code/launcher/ps_centerlabel.cpp
        src/code/launcher/ps_centerlabel.h
        src/code/launcher/ps_stateselector.cpp
        src/code/launcher/ps_stateselector.h
        src/code/launcher/gui_btn_guide.cpp
        src/code/launcher/gui_btn_guide.h
        src/code/engine/padmapper.cpp
        src/code/engine/padmapper.h
        src/code/gui/gui_padconfig.cpp
        src/code/gui/gui_padconfig.h
		src/code/launcher/retboot_interceptor.cpp
		src/code/launcher/retboot_interceptor.h
		src/code/launcher/emu_interceptor.cpp
		src/code/launcher/emu_interceptor.h
		src/code/nlohmann/json.h src/code/nlohmann/fifo_map.h)



add_executable(starter
		src/code/starter.cpp
		src/code/util.h
		src/code/util.cpp
        src/code/engine/inifile.cpp
        src/code/engine/inifile.cpp
        src/code/engine/memcard.cpp src/code/engine/memcard.h
		)

target_link_libraries(autobleem-gui
         Threads::Threads
         SDL2
         SDL2_image
         SDL2_mixer
         SDL2_ttf
         ${CMAKE_DL_LIBS}
)


#add_custom_command(TARGET autobleem-gui PRE_BUILD
#	   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#	   VERBATIM
#       COMMAND /bin/bash make_english.txt.sh
#	   )

add_custom_command(TARGET autobleem-gui PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:autobleem-gui>)


configure_file(src/resources/default.lic default.lic COPYONLY)
configure_file(src/resources/default.png default.png COPYONLY)
configure_file(src/resources/pcsx.cfg pcsx.cfg COPYONLY)

