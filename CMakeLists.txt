# CMakeLists.txt for AstroMenace by Dmitry Marakasov <amdmi3@amdmi3.ru>
# 
# Usage:
#
# unzip AstroMenaceSourceCode_070914.zip
# cd AstroMenaceSourceCode
# cmake .
# make
#
# Of course you need cmake installed on your system. After the build
# is complete, copy gamedata.vfs, gamelang.vfs and DATA directory from
# game binary distribution archive into current directory and run
# ./AstroMenace
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# project name
PROJECT(AstroMenace)

# depends
set(SDL_CONFIG "sdl-config" CACHE STRING "Path to sdl-config script")

FIND_PACKAGE(OpenGL REQUIRED)
IF(OPENGL_FOUND)
	INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
ELSE(OPENGL_FOUND)
	MESSAGE(FATAL "OpenGL not found")
ENDIF(OPENGL_FOUND)

#
# XXX: There is no FIND_PACKAGE for alut and libvorbis.
# We just link required libraries, and hope that include/library
# dir paths from other detected packages are enough
#
FIND_PACKAGE(GLU REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)

# sources
FILE(GLOB_RECURSE astromenace_SRCS AstroMenaceSource/*.cpp)

# targets
 IF(DEFINED DATADIR)
	ADD_DEFINITIONS(-DDATADIR=\\"${DATADIR}\\")
 ENDIF(DEFINED DATADIR)
 EXEC_PROGRAM(${SDL_CONFIG} ARGS "--cflags" OUTPUT_VARIABLE SDL_CFLAGS)
 EXEC_PROGRAM(${SDL_CONFIG} ARGS "--libs" OUTPUT_VARIABLE SDL_LIBS)

 ADD_DEFINITIONS(${SDL_CFLAGS})
 ADD_EXECUTABLE(AstroMenace ${astromenace_SRCS})
 TARGET_LINK_LIBRARIES(AstroMenace ${SDL_LIBS} ${OPENGL_gl_LIBRARY} ${OPENAL_LIBRARY} ${JPEG_LIBRARY} ${GLU_LIBRARY} -lalut -lvorbis -lvorbisfile)
 IF(CYGWIN)
	TARGET_LINK_LIBRARIES(AstroMenace -lGL.dll  -lGLU.dll -lglfw -lX11.dll)
#	ADD_DEFINITIONS(-mno-cygwin)
#	TARGET_LINK_LIBRARIES(AstroMenace  -mwin32 -mno-cygwin)
#	TARGET_LINK_LIBRARIES(AstroMenace  -lws2_32 -lwinmm -lgdi32 -ldxguid -lole32 )
ENDIF(CYGWIN)

