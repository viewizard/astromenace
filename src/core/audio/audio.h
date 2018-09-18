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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef CORE_AUDIO_AUDIO_H
#define CORE_AUDIO_AUDIO_H

#include "../base.h"

namespace viewizard {

struct sVECTOR3D;

/*
 * Buffer.
 */

// Release sound buffer.
void vw_ReleaseSoundBuffer(const std::string &Name);

/*
 * Sound FX.
 */

// Play sound.
unsigned int vw_PlaySound(const std::string &Name, float LocalVolume, float GlobalVolume,
			  const sVECTOR3D &Location, bool Relative, bool AllowStop, int AtType);
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
void vw_StopSound(unsigned int ID, uint32_t StopDelayTicks);
// Update all sounds.
void vw_UpdateSound(uint32_t CurrentTick);
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
// Check, is music theme playing.
bool vw_IsMusicPlaying(const std::string &Name);
// Check, is any music theme playing.
bool vw_IsAnyMusicPlaying();
// Fade-out all music themes, except provided.
void vw_FadeOutAllMusicWithException(const std::string &Name, uint32_t Ticks,
				     float ExceptionFadeInEndVol, uint32_t ExceptionFadeInTicks);
// Set music fade-in.
void vw_SetMusicFadeIn(const std::string &Name, float EndVol, uint32_t Ticks);
// Update all music themes status and calculate effects.
void vw_UpdateMusic(uint32_t CurrentTick);
// Release particular music theme by name. Also could be used for "stop" playing.
void vw_ReleaseMusic(const std::string &Name);
// Release all music. Also could be used for "stop" playing all music themes.
void vw_ReleaseAllMusic();

/*
 * Audio.
 */

// Initialize audio.
bool vw_InitAudio();
// Get audio initialization status.
bool vw_GetAudioStatus();
// Shutdown audio.
void vw_ShutdownAudio();
// Setup listener in 3D space.
// Note, orientation expressed as "at" and "up" vectors.
void vw_Listener(float (&ListenerPosition)[3], float (&ListenerVelocity)[3], float (&ListenerOrientation)[6]);

} // viewizard namespace

#endif // CORE_AUDIO_AUDIO_H
