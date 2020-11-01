CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

IF(POLICY CMP0072)
    # OpenGL related
    CMAKE_POLICY(SET CMP0072 NEW)
ENDIF()

# MinGW32 support for Windows build from Linux, for example:
# $ sudo apt-get install mingw-w64
# $ mkdir build
# $ cd build
# $ cmake .. -DMINGW=1 -DDEPS_PATH=/path -DCMAKE_INSTALL_PREFIX=$PWD/../bin -DCMAKE_BUILD_TYPE=Release
# $ cmake --build . --target install
# Will automatically create VFS file on Windows and Linux (wine package must be installed).
# If Linux don't have wine installed, use -DDONTCREATEVFS=1 and create VFS file manually.
IF(MINGW)
    # make sure, this block located before PROJECT
    SET(CMAKE_SYSTEM_NAME Windows)
    SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
    SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
ENDIF(MINGW)

# project name
PROJECT(AstroMenace)

IF(CMAKE_HOST_WIN32)
    # at this moment, only MinGW supported for Windows build
    SET(MINGW 1)
ELSEIF(CMAKE_HOST_APPLE)
    # make sure, that CMAKE_HOST_APPLE called before CMAKE_HOST_UNIX
    SET(MACOS 1)
ELSEIF(CMAKE_HOST_UNIX)
    SET(LINUX 1)
ENDIF()


IF(MINGW)
    # provide deps location with -DDEPS_PATH=path
    # all libs must be in 'lib' folder
    # all headers must be in 'include' folder
    SET(ALL_LIBRARIES "mingw32 \
                       -lopengl32 \
                       -lglu32 \
                       ${DEPS_PATH}/lib/libSDL2main.a \
                       ${DEPS_PATH}/lib/libSDL2.dll.a \
                       ${DEPS_PATH}/lib/libalut.dll.a \
                       ${DEPS_PATH}/lib/libogg.dll.a \
                       ${DEPS_PATH}/lib/libvorbis.dll.a \
                       ${DEPS_PATH}/lib/libvorbisfile.dll.a \
                       ${DEPS_PATH}/lib/libOpenAL32.dll.a \
                       ${DEPS_PATH}/lib/libfreetype.dll.a")
    INCLUDE_DIRECTORIES(SYSTEM "${DEPS_PATH}/include/")
    INCLUDE_DIRECTORIES(SYSTEM "${DEPS_PATH}/include/freetype2")
ELSE(MINGW)
    # libSDL2 (./share/cmake/FindSDL2.cmake)

    SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/share/cmake)
    FIND_PACKAGE(SDL2 REQUIRED)
    IF(SDL2_FOUND)
        INCLUDE_DIRECTORIES(SYSTEM ${SDL2_INCLUDE_DIR})
        SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${SDL2_LIBRARY})
    ELSE(SDL2_FOUND)
        MESSAGE(FATAL_ERROR "libSDL2 not found")
    ENDIF(SDL2_FOUND)

    FIND_PACKAGE(OpenGL REQUIRED)
    IF(OPENGL_FOUND)
        INCLUDE_DIRECTORIES(SYSTEM ${OPENGL_INCLUDE_DIR})
        SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${OPENGL_LIBRARIES})
    ELSE(OPENGL_FOUND)
        MESSAGE(FATAL_ERROR "OpenGL not found")
    ENDIF(OPENGL_FOUND)

    FIND_PACKAGE(OpenAL REQUIRED)
    SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${OPENAL_LIBRARY})

    # alut lib + header

    FIND_LIBRARY(ALUT_LIBRARY alut)
    IF(ALUT_LIBRARY)
        MESSAGE(STATUS "Found alut: ${ALUT_LIBRARY}")
        SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${ALUT_LIBRARY})
    ELSE(ALUT_LIBRARY)
        MESSAGE(FATAL_ERROR "alut not found")
    ENDIF(ALUT_LIBRARY)
    FIND_PATH(ALUT_INCLUDE_DIR alut.h
        HINTS
        $ENV{ALUTDIR}
        $ENV{ALUT_PATH}
        PATHS
        PATH_SUFFIXES include/AL include)
    IF(ALUT_INCLUDE_DIR)
        MESSAGE(STATUS "Found alut headers: ${ALUT_INCLUDE_DIR}")
        INCLUDE_DIRECTORIES(SYSTEM ${ALUT_INCLUDE_DIR})
    ELSE(ALUT_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR "alut headers not found")
    ENDIF(ALUT_INCLUDE_DIR)

    # ogg lib + header

    FIND_LIBRARY(OGG_LIBRARY ogg)
    IF(OGG_LIBRARY)
        MESSAGE(STATUS "Found ogg: ${OGG_LIBRARY}")
        SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${OGG_LIBRARY})
    ELSE(OGG_LIBRARY)
        MESSAGE(FATAL_ERROR "ogg not found")
    ENDIF(OGG_LIBRARY)
    FIND_PATH(OGG_INCLUDE_DIR ogg/ogg.h
        HINTS
        $ENV{OGGDIR}
        $ENV{OGG_PATH}
        PATHS
        PATH_SUFFIXES include)
    IF(OGG_INCLUDE_DIR)
        MESSAGE(STATUS "Found ogg headers: ${OGG_INCLUDE_DIR}")
        INCLUDE_DIRECTORIES(SYSTEM ${OGG_INCLUDE_DIR})
    ELSE(OGG_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR "ogg headers not found")
    ENDIF(OGG_INCLUDE_DIR)

    # vorbis lib

    FIND_LIBRARY(VORBIS_LIBRARY vorbis)
    IF(VORBIS_LIBRARY)
        MESSAGE(STATUS "Found vorbis: ${VORBIS_LIBRARY}")
        SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${VORBIS_LIBRARY})
    ELSE(VORBIS_LIBRARY)
        MESSAGE(FATAL_ERROR "vorbis not found")
    ENDIF(VORBIS_LIBRARY)

    # vorbisfile lib + header

    FIND_LIBRARY(VORBISFILE_LIBRARY vorbisfile)
    IF(VORBISFILE_LIBRARY)
        MESSAGE(STATUS "Found vorbisfile: ${VORBISFILE_LIBRARY}")
        SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${VORBISFILE_LIBRARY})
    ELSE(VORBISFILE_LIBRARY)
        MESSAGE(FATAL_ERROR "vorbisfile not found")
    ENDIF(VORBISFILE_LIBRARY)
    FIND_PATH(VORBISFILE_INCLUDE_DIR vorbis/vorbisfile.h
        HINTS
        $ENV{VORBISFILEDIR}
        $ENV{VORBISFILE_PATH}
        $ENV{VORBISDIR}
        $ENV{VORBIS_PATH}
        PATHS
        PATH_SUFFIXES vorbis include)
    IF(VORBISFILE_INCLUDE_DIR)
        MESSAGE(STATUS "Found vorbisfile headers: ${VORBISFILE_INCLUDE_DIR}")
        INCLUDE_DIRECTORIES(SYSTEM ${VORBISFILE_INCLUDE_DIR})
    ELSE(VORBISFILE_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR "vorbisfile headers not found")
    ENDIF(VORBISFILE_INCLUDE_DIR)

    # freetype lib + header

    FIND_PACKAGE(Freetype REQUIRED)
    IF(FREETYPE_FOUND)
        SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${FREETYPE_LIBRARIES})
        INCLUDE_DIRECTORIES(SYSTEM ${FREETYPE_INCLUDE_DIRS})
        MESSAGE(STATUS "Found freetype2 headers: ${FREETYPE_INCLUDE_DIRS}")
    ELSE(FREETYPE_FOUND)
        MESSAGE(FATAL_ERROR "freetype not found")
    ENDIF(FREETYPE_FOUND)
ENDIF(MINGW)


# Use C++11.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")

# Warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmain")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")

# AppleClang 8.1.0.8020042 don't support this flag
IF(NOT (MACOS AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")))
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wzero-as-null-pointer-constant")
ENDIF(NOT (MACOS AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")))

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")

# the compiler flags for compiling C++ sources
MESSAGE(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})


# sources
# "src/*.h" added in order to have mixed cpp/h files overview in QtCreator's
# Projects list, that based on FILE for cmake projects.
FILE(GLOB_RECURSE astromenace_SRCS src/*.cpp src/*.h)
SET(astromenace_DATA "${CMAKE_CURRENT_LIST_DIR}/gamedata/")
MESSAGE("Game data dir: ${astromenace_DATA}")


# custom game data sources location
IF(DATADIR)
    ADD_DEFINITIONS(-DDATADIR="${DATADIR}")
ENDIF(DATADIR)

IF(MINGW)
    ADD_DEFINITIONS(-D__GNUWIN32__)
    ADD_DEFINITIONS(-DWIN32)
    ADD_DEFINITIONS(-DOV_EXCLUDE_STATIC_CALLBACKS)
    SET(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF(MINGW)

IF(MINGW)
    # WIN32 - suppress terminal
    ADD_EXECUTABLE(astromenace WIN32 ${astromenace_SRCS})
ELSE(MINGW)
    ADD_EXECUTABLE(astromenace ${astromenace_SRCS})
ENDIF(MINGW)
TARGET_LINK_LIBRARIES(astromenace ${ALL_LIBRARIES})


ADD_CUSTOM_TARGET(CopyLIBS
    COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_PATH}/bin/SDL2.dll          ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_PATH}/bin/libalut.dll       ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_PATH}/bin/libogg.dll        ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_PATH}/bin/libvorbis.dll     ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_PATH}/bin/libvorbisfile.dll ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_PATH}/bin/soft_oal.dll      ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_PATH}/bin/libfreetype.dll   ${CMAKE_BINARY_DIR}
)
IF(MINGW)
    ADD_DEPENDENCIES(astromenace CopyLIBS)
ENDIF(MINGW)


# generate game data file (gamedata.vfs)
IF(NOT DONTCREATEVFS)
    SET(astromenace_BIN "astromenace")
    IF(MINGW)
        SET(astromenace_BIN "astromenace.exe")
    ENDIF(MINGW)

    ADD_CUSTOM_COMMAND(
        TARGET astromenace
        POST_BUILD
        COMMAND ${CMAKE_BINARY_DIR}/${astromenace_BIN} --pack --rawdata=${astromenace_DATA} --dir=${PROJECT_BINARY_DIR}
    )
ENDIF(NOT DONTCREATEVFS)


INSTALL(TARGETS astromenace DESTINATION ${CMAKE_INSTALL_PREFIX})
IF(MINGW)
    INSTALL(FILES "${DEPS_PATH}/bin/SDL2.dll"          DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(FILES "${DEPS_PATH}/bin/libalut.dll"       DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(FILES "${DEPS_PATH}/bin/libogg.dll"        DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(FILES "${DEPS_PATH}/bin/libvorbis.dll"     DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(FILES "${DEPS_PATH}/bin/libvorbisfile.dll" DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(FILES "${DEPS_PATH}/bin/soft_oal.dll"      DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(FILES "${DEPS_PATH}/bin/libfreetype.dll"   DESTINATION ${CMAKE_INSTALL_PREFIX})
ENDIF(MINGW)
IF(NOT DONTCREATEVFS)
    INSTALL(FILES "${PROJECT_BINARY_DIR}/gamedata.vfs"   DESTINATION ${CMAKE_INSTALL_PREFIX})
ENDIF(NOT DONTCREATEVFS)
