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
# is complete, copy gamedata.vfs from game binary distribution archive
# into current directory and run ./AstroMenace
#

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

FIND_PACKAGE(GLU REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)

# sources
FILE(GLOB_RECURSE astromenace_SRCS AstroMenaceSource/*.cpp)

# targets
ADD_DEFINITIONS(`${SDL_CONFIG} --cflags` -g -Wall)
ADD_EXECUTABLE(AstroMenace ${astromenace_SRCS})
TARGET_LINK_LIBRARIES(AstroMenace `${SDL_CONFIG} --libs` ${OPENGL_gl_LIBRARY} ${OPENAL_LIBRARY} ${JPEG_LIBRARY} ${GLU_LIBRARY} -lalut -lvorbis -lvorbisfile)
IF(CYGWIN)
	TARGET_LINK_LIBRARIES(AstroMenace -lGL.dll  -lGLU.dll -lglfw -lX11.dll)
#	ADD_DEFINITIONS(-mno-cygwin)
#	TARGET_LINK_LIBRARIES(AstroMenace  -mwin32 -mno-cygwin)
#	TARGET_LINK_LIBRARIES(AstroMenace  -lws2_32 -lwinmm -lgdi32 -ldxguid -lole32 )
ENDIF(CYGWIN)
