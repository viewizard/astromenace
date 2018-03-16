/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef Sound_H
#define Sound_H

#include "../base.h"

/*
 * Sound FX.
 */

// Play sound.
unsigned int vw_PlaySound(const std::string &Name, float _LocalVolume, float _GlobalVolume,
			  float x, float y, float z, bool Relative, bool AllowStop, int AtType);
// Load sound buffer data according to file extension.
unsigned int vw_LoadSoundBuffer(const std::string &Name);
// Check, is sound available (created) or not.
bool vw_IsSoundAvailable(unsigned int ID);
// Replay from the beginning first sound, found by name.
unsigned int vw_ReplayFirstFoundSound(const std::string &Name);
// Set sound global volume.
void vw_SetSoundGlobalVolume(const std::string &Name, float NewGlobalVolume);
// Set sound location.
void vw_SetSoundLocation(unsigned int ID, float x, float y, float z);
// Stop sound with delay.
void vw_StopSound(unsigned int ID, float StopDelay);
// Update all sounds.
void vw_UpdateSound();
// Release all sounds.
void vw_ReleaseAllSounds();
// Stop (release) only allowed sounds.
void vw_StopAllSoundsIfAllowed();

/*
 * Music.
 */

// Create and play music.
bool vw_PlayMusic(const std::string &Name, float LocalVolume, float GlobalVolume,
		  bool Loop, const std::string &LoopFileName);
// Set global music volume.
void vw_SetMusicGlobalVolume(float NewGlobalVolume);
// Check, is any music theme playing.
bool vw_IsAnyMusicPlaying();
// Set all music fade-out, if playing.
void vw_FadeOutIfMusicPlaying(float Time);
// Set music fade-in.
void vw_SetMusicFadeIn(const std::string &Name, float EndVol, float Time);
// Update all music themes status and calculate effects.
void vw_UpdateMusic();
// Release particular music theme by name. Also could be used for "stop" playing.
void vw_ReleaseMusic(const std::string &Name);
// Release all music. Also could be used for "stop" playing all music themes.
void vw_ReleaseAllMusic();

/*
 * Misc.
 */

bool vw_InitSound();
void vw_ShutdownSound();
void vw_Listener(float ListenerPos[3], float ListenerVel[3], float ListenerOri[6]);

#endif // Sound_H
