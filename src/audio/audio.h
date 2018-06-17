/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef AUDIO_AUDIO_H
#define AUDIO_AUDIO_H

#include "../core/base.h"

// game sfx
enum class eGameSFX {
	none,
	WeaponMalfunction_Kinetic, // Kinetic
	WeaponMalfunction_Particle, // Ion, Plasma
	WeaponMalfunction_Beam, // Maser, Laser
	WeaponMalfunction_Energy, // Antimatter, Gauss
	WeaponMalfunction_Launcher, // Missile, Torpedo, Bomb
	Explosion_Small, // Asteroid
	Explosion_Medium, // Missiles
	Explosion_Big, // Torpedo, Alien Ship
	Explosion_Big_Energy, // Bomb, Ground Object, Earth Fighter, Pirate Ship
	WeaponFire_Kinetic1,
	WeaponFire_Kinetic2,
	WeaponFire_Kinetic3,
	WeaponFire_Kinetic4,
	WeaponFire_Ion1,
	WeaponFire_Ion2,
	WeaponFire_Ion3,
	WeaponFire_Plasma1,
	WeaponFire_Plasma2,
	WeaponFire_Plasma3,
	WeaponFire_Maser1,
	WeaponFire_Maser2,
	WeaponFire_Antimatter,
	WeaponFire_Laser,
	WeaponFire_Gauss,
	WeaponFire_SmallMissile,
	WeaponFire_NormalMissile,
	WeaponFire_Torpedo,
	WeaponFire_Bomb,
	Hit_Kinetic,
	Hit_Ion,
	Hit_Plasma,
	Hit_Antimatter,
	Hit_Gauss
};

// voice
enum class eVoicePhrase {
	Attention,
	EngineMalfunction,
	MissileDetected,
	PowerSupplyReestablished,
	PrepareForAction,
	ReactorMalfunction,
	Warning,
	WeaponDamaged,
	WeaponDestroyed,
	WeaponMalfunction
};

// music themes
enum class eMusicTheme {
	NONE,
	MENU,
	GAME,
	BOSS,
	FAILED,
	CREDITS
};


// Play "2D" sfx (menu sfx).
unsigned int Audio_PlaySound2D(unsigned int SoundID, float LocalVolume);
// Play game sfx (3D).
unsigned int PlayGameSFX(eGameSFX GameSFX, float LocalVolume, const sVECTOR3D &Location, int AtType = 1);
// Play voice phrase.
unsigned int PlayVoicePhrase(eVoicePhrase VoicePhrase, float LocalVolume);
// Play music theme with fade-in and fade-out previous music theme (if need).
void PlayMusicTheme(eMusicTheme MusicTheme, uint32_t FadeInTicks, uint32_t FadeOutTicks);
// Change "global" volume for "2D" (menu) sfx.
void Audio_SetSound2DGlobalVolume(float NewGlobalVolume);
// Change "global" volume for voice.
void Audio_SetVoiceGlobalVolume(float NewGlobalVolume);
// Main audio loop.
void AudioLoop();

#endif // AUDIO_AUDIO_H
