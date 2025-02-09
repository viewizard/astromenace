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

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// empirical found "everage load value" for one texture asset
// small value mean that asset loads fast, big value - slow
constexpr unsigned TextureLoadValue{200};

struct sTextureAsset {
    std::string TextureFile{};
    // alpha channel related
    const bool Alpha{false};
    const eAlphaCreateMode AlphaMode{eAlphaCreateMode::EQUAL};
    // filtering related
    const eTextureWrapMode TextWrap{eTextureWrapMode::CLAMP_TO_EDGE};
    const eTextureBasicFilter TextFilter{eTextureBasicFilter::BILINEAR};
    const bool NeedAnisotropy{false};
    const bool MipMap{false};
    // preload
    GLtexture PreloadedTexture{0};

    sTextureAsset() = delete;
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    sTextureAsset(const sTextureAsset &) = default;
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    sTextureAsset(sTextureAsset &&) = default;
    void operator = (const sTextureAsset &) = delete;
    void operator = (sTextureAsset &&) = delete;

    [[gnu::noinline, clang::noinline, msvc::noinline]]
    explicit sTextureAsset(const std::string &_TextureFile,
                           bool _Alpha = false,
                           eAlphaCreateMode _AlphaMode = eAlphaCreateMode::EQUAL,
                           eTextureWrapMode _TextWrap = eTextureWrapMode::CLAMP_TO_EDGE,
                           eTextureBasicFilter _TextFilter = eTextureBasicFilter::BILINEAR,
                           bool _NeedAnisotropy = false,
                           bool _MipMap = false) :
        TextureFile{_TextureFile},
        Alpha{_Alpha},
        AlphaMode{_AlphaMode},
        TextWrap{_TextWrap},
        TextFilter{_TextFilter},
        NeedAnisotropy{_NeedAnisotropy},
        MipMap{_MipMap}
    {}

    [[gnu::noinline, clang::noinline, msvc::noinline]]
    ~sTextureAsset() = default;
};

std::unordered_map<unsigned, sTextureAsset> TextureMap{
    {constexpr_hash_djb2a("menu/astromenace.tga"),        sTextureAsset{"menu/astromenace.tga", true}},
    {constexpr_hash_djb2a("menu/button384_back.tga"),     sTextureAsset{"menu/button384_back.tga", true}},
    {constexpr_hash_djb2a("menu/button384_in.tga"),       sTextureAsset{"menu/button384_in.tga", true}},
    {constexpr_hash_djb2a("menu/button384_out.tga"),      sTextureAsset{"menu/button384_out.tga", true}},
    {constexpr_hash_djb2a("menu/button256_back.tga"),     sTextureAsset{"menu/button256_back.tga", true}},
    {constexpr_hash_djb2a("menu/button256_in.tga"),       sTextureAsset{"menu/button256_in.tga", true}},
    {constexpr_hash_djb2a("menu/button256_out.tga"),      sTextureAsset{"menu/button256_out.tga", true}},
    {constexpr_hash_djb2a("menu/button256_off.tga"),      sTextureAsset{"menu/button256_off.tga", true}},
    {constexpr_hash_djb2a("menu/blackpoint.tga"),         sTextureAsset{"menu/blackpoint.tga"}},
    {constexpr_hash_djb2a("menu/whitepoint.tga"),         sTextureAsset{"menu/whitepoint.tga"}},
    {constexpr_hash_djb2a("menu/line.tga"),               sTextureAsset{"menu/line.tga", true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT}},
    {constexpr_hash_djb2a("menu/perc.tga"),               sTextureAsset{"menu/perc.tga", true}},
    {constexpr_hash_djb2a("menu/perc_none.tga"),          sTextureAsset{"menu/perc_none.tga", true}},
    {constexpr_hash_djb2a("menu/checkbox_main.tga"),      sTextureAsset{"menu/checkbox_main.tga", true}},
    {constexpr_hash_djb2a("menu/checkbox_in.tga"),        sTextureAsset{"menu/checkbox_in.tga", true}},
    {constexpr_hash_djb2a("menu/workshop_panel1.tga"),    sTextureAsset{"menu/workshop_panel1.tga", true}},
    {constexpr_hash_djb2a("menu/workshop_panel1+.tga"),   sTextureAsset{"menu/workshop_panel1+.tga", true}},
    {constexpr_hash_djb2a("menu/workshop_panel2.tga"),    sTextureAsset{"menu/workshop_panel2.tga", true}},
    {constexpr_hash_djb2a("menu/workshop_panel2+.tga"),   sTextureAsset{"menu/workshop_panel2+.tga", true}},
    {constexpr_hash_djb2a("menu/workshop_panel3.tga"),    sTextureAsset{"menu/workshop_panel3.tga", true}},
    {constexpr_hash_djb2a("menu/workshop_panel4.tga"),    sTextureAsset{"menu/workshop_panel4.tga", true}},
    {constexpr_hash_djb2a("menu/workshop_panel5.tga"),    sTextureAsset{"menu/workshop_panel5.tga", true}},
    {constexpr_hash_djb2a("menu/ammo.tga"),               sTextureAsset{"menu/ammo.tga", true}},
    {constexpr_hash_djb2a("lang/en/menu/button_weaponry_out.tga"),    sTextureAsset{"lang/en/menu/button_weaponry_out.tga"}},
    {constexpr_hash_djb2a("lang/en/menu/button_weaponry_in.tga"),     sTextureAsset{"lang/en/menu/button_weaponry_in.tga"}},
    {constexpr_hash_djb2a("lang/de/menu/button_weaponry_out.tga"),    sTextureAsset{"lang/de/menu/button_weaponry_out.tga"}},
    {constexpr_hash_djb2a("lang/de/menu/button_weaponry_in.tga"),     sTextureAsset{"lang/de/menu/button_weaponry_in.tga"}},
    {constexpr_hash_djb2a("lang/ru/menu/button_weaponry_out.tga"),    sTextureAsset{"lang/ru/menu/button_weaponry_out.tga"}},
    {constexpr_hash_djb2a("lang/ru/menu/button_weaponry_in.tga"),     sTextureAsset{"lang/ru/menu/button_weaponry_in.tga"}},
    {constexpr_hash_djb2a("lang/pl/menu/button_weaponry_out.tga"),    sTextureAsset{"lang/pl/menu/button_weaponry_out.tga"}},
    {constexpr_hash_djb2a("lang/pl/menu/button_weaponry_in.tga"),     sTextureAsset{"lang/pl/menu/button_weaponry_in.tga"}},
    {constexpr_hash_djb2a("menu/back_spot.tga"),              sTextureAsset{"menu/back_spot.tga", true}},
    {constexpr_hash_djb2a("menu/back_spot2.tga"),             sTextureAsset{"menu/back_spot2.tga", true}},
    {constexpr_hash_djb2a("menu/dialog512_256.tga"),          sTextureAsset{"menu/dialog512_256.tga", true}},
    {constexpr_hash_djb2a("menu/dialog512_512.tga"),          sTextureAsset{"menu/dialog512_512.tga", true}},
    {constexpr_hash_djb2a("menu/dialog768_600.tga"),          sTextureAsset{"menu/dialog768_600.tga", true}},
    {constexpr_hash_djb2a("menu/panel444_333_back.tga"),      sTextureAsset{"menu/panel444_333_back.tga", true}},
    {constexpr_hash_djb2a("menu/panel444_333_border.tga"),    sTextureAsset{"menu/panel444_333_border.tga", true}},
    {constexpr_hash_djb2a("menu/panel800_444_back.tga"),      sTextureAsset{"menu/panel800_444_back.tga", true}},
    {constexpr_hash_djb2a("menu/button_dialog200_out.tga"),   sTextureAsset{"menu/button_dialog200_out.tga", true}},
    {constexpr_hash_djb2a("menu/button_dialog200_in.tga"),    sTextureAsset{"menu/button_dialog200_in.tga", true}},
    {constexpr_hash_djb2a("menu/button_dialog200_off.tga"),   sTextureAsset{"menu/button_dialog200_off.tga", true}},
    {constexpr_hash_djb2a("menu/button_dialog128_out.tga"),   sTextureAsset{"menu/button_dialog128_out.tga", true}},
    {constexpr_hash_djb2a("menu/button_dialog128_in.tga"),    sTextureAsset{"menu/button_dialog128_in.tga", true}},
    {constexpr_hash_djb2a("menu/button_dialog128_off.tga"),   sTextureAsset{"menu/button_dialog128_off.tga", true}},
    {constexpr_hash_djb2a("menu/arrow_list_up.tga"),          sTextureAsset{"menu/arrow_list_up.tga", true}},
    {constexpr_hash_djb2a("menu/arrow_list_down.tga"),        sTextureAsset{"menu/arrow_list_down.tga", true}},
    {constexpr_hash_djb2a("script/mission1_icon.tga"),        sTextureAsset{"script/mission1_icon.tga"}},
    {constexpr_hash_djb2a("script/mission2_icon.tga"),        sTextureAsset{"script/mission2_icon.tga"}},
    {constexpr_hash_djb2a("script/mission3_icon.tga"),        sTextureAsset{"script/mission3_icon.tga"}},
    {constexpr_hash_djb2a("script/mission4_icon.tga"),        sTextureAsset{"script/mission4_icon.tga"}},
    {constexpr_hash_djb2a("script/mission5_icon.tga"),        sTextureAsset{"script/mission5_icon.tga"}},
    {constexpr_hash_djb2a("script/mission6_icon.tga"),        sTextureAsset{"script/mission6_icon.tga"}},
    {constexpr_hash_djb2a("script/mission7_icon.tga"),        sTextureAsset{"script/mission7_icon.tga"}},
    {constexpr_hash_djb2a("script/mission8_icon.tga"),        sTextureAsset{"script/mission8_icon.tga"}},
    {constexpr_hash_djb2a("script/mission9_icon.tga"),        sTextureAsset{"script/mission9_icon.tga"}},
    {constexpr_hash_djb2a("script/mission10_icon.tga"),       sTextureAsset{"script/mission10_icon.tga"}},
    {constexpr_hash_djb2a("script/mission11_icon.tga"),       sTextureAsset{"script/mission11_icon.tga"}},
    {constexpr_hash_djb2a("script/mission12_icon.tga"),       sTextureAsset{"script/mission12_icon.tga"}},
    {constexpr_hash_djb2a("script/mission13_icon.tga"),       sTextureAsset{"script/mission13_icon.tga"}},
    {constexpr_hash_djb2a("script/mission14_icon.tga"),       sTextureAsset{"script/mission14_icon.tga"}},
    {constexpr_hash_djb2a("script/mission15_icon.tga"),       sTextureAsset{"script/mission15_icon.tga"}},
    {constexpr_hash_djb2a("menu/system_engine1.tga"),         sTextureAsset{"menu/system_engine1.tga", true}},
    {constexpr_hash_djb2a("menu/system_engine2.tga"),         sTextureAsset{"menu/system_engine2.tga", true}},
    {constexpr_hash_djb2a("menu/system_engine3.tga"),         sTextureAsset{"menu/system_engine3.tga", true}},
    {constexpr_hash_djb2a("menu/system_engine4.tga"),         sTextureAsset{"menu/system_engine4.tga", true}},
    {constexpr_hash_djb2a("menu/system_mechan1.tga"),         sTextureAsset{"menu/system_mechan1.tga", true}},
    {constexpr_hash_djb2a("menu/system_mechan2.tga"),         sTextureAsset{"menu/system_mechan2.tga", true}},
    {constexpr_hash_djb2a("menu/system_mechan3.tga"),         sTextureAsset{"menu/system_mechan3.tga", true}},
    {constexpr_hash_djb2a("menu/system_mechan4.tga"),         sTextureAsset{"menu/system_mechan4.tga", true}},
    {constexpr_hash_djb2a("menu/system_power1.tga"),          sTextureAsset{"menu/system_power1.tga", true}},
    {constexpr_hash_djb2a("menu/system_power2.tga"),          sTextureAsset{"menu/system_power2.tga", true}},
    {constexpr_hash_djb2a("menu/system_power3.tga"),          sTextureAsset{"menu/system_power3.tga", true}},
    {constexpr_hash_djb2a("menu/system_power4.tga"),          sTextureAsset{"menu/system_power4.tga", true}},
    {constexpr_hash_djb2a("menu/system_protect1.tga"),        sTextureAsset{"menu/system_protect1.tga", true}},
    {constexpr_hash_djb2a("menu/system_protect2.tga"),        sTextureAsset{"menu/system_protect2.tga", true}},
    {constexpr_hash_djb2a("menu/system_protect3.tga"),        sTextureAsset{"menu/system_protect3.tga", true}},
    {constexpr_hash_djb2a("menu/system_protect4.tga"),        sTextureAsset{"menu/system_protect4.tga", true}},
    {constexpr_hash_djb2a("menu/system_target1.tga"),         sTextureAsset{"menu/system_target1.tga", true}},
    {constexpr_hash_djb2a("menu/system_target2.tga"),         sTextureAsset{"menu/system_target2.tga", true}},
    {constexpr_hash_djb2a("menu/system_target3.tga"),         sTextureAsset{"menu/system_target3.tga", true}},
    {constexpr_hash_djb2a("menu/system_target4.tga"),         sTextureAsset{"menu/system_target4.tga", true}},
    {constexpr_hash_djb2a("menu/system_empty.tga"),           sTextureAsset{"menu/system_empty.tga", true}},
    {constexpr_hash_djb2a("menu/weapon1_icon.tga"),           sTextureAsset{"menu/weapon1_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon2_icon.tga"),           sTextureAsset{"menu/weapon2_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon3_icon.tga"),           sTextureAsset{"menu/weapon3_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon4_icon.tga"),           sTextureAsset{"menu/weapon4_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon5_icon.tga"),           sTextureAsset{"menu/weapon5_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon6_icon.tga"),           sTextureAsset{"menu/weapon6_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon7_icon.tga"),           sTextureAsset{"menu/weapon7_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon8_icon.tga"),           sTextureAsset{"menu/weapon8_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon9_icon.tga"),           sTextureAsset{"menu/weapon9_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon10_icon.tga"),          sTextureAsset{"menu/weapon10_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon11_icon.tga"),          sTextureAsset{"menu/weapon11_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon12_icon.tga"),          sTextureAsset{"menu/weapon12_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon13_icon.tga"),          sTextureAsset{"menu/weapon13_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon14_icon.tga"),          sTextureAsset{"menu/weapon14_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon15_icon.tga"),          sTextureAsset{"menu/weapon15_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon16_icon.tga"),          sTextureAsset{"menu/weapon16_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon17_icon.tga"),          sTextureAsset{"menu/weapon17_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon18_icon.tga"),          sTextureAsset{"menu/weapon18_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon19_icon.tga"),          sTextureAsset{"menu/weapon19_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon_on_icon.tga"),         sTextureAsset{"menu/weapon_on_icon.tga", true}},
    {constexpr_hash_djb2a("menu/weapon_empty_icon.tga"),      sTextureAsset{"menu/weapon_empty_icon.tga", true}},
    {constexpr_hash_djb2a("menu/cursor.tga"),                 sTextureAsset{"menu/cursor.tga", true}},
    {constexpr_hash_djb2a("menu/cursor_shadow.tga"),          sTextureAsset{"menu/cursor_shadow.tga", true}},
    {constexpr_hash_djb2a("gfx/flare.tga"),                   sTextureAsset{"gfx/flare.tga", true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/flare1.tga"),                  sTextureAsset{"gfx/flare1.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/flare2.tga"),                  sTextureAsset{"gfx/flare2.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/flare3.tga"),                  sTextureAsset{"gfx/flare3.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/trail1.tga"),                  sTextureAsset{"gfx/trail1.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/trail2.tga"),                  sTextureAsset{"gfx/trail2.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/trail3.tga"),                  sTextureAsset{"gfx/trail3.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/trail4.tga"),                  sTextureAsset{"gfx/trail4.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("gfx/trail5.tga"),                  sTextureAsset{"gfx/trail5.tga", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("game/nums.tga"),                   sTextureAsset{"game/nums.tga", true}},
    {constexpr_hash_djb2a("game/ammo.tga"),                   sTextureAsset{"game/ammo.tga", true}},
    {constexpr_hash_djb2a("game/energy.tga"),                 sTextureAsset{"game/energy.tga", true}},
    {constexpr_hash_djb2a("game/game_panel.tga"),             sTextureAsset{"game/game_panel.tga", true}},
    {constexpr_hash_djb2a("game/game_panel2.tga"),            sTextureAsset{"game/game_panel2.tga", true}},
    {constexpr_hash_djb2a("game/game_panel_el.tga"),          sTextureAsset{"game/game_panel_el.tga", true}},
    {constexpr_hash_djb2a("game/game_num.tga"),               sTextureAsset{"game/game_num.tga", true}},
    {constexpr_hash_djb2a("game/weapon_panel_left.tga"),      sTextureAsset{"game/weapon_panel_left.tga", true}},
    {constexpr_hash_djb2a("game/weapon_panel_right.tga"),     sTextureAsset{"game/weapon_panel_right.tga", true}},
    {constexpr_hash_djb2a("game/weapon_ammo.tga"),            sTextureAsset{"game/weapon_ammo.tga", true}},
    {constexpr_hash_djb2a("game/weapon_energy.tga"),          sTextureAsset{"game/weapon_energy.tga", true}},
    {constexpr_hash_djb2a("lang/en/game/mission.tga"),        sTextureAsset{"lang/en/game/mission.tga", true}},
    {constexpr_hash_djb2a("lang/en/game/missionfailed.tga"),  sTextureAsset{"lang/en/game/missionfailed.tga", true}},
    {constexpr_hash_djb2a("lang/en/game/pause.tga"),          sTextureAsset{"lang/en/game/pause.tga", true}},
    {constexpr_hash_djb2a("lang/de/game/mission.tga"),        sTextureAsset{"lang/de/game/mission.tga", true}},
    {constexpr_hash_djb2a("lang/de/game/missionfailed.tga"),  sTextureAsset{"lang/de/game/missionfailed.tga", true}},
    {constexpr_hash_djb2a("lang/de/game/pause.tga"),          sTextureAsset{"lang/de/game/pause.tga", true}},
    {constexpr_hash_djb2a("lang/ru/game/mission.tga"),        sTextureAsset{"lang/ru/game/mission.tga", true}},
    {constexpr_hash_djb2a("lang/ru/game/missionfailed.tga"),  sTextureAsset{"lang/ru/game/missionfailed.tga", true}},
    {constexpr_hash_djb2a("lang/ru/game/pause.tga"),          sTextureAsset{"lang/ru/game/pause.tga", true}},
    {constexpr_hash_djb2a("skybox/tile_back.tga"),            sTextureAsset{"skybox/tile_back.tga", true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("skybox/tile_stars.tga"),           sTextureAsset{"skybox/tile_stars.tga", true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text00.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text00.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text04.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text04.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text05.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text05.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text06.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text06.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text07.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text07.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text08.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text08.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text09.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text09.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-text10.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-text10.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-illum01.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-illum01.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-illum02.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-illum02.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-illum03.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-illum03.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/sf-illum04.vw2d"),
                                        sTextureAsset{"models/earthfighter/sf-illum04.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/lnch12.tga"),
                                        sTextureAsset{"models/earthfighter/lnch12.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/lnch34.tga"),
                                        sTextureAsset{"models/earthfighter/lnch34.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/earthfighter/rockets.tga"),
                                        sTextureAsset{"models/earthfighter/rockets.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine1.tga"),           sTextureAsset{"models/mine/mine1.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine1i.tga"),          sTextureAsset{"models/mine/mine1i.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine2.tga"),           sTextureAsset{"models/mine/mine2.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine2i.tga"),          sTextureAsset{"models/mine/mine2i.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine3.tga"),           sTextureAsset{"models/mine/mine3.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine3i.tga"),          sTextureAsset{"models/mine/mine3i.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine4.tga"),           sTextureAsset{"models/mine/mine4.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/mine/mine4i.tga"),          sTextureAsset{"models/mine/mine4i.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienfighter/al-text04.vw2d"),
                                        sTextureAsset{"models/alienfighter/al-text04.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienfighter/al-illum04.vw2d"),
                                        sTextureAsset{"models/alienfighter/al-illum04.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/gr-01.vw2d"),               sTextureAsset{"models/gr-01.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/gr-02.vw2d"),               sTextureAsset{"models/gr-02.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/gr-03.vw2d"),               sTextureAsset{"models/gr-03.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/gr-04.vw2d"),               sTextureAsset{"models/gr-04.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/bomber_nm.tga"),
                                        sTextureAsset{"models/normalmap/bomber_nm.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/gr-05.vw2d"),               sTextureAsset{"models/gr-05.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/gr-06.vw2d"),               sTextureAsset{"models/gr-06.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/gr-07.vw2d"),               sTextureAsset{"models/gr-07.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/track.vw2d"),               sTextureAsset{"models/track.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/turret/turrets.tga"),       sTextureAsset{"models/turret/turrets.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/spacebase/allalpha.tga"),
                                        sTextureAsset{"models/spacebase/allalpha.tga", true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/spacebase/metal.tga"),      sTextureAsset{"models/spacebase/metal.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/space/asteroid-01.tga"),
                                        sTextureAsset{"models/space/asteroid-01.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/planet/asteroid.tga"),      sTextureAsset{"models/planet/asteroid.tga", true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/planet/clouds.tga"),        sTextureAsset{"models/planet/clouds.tga", true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/planet/q_class2.tga"),      sTextureAsset{"models/planet/q_class2.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/planet/a_class4.tga"),      sTextureAsset{"models/planet/a_class4.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/planet/m_class7.tga"),      sTextureAsset{"models/planet/m_class7.tga",false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/planet/d_class3.tga"),      sTextureAsset{"models/planet/d_class3.tga",false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/m_class7_nm.tga"),
                                        sTextureAsset{"models/normalmap/m_class7_nm.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/d_class3_nm.tga"),
                                        sTextureAsset{"models/normalmap/d_class3_nm.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/a_class4_nm.tga"),
                                        sTextureAsset{"models/normalmap/a_class4_nm.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/planet_asteroids_nm.tga"),
                                        sTextureAsset{"models/normalmap/planet_asteroids_nm.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/q_class2_nm.tga"),
                                        sTextureAsset{"models/normalmap/q_class2_nm.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-text02.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-text02.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-text03.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-text03.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-text04.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-text04.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-text08.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-text08.vw2d", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-illum02.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-illum02.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-illum03.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-illum03.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-illum04.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-illum04.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/alienmothership/alm-illum08.vw2d"),
                                        sTextureAsset{"models/alienmothership/alm-illum08.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/alien_mothership_nm.tga"),
                                        sTextureAsset{"models/normalmap/alien_mothership_nm.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/building/bld.vw2d"),        sTextureAsset{"models/building/bld.vw2d", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
                                                              eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/normalmap/buildings_nm.tga"),
                                        sTextureAsset{"models/normalmap/buildings_nm.tga", false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("models/building/bld_illum.vw2d"),
                                        sTextureAsset{"models/building/bld_illum.vw2d", true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
                                                      eTextureBasicFilter::TRILINEAR, true, true}},
    {constexpr_hash_djb2a("skybox/1/skybox_back6.tga"),       sTextureAsset{"skybox/1/skybox_back6.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/1/skybox_bottom4.tga"),     sTextureAsset{"skybox/1/skybox_bottom4.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/1/skybox_front5.tga"),      sTextureAsset{"skybox/1/skybox_front5.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/1/skybox_left2.tga"),       sTextureAsset{"skybox/1/skybox_left2.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/1/skybox_right1.tga"),      sTextureAsset{"skybox/1/skybox_right1.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/1/skybox_top3.tga"),        sTextureAsset{"skybox/1/skybox_top3.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/2/skybox_back6.tga"),       sTextureAsset{"skybox/2/skybox_back6.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/2/skybox_bottom4.tga"),     sTextureAsset{"skybox/2/skybox_bottom4.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/2/skybox_front5.tga"),      sTextureAsset{"skybox/2/skybox_front5.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/2/skybox_left2.tga"),       sTextureAsset{"skybox/2/skybox_left2.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/2/skybox_right1.tga"),      sTextureAsset{"skybox/2/skybox_right1.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
    {constexpr_hash_djb2a("skybox/2/skybox_top3.tga"),        sTextureAsset{"skybox/2/skybox_top3.tga", false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}}
};

} // unnamed namespace


/*
 * Get all texture assets load value.
 */
unsigned GetTextureAssetsLoadValue()
{
    return TextureMap.size() * TextureLoadValue;
}

/*
 * Cycle with function callback on each texture asset load.
 */
void ForEachTextureAssetLoad(std::function<void (unsigned AssetValue)> function)
{
    vw_SetTextureAlpha(0, 0, 0);

#ifndef NDEBUG
    {
        std::unordered_set<unsigned> CollisionTest;
        for (auto &tmpAsset : TextureMap) {
            unsigned tmpHash = constexpr_hash_djb2a(tmpAsset.second.TextureFile.c_str());

            // hash collision detected? change file name in order to prevent hash collision
            assert(CollisionTest.find(tmpHash) == CollisionTest.end());
            CollisionTest.emplace(tmpHash);
            // key and generated hash must be equal
            assert(tmpAsset.first == tmpHash);
        }
    }
#endif // NDEBUG

    for (auto &tmpAsset : TextureMap) {
        vw_SetTextureProp(sTextureFilter{tmpAsset.second.TextFilter},
                          tmpAsset.second.NeedAnisotropy ? GameConfig().AnisotropyLevel : 1,
                          sTextureWrap{tmpAsset.second.TextWrap}, tmpAsset.second.Alpha,
                          tmpAsset.second.AlphaMode, tmpAsset.second.MipMap);
        tmpAsset.second.PreloadedTexture = vw_LoadTexture(tmpAsset.second.TextureFile);
        function(TextureLoadValue);
    }
}

/*
 * Change anisotropy level for all textures that was loaded with anisotropy.
 */
void ChangeTexturesAnisotropyLevel()
{
    for (auto &tmpAsset : TextureMap) {
        if (tmpAsset.second.NeedAnisotropy) {
            vw_BindTexture(0, tmpAsset.second.PreloadedTexture);
            vw_SetTextureAnisotropy(GameConfig().AnisotropyLevel);
        }
    }
    vw_BindTexture(0, 0);
}

/*
 * Get preloaded texture asset (preloaded by ForEachTextureAssetLoad() call).
 * Note, we don't validate textures, caller should care about call
 * ForEachTextureAssetLoad() each time, when this need.
 */
GLtexture GetPreloadedTextureAsset(unsigned FileNameHash)
{
    auto tmpAsset = TextureMap.find(FileNameHash);
    if (tmpAsset != TextureMap.end() && tmpAsset->second.PreloadedTexture) {
        return tmpAsset->second.PreloadedTexture;
    }

    std::cerr << __func__ << "(): " << "preloaded texture not found, hash: " << FileNameHash << "\n";
    return 0;
}

} // astromenace namespace
} // viewizard namespace
