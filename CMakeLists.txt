project(FallingTime)
cmake_minimum_required(VERSION 2.8.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_CROSSCOMPILING)
    # check for gcw0
    if(CMAKE_C_COMPILER MATCHES ".*gcw0-linux.*")
        add_definitions(-D__GCW0__)
        set(GCW0 1)
    endif()
endif()
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CLANG 1)
endif()

# Chipmunk 2D
set(BUILD_STATIC ON FORCE)
set(chipmunk_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/chipmunk)
add_subdirectory(chipmunk/src)

# Default definitions
if(MSVC)
    add_definitions(-Wall -W4 -wd"4996" -wd"4204")
else()
    add_definitions(-Wall -Wextra -Wunused -Wshadow -Wunreachable-code -std=c99)
    add_definitions(-fno-math-errno -fno-exceptions)
    if(CLANG)
    else()
        # Probably GCC...
        add_definitions(-freg-struct-return -std=gnu99)
		if(NOT APPLE AND NOT BEOS AND NOT HAIKU)
			add_definitions(-Wshadow)
		endif()
	endif()
	if(NOT BEOS AND NOT HAIKU)
		add_definitions(-Winline)
		set(EXTRA_LIBRARIES "m")
	endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -pg")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-pg")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "-pg")
endif()

# Check for SDL
find_package(SDL REQUIRED)
if(NOT SDL_FOUND)
	message(FATAL_ERROR "Couldn't find SDL development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl1.2-dev' package.")
else()
	include_directories(${SDL_INCLUDE_DIR})
endif()
find_package(SDL_image REQUIRED)
if(NOT SDLIMAGE_FOUND)
	message(FATAL_ERROR "Couldn't find SDL-image development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-image1.2-dev' package.")
else()
	include_directories(${SDLIMAGE_INCLUDE_DIR})
endif()
find_package(SDL_mixer REQUIRED)
if(NOT SDLMIXER_FOUND)
	message(FATAL_ERROR "Couldn't find SDL-mixer development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-mixer1.2-dev' package.")
else()
	include_directories(${SDLMIXER_INCLUDE_DIR})
endif()
find_package(SDL_ttf REQUIRED)
if(NOT SDLTTF_FOUND)
	message(FATAL_ERROR "Couldn't find SDL-ttf development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-ttf1.2-dev' package.")
ELSE()
	include_directories(${SDLTTF_INCLUDE_DIR})
ENDIF()

include_directories(.)
include_directories(chipmunk/include)

file(GLOB SOURCES *.c)
if(GCW0)
    add_definitions(-D__GCW0__)
    set(SOURCES ${SOURCES} platform/opendingux.c)
else()
    set(SOURCES ${SOURCES} platform/general.c)
endif()
file(GLOB HEADERS *.h)

if(APPLE)
    set(EXTRA_FILES
        pkg/macosx/SDLMain.m
        pkg/macosx/SDLMain.h)
    set_source_files_properties(pkg/macosx/SDLMain.m
        PROPERTIES LANGUAGE C)
    set(EXTRA_LIBRARIES "${EXTRA_LIBRARIES} -framework Carbon -framework IOKit")
endif()

add_executable(falling_time ${SOURCES} ${HEADERS} ${EXTRA_FILES})

# libSDLMain comes with libSDL if needed on certain platforms
if(NOT SDLMAIN_LIBRARY)
  set(SDLMAIN_LIBRARY "")
endif()

target_link_libraries(falling_time
	${CMAKE_LD_FLAGS}
	chipmunk_static
	${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLTTF_LIBRARY}
	${SDLMAIN_LIBRARY} ${EXTRA_LIBRARIES})
set_target_properties(falling_time PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}
)


################
# Installation #
################

# Executables
set(INSTALL_PREFIX ".")
if(WIN32)
	set(EXE_EXTENSION ".exe")
	set(LIB_EXTENSION ".dll")
elseif(APPLE)
	set(LIB_EXTENSION ".dylib")
	set(INSTALL_PREFIX "falling_time")
else()
	set(EXE_EXTENSION "")
	set(LIB_EXTENSION ".so")
endif()
if(APPLE)
	install(
		DIRECTORY falling_time.app
		DESTINATION ${INSTALL_PREFIX}
		USE_SOURCE_PERMISSIONS)
else()
	install(
		PROGRAMS falling_time${EXE_EXTENSION}
		DESTINATION ${INSTALL_PREFIX})
endif()

SET(SOURCE_DIRECTORY ".")
INSTALL(DIRECTORY
	${SOURCE_DIRECTORY}/data
	DESTINATION ${INSTALL_PREFIX})
INSTALL(FILES
	${SOURCE_DIRECTORY}/README.md
	DESTINATION ${INSTALL_PREFIX})
IF(UNIX AND NOT APPLE AND NOT BEOS AND NOT HAIKU)
	# TODO: .desktop file and png icon
elseif(WIN32)
	# Package for Windows
	FILE(GLOB DLLS "${SOURCE_DIRECTORY}/*.dll")
	FOREACH(DLL ${DLLS})
		INSTALL(FILES "${DLL}" DESTINATION ${INSTALL_PREFIX})
	ENDFOREACH()
elseif(APPLE)
	set(CMAKE_INSTALL_PREFIX "/Applications/Falling Time")
endif()

# Packaging
SET(CPACK_PACKAGE_NAME "Falling Time")
SET(CPACK_PACKAGE_VENDOR "Falling Time Team")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Falling Time: 2D Physical Arcade Game")
SET(CPACK_PACKAGE_VERSION 1.0.0)
SET(CPACK_PACKAGE_VERSION_MAJOR 1)
SET(CPACK_PACKAGE_VERSION_MINOR 0)
SET(CPACK_PACKAGE_VERSION_PATCH 0)
SET(CPACK_PACKAGE_EXECUTABLES "falling_time;Falling Time")
if(WIN32)
	SET(CPACK_GENERATOR ZIP)
elseif(APPLE)
	set(CPACK_GENERATOR "DragNDrop")
	set(CPACK_DMG_FORMAT "UDBZ")
	set(CPACK_DMG_VOLUME_NAME "Falling Time")
	set(CPACK_SYSTEM_NAME "OSX")
	set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_BINARY_DIR}/pkg/macosx/icon.icns")

	# copy SDL frameworks into app bundle for Mac OS X
	set(APPS falling_time)
	set(LIBS SDL SDL_image SDL_mixer)
	foreach(APP ${APPS})
		foreach(LIB ${LIBS})
			install(DIRECTORY /Library/Frameworks/${LIB}.framework
				DESTINATION ${INSTALL_PREFIX}/bin/${APP}.app/Contents/Frameworks)
		endforeach()
	endforeach()
endif()
include(CPack)
