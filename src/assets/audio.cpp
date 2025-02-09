/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


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

*****************************************************************************/

#include "../core/core.h"
#include "../config/config.h"
#include "audio.h"
#include "SDL2/SDL.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// empirical found "average load value" for one sfx asset
// small value mean that asset loads fast, big value - slow
constexpr unsigned SFXLoadValue{20};

eMusicTheme CurrentPlayingMusicTheme{eMusicTheme::NONE};
unsigned int CurrentLoadedVoiceAssetsLanguage{0}; // English

struct sSoundMetadata {
    std::string FileName{};
    float VolumeCorrection{1.0f};
    bool AllowStop{true}; // allow stop this sfx in vw_StopAllSoundsIfAllowed()

    sSoundMetadata() = delete;
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    sSoundMetadata(const sSoundMetadata &) = default;
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    sSoundMetadata(sSoundMetadata &&) = default;
    void operator = (const sSoundMetadata &) = delete;
    void operator = (sSoundMetadata &&) = delete;

    [[gnu::noinline, clang::noinline, msvc::noinline]]
    explicit sSoundMetadata(const std::string &_FileName,
                            float _VolumeCorrection = 1.0f,
                            bool _AllowStop = true) :
        FileName{_FileName},
        VolumeCorrection{_VolumeCorrection},
        AllowStop{_AllowStop}
    {}

    [[gnu::noinline, clang::noinline, msvc::noinline]]
    ~sSoundMetadata() = default;
};

struct sMusicMetadata {
    std::string FileName{};
    std::string FileNameLoop{};
    float VolumeCorrection{1.0f};
    bool NeedRelease{false}; // if this music theme playing now - release it first

    sMusicMetadata() = delete;
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    sMusicMetadata(const sMusicMetadata &) = default;
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    sMusicMetadata(sMusicMetadata &&) = default;
    void operator = (const sMusicMetadata &) = delete;
    void operator = (sMusicMetadata &&) = delete;

    [[gnu::noinline, clang::noinline, msvc::noinline]]
    explicit sMusicMetadata(const std::string &_FileName,
                            const std::string &_FileNameLoop = std::string{},
                            float _VolumeCorrection = 1.0f,
                            bool _NeedRelease = false) :
        FileName{_FileName},
        FileNameLoop{_FileNameLoop},
        VolumeCorrection{_VolumeCorrection},
        NeedRelease{_NeedRelease}
    {}

    [[gnu::noinline, clang::noinline, msvc::noinline]]
    ~sMusicMetadata() = default;
};

const std::unordered_map<eMenuSFX, sSoundMetadata, sEnumHash> MenuSFXMap{
    // key                              metadata
    {eMenuSFX::OverSmallButton,         sSoundMetadata{"sfx/menu_onbutton2.wav", 0.15f, false}},
    {eMenuSFX::OverBigButton,           sSoundMetadata{"sfx/menu_onbutton.wav", 0.4f, false}},
    {eMenuSFX::Click,                   sSoundMetadata{"sfx/menu_click.wav", 0.6f, false}},
    {eMenuSFX::SwitchToAnotherMenu,     sSoundMetadata{"sfx/menu_new.wav"}},
    {eMenuSFX::TapingClick,             sSoundMetadata{"sfx/menu_taping.wav", 0.8f}},
    {eMenuSFX::OverLine,                sSoundMetadata{"sfx/menu_online.wav", 0.75f}},
    {eMenuSFX::SelectLine,              sSoundMetadata{"sfx/menu_selectline.wav"}},
    {eMenuSFX::CanNotClick,             sSoundMetadata{"sfx/menu_nclick.wav", 1.0f, false}},
    {eMenuSFX::DragError,               sSoundMetadata{"sfx/drag_error.wav"}},
    {eMenuSFX::DragUninstallFromSlot,   sSoundMetadata{"sfx/drag_offslot.wav", 0.65f}},
    {eMenuSFX::DragInstallToSlot,       sSoundMetadata{"sfx/drag_onslot.wav", 0.82f}},
    {eMenuSFX::DragRelease,             sSoundMetadata{"sfx/drag_release.wav", 0.6f}},
    {eMenuSFX::MissionShowMenu,         sSoundMetadata{"sfx/game_showmenu.wav", 1.0f, false}},
    {eMenuSFX::MissionHideMenu,         sSoundMetadata{"sfx/game_hidemenu.wav", 1.0f, false}},
    {eMenuSFX::WarningLowLife,          sSoundMetadata{"sfx/lowlife.wav"}}
};

const std::unordered_map<eGameSFX, sSoundMetadata, sEnumHash> GameSFXMap{
    // key                                  metadata
    {eGameSFX::WeaponMalfunction_Kinetic,   sSoundMetadata{"sfx/weapon1probl.wav"}},
    {eGameSFX::WeaponMalfunction_Particle,  sSoundMetadata{"sfx/weapon2probl.wav"}},
    {eGameSFX::WeaponMalfunction_Beam,      sSoundMetadata{"sfx/weapon3probl.wav"}},
    {eGameSFX::WeaponMalfunction_Energy,    sSoundMetadata{"sfx/weapon4probl.wav"}},
    {eGameSFX::WeaponMalfunction_Launcher,  sSoundMetadata{"sfx/weapon5probl.wav"}},
    {eGameSFX::Explosion_Small,             sSoundMetadata{"sfx/explosion4.wav"}},
    {eGameSFX::Explosion_Medium,            sSoundMetadata{"sfx/explosion1.wav"}},
    {eGameSFX::Explosion_Big,               sSoundMetadata{"sfx/explosion2.wav"}},
    {eGameSFX::Explosion_Big_Energy,        sSoundMetadata{"sfx/explosion3.wav"}},
    {eGameSFX::WeaponFire_Kinetic1,         sSoundMetadata{"sfx/weaponfire1.wav", 0.7f}},
    {eGameSFX::WeaponFire_Kinetic2,         sSoundMetadata{"sfx/weaponfire2.wav", 0.65f}},
    {eGameSFX::WeaponFire_Kinetic3,         sSoundMetadata{"sfx/weaponfire3.wav", 0.7f}},
    {eGameSFX::WeaponFire_Kinetic4,         sSoundMetadata{"sfx/weaponfire4.wav"}},
    {eGameSFX::WeaponFire_Ion1,             sSoundMetadata{"sfx/weaponfire5.wav"}},
    {eGameSFX::WeaponFire_Ion2,             sSoundMetadata{"sfx/weaponfire6.wav"}},
    {eGameSFX::WeaponFire_Ion3,             sSoundMetadata{"sfx/weaponfire7.wav", 0.7f}},
    {eGameSFX::WeaponFire_Plasma1,          sSoundMetadata{"sfx/weaponfire8.wav", 0.85f}},
    {eGameSFX::WeaponFire_Plasma2,          sSoundMetadata{"sfx/weaponfire9.wav", 0.95f}},
    {eGameSFX::WeaponFire_Plasma3,          sSoundMetadata{"sfx/weaponfire10.wav", 0.9f}},
    {eGameSFX::WeaponFire_Maser1,           sSoundMetadata{"sfx/weaponfire11.wav", 0.6f}},
    {eGameSFX::WeaponFire_Maser2,           sSoundMetadata{"sfx/weaponfire12.wav", 0.55f}},
    {eGameSFX::WeaponFire_Antimatter,       sSoundMetadata{"sfx/weaponfire13.wav", 0.9f}},
    {eGameSFX::WeaponFire_Laser,            sSoundMetadata{"sfx/weaponfire14.wav", 0.8f}},
    {eGameSFX::WeaponFire_Gauss,            sSoundMetadata{"sfx/weaponfire15.wav", 0.8f}},
    {eGameSFX::WeaponFire_SmallMissile,     sSoundMetadata{"sfx/weaponfire16.wav"}},
    {eGameSFX::WeaponFire_NormalMissile,    sSoundMetadata{"sfx/weaponfire17.wav"}},
    {eGameSFX::WeaponFire_Torpedo,          sSoundMetadata{"sfx/weaponfire18.wav"}},
    {eGameSFX::WeaponFire_Bomb,             sSoundMetadata{"sfx/weaponfire19.wav"}},
    {eGameSFX::Hit_Kinetic,                 sSoundMetadata{"sfx/kinetichit.wav"}},
    {eGameSFX::Hit_Ion,                     sSoundMetadata{"sfx/ionhit.wav"}},
    {eGameSFX::Hit_Plasma,                  sSoundMetadata{"sfx/plasmahit.wav"}},
    {eGameSFX::Hit_Antimatter,              sSoundMetadata{"sfx/antimaterhit.wav"}},
    {eGameSFX::Hit_Gauss,                   sSoundMetadata{"sfx/gausshit.wav"}}
};

const std::unordered_map<eVoicePhrase, sSoundMetadata, sEnumHash> VoiceMap{
    // key                                  metadata (note, 'en' here, since we use vw_GetText() for file name)
    {eVoicePhrase::Attention,               sSoundMetadata{"lang/en/voice/Attention.wav"}},
    {eVoicePhrase::EngineMalfunction,       sSoundMetadata{"lang/en/voice/EngineMalfunction.wav"}},
    {eVoicePhrase::MissileDetected,         sSoundMetadata{"lang/en/voice/MissileDetected.wav"}},
    {eVoicePhrase::PowerSupplyReestablished,sSoundMetadata{"lang/en/voice/PowerSupplyReestablished.wav"}},
    {eVoicePhrase::PrepareForAction,        sSoundMetadata{"lang/en/voice/PrepareForAction.wav"}},
    {eVoicePhrase::ReactorMalfunction,      sSoundMetadata{"lang/en/voice/ReactorMalfunction.wav"}},
    {eVoicePhrase::Warning,                 sSoundMetadata{"lang/en/voice/Warning.wav"}},
    {eVoicePhrase::WeaponDamaged,           sSoundMetadata{"lang/en/voice/WeaponDamaged.wav"}},
    {eVoicePhrase::WeaponDestroyed,         sSoundMetadata{"lang/en/voice/WeaponDestroyed.wav"}},
    {eVoicePhrase::WeaponMalfunction,       sSoundMetadata{"lang/en/voice/WeaponMalfunction.wav"}}
};

const std::unordered_map<eMusicTheme, sMusicMetadata, sEnumHash> MusicMap{
    // key                  metadata
    {eMusicTheme::MENU,     sMusicMetadata{"music/menu.ogg"}},
    {eMusicTheme::GAME,     sMusicMetadata{"music/game.ogg", "", 1.0f, true}},
    {eMusicTheme::BOSS,     sMusicMetadata{"music/boss-loop.ogg"}},
    {eMusicTheme::FAILED,   sMusicMetadata{"music/missionfailed.ogg", "", 0.7f}},
    {eMusicTheme::CREDITS,  sMusicMetadata{"music/boss-intro.ogg", "music/boss-loop.ogg"}}
};

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
 * Cycle with function callback on each audio asset load.
 */
void ForEachAudioAssetLoad(std::function<void (unsigned AssetValue)> function)
{
    if (!vw_GetAudioStatus()) {
        return;
    }

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
    if (tmpMusic == MusicMap.end()) {
        return;
    }

    if (tmpMusic->second.NeedRelease) {
        vw_ReleaseMusic(tmpMusic->second.FileName);
    }

    CurrentPlayingMusicTheme = MusicTheme;

    if (vw_GetAudioStatus() && GameConfig().MusicVolume) {
        vw_FadeOutAllMusicWithException(tmpMusic->second.FileName, FadeOutTicks, 1.0f, FadeInTicks);

        if (vw_IsMusicPlaying(tmpMusic->second.FileName)) {
            return;
        }

        if (!vw_PlayMusic(tmpMusic->second.FileName, 0.0f,
                          tmpMusic->second.VolumeCorrection * (GameConfig().MusicVolume / 10.0f),
                          tmpMusic->second.FileNameLoop.empty(), tmpMusic->second.FileNameLoop)) {
            vw_ReleaseMusic(tmpMusic->second.FileName);
            CurrentPlayingMusicTheme = eMusicTheme::NONE;
        } else { // we are playing new music theme, fade-in it
            vw_SetMusicFadeIn(tmpMusic->second.FileName, 1.0f, FadeInTicks);
        }
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
    if (!vw_GetAudioStatus() || !GameConfig().SoundVolume) {
        return 0;
    }

    auto tmpSFX = MenuSFXMap.find(MenuSFX);
    if (tmpSFX == MenuSFXMap.end()) {
        return 0;
    }

    // restart, if already playing this SFX
    int ret = vw_ReplayFirstFoundSound(tmpSFX->second.FileName);
    if (ret) {
        return ret;
    }

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
    if (!vw_GetAudioStatus() || !GameConfig().SoundVolume) {
        return 0;
    }

    auto tmpSFX = GameSFXMap.find(GameSFX);
    if (tmpSFX == GameSFXMap.end()) {
        return 0;
    }

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
    if (!vw_GetAudioStatus() || !GameConfig().VoiceVolume) {
        return 0;
    }

    auto tmpVoice = VoiceMap.find(VoicePhrase);
    if (tmpVoice == VoiceMap.end()) {
        return 0;
    }

    // FIXME revise Russian voice files for proper volume
    if (GameConfig().VoiceLanguage == 2) {
        LocalVolume *= 0.6f;
    }

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
    // update buffers
    vw_UpdateSound(SDL_GetTicks());
    vw_UpdateMusic(SDL_GetTicks());

    if (!vw_IsAnyMusicPlaying()) {
        // start playing music
        if (vw_GetAudioStatus() && GameConfig().MusicVolume && CurrentPlayingMusicTheme != eMusicTheme::NONE) {
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
            } else { // we are playing new music theme, FadeIn it
                vw_SetMusicFadeIn(tmpMusic->second.FileName, 1.0f, 2000);
            }
        }
    } else {
        // turn off music, if music volume set to 0
        if (vw_GetAudioStatus() && !GameConfig().MusicVolume) {
            vw_ReleaseAllMusic();
        }
    }
}

} // astromenace namespace
} // viewizard namespace
