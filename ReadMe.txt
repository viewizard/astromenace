AstroMenace

Runtime dependencies:
libSDL (ver 1.2.10+), GLU, libopenal (ver 1.0+), libalut (ver 1.0+), libogg (ver 1.1+), 
libvorbis (ver 1.1+), libjpeg (libjpeg-turbo prefered), freetype (ver 2.1.6+),
libXinerama (optional).


The easy way to get up to date AstroMenace on your computer:

1) Make sure all necessary packages with development headers
   installed in your system: 

   gcc (with g++), cmake, subversion, libsdl, libogg, libvorbis, openal, 
   jpeg (libjpeg-turbo prefered), alut (freealut), libXinerama, GLU, freetype2

For Ubuntu/Debian:
$ sudo apt-get install g++ cmake make subversion libsdl1.2-dev
  libogg-dev libvorbis-dev libopenal-dev libjpeg-dev libXinerama-dev
  libalut-dev libfreetype6-dev

For OpenSuse:
$ sudo zypper install gcc-c++ cmake subversion libSDL-devel libvorbis-devel
  openal-soft-devel libjpeg-devel libXinerama-devel freealut-devel
  ylibogg-devel freetype2-devel

For FreeBSD:
$ sudo pkg_add -r -v gcc cmake subversion sdl libogg libvorbis openal
  jpeg libXinerama freealut freetype2

For MacOSX 10.5+:
  Download and install "Command Line Tools for Xcode" or "Xcode".
  Install HomeBrew package manager: http://mxcl.github.com/homebrew/
$ brew install cmake sdl jpeg subversion libogg libvorbis freealut freetype

2) Checkout svn with latest source files
$ cd ~/
$ svn checkout http://svn.code.sf.net/p/openastromenace/code/ AstroMenace

3) Compile game binary
$ cd ~/AstroMenace
$ cmake ./
$ make

4) Create game VFS data file
$ ./AstroMenace --pack --rawdata=./RAW_VFS_DATA

5) Create desktop shortcut or start AstroMenace
$ ~/AstroMenace/AstroMenace



Game launch options:
   /game/bin/folder/AstroMenace [optional parameters]
   where,   
   /game/bin/folder/AstroMenace - the game executable file
   optional parameters:
    --dir=/game/data/folder/ - folder with gamedata.vfs file
    --mouse - launch the game without system cursor hiding.
    --safe-mode - reset all settings not connected to Pilots Profiles at the game start.
    --pack - pack raw game data to gamedata.vfs file.
    --rawdata=/game/rawdata/folder/ - folder with game raw data for gamedata.vfs.
    --help - info about all launch options.
Note: game settings file stored in the "$HOME/.astromenace" folder.

For Xinerama/TwinView, use SDL_VIDEO_FULLSCREEN_DISPLAY environment variable if need. 
The SDL_VIDEO_FULLSCREEN_DISPLAY environment variable set to 0 to place 
fullscreen/windowed SDL windows on the first Xinerama/TwinView screen by default.
