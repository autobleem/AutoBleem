cmake_minimum_required(VERSION 3.6)
project(autobleem-gui)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_VERBOSE_MAKEFILE ON)

if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm" )

    message(STATUS "Building for ARM")

	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8.0)
		SET(CMAKE_C_FLAGS "-mfloat-abi=hard -march=armv8-a+simd -Os -s")
		SET(CMAKE_CXX_FLAGS " -mfloat-abi=hard -march=armv8-a+simd  -Os -s")
            message(STATUS "Newer GCC installed")
	ELSE()
     
		SET(CMAKE_C_FLAGS "-mfloat-abi=hard -march=armv7ve -s -Os -O3")
		SET(CMAKE_CXX_FLAGS "-mfloat-abi=hard -march=armv7ve -s -Os -O3")
	ENDIF()

	include_directories(src/include "/opt/toolchain/armv8-sony-linux-gnueabihf/sysroot/usr/include")
	link_directories(/opt/toolchain/armv8-sony-linux-gnueabihf/sysroot/usr/lib)
else()
	if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
		if ( "$ENV{MEM_DEBUG}" STREQUAL "1" )
			message("Enabling additional memory debugging")
			SET(CMAKE_C_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
			SET(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
			SET(LDFLAGS "-fsanitize=address -fno-omit-frame-pointer")
		endif()
	endif()
endif()

find_package(Threads REQUIRED
             SDL2 REQUIRED)

add_library(sqlite3 STATIC libs/sqlite/sqlite3ab.c )

add_library(autobleemui STATIC
		libs/autobleemui/autobleem/util.cpp
		libs/autobleemui/autobleem/environment.cpp
		libs/autobleemui/autobleem/direntry.cpp
		libs/autobleemui/autobleem/stringinline.h
		libs/autobleemui/autobleem/enums.h
		libs/autobleemui/autobleem/lang.cpp
		libs/autobleemui/autobleem/inifile.cpp
		libs/autobleemui/autobleem/debugtimer.cpp
		libs/autobleemui/guigfx/gui_sdl_wrapper.cpp
		libs/autobleemui/guigfx/gui_screen.cpp

		libs/autobleemui/ecm/unecm.c
		libs/autobleemui/ecm/ecmhelper.cpp
		libs/autobleemui/guigfx/gui.h)

target_include_directories(autobleemui PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/autobleemui>
		)

add_executable(
        autobleem-gui
        src/code/main.cpp
        src/code/engine/database.cpp
        src/code/engine/database.h
        src/code/engine/game.cpp
        src/code/engine/game.h
        src/code/engine/scanner.cpp
        src/code/engine/scanner.h
        src/code/engine/GetGameDirHierarchy.h
        src/code/engine/GetGameDirHierarchy.cpp
        src/code/engine/metadata.cpp
        src/code/engine/metadata.h
        src/code/gui/gui.h
        src/code/gui/gui.cpp
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
        src/code/gui/gui_font.h
        src/code/gui/gui_font.cpp
		src/code/gui/gui_gameDirMenu.h
		src/code/gui/gui_gameDirMenu.cpp
        src/code/engine/coverdb.cpp
        src/code/engine/coverdb.h
        src/code/engine/serialscanner.cpp
        src/code/engine/serialscanner.h
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
		src/code/launcher/gui_launcher_loop.cpp
        src/code/engine/padmapper.cpp
        src/code/engine/padmapper.h
        src/code/gui/gui_padconfig.cpp
        src/code/gui/gui_padconfig.h
		src/code/launcher/retboot_interceptor.cpp
		src/code/launcher/retboot_interceptor.h
		src/code/launcher/emu_interceptor.cpp
		src/code/launcher/emu_interceptor.h
		src/code/launcher/ra_integrator.cpp
		src/code/launcher/ra_integrator.h
		src/code/gui/gui_playlists.cpp
		src/code/gui/gui_playlists.h
        src/code/engine/cardedit.h
		src/code/engine/cardedit.cpp
        src/code/launcher/gui_mc_manager.h
		src/code/launcher/gui_mc_manager.cpp
		src/code/gui/starfx.cpp
		src/code/gui/starfx.h
        src/code/launcher/launch_interceptor.cpp src/code/launcher/launch_interceptor.h src/code/launcher/gui_app_start.cpp src/code/launcher/gui_app_start.h)

target_include_directories(autobleem-gui PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/autobleemui>
		)



add_executable(starter
		src/code/starter.cpp
        src/code/engine/memcard.cpp
		src/code/engine/memcard.h
		)

target_include_directories(starter PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/autobleemui>
		)

target_link_libraries(autobleem-gui
         Threads::Threads
         SDL2
         SDL2_image
         SDL2_mixer
         SDL2_ttf
		 sqlite3
		 autobleemui
         ${CMAKE_DL_LIBS}
)

target_link_libraries(starter
		autobleemui
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

