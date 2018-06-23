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

/*

We preload all sfx and voice sound buffers before first access, otherwise
they will be load on first access (that may lag the game process).

*/

#include "../core/core.h"
#include "../config/config.h"
#include "audio.h"

namespace {

// empirical found "everage load value" for one sfx asset
// small value mean that asset loads fast, big value - slow
constexpr unsigned SFXLoadValue{20};

eMusicTheme CurrentPlayingMusicTheme{eMusicTheme::NONE};
unsigned int CurrentLoadedVoiceAssetsLanguage{0}; // English

struct sSoundMetadata {
	std::string FileName{};
	float VolumeCorrection{0.0f};
	bool AllowStop{true}; // allow stop this sfx in vw_StopAllSoundsIfAllowed()

	sSoundMetadata(const std::string &_FileName,
		       float _VolumeCorrection,
		       bool _AllowStop) :
		FileName{_FileName},
		VolumeCorrection{_VolumeCorrection},
		AllowStop{_AllowStop}
	{}
};

struct sMusicMetadata {
	std::string FileName{};
	std::string FileNameLoop{};
	float VolumeCorrection{0.0f};
	bool NeedRelease{false}; // if this music theme playing now - release it first

	sMusicMetadata(const std::string &_FileName,
		       const std::string &_FileNameLoop,
		       float _VolumeCorrection,
		       bool _NeedRelease) :
		FileName{_FileName},
		FileNameLoop{_FileNameLoop},
		VolumeCorrection{_VolumeCorrection},
		NeedRelease{_NeedRelease}
	{}
};

const std::unordered_map<eMenuSFX, sSoundMetadata, sEnumHash> MenuSFXMap{
	// key					metadata
	{eMenuSFX::OverSmallButton,		{"sfx/menu_onbutton2.wav", 0.15f, false}},
	{eMenuSFX::OverBigButton,		{"sfx/menu_onbutton.wav", 0.4f, false}},
	{eMenuSFX::Click,			{"sfx/menu_click.wav", 0.6f, false}},
	{eMenuSFX::SwitchToAnotherMenu,		{"sfx/menu_new.wav", 1.0f, true}},
	{eMenuSFX::TapingClick,			{"sfx/menu_taping.wav", 0.8f, true}},
	{eMenuSFX::OverLine,			{"sfx/menu_online.wav", 0.75f, true}},
	{eMenuSFX::SelectLine,			{"sfx/menu_selectline.wav", 1.0f, true}},
	{eMenuSFX::CanNotClick,			{"sfx/menu_nclick.wav", 1.0f, false}},
	{eMenuSFX::DragError,			{"sfx/drag_error.wav", 1.0f, true}},
	{eMenuSFX::DragUninstallFromSlot,	{"sfx/drag_offslot.wav", 0.65f, true}},
	{eMenuSFX::DragInstallToSlot,		{"sfx/drag_onslot.wav", 0.82f, true}},
	{eMenuSFX::DragRelease,			{"sfx/drag_release.wav", 0.6f, true}},
	{eMenuSFX::MissionShowMenu,		{"sfx/game_showmenu.wav", 1.0f, false}},
	{eMenuSFX::MissionHideMenu,		{"sfx/game_hidemenu.wav", 1.0f, false}},
	{eMenuSFX::WarningLowLife,		{"sfx/lowlife.wav", 1.0f, true}}
};

const std::unordered_map<eGameSFX, sSoundMetadata, sEnumHash> GameSFXMap{
	// key					metadata
	{eGameSFX::WeaponMalfunction_Kinetic,	{"sfx/weapon1probl.wav", 1.0f, true}},
	{eGameSFX::WeaponMalfunction_Particle,	{"sfx/weapon2probl.wav", 1.0f, true}},
	{eGameSFX::WeaponMalfunction_Beam,	{"sfx/weapon3probl.wav", 1.0f, true}},
	{eGameSFX::WeaponMalfunction_Energy,	{"sfx/weapon4probl.wav", 1.0f, true}},
	{eGameSFX::WeaponMalfunction_Launcher,	{"sfx/weapon5probl.wav", 1.0f, true}},
	{eGameSFX::Explosion_Small,		{"sfx/explosion4.wav", 1.0f, true}},
	{eGameSFX::Explosion_Medium,		{"sfx/explosion1.wav", 1.0f, true}},
	{eGameSFX::Explosion_Big,		{"sfx/explosion2.wav", 1.0f, true}},
	{eGameSFX::Explosion_Big_Energy,	{"sfx/explosion3.wav", 1.0f, true}},
	{eGameSFX::WeaponFire_Kinetic1,		{"sfx/weaponfire1.wav", 0.7f, true}},
	{eGameSFX::WeaponFire_Kinetic2,		{"sfx/weaponfire2.wav", 0.65f, true}},
	{eGameSFX::WeaponFire_Kinetic3,		{"sfx/weaponfire3.wav", 0.7f, true}},
	{eGameSFX::WeaponFire_Kinetic4,		{"sfx/weaponfire4.wav", 1.0f, true}},
	{eGameSFX::WeaponFire_Ion1,		{"sfx/weaponfire5.wav", 1.0f, true}},
	{eGameSFX::WeaponFire_Ion2,		{"sfx/weaponfire6.wav", 1.0f, true}},
	{eGameSFX::WeaponFire_Ion3,		{"sfx/weaponfire7.wav", 0.7f, true}},
	{eGameSFX::WeaponFire_Plasma1,		{"sfx/weaponfire8.wav", 0.85f, true}},
	{eGameSFX::WeaponFire_Plasma2,		{"sfx/weaponfire9.wav", 0.95f, true}},
	{eGameSFX::WeaponFire_Plasma3,		{"sfx/weaponfire10.wav", 0.9f, true}},
	{eGameSFX::WeaponFire_Maser1,		{"sfx/weaponfire11.wav", 0.6f, true}},
	{eGameSFX::WeaponFire_Maser2,		{"sfx/weaponfire12.wav", 0.55f, true}},
	{eGameSFX::WeaponFire_Antimatter,	{"sfx/weaponfire13.wav", 0.9f, true}},
	{eGameSFX::WeaponFire_Laser,		{"sfx/weaponfire14.wav", 0.8f, true}},
	{eGameSFX::WeaponFire_Gauss,		{"sfx/weaponfire15.wav", 0.8f, true}},
	{eGameSFX::WeaponFire_SmallMissile,	{"sfx/weaponfire16.wav", 1.0f, true}},
	{eGameSFX::WeaponFire_NormalMissile,	{"sfx/weaponfire17.wav", 1.0f, true}},
	{eGameSFX::WeaponFire_Torpedo,		{"sfx/weaponfire18.wav", 1.0f, true}},
	{eGameSFX::WeaponFire_Bomb,		{"sfx/weaponfire19.wav", 1.0f, true}},
	{eGameSFX::Hit_Kinetic,			{"sfx/kinetichit.wav", 1.0f, true}},
	{eGameSFX::Hit_Ion,			{"sfx/ionhit.wav", 1.0f, true}},
	{eGameSFX::Hit_Plasma,			{"sfx/plasmahit.wav", 1.0f, true}},
	{eGameSFX::Hit_Antimatter,		{"sfx/antimaterhit.wav", 1.0f, true}},
	{eGameSFX::Hit_Gauss,			{"sfx/gausshit.wav", 1.0f, true}}
};

const std::unordered_map<eVoicePhrase, sSoundMetadata, sEnumHash> VoiceMap{
	// key					metadata (note, 'en' here, since we use vw_GetText() for file name)
	{eVoicePhrase::Attention,		{"lang/en/voice/Attention.wav", 1.0f, true}},
	{eVoicePhrase::EngineMalfunction,	{"lang/en/voice/EngineMalfunction.wav", 1.0f, true}},
	{eVoicePhrase::MissileDetected,		{"lang/en/voice/MissileDetected.wav", 1.0f, true}},
	{eVoicePhrase::PowerSupplyReestablished,{"lang/en/voice/PowerSupplyReestablished.wav", 1.0f, true}},
	{eVoicePhrase::PrepareForAction,	{"lang/en/voice/PrepareForAction.wav", 1.0f, true}},
	{eVoicePhrase::ReactorMalfunction,	{"lang/en/voice/ReactorMalfunction.wav", 1.0f, true}},
	{eVoicePhrase::Warning,			{"lang/en/voice/Warning.wav", 1.0f, true}},
	{eVoicePhrase::WeaponDamaged,		{"lang/en/voice/WeaponDamaged.wav", 1.0f, true}},
	{eVoicePhrase::WeaponDestroyed,		{"lang/en/voice/WeaponDestroyed.wav", 1.0f, true}},
	{eVoicePhrase::WeaponMalfunction,	{"lang/en/voice/WeaponMalfunction.wav", 1.0f, true}}
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
const std::unordered_map<eMusicTheme, sMusicMetadata, sEnumHash> MusicMap{
	// key			metadata
	{eMusicTheme::MENU,	{"music/menu.ogg", "", 1.0f, false}},
	{eMusicTheme::GAME,	{"music/game.ogg", "", 1.0f, true}},
	{eMusicTheme::BOSS,	{"music/boss-loop.ogg", "", 1.0f, false}},
	{eMusicTheme::FAILED,	{"music/missionfailed.ogg", "", 0.7f, false}},
	{eMusicTheme::CREDITS,	{"music/boss-intro.ogg", "music/boss-loop.ogg", 1.0f, false}}
};
#pragma GCC diagnostic pop

} // unnamed namespace


/*
 * Get all audio assets load value.
 */
unsigned GetAudioAssetsLoadValue()
{
	return (MenuSFXMap.size() * SFXLoadValue +
		GameSFXMap.size() * SFXLoadValue +
		VoiceMap.size() * SFXLoadValue);
}

/*
 * Cycle with function call on each audio asset load.
 */
void ForEachAudioAssetLoad(std::function<void (unsigned AssetValue)> function)
{
	if (!vw_GetAudioStatus())
		return;

	for (auto &tmpAsset : MenuSFXMap) {
		vw_LoadSoundBuffer(tmpAsset.second.FileName);
		function(SFXLoadValue);
	}
	for (auto &tmpAsset : GameSFXMap) {
		vw_LoadSoundBuffer(tmpAsset.second.FileName);
		function(SFXLoadValue);
	}
	for (auto &tmpAsset : VoiceMap) {
		vw_LoadSoundBuffer(vw_GetText(tmpAsset.second.FileName, GameConfig().VoiceLanguage));
		function(SFXLoadValue);
	}
	CurrentLoadedVoiceAssetsLanguage = GameConfig().VoiceLanguage;
}

/*
 * Reload voice assets for new language.
 */
void ReloadVoiceAssets()
{
	for (auto &tmpAsset : VoiceMap) {
		std::string OldFileName{vw_GetText(tmpAsset.second.FileName, CurrentLoadedVoiceAssetsLanguage)};
		std::string NewFileName{vw_GetText(tmpAsset.second.FileName, GameConfig().VoiceLanguage)};
		// we may have situation, when language don't provide voice file and
		// use voice from another language
		if (OldFileName != NewFileName) {
			vw_ReleaseSoundBuffer(OldFileName);
			vw_LoadSoundBuffer(NewFileName);
		}
	}
	CurrentLoadedVoiceAssetsLanguage = GameConfig().VoiceLanguage;
}

/*
 * Play music theme with fade-in and fade-out previous music theme (if need).
 */
void PlayMusicTheme(eMusicTheme MusicTheme, uint32_t FadeInTicks, uint32_t FadeOutTicks)
{
	auto tmpMusic = MusicMap.find(MusicTheme);
	if (tmpMusic == MusicMap.end())
		return;

	if (tmpMusic->second.NeedRelease)
		vw_ReleaseMusic(tmpMusic->second.FileName);

	CurrentPlayingMusicTheme = MusicTheme;

	if (vw_GetAudioStatus() &&
	    GameConfig().MusicVolume) {
		vw_FadeOutAllMusicWithException(tmpMusic->second.FileName, FadeOutTicks, 1.0f, FadeInTicks);

		if (vw_IsMusicPlaying(tmpMusic->second.FileName))
			return;

		if (!vw_PlayMusic(tmpMusic->second.FileName, 0.0f,
				  tmpMusic->second.VolumeCorrection * (GameConfig().MusicVolume / 10.0f),
				  tmpMusic->second.FileNameLoop.empty(), tmpMusic->second.FileNameLoop)) {
			vw_ReleaseMusic(tmpMusic->second.FileName);
			CurrentPlayingMusicTheme = eMusicTheme::NONE;
		} else // we are playing new music theme, fade-in it
			vw_SetMusicFadeIn(tmpMusic->second.FileName, 1.0f, FadeInTicks);
	}
}

/*
 * Change "global" volume for menu sfx (2D).
 */
void ChangeMenuSFXGlobalVolume(float NewGlobalVolume)
{
	for (auto &tmpSFX : MenuSFXMap) {
		vw_SetSoundGlobalVolume(tmpSFX.second.FileName, NewGlobalVolume);
	}
}

/*
 * Change "global" volume for voice.
 */
void ChangeVoiceGlobalVolume(float NewGlobalVolume)
{
	for (auto &tmpVoice : VoiceMap) {
		vw_SetSoundGlobalVolume(vw_GetText(tmpVoice.second.FileName, GameConfig().VoiceLanguage), NewGlobalVolume);
	}
}

/*
 * Play menu sfx (2D).
 */
unsigned int PlayMenuSFX(eMenuSFX MenuSFX, float LocalVolume)
{
	if (!vw_GetAudioStatus() ||
	    !GameConfig().SoundVolume)
		return 0;

	auto tmpSFX = MenuSFXMap.find(MenuSFX);
	if (tmpSFX == MenuSFXMap.end())
		return 0;

	// restart, if already playing this SFX
	int ret = vw_ReplayFirstFoundSound(tmpSFX->second.FileName);
	if (ret)
		return ret;

	return vw_PlaySound(tmpSFX->second.FileName,
			    LocalVolume * tmpSFX->second.VolumeCorrection,
			    GameConfig().SoundVolume / 10.0f, sVECTOR3D{},
			    true, tmpSFX->second.AllowStop, 1);
}

/*
 * Play game sfx (3D).
 */
unsigned int PlayGameSFX(eGameSFX GameSFX, float LocalVolume, const sVECTOR3D &Location, int AtType)
{
	if (!vw_GetAudioStatus() ||
	    !GameConfig().SoundVolume)
		return 0;

	auto tmpSFX = GameSFXMap.find(GameSFX);
	if (tmpSFX == GameSFXMap.end())
		return 0;

	return vw_PlaySound(tmpSFX->second.FileName,
			    LocalVolume * tmpSFX->second.VolumeCorrection,
			    GameConfig().SoundVolume / 10.0f, Location,
			    false, tmpSFX->second.AllowStop, AtType);
}

/*
 * Play voice phrase.
 */
unsigned int PlayVoicePhrase(eVoicePhrase VoicePhrase, float LocalVolume)
{
	if (!vw_GetAudioStatus() ||
	    !GameConfig().VoiceVolume)
		return 0;

	auto tmpVoice = VoiceMap.find(VoicePhrase);
	if (tmpVoice == VoiceMap.end())
		return 0;

	// FIXME revise Russian voice files for proper volume
	if (GameConfig().VoiceLanguage == 2)
		LocalVolume *= 0.6f;

	return vw_PlaySound(vw_GetText(tmpVoice->second.FileName, GameConfig().VoiceLanguage),
			    LocalVolume * tmpVoice->second.VolumeCorrection,
			    GameConfig().VoiceVolume / 10.0f, sVECTOR3D{},
			    true, tmpVoice->second.AllowStop, 1);
}

/*
 * Main audio loop.
 */
void AudioLoop()
{
	sVECTOR3D CurrentCameraLocation;
	vw_GetCameraLocation(&CurrentCameraLocation);
	sVECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);

	sVECTOR3D ListenerOriV1(0.0f, 0.0f, -1.0f);
	vw_RotatePoint(ListenerOriV1, CurrentCameraRotation);
	sVECTOR3D ListenerOriV2(0.0f, 1.0f, 0.0f);
	vw_RotatePoint(ListenerOriV2, CurrentCameraRotation);

	// position of the Listener
	float ListenerPos[3] = {CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z};
	// velocity of the Listener
	float ListenerVel[3] = {0.0f, 0.0f, 0.0f};
	// orientation of the Listener (first "look at", second "up")
	float ListenerOri[6] = {ListenerOriV1.x, ListenerOriV1.y, ListenerOriV1.z,
				ListenerOriV2.x, ListenerOriV2.y, ListenerOriV2.z};

	vw_Listener(ListenerPos, ListenerVel, ListenerOri);

	// update buffers
	vw_UpdateSound(SDL_GetTicks());
	vw_UpdateMusic(SDL_GetTicks());

	if (!vw_IsAnyMusicPlaying()) {
		// start playing music
		if (vw_GetAudioStatus() &&
		    GameConfig().MusicVolume &&
		    (CurrentPlayingMusicTheme != eMusicTheme::NONE)) {
			auto tmpMusic = MusicMap.find(CurrentPlayingMusicTheme);
			if (tmpMusic == MusicMap.end()) {
				CurrentPlayingMusicTheme = eMusicTheme::NONE;
				return;
			}

			if (!vw_PlayMusic(tmpMusic->second.FileName, 0.0f,
					  tmpMusic->second.VolumeCorrection * (GameConfig().MusicVolume / 10.0f),
					  tmpMusic->second.FileNameLoop.empty(), tmpMusic->second.FileNameLoop)) {
				vw_ReleaseMusic(tmpMusic->second.FileName);
				CurrentPlayingMusicTheme = eMusicTheme::NONE;
			} else // we are playing new music theme, FadeIn it
				vw_SetMusicFadeIn(tmpMusic->second.FileName, 1.0f, 2000);
		}
	} else {
		// turn off music, if music volume set to 0
		if (vw_GetAudioStatus() &&
		    !GameConfig().MusicVolume) {
			vw_ReleaseAllMusic();
		}
	}
}
