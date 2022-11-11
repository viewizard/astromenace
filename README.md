# AstroMenace  [![Build Status](https://travis-ci.org/viewizard/astromenace.svg?branch=master)](https://travis-ci.org/viewizard/astromenace)

* [Installation (Windows, macOS, Linux)](https://github.com/viewizard/astromenace#installation)
* [Build (Windows)](https://github.com/viewizard/astromenace#build-windows)
* [Build (macOS, Linux, BSD)](https://github.com/viewizard/astromenace#build-macos-linux-bsd)
* [Translators Wanted](https://github.com/viewizard/astromenace#translators-wanted)
* [Work In Progress](https://github.com/viewizard/astromenace#work-in-progress)

---

[AstroMenace on YouTube, a video is worth a thousand words.](https://www.youtube.com/watch?v=ysY9vKKisbo&index=1&list=PLrWi_GXhwHyznYT19oAQL4zNldlVXFlj2)

Immerse into a decisive battle against tons of cunning foes, face the terrifying bosses and protect your homeland throughout 15 diverse levels of the game. The hardcore gameplay of AstroMenace, packed with pure non-stop action, will become a full scale test for your basic instinct of survival.

<p align="center">
  <img src="./share/preview1.png" alt="preview"/>
</p>

The game provides a wide variety of armaments and weapon upgrades for discharging the retributive wrath upon the hordes of enemies, besides it has a great number of improvements for enhancing the defensive abilities of your spaceship. Collect money during the combat and invest them into turning your spaceship into an ultimate weapon of mass destruction.

<p align="center">
  <img src="./share/preview2.png" alt="preview"/>
</p>

More information and screenshots could be found at [AstroMenace website](https://viewizard.com/).

## Installation

Installer for Windows could be found in [Releases](https://github.com/viewizard/astromenace/releases).

Almost all popular Linux distros already have a stable version of the game in their repositories. Install the "astromenace" package with your distro's package manager. We do not provide packages for any Linux anymore, contact maintainers of your Linux distro.

[![latest packaged version(s)](https://repology.org/badge/latest-versions/astromenace.svg)](https://repology.org/metapackage/astromenace) [![Packaging status](https://repology.org/badge/tiny-repos/astromenace.svg)](https://repology.org/metapackage/astromenace)

#### Debian/Ubuntu and derivatives
```
sudo apt-get install astromenace
```

#### OpenSuse
```
sudo zypper install astromenace
```

#### Fedora
```
sudo yum install astromenace
```

#### Gentoo
```
sudo emerge astromenace
```

## Build (Windows)

- Install MSYS2 from https://www.msys2.org (follow step-by-step instruction on first page in order to install and update MSYS2)

**For Windows 32bit build.** Start `MSYS2 64bit` > `MSYS2 MinGW 32-bit` terminal session from `Start` menu. Install all necessary packages and download prebuilt dependencies:
```
$ cd /
$ pacman -S git unzip mingw-w64-i686-{toolchain,make,cmake,ninja}
$ wget https://github.com/viewizard/astromenace-windows-prebuilt-dependencies/releases/download/latest/prebuilt_dep_mingw_32bit.zip
$ unzip ./prebuilt_dep_mingw_32bit.zip
```
**For Windows 64bit build.** Start `MSYS2 64bit` > `MSYS2 MinGW 64-bit` terminal session from `Start` menu. Install all necessary packages and download prebuilt dependencies:
```
$ cd /
$ pacman -S git unzip mingw-w64-x86_64-{toolchain,make,cmake,ninja}
$ wget https://github.com/viewizard/astromenace-windows-prebuilt-dependencies/releases/download/latest/prebuilt_dep_mingw_64bit.zip
$ unzip ./prebuilt_dep_mingw_64bit.zip
```

- Clone git with latest source files
```
$ git clone https://github.com/viewizard/astromenace.git
```
- Compile game binary (game data file will be created automatically):
```
$ mkdir ~/astromenace/build
```
```
$ cd ~/astromenace/build
```
**For Windows 32bit build:**
```
$ cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=$PWD/../bin -DCMAKE_BUILD_TYPE=Release -DDEPS_PATH=$PWD/../../prebuilt_dep_mingw_32bit
```
**For Windows 64bit build:**
```
$ cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=$PWD/../bin -DCMAKE_BUILD_TYPE=Release -DDEPS_PATH=$PWD/../../prebuilt_dep_mingw_64bit
```
```
$ cmake --build . --target install
```
- Create desktop shortcut or start AstroMenace
```
$ ~/astromenace/bin/astromenace
```
Note, in case of default MSYS2 path, all AstroMenace binary files will be stored into folder `C:\msys64\home\user\astromenace\bin`. You can copy this files into folder you want and remove MSYS2 from your system.

## Build (macOS, Linux, BSD)

Build dependencies:
libSDL2 (ver 2.0.5+), libopenal (ver 1.0+), libalut (ver 1.0+), libogg (ver 1.1+), libvorbis (ver 1.1+), freetype (ver 2.1.6+)


The easy way to get up to date AstroMenace on your computer:

### 1) Make sure all necessary packages with development headers installed in your system: 

gcc or clang or any compiler with full [ISO/IEC 14882:2011 (C++11)](https://www.iso.org/standard/50372.html) support, cmake, make, git, libsdl2, libogg, libvorbis, openal, alut (freealut), freetype2

#### For macOS:
Download and install "Command Line Tools for Xcode" or "Xcode".
Install HomeBrew package manager: https://brew.sh/
```
$ brew install ninja cmake sdl2 git libogg libvorbis freealut freetype
```

#### For Ubuntu/Mint/Debian:
```
$ sudo apt-get install g++ cmake make ninja-build libsdl2-dev libogg-dev \
  git libvorbis-dev libopenal-dev libalut-dev libfreetype6-dev
```

#### For Mageia:
```
$ sudo urpmi gcc-c++ ninja cmake make git libsdl2.0-devel libogg-devel \
  libvorbis-devel libopenal-devel libfreealut-devel freetype2-devel
```

#### For OpenSuse:
```
$ sudo zypper install gcc-c++ ninja cmake make libSDL2-devel libogg-devel \
  git libvorbis-devel openal-soft-devel freealut-devel freetype2-devel
```

#### For Fedora:
```
$ sudo yum install gcc-c++ cmake make git SDL2-devel libogg-devel libvorbis-devel \
  ninja-build openal-soft-devel freealut-devel freetype-devel linux-libertine-fonts
```

#### For FreeBSD:
```
$ sudo pkg_add -r -v gcc cmake git ninja sdl20 libogg libvorbis openal freealut freetype2
```

### 2) Clone git with latest source files
```
$ cd ~/
```
```
$ git clone https://github.com/viewizard/astromenace.git
```

### 3) Compile game binary (game data file will be created automatically)
```
$ mkdir ~/astromenace/build
```
```
$ cd ~/astromenace/build
```
```
$ cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=$PWD/../bin -DCMAKE_BUILD_TYPE=Release
```
```
$ cmake --build . --target install
```

### 4) Create desktop shortcut or start AstroMenace
```
$ ~/astromenace/bin/astromenace
```

## Translators Wanted
Please help us make AstroMenace more friendly and clear for all gamers.

AstroMenace localization now supported by Crowdin https://crowdin.com/project/astromenace.

Don't hesitate to create [issue](https://github.com/viewizard/astromenace/issues), if you have questions or you need help.

## Work In Progress
Yes, AstroMenace was first released in 2007, but work still in progress. For open source game, no need to change game's title in order to evolve.

The main goal now is code refactoring for modern OpenGL features and new C++ standards (C++11, STL). Plus, I still provide AstroMenace code support, that means changes for new libs versions and new compilers versions.

Unfortunately, I could not provide you with any new version's release dates, since this is extremely depends from my free time and limited resources. Check project's [plans](https://github.com/viewizard/astromenace/projects) for more info about current progress.

We also have a surprise for you, here is the artwork 1 of 12 light ships and 1 of 8 capital ships we already have, with best wishes from Oleg Linkov (3D models author). Since this is ours 3D models, we will release sources under GPLv3 in [astromenace-artwork](https://github.com/viewizard/astromenace-artwork) repository as soon, as AstroMenace will use them. I hope, some day we will replace all 3D models licensed under CC-BY-SA (without sources) to open source 3D models.
<p align="center">
  <img src="./share/wip/scout.png" alt="preview"/>
</p>
<p align="center">
  <img src="./share/wip/heavy.png" alt="preview"/>
</p>
