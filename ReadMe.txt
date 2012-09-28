AstroMenace

Runtime dependencies:
libSDL (ver 1.2.10+), GLU, libopenal (ver 1.0+), libalut (ver 1.0+), libogg (ver 1.1+), 
libvorbis (ver 1.1+), libjpeg (libjpeg-turbo prefered), libXinerama (optional).

Requirements:
CPU 1+ GHz, 128 MB RAM, 3D acceleration video card with OpenGL and 32+ MB on-board,
mouse. Recommended: sound card.

Game launch options:
   /game/bin/folder/AstroMenace [optional parameters]
   where,   
   /game/bin/folder/AstroMenace - the game executable file
   optional parameters:
    --dir=/game/data/folder/ - folder with gamedata.vfs file
    --mouse - launch the game without system cursor hiding.
    --noAA - disable AA antialiasing test at the game start.
    --safe-mode - reset all settings not connected to Pilots Profiles at the game launch.
    --pack - pack data to gamedata.vfs file.
    --rawdata=/game/rawdata/folder/ - folder with game raw data for gamedata.vfs.
    --help - info about all launch options.
Note: game settings file stored in the "$HOME/.astromenace" folder.

For Xinerama/TwinView, use SDL_VIDEO_FULLSCREEN_DISPLAY environment variable. 
The SDL_VIDEO_FULLSCREEN_DISPLAY environment variable set to 0 to place 
fullscreen/windowed SDL windows on the first Xinerama screen by default.




GAME KEYBOARD SHORTCUTS


F2 - toggle show/hide game FPS counter.
F12 - Capture Screenshot. (File will be saved on your Desktop.)
<Alt>+<F4> or <Alt>+<Q> - force quit.

In-Game only:

F5 - decrease game speed.
F6 - reset game speed to normal.F7 - increase game speed.
Note: do not increase game speed, if you have low FPS.

F8 - change weapon panels view.
F9 - change Primary weapon group fire mode.
F10 - change Secondary weapon group fire mode.

Esc - show/hide game menu. (Pause.)
Note: all changes will be saved in the game configuration file.