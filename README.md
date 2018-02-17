# AstroMenace

Immerse into a decisive battle against tons of cunning foes, face the terrifying bosses and protect your homeland throughout 15 diverse levels of the game. The hardcore gameplay of AstroMenace, packed with pure non-stop action, will become a full scale test for your basic instinct of survival.

<p align="center">
  <img src="https://raw.githubusercontent.com/viewizard/astromenace/master/preview1.jpg" alt="preview"/>
</p>

The game provides a wide variety of armaments and weapon upgrades for discharging the retributive wrath upon the hordes of enemies, besides it has a great number of improvements for enhancing the defensive abilities of your spaceship. Collect money during the combat and invest them into turning your spaceship into an ultimate weapon of mass destruction.

<p align="center">
  <img src="https://raw.githubusercontent.com/viewizard/astromenace/master/preview2.png" alt="preview"/>
</p>

More information and screenshots could be found at [AstroMenace web site](http://www.viewizard.com/).

Installers for Windows and Mac could be found at [release](https://github.com/viewizard/astromenace/releases) section.

## Build

Runtime dependencies:
libSDL (ver 1.2.10+), libopenal (ver 1.0+), libalut (ver 1.0+), libogg (ver 1.1+), 
libvorbis (ver 1.1+), freetype (ver 2.1.6+), GLU, libXinerama (optional), 
fontconfig (optional), Linux Libertine/Biolinum fonts (optional)


The easy way to get up to date AstroMenace on your computer:

### 1) Make sure all necessary packages with development headers installed in your system: 

   gcc (with g++), cmake, make, subversion, libsdl, libogg, libvorbis, openal, 
   alut (freealut), libXinerama, GLU, freetype2, fontconfig

#### For Ubuntu/Mint/Debian:
```bash
$ sudo apt-get install g++ cmake make subversion libsdl1.2-dev
  libogg-dev libvorbis-dev libopenal-dev libXinerama-dev libalut-dev
  libfreetype6-dev libfontconfig1-dev fonts-linuxlibertine
```

#### For Mageia:
```bash
$ sudo urpmi gcc-c++ cmake make subversion libSDL-devel libogg-devel
  libvorbis-devel libopenal-devel libxinerama-devel libfreealut-devel
  freetype2-devel libfontconfig-devel fonts-ttf-libertine
```

#### For OpenSuse:
```bash
$ sudo zypper install gcc-c++ cmake make subversion libSDL-devel
  libogg-devel libvorbis-devel openal-soft-devel libXinerama-devel
  freealut-devel freetype2-devel fontconfig-devel linux-libertine-fonts
```

#### For Fedora:
```bash
$ sudo yum install gcc-c++ cmake make subversion SDL-devel libogg-devel
  libvorbis-devel openal-soft-devel libXinerama-devel freealut-devel 
  freetype-devel fontconfig-devel linux-libertine-fonts
  linux-libertine-biolinum-fonts
```

#### For FreeBSD:
```bash
$ sudo pkg_add -r -v gcc cmake subversion sdl libogg libvorbis openal
  libXinerama freealut freetype2 fontconfig linuxlibertine
```

#### For MacOSX 10.5+:
Download and install "Command Line Tools for Xcode" or "Xcode".
Install HomeBrew package manager: https://brew.sh/
```bash
$ brew install cmake sdl subversion libogg libvorbis freealut freetype
```

### 2) Clone git with latest source files
```bash
$ cd ~/
$ git clone https://github.com/viewizard/astromenace.git AstroMenace
```

### 3) Compile game binary
```bash
$ cd ~/AstroMenace
$ cmake ./
$ make
```

### 4) Create game VFS data file
```bash
$ ./AstroMenace --pack --rawdata=./gamedata
```

### 5) Create desktop shortcut or start AstroMenace
```bash
$ ~/AstroMenace/AstroMenace
```
