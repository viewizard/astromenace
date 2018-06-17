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
// Play "3D" sfx (game sfx).
unsigned int Audio_PlaySound3D(int SoundID, float LocalVolume, sVECTOR3D Location, int AtType = 1);
// Play voice.
unsigned int Audio_PlayVoice(unsigned int VoiceID, float LocalVolume);
// Start music theme with fade.
void StartMusicWithFade(eMusicTheme StartMusic, uint32_t FadeInTicks, uint32_t FadeOutTicks);
// Change "global" volume for "2D" (menu) sfx.
void Audio_SetSound2DGlobalVolume(float NewGlobalVolume);
// Change "global" volume for voice.
void Audio_SetVoiceGlobalVolume(float NewGlobalVolume);
// Main audio loop.
void AudioLoop();

#endif // AUDIO_AUDIO_H
