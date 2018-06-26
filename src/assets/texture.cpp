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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../core/core.h"
#include "../config/config.h"

namespace {

// empirical found "everage load value" for one texture asset
// small value mean that asset loads fast, big value - slow
constexpr unsigned TextureLoadValue{200};

struct sTextureAsset {
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

	explicit sTextureAsset(bool _Alpha = false,
			       eAlphaCreateMode _AlphaMode = eAlphaCreateMode::EQUAL,
			       eTextureWrapMode _TextWrap = eTextureWrapMode::CLAMP_TO_EDGE,
			       eTextureBasicFilter _TextFilter = eTextureBasicFilter::BILINEAR,
			       bool _NeedAnisotropy = false,
			       bool _MipMap = false) :
		Alpha{_Alpha},
		AlphaMode{_AlphaMode},
		TextWrap{_TextWrap},
		TextFilter{_TextFilter},
		NeedAnisotropy{_NeedAnisotropy},
		MipMap{_MipMap}
	{}
};

std::unordered_map<std::string, sTextureAsset> TextureMap{
	{"menu/astromenace.tga",	sTextureAsset{true}},
	{"menu/button384_back.tga",	sTextureAsset{true}},
	{"menu/button384_in.tga",	sTextureAsset{true}},
	{"menu/button384_out.tga",	sTextureAsset{true}},
	{"menu/button256_back.tga",	sTextureAsset{true}},
	{"menu/button256_in.tga",	sTextureAsset{true}},
	{"menu/button256_out.tga",	sTextureAsset{true}},
	{"menu/button256_off.tga",	sTextureAsset{true}},
	{"menu/blackpoint.tga",		sTextureAsset{}},
	{"menu/whitepoint.tga",		sTextureAsset{}},
	{"menu/line.tga",		sTextureAsset{true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT}},
	{"menu/perc.tga",		sTextureAsset{true}},
	{"menu/perc_none.tga",		sTextureAsset{true}},
	{"menu/checkbox_main.tga",	sTextureAsset{true}},
	{"menu/checkbox_in.tga",	sTextureAsset{true}},
	{"menu/workshop_panel1.tga",	sTextureAsset{true}},
	{"menu/workshop_panel1+.tga",	sTextureAsset{true}},
	{"menu/workshop_panel2.tga",	sTextureAsset{true}},
	{"menu/workshop_panel2+.tga",	sTextureAsset{true}},
	{"menu/workshop_panel3.tga",	sTextureAsset{true}},
	{"menu/workshop_panel4.tga",	sTextureAsset{true}},
	{"menu/workshop_panel5.tga",	sTextureAsset{true}},
	{"menu/ammo.tga",		sTextureAsset{true}},
	{"lang/en/menu/button_weaponry_out.tga",	sTextureAsset{}},
	{"lang/en/menu/button_weaponry_in.tga",		sTextureAsset{}},
	{"lang/de/menu/button_weaponry_out.tga",	sTextureAsset{}},
	{"lang/de/menu/button_weaponry_in.tga",		sTextureAsset{}},
	{"lang/ru/menu/button_weaponry_out.tga",	sTextureAsset{}},
	{"lang/ru/menu/button_weaponry_in.tga",		sTextureAsset{}},
	{"lang/pl/menu/button_weaponry_out.tga",	sTextureAsset{}},
	{"lang/pl/menu/button_weaponry_in.tga",		sTextureAsset{}},
	{"menu/back_spot.tga",		sTextureAsset{true}},
	{"menu/back_spot2.tga",		sTextureAsset{true}},
	{"menu/dialog512_256.tga",	sTextureAsset{true}},
	{"menu/dialog512_512.tga",	sTextureAsset{true}},
	{"menu/dialog768_600.tga",	sTextureAsset{true}},
	{"menu/panel444_333_back.tga",	sTextureAsset{true}},
	{"menu/panel444_333_border.tga",	sTextureAsset{true}},
	{"menu/panel800_444_back.tga",		sTextureAsset{true}},
	{"menu/button_dialog200_out.tga",	sTextureAsset{true}},
	{"menu/button_dialog200_in.tga",	sTextureAsset{true}},
	{"menu/button_dialog200_off.tga",	sTextureAsset{true}},
	{"menu/button_dialog128_out.tga",	sTextureAsset{true}},
	{"menu/button_dialog128_in.tga",	sTextureAsset{true}},
	{"menu/button_dialog128_off.tga",	sTextureAsset{true}},
	{"menu/arrow_list_up.tga",	sTextureAsset{true}},
	{"menu/arrow_list_down.tga",	sTextureAsset{true}},
	{"script/mission1_icon.tga",	sTextureAsset{}},
	{"script/mission2_icon.tga",	sTextureAsset{}},
	{"script/mission3_icon.tga",	sTextureAsset{}},
	{"script/mission4_icon.tga",	sTextureAsset{}},
	{"script/mission5_icon.tga",	sTextureAsset{}},
	{"script/mission6_icon.tga",	sTextureAsset{}},
	{"script/mission7_icon.tga",	sTextureAsset{}},
	{"script/mission8_icon.tga",	sTextureAsset{}},
	{"script/mission9_icon.tga",	sTextureAsset{}},
	{"script/mission10_icon.tga",	sTextureAsset{}},
	{"script/mission11_icon.tga",	sTextureAsset{}},
	{"script/mission12_icon.tga",	sTextureAsset{}},
	{"script/mission13_icon.tga",	sTextureAsset{}},
	{"script/mission14_icon.tga",	sTextureAsset{}},
	{"script/mission15_icon.tga",	sTextureAsset{}},
	{"menu/system_engine1.tga",	sTextureAsset{true}},
	{"menu/system_engine2.tga",	sTextureAsset{true}},
	{"menu/system_engine3.tga",	sTextureAsset{true}},
	{"menu/system_engine4.tga",	sTextureAsset{true}},
	{"menu/system_mechan1.tga",	sTextureAsset{true}},
	{"menu/system_mechan2.tga",	sTextureAsset{true}},
	{"menu/system_mechan3.tga",	sTextureAsset{true}},
	{"menu/system_mechan4.tga",	sTextureAsset{true}},
	{"menu/system_power1.tga",	sTextureAsset{true}},
	{"menu/system_power2.tga",	sTextureAsset{true}},
	{"menu/system_power3.tga",	sTextureAsset{true}},
	{"menu/system_power4.tga",	sTextureAsset{true}},
	{"menu/system_protect1.tga",	sTextureAsset{true}},
	{"menu/system_protect2.tga",	sTextureAsset{true}},
	{"menu/system_protect3.tga",	sTextureAsset{true}},
	{"menu/system_protect4.tga",	sTextureAsset{true}},
	{"menu/system_target1.tga",	sTextureAsset{true}},
	{"menu/system_target2.tga",	sTextureAsset{true}},
	{"menu/system_target3.tga",	sTextureAsset{true}},
	{"menu/system_target4.tga",	sTextureAsset{true}},
	{"menu/system_empty.tga",	sTextureAsset{true}},
	{"menu/weapon1_icon.tga",	sTextureAsset{true}},
	{"menu/weapon2_icon.tga",	sTextureAsset{true}},
	{"menu/weapon3_icon.tga",	sTextureAsset{true}},
	{"menu/weapon4_icon.tga",	sTextureAsset{true}},
	{"menu/weapon5_icon.tga",	sTextureAsset{true}},
	{"menu/weapon6_icon.tga",	sTextureAsset{true}},
	{"menu/weapon7_icon.tga",	sTextureAsset{true}},
	{"menu/weapon8_icon.tga",	sTextureAsset{true}},
	{"menu/weapon9_icon.tga",	sTextureAsset{true}},
	{"menu/weapon10_icon.tga",	sTextureAsset{true}},
	{"menu/weapon11_icon.tga",	sTextureAsset{true}},
	{"menu/weapon12_icon.tga",	sTextureAsset{true}},
	{"menu/weapon13_icon.tga",	sTextureAsset{true}},
	{"menu/weapon14_icon.tga",	sTextureAsset{true}},
	{"menu/weapon15_icon.tga",	sTextureAsset{true}},
	{"menu/weapon16_icon.tga",	sTextureAsset{true}},
	{"menu/weapon17_icon.tga",	sTextureAsset{true}},
	{"menu/weapon18_icon.tga",	sTextureAsset{true}},
	{"menu/weapon19_icon.tga",	sTextureAsset{true}},
	{"menu/weapon_on_icon.tga",	sTextureAsset{true}},
	{"menu/weapon_empty_icon.tga",	sTextureAsset{true}},
	{"menu/cursor.tga",		sTextureAsset{true}},
	{"menu/cursor_shadow.tga",	sTextureAsset{true}},
	{"gfx/flare.tga",		sTextureAsset{true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/flare1.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/flare2.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/flare3.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/trail1.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/trail2.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/trail3.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/trail4.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"gfx/trail5.tga",		sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"game/nums.tga",			sTextureAsset{true}},
	{"game/ammo.tga",			sTextureAsset{true}},
	{"game/energy.tga",			sTextureAsset{true}},
	{"game/game_panel.tga",			sTextureAsset{true}},
	{"game/game_panel2.tga",		sTextureAsset{true}},
	{"game/game_panel_el.tga",		sTextureAsset{true}},
	{"game/game_num.tga",			sTextureAsset{true}},
	{"game/weapon_panel_left.tga",		sTextureAsset{true}},
	{"game/weapon_panel_right.tga",		sTextureAsset{true}},
	{"game/weapon_ammo.tga",		sTextureAsset{true}},
	{"game/weapon_energy.tga",		sTextureAsset{true}},
	{"lang/en/game/mission.tga",		sTextureAsset{true}},
	{"lang/en/game/missionfailed.tga",	sTextureAsset{true}},
	{"lang/en/game/pause.tga",		sTextureAsset{true}},
	{"lang/de/game/mission.tga",		sTextureAsset{true}},
	{"lang/de/game/missionfailed.tga",	sTextureAsset{true}},
	{"lang/de/game/pause.tga",		sTextureAsset{true}},
	{"lang/ru/game/mission.tga",		sTextureAsset{true}},
	{"lang/ru/game/missionfailed.tga",	sTextureAsset{true}},
	{"lang/ru/game/pause.tga",		sTextureAsset{true}},
	{"skybox/tile_back.tga",	sTextureAsset{true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"skybox/tile_stars.tga",	sTextureAsset{true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text00.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text04.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text05.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text06.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text07.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text08.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text09.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-text10.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-illum01.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-illum02.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-illum03.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/sf-illum04.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/lnch12.tga",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/lnch34.tga",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/earthfighter/rockets.tga",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine1.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine1i.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine2.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine2i.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine3.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine3i.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine4.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/mine/mine4i.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienfighter/al-text04.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienfighter/al-illum04.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/gr-01.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/gr-02.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/gr-03.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/gr-04.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/bomber_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/gr-05.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/gr-06.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/gr-07.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/track.vw2d",		sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/turret/turrets.tga",	sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/spacebase/allalpha.tga",
					sTextureAsset{true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/spacebase/metal.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/space/asteroid-01.tga",
					sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/planet/asteroid.tga",	sTextureAsset{true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/planet/clouds.tga",	sTextureAsset{true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/planet/q_class2.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/planet/a_class4.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/planet/m_class7.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/planet/d_class3.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/m_class7_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/d_class3_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/a_class4_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/planet_asteroids_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/q_class2_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-text02.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-text03.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-text04.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-text08.vw2d",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-illum02.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-illum03.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-illum04.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/alienmothership/alm-illum08.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/alien_mothership_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/building/bld.vw2d",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/normalmap/buildings_nm.tga",
					sTextureAsset{false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"models/building/bld_illum.vw2d",
					sTextureAsset{true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT,
						      eTextureBasicFilter::TRILINEAR, true, true}},
	{"skybox/1/skybox_back6.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/1/skybox_bottom4.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/1/skybox_front5.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/1/skybox_left2.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/1/skybox_right1.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/1/skybox_top3.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/2/skybox_back6.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/2/skybox_bottom4.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/2/skybox_front5.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/2/skybox_left2.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/2/skybox_right1.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
	{"skybox/2/skybox_top3.tga",	sTextureAsset{false, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE}},
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

	for (auto &tmpAsset : TextureMap) {
		vw_SetTextureProp(sTextureFilter{tmpAsset.second.TextFilter},
				  tmpAsset.second.NeedAnisotropy ? GameConfig().AnisotropyLevel : 1,
				  sTextureWrap{tmpAsset.second.TextWrap}, tmpAsset.second.Alpha,
				  tmpAsset.second.AlphaMode, tmpAsset.second.MipMap);
		tmpAsset.second.PreloadedTexture = vw_LoadTexture(tmpAsset.first);
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
GLtexture GetPreloadedTextureAsset(const std::string &FileName)
{
	auto tmpAsset = TextureMap.find(FileName);
	if ((tmpAsset != TextureMap.end()) &&
	    tmpAsset->second.PreloadedTexture)
		return tmpAsset->second.PreloadedTexture;

	std::cerr << __func__ << "(): " << "preloaded texture not found: " << FileName << "\n";
	return 0;
}
