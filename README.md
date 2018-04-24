# AstroMenace  [![Build Status](https://travis-ci.org/viewizard/astromenace.svg?branch=master)](https://travis-ci.org/viewizard/astromenace)  [![bitcoin](https://img.shields.io/badge/donate-bitcoin-orange.svg)](https://github.com/viewizard/astromenace#bitcoin)  [![paypal](https://img.shields.io/badge/donate-paypal-orange.svg)](https://github.com/viewizard/astromenace#paypal)

Immerse into a decisive battle against tons of cunning foes, face the terrifying bosses and protect your homeland throughout 15 diverse levels of the game. The hardcore gameplay of AstroMenace, packed with pure non-stop action, will become a full scale test for your basic instinct of survival.

<p align="center">
  <img src="https://raw.githubusercontent.com/viewizard/astromenace/master/share/preview1.png" alt="preview"/>
</p>

The game provides a wide variety of armaments and weapon upgrades for discharging the retributive wrath upon the hordes of enemies, besides it has a great number of improvements for enhancing the defensive abilities of your spaceship. Collect money during the combat and invest them into turning your spaceship into an ultimate weapon of mass destruction.

<p align="center">
  <img src="https://raw.githubusercontent.com/viewizard/astromenace/master/share/preview2.png" alt="preview"/>
</p>

More information and screenshots could be found at [AstroMenace web site](http://www.viewizard.com/).

Installers for Windows and Mac could be found at [release](https://github.com/viewizard/astromenace/releases) section.

## Installation

Almost all popular Linux distros already have a stable version of the game in their repositories. Install the "astromenace" package with your distro's package manager. We do not provide compiled binary packages for Linux anymore, contact maintainers of your Linux distro in order to get binary package.

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
sudo emerge openastromenace
```

## Build

Runtime dependencies:
libSDL2 (ver 2.0.0+), libopenal (ver 1.0+), libalut (ver 1.0+), libogg (ver 1.1+), libvorbis (ver 1.1+), freetype (ver 2.1.6+), GLU, fontconfig (optional), Linux Libertine/Biolinum fonts (optional)


The easy way to get up to date AstroMenace on your computer:

### 1) Make sure all necessary packages with development headers installed in your system: 

   gcc (with g++), cmake, make, git, libsdl2, libogg, libvorbis, openal, 
   alut (freealut), GLU, freetype2, fontconfig

#### For Ubuntu/Mint/Debian:
```bash
$ sudo apt-get install g++ cmake make git libsdl2-dev
  libogg-dev libvorbis-dev libopenal-dev libalut-dev
  libfreetype6-dev libfontconfig1-dev fonts-linuxlibertine
```

#### For Mageia:
```bash
$ sudo urpmi gcc-c++ cmake make git libsdl2.0-devel libogg-devel
  libvorbis-devel libopenal-devel libfreealut-devel freetype2-devel
  libfontconfig-devel fonts-ttf-libertine
```

#### For OpenSuse:
```bash
$ sudo zypper install gcc-c++ cmake make git libSDL2-devel
  libogg-devel libvorbis-devel openal-soft-devel freealut-devel
  freetype2-devel fontconfig-devel linux-libertine-fonts
```

#### For Fedora:
```bash
$ sudo yum install gcc-c++ cmake make git SDL2-devel libogg-devel
  libvorbis-devel openal-soft-devel freealut-devel freetype-devel
  fontconfig-devel linux-libertine-fonts linux-libertine-biolinum-fonts
```

#### For FreeBSD:
```bash
$ sudo pkg_add -r -v gcc cmake git sdl20 libogg libvorbis openal
  freealut freetype2 fontconfig linuxlibertine
```

#### For MacOSX 10.5+:
Download and install "Command Line Tools for Xcode" or "Xcode".
Install HomeBrew package manager: https://brew.sh/
```bash
$ brew install cmake sdl2 git libogg libvorbis freealut freetype
```

### 2) Clone git with latest source files
```bash
$ cd ~/
$ git clone https://github.com/viewizard/astromenace.git astromenace
```

### 3) Compile game binary (game data file will be created automatically)
```bash
$ cd ~/astromenace
$ cmake ./
$ make
```

### 4) Create desktop shortcut or start AstroMenace
```bash
$ ~/astromenace/astromenace
```

## WIP
Yes, AstroMenace was first released in 2007 and still in development (probably, as all open source projects, since we don't need new titles to sell more). The main goal now is code refactoring for new libs versions support, modern OpenGL features, new C++ standards (C++11, STL). New features planned too, but this is a secret for now. Check ours [plans](https://github.com/viewizard/astromenace/projects) for more info.

We also have a surprise for you, here is the artwork 1 of 12 light ships and 1 of 8 capital ships we already have, with best wishes from Oleg Linkov (3D models author). Since this is ours 3D models, we will release sources under GPLv3 in [astromenace-artwork](https://github.com/viewizard/astromenace-artwork) repository as soon, as AstroMenace will use them. I hope, some day we will replace all 3D models licensed under CC-BY-SA (without sources) to open source 3D models.
<p align="center">
  <img src="https://raw.githubusercontent.com/viewizard/astromenace/master/share/wip/scout.png" alt="preview"/>
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/viewizard/astromenace/master/share/wip/heavy.png" alt="preview"/>
</p>

## Donate to Support Development
### Bitcoin
![bitcoin](https://raw.githubusercontent.com/viewizard/astromenace/master/share/donate/bitcoin-qr-code.png)
##### 14kDaLnNTroFfuur2tZmUdUp8eK3pS2Uq6

### PayPal
https://www.paypal.me/viewizard

### Bank Transfer
* All donations are anonymous. Any sender details are not published or shared.
* A tax-related donation receipt can not be issued because Viewizard is not a commercial nor registered organization.
* Legally, it is a private donation.

##### On the transfer form, «Donation» or «Financial assistance» must be mentioned in the purpose area.
```
Beneficiary’s bank             ALFA-BANK
Beneficiary’s bank address     27 Kalanchevskaya str., Moscow, 107078
S.W.I.F.T.                     ALFARUMM
Beneficiary’s Acc. №           40817840506220005728
Beneficiary                    Kurinnoi Mikhail Georgievich

Correspondent bank of beneficiary’s bank
CITIBANK NA
399 Park Avenue, New York, NY 10043, USA
CITIUS33
Acc. With corresp Bank № 36310481
```