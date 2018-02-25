# Version 1.3.2

### Overall changes:

- Added "Joystick DeadZone" option in "Options" menu.
- Removed Ubuntu Family Fonts from game bundle. All bundled with game fonts are GPLv3 or OFL v1.1 now.
- Separate Game Options menu code removed from source, fully-featured main Options menu available as Game Menu now.
- Added standalone pack game data util.
- Added fontconfig support.

### Overall code changes:

- Minor correction in text output code during game window initialization.
- Added $XDG_CONFIG_HOME environment variable check during config file path detection.
- Revised web browser search code, added xdg-open.
- Some misspelled variables name corrected.
- Added copy file from real FS to VFS function.
- Improved font manager code. Menu Font option changes don't require game restart any more.
- Improved font rendering code. Rendering operate with glyph's advance width (also known as its escapement) now.
- Removed vw_TextureFromText function.
- Added more text position checks in "Tips and hints" dialogs for proper text rendering with different fonts.
- Fixed Info dialogs title position in Workshop menu.
- Minor font manager code improvements, added global offset for text rendering.
- Fixed minor bug with keyboard control over active menu elements during setup game control keys.
- Fixed Textures Quality option work during game data loading with new planetary and normal map textures.
- Fixed bug that might corrupt overall game rendering (big blinking triangles over scene).
- Improved font rendering for big font size characters (outlines will be more clear now).
- Test image file with font generated bitmap now will be stored in TGA format.
- Fixed memory overflow in VFS code.
- All OpenGL functions in shadow mapping code moved to core.
- Fixed minor bug in texture blend setup code (now will change OpenGL state proper during blend disabling).
- Improved SkyBox rendering for more speed.
- Improved textures related code, redundant OpenGL functions calls removed.
- Fixed menu elements texture generation, will count on compression options now.
- Improved lighting related code, redundant OpenGL functions calls removed.
- Minor code corrections and improvements.
- Improved vfs file creation code. VFS core code corrected to be more flexible.
- Fixed text input code in Profile Menu for percent sign (%).
- Revised Profiles duplication and deletion code.
- All language-related switch statements corrected, English language text/voice/images will be used now, if current language text/voice/images not implemented.
- All color-related switch statements corrected, white color in mission list/mission text will be used now, if current color not implemented.
- Minor code correction for some functions with switch statements, added text output, if required statement not implemented.
- Minor code optimization for ground explosions.
- Minor code optimization for "CullFace" OpenGL routine.
- Some redundant functions declarations removed.
- Minor code correction to avoid possible situation when local variable shadows another local variable.
- Minor VFS code correction.
- CMakeLists.txt file improved, added FS2VFS flag for AstroMenaceFS2VFS (standalone pack game data util) compilation.
- Fixed CMakeLists.txt, will use freetype-config util now in order to find freetype headers location (multilib-aware freetype issue).
- Fixed CFLAGS mess in CMakeLists.txt and Code::Blocks project files.
- VAO feature turned off by default in configuration file from now due to game crashes and not proper work on some AMD and Intel video cards (probably, drivers issue).
- Added game configuration file save after hardware detection to prevent configuration data loss.
- Added game configuration file save at mission start and mission complete to prevent game progress lost.
- Minor file mode bits correction in folder creation code for Linux.
- CMakeLists.txt file improved, fontconfig lib and header files detection added as non required part. If fontconfig not installed, game will be compiled without fontconfig support.
- CMakeLists.txt file improved, Xinerama lib and header files detection corrected. If Xinerama not installed, game will be compiled without Xinerama support.
- Updated GLext.h file.
- Added GL_ARB_texture_compression_bptc OpenGL extension support.
- Fixed compression flag in language-related textures initialization code during language change.
- Fixed game configuration folder owner issue in Linux during gamedata.vfs creation.
- Fixed bug in Control Menu code relative to Joystick name.

### Menu changes:

- Removed "Mouse Control" option from "Options" menu.
- Added current Pilot Profile title in Missions List menu.
- Added "Change Profile" button for Pilot Profile selection in Missions List menu. From now, Pilots Profiles menu will be skipped during game start process, if at least one Pilot Profile already created and set as current.
- Added mouse wheel support for more convenient manipulations with missions list.
- Added mouse wheel support in Weaponry, Information and Shipyard menu.
- Added mouse control for scrollbar slider.
- Added profile name edit box field reset on Pilots Profiles menu selection.
- Added "Font Size" option in "Interface" menu.
- Revised Workshop menu (System stock panel).
- Minor corrections in Credits menu.
- Improved "Textures Compression" option in Advanced Menu, compression type can be chosen now.

### Compilation flags changes (config.h):

- Added "vfs_pack_standalone" compilation flag for standalone pack game data util compilation.
- Added "fontconfig" compilation flag. With fontconfig you don't need fonts bundled with game, system fonts will be used. Make sure, that you have installed bold style TrueType, Type 1 or CFF font with en, de and ru languages support.
- Added "default_font_family" compilation flag. Default fontconfig font family.

### Game data changes:

- Added Linux Libertine (Linux Biolinum) fonts.
- Removed Ubuntu fonts.
- Fixed planet 4 geometry.
- Minor correction for License.txt file (GLext.h file info added).

# Version 1.3.1 

### Overall changes:

- Added normal mapping support. Added normal maps for all motherships, planets, big asteroids, battle freighters, buildings. 
- All jpeg image format textures replaced on tga image format. libjpeg removed from dependencies. 
- All planetary, skybox, background tile-animated textures replaced on new one.
- Added keyboard control over active menu elements with Arrow Keys, "Tab" and "Enter" keyboard buttons.
- (Linux) Game config file from now stored in "~/.config/astromenace" folder, if "~/.config" folder detected, otherwise in "~/.astromenace" folder. Move "amconfig.xml" from "~/.astromenace" to "~/.config/astromenace" folder if you need. 

### Overall code changes:

- Fixed bug in game data loading code, that might crash game in Linux/BSD on pirates missions. 
- (CmakeList.txt) Improved libs and headers detection, added text output. 
- Minor corrections in shadow mapping code. 
- Added 3d models game native format "write" function, for future code purposes. 
- Fixed minor memory leak on 3d object block destruction. 
- Functions and variables names connected to VBO and IBO corrected. 
- (GCC/MinGW) Fixed "unused parameter" warning for several functions, to avoid useless warnings.
- Fixed particle systems rendering in Information and Workshop menus to avoid all particle systems visual frustum double check. 
- Fixed 3d models (part of star system background) rendering, redundant OpenGL functions calls removed. 
- Improved 3d models mesh explosion effect, all big triangles in 3d model mesh divide on smaller now for more smooth visual disappearing. 
- Added shadow from destroyed 3d models pieces during object explosion for smooth shadow disappearing. 
- Fixed source code for Mac OS X full support. Now, can be compiled with cmake without any problems, if you have all libs installed. Added XCode project files for game compilation in Mac OS X with XCode IDE.
- Revised shadows shader for more speed. 
- Fixed aspect ratio setup during first game start for monitors with standard aspect ratio. 
- Minor resolutions list initialization code correction. 
- Fixed bug in second game window initialization, if first window initialization failed. 
- Added check for game configuration file content to avoid game crash if file content corrupted. 
- Improved skybox stars rendering. 
- Fixed bug with OpenAL device release at game shutdown. 
- Fixed missile targeting, will not change target if current target still alive, if current target alive but not in front - target new object.
- Improved targeting code. Missiles and weapons use "geometry center" now as target point in 3d object. 
- Text manager revised for .csv file format support and more speed. 
- Fixed bug with depth buffer range in shadow initialization, shadow mapping code revised. 
- Improved work with depth buffer in Information menu and Workshop, redundant OpenGL functions calls removed. 
- Fixed 3D object rotation in Information menu (will not rotate on right during up or down rotation any more). 
- Fixed bug with 4 and 5 jeeps targeting (ion weapon). 
- Minor code fix, multitexture functions pointers check added. 
- Text related code improved for easy future new languages support. All text related code moved to "core". 
- Fixed minor bug in particle systems visual frustrum check. 
- Added missile trail effect. 
- Added build number to VFS data file structure, to avoid game launch with outdated game data file. 
- Added shortcut keys for emergency game exit: Ctrl+Q for Linux/Windows, Command+Q for Mac OS X. 
- Improved explosion shaders (fragment shader optimized for explosions now). 
- Minor code correction for wheels. 
- Minor code correction for tile animation (texture matrix). 
- Renamed several variables for more clear look code (legacy of DirectX game rendering code). 
- Minor 3D models loading code optimization. 
- Minor fix in particle generation code. 
- Removed BMP image format support, all needs covered by TGA. 
- Several Aliens Motherships visual effects removed. 
- Added image game native VW2D format support. 
- Minor shadow map generation improvement, removed z-fighting during pirate base models rendering with shadows. 
- Fixed shadows rendering for some 3d models in Information menu.
- Minor particle system code improvement.
- Changed background tile animated layer code, first layer can't be turned off any more.
- Minor bug fixed in stars generation code (removed theoretical chance divide by zero during vector normalization, with game freeze). 
- Added additional check for loaded textures to avoid duplicates. 
- Fixed missile targeting, will not target objects out of range (count on left missile life time). 
- Mersenne Twister random number generator source code replaced on pre-generated random numbers.
- (Linux) Corrected web browsers list in installed web browser search code. 
- Added XML parser code. TinyXML code removed from sources. 
- Optimized mission script connected code for more speed. 
- Fixed VFS error messages output, will use "stderr" now instead of normal stream. 
- Fixed point lights color in player ship engines (antimatter engines). 
- Changed menu backgroud script changer (will load scripts one by one now, instead of random). 
- Added "shock wave" effect for missile trails during missile explosion. 
- Fixed work with texture compression on modern video card that support glTexStorage2D.
- Menu language and voice language change don't required game restart any more.

### Menu changes:

- Fixed "Shadow Quality" option status, if hardware don't support FBO Depth size more than 16 bits (game require at least 24 bits for shadows). 
- Added font preview in "Interface" menu.
- Added "VSynс" option in "Options" menu, default value is "Off".
- Added PCF control in "Advanced" menu for shadow mapping shaders. 
- "Texture Filtering Mode" option moved to "Advanced" menu. 
- "Textures Quality" option moved to "Advanced" menu. 
- "Gamma" option renamed to "Brightness". 

### Compilation flags changes (config.h):

- Added "portable". Game configuration file will be stored with game binary file. 
- Added "separate_cc_vfs". Since game use content released under different licenses, you might need create separate VFS game data file (gamedata_cc.vfs) with CC BY-SA licensed content only. 
- Removed "buildin_tynixml". 

### Game data changes: 

- Added license files for fonts.
- Fixed shadow on game icons. 
- Fixed normals in asteroid rings for planet 3d model with asteroid rings. 
- Text .xml files merged and converted to .csv file format. 
- Added more info about artwork licenses, since RAW_VFS_DATA included into SVN. 
- All planetary textures replaced (thanks to guys from VegaTrek http://sourceforge.net/projects/vegatrek/). 
- All skybox textures replaced on textures generated in Spacescape program (http://sourceforge.net/projects/spacescape/). 
- Tile animated layer textures replaced. 
- Fixed issue with license for models. More info added to License.txt file.

# Version 1.3.0

### Overall changes:

- Added Xinerama/TwinView support.
- Added multilingual support (en/de/ru), all language data files merged with main VFS data file.
- Added VFS game data file creation feature.
- Added FreeType2 support for all font rendering, 7 ttf fonts added to game data file.
- Added current OS keyboard layout support for Profile name input.
- Added soft shadows (Shadow Mapping with PCF).

### Overall code changes:

- Fixed compilation error connected to "PFNGLCLIENTACTIVETEXTUREPROC" and new mesa version.
- Fixed compilation warnings "deprecated conversion from string constant to ‘char*’" with new gcc version.
- Fixed several code issues for Windows game version compilation.
- Fixed minor memory leak during game data file close.
- Fixed bug with explosion initialization.
- Fixed turrets rotation bug.
- Fixed minor memory leaks during explosion initialization.
- Fixed bug with point lights in lights manager.
- Fixed voice warning time for collision course.
- Fixed bug with weapon orientation in Weaponry (new weapon mount and weapon custom angle change).
- Fixed bug with weapon targeting with custom weapon angles.
- Fixed bug with Hit Box collision detection.
- Fixed bug in particle system manager work with shaders.
- Updated Windows Code::Blocks project for new version libsdl, libjpeg-turbo, libogg, libvorbis, libopenal-soft.
- Updated glext.h file.
- Updated TintXML code to version 2.6.2
- Added PNG (via libpng) and BMP image formats support.
- Added shader model detection for GLSL version more then 4.0.
- Added GL_ARB_texture_storage OpenGL extension support.
- Added GL_ARB_vertex_array_object OpenGL extension support.
- Added GL_ARB_framebuffer_object OpenGL extension support.
- Added Multisample Coverage anti aliasing support via GL_NV_framebuffer_multisample_coverage OpenGL extension.
- Added text warning about collision course detection.
- Improved turrets barrel rotation and tile tracks animation.
- Improved "Gamma" connected code. Now work in both - fullscreen and windowed game mode.
- Improved game initialization code.
- Improved XML parser connected code.
- Improved all textures connected code.
- Improved collision detection code.
- Improved shaders connected code.
- Revised all shaders code.
- Revised direct lights in Workshop and Information menu.
- Revised all code connected to objects rotation, changed to right-handed coordinate system. All scripts revised.
- Removed all Windows demo version related code.

### Menu changes:

- Added box with Language selection on first game start.
- Added "Ogg Vorbis" and "FreeType2" to credits list.
- Added comparison details with current system/ship in "Workshop" menu info dialogs.
- Added model rotation buttons in "Information" menu.
- Added "Reload All" button in "Weaponry" menu.
- Added "Voice volume" option in "Options" menu.
- Added "Interface" sub-menu in "Options" menu. 
- Added "Menu Language" option in "Interface" menu.
- Added "Voice Language" option in "Interface" menu.
- Added "Menu Font" option in "Interface" menu.
- Added "Tips and hints" status reset option in "Interface" menu.
- Added "FPS Сounter" option in "Interface" menu.
- Added "Weapon Panels View" option in "Interface" menu.
- Added "Shadow Quality" option in "Advanced" menu.
- Added "Visual Effects Quality" option in "Advanced" menu.
- Removed "Refresh Rate" option from "Options" menu.
- Removed "Particles GFX Quality" option in "Advanced" menu.
- Removed "Explosions GFX Quality" option in "Advanced" menu.
- Removed "Background Tile Animation" option in "Advanced" menu.
- Removed "Stars Quantity" option in "Advanced" menu.

### Launch options changes:

- Added libSDL "SDL_VIDEO_FULLSCREEN_DISPLAY" environment variable support for Xinerama/TwinView.
- Added "--pack" parameter to switch AstroMenace in VFS creation mode (terminal output only).
- Added "--rawdata" parameter (used with "--pack" only), to define raw data folder location (./RAW_VFS_DATA by default).
- Removed "--noAA" parameter.

### Compilation flags changes (config.h):

- Removed EN, DE and RU.
- Added "joystick". Required libSDL compilled with "joystick" flag.
- Added "compression". Compression during gamedata.vfs creation, game will take more time for loading in exchange of game data file size reducing (about 30 MB).
- Added "vbo". VBO OpenGL extension support switcher (if defined - game will detect vbo support in hardware during initialization).
- Added "png". Required libpng.
- Added "gamedebug". More console output with debug info.
- Added "buildin_tynixml". Compile with build-in tynixml lib (version 2.6.2) if defined.
- Added "vao". VAO OpenGL extension support switcher (if defined - game will detect vao support in hardware during initialization).
- Added "fbo". FBO OpenGL extension support switcher (if defined - game will detect fbo support in hardware during initialization).

# Version 1.2.0

Initial release.
