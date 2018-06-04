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

// TODO translate comments

// TODO add list initialization via XML file, hard coded list should be removed
//      probably, we could combine with integrity check list (that used for gamedata creation)
//      make sure, that logo and loading images included too

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "game.h"
#include "config/config.h"
#include "object3d/object3d.h"
#include "gfx/star_system.h"
#include "gfx/shadow_map.h"
#include "ui/font.h"


struct sLoadList {
	// имя файла
	std::string FileName;
	// тип файла (0-2д текстура, 1-текстура, 2-VW3D, 3-music, 4-sfx)
	int FileType;
	// вес данного объекта при загрузке (для текстур берем кб)
	int Value;
	// альфа канал, если нужно
	bool Alpha;
	// цвет альфа канала
	uint8_t Red, Green, Blue;
	// режим создания альфа канала
	eAlphaCreateMode AlphaMode;
	// фильтр текстуры
	eTextureWrapMode TextWrap;
	eTextureBasicFilter TextFilter;
	int TextAnisotropy; // 1 - need, 0 - don't need
	bool MipMap;
	// нужно ли для этой текстуры сжатие
	bool NeedCompression;
	// для 3д моделей, если 1.0f делать структуру с мелкими треугольниками
	float TriangleSizeLimit;
	// для 3д моделей, если надо - делаем тангенты и бинормали
	bool NeedTangentAndBinormal;
};

std::weak_ptr<cGLSL> GLSLShaderType1{};
std::weak_ptr<cGLSL> GLSLShaderType2{};
std::weak_ptr<cGLSL> GLSLShaderType3{};

struct cGLSLLoadList {
	std::string Name;
	std::string VertexShaderFileName;
	std::string FragmentShaderFileName;
};
const cGLSLLoadList GLSLLoadList[] = {
	{"ParticleSystem",		"glsl/particle.vert",		"glsl/particle.frag"},
	{"PerPixelLight",		"glsl/light.vert",		"glsl/light.frag"},
	{"PerPixelLight_ShadowMap",	"glsl/light_shadowmap.vert",	"glsl/light_shadowmap.frag"},
	{"PerPixelLight_Explosion",	"glsl/light_explosion.vert",	"glsl/light_explosion.frag"},
};
#define GLSLLoadListCount sizeof(GLSLLoadList)/sizeof(GLSLLoadList[0])


// лист загрузки меню
static sLoadList LoadList[] = {
// текстуры меню... кнопки, диалоги, название игры
	{"menu/astromenace.tga",	0, 512, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button384_back.tga",	0, 192, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button384_in.tga",	0, 96, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button384_out.tga",	0, 96, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_back.tga",	0, 192, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_in.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_out.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_off.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/blackpoint.tga",		0, 0, true,  255,255,255, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/whitepoint.tga",		0, 0, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/line.tga",		0, 4, true,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/perc.tga",		0, 2, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/perc_none.tga",		0, 2, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/checkbox_main.tga",	0, 5, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/checkbox_in.tga",	0, 5, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// панельки и кнопки вывода информации в меню модернизации корабля
	{"menu/workshop_panel1.tga",			0, 270, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel1+.tga",			0, 270, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel2.tga",			0, 492, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel2+.tga",			0, 492, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel3.tga",			0, 110, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel4.tga",			0, 54, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel5.tga",			0, 899, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/ammo.tga",				0, 8, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/en/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/en/menu/button_weaponry_in.tga",		0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/de/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/de/menu/button_weaponry_in.tga",		0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/ru/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/ru/menu/button_weaponry_in.tga",		0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/pl/menu/button_weaponry_out.tga",	0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/pl/menu/button_weaponry_in.tga",		0, 6, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/back_spot.tga",				0, 256, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/back_spot2.tga",				0, 256, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// диалоги
	{"menu/dialog512_256.tga",	0, 706, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/dialog512_512.tga",	0, 1242, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/dialog768_600.tga",	0, 2131, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// панельки
	{"menu/panel444_333_back.tga",		0, 639, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/panel444_333_border.tga",	0, 705, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/panel800_444_back.tga",		0, 1631, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// малые кнопки для диалогов и панелек
	{"menu/button_dialog200_out.tga",	0, 57, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog200_in.tga",	0, 57, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog200_off.tga",	0, 57, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog128_out.tga",	0, 39, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog128_in.tga",	0, 39, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog128_off.tga",	0, 39, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/arrow_list_up.tga",		0, 69, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/arrow_list_down.tga",		0, 69, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// иконки отображения в меню выбора миссий
	{"script/mission1_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission2_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission3_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission4_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission5_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission6_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission7_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission8_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission9_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission10_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission11_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission12_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission13_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission14_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission15_icon.tga",	0, 12, false,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// иконки систем корабля
	{"menu/system_engine1.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_engine2.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_engine3.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_engine4.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan1.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan2.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan3.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan4.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power1.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power2.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power3.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power4.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect1.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect2.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect3.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect4.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target1.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target2.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target3.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target4.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_empty.tga",	0, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// иконки оружия для передаскивания-отображения в слотах оружия
	{"menu/weapon1_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon2_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon3_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon4_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon5_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon6_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon7_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon8_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon9_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon10_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon11_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon12_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon13_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon14_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon15_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon16_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon17_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon18_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon19_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon_on_icon.tga",	0, 32, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon_empty_icon.tga",	0, 256, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// курсор
	{"menu/cursor.tga",		0, 16, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/cursor_shadow.tga",	0, 16, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// спец эффекты
	{"gfx/flare.tga",	1, 16, true,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/flare1.tga",	1, 16, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/flare2.tga",	1, 16, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/flare3.tga",	1, 16, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail1.tga",	1, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail2.tga",	1, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail3.tga",	1, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail4.tga",	1, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail5.tga",	1, 64, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},

// 2д часть
	{"game/nums.tga",			0, 104, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/ammo.tga",			0, 2, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/energy.tga",			0, 2, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/game_panel.tga",			0, 296, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/game_panel2.tga",		0, 347, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/game_panel_el.tga",		0, 256, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/game_num.tga",			0, 31, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/weapon_panel_left.tga",		0, 56, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/weapon_panel_right.tga",		0, 56, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/weapon_ammo.tga",		0, 1, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/weapon_energy.tga",		0, 1, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/en/game/mission.tga",		0, 64, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/en/game/missionfailed.tga",	0, 168, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/en/game/pause.tga",		0, 64, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/de/game/mission.tga",		0, 64, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/de/game/missionfailed.tga",	0, 168, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/de/game/pause.tga",		0, 64, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/ru/game/mission.tga",		0, 64, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/ru/game/missionfailed.tga",	0, 168, true,  0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/ru/game/pause.tga",		0, 64, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/tile_back.tga",		1, 4096/2, true,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"skybox/tile_stars.tga",		1, 4096/2, true,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// текстура брони для кораблей землян
	{"models/earthfighter/sf-text00.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text04.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text05.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text06.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text07.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text08.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text09.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text10.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum01.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum02.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum03.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum04.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/lnch12.tga",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/lnch34.tga",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/rockets.tga",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"sfx/weapon1probl.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weapon2probl.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weapon3probl.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weapon4probl.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weapon5probl.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/explosion1.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/explosion2.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/explosion3.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/explosion4.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire1.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire2.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire3.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire4.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire5.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire6.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire7.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire8.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire9.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire10.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire11.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire12.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire13.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire14.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire15.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire16.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire17.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire18.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/weaponfire19.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/kinetichit.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/ionhit.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/plasmahit.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/antimaterhit.wav",	4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/gausshit.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
	{"sfx/lowlife.wav",		4, 20, false, 0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, true, -1.0f, false},
// мины
	{"models/mine/mine1.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine1i.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine2.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine2i.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine3.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine3i.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine4.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine4i.tga",	1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// модели мин
	{"models/mine/mine-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/mine/mine-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/mine/mine-03.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/mine/mine-04.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
// корабли землян + их оружие
	{"models/earthfighter/sf-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-03.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-04.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-05.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-06.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-07.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-08.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-09.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-10.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-11.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-12.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-13.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-14.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-15.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-16.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-17.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-18.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-19.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-20.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-21.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/sf-22.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon1.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon2.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon3.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon4.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon5.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon6.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon7.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon8.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon9.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon10.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon11.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon12.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon13.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon14.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/weapon15.vw3d",	2, 10, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/lnch1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/lnch2.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/lnch3.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/lnch4.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/missile.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/swarm.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/torpedo.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/earthfighter/nuke.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},

// AlienFighter – load alien fighters textures.
	{"models/alienfighter/al-text04.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienfighter/al-illum04.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// малые корабли пришельцев
	{"models/alienfighter/al-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-03.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-04.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-05.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-06.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-07.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-08.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-09.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-10.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-11.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-12.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-13.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-14.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-15.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-16.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/alienfighter/al-17.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},

// Pirate – load all pirate data (vehicles, military buildings, ships...) testures.
	{"models/gr-01.vw2d",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-02.vw2d",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-03.vw2d",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-04.vw2d",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/bomber_nm.tga",	1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-05.vw2d",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-06.vw2d",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-07.vw2d",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/track.vw2d",			1, 48, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/turret/turrets.tga",		1, 192, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// корабли пиратов
	{"models/pirateship/gunship-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/bomber-07.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/gunship-03.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/gunship-04.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/gunship-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/bomber-03.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/bomber-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/bomber-04.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/pirateship/bomber-05.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/pirateship/bomber-06.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
// турелей
	{"models/turret/turret-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/turret/turret-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
// гусенечный транспорт
	{"models/tracked/engineering-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/apc-aa-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/apc-aa-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/apc-03.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/apc-01.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-11.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-10.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-09.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-08.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-07.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-06.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-05.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-03.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/tracked/tank-01.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
// колесный транспорт
	{"models/wheeled/r-launcher-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/wheeled/apc-04.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/wheeled/apc-02.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/wheeled/jeep-05.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/wheeled/jeep-04.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/wheeled/jeep-03.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/wheeled/jeep-02.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/wheeled/jeep-01.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
// военные сооружения
	{"models/militarybuilding/artiler-gun-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/militarybuilding/artiler-gun-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/militarybuilding/aa-gun-05.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/militarybuilding/aa-gun-04.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/militarybuilding/aa-gun-03.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/militarybuilding/aa-gun-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},
	{"models/militarybuilding/aa-gun-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, false},

// BasePart – load pirate base textures.
	{"models/spacebase/allalpha.tga",	1, 768, true, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/spacebase/metal.tga",		1, 1024, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
// геометрия базы пиратов
	{"models/spacebase/1/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/1/2.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/1/3.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/1/4.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/1/5.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/2/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/2/2.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/2/3.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/2/4.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/2/5.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/3/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/spacebase/4/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/spacebase/5/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/spacebase/6/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/6/2.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/6/3.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/6/4.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/6/5.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/7/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/spacebase/8/1.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/8/2.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/8/3.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/8/4.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/spacebase/8/5.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},

// Asteroid – load asteroids (for AsteroidField) textures.
	{"models/space/asteroid-01.tga",	1, 96, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// геометрия больших астероидов
	{"models/space/bigasteroid-01.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/space/bigasteroid-02.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/space/bigasteroid-03.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/space/bigasteroid-04.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/space/bigasteroid-05.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
// астероиды
	{"models/space/asteroid-010.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-011.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-012.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-013.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-014.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-015.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-016.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-017.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-018.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-019.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0110.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0111.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0112.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0113.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0114.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0115.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0116.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0117.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0118.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},
	{"models/space/asteroid-0119.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, false},

// Planet – load planets textures.
	{"models/planet/asteroid.tga",			1, 512, true, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/clouds.tga",			1, 1024, true, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"models/planet/q_class2.tga",			1, 192, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/a_class4.tga",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/m_class7.tga",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/d_class3.tga",			1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/m_class7_nm.tga",		1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/d_class3_nm.tga",		1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/a_class4_nm.tga",		1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/planet_asteroids_nm.tga",	1, 384, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/q_class2_nm.tga",		1, 192, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// модели планет
	{"models/planet/aplanet.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/planet/dplanet.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/planet/gplanet.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/planet/moon.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/planet/planet5.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/planet/planet6.vw3d",	2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},

// AlienMotherShip – load alien motherships textures.
	{"models/alienmothership/alm-text02.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-text03.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-text04.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-text08.vw2d",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum02.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum03.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum04.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum08.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/alien_mothership_nm.tga",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// большие корабли пришельцев
	{"models/alienmothership/alm-01.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/alienmothership/alm-02.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/alienmothership/alm-03.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/alienmothership/alm-04.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/alienmothership/alm-05.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/alienmothership/alm-06.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/alienmothership/alm-07.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},
	{"models/alienmothership/alm-08.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, 2.0f, true},

// Building – load buildings textures.
	{"models/building/bld.vw2d",		1, 768, false, 0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/buildings_nm.tga",	1, 768, false, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/building/bld_illum.vw2d",	1, 1024, true, 0,0,0, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// постройки (мирные)
	{"models/building/bld-01.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-02.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-03.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-04.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-05.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-06.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-07.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-08.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-09.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-10.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},
	{"models/building/bld-11.vw3d",		2, 20, true,  0,0,0, eAlphaCreateMode::NONE, eTextureWrapMode::REPEAT, eTextureBasicFilter::NONE, 0, true, false, -1.0f, true},

// StarSystem1 – load StarSystem 1 SkyBox textures.
	{"skybox/1/skybox_back6.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_bottom4.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_front5.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_left2.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_right1.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_top3.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},

//StarSystem2 – load StarSystem 2 SkyBox textures.
	{"skybox/2/skybox_back6.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_bottom4.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_front5.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_left2.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_right1.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_top3.tga",	1, 3072/2, false,  0,0,0, eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
};
#define LoadListCount sizeof(LoadList)/sizeof(LoadList[0])


//------------------------------------------------------------------------------------
// процедура прорисовки логотипа
//------------------------------------------------------------------------------------
static void DrawViewizardLogo(GLtexture ViewizardLogoTexture)
{
	int ShowLogoTime{6000}; // сколько нужно показывать логотип
	int ShowLogoLife{ShowLogoTime}; // сколько осталось показывать
	uint32_t ShowLogoPrevTime = SDL_GetTicks();

	vw_SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	while (ShowLogoLife > 0) {
		sRECT SrcRect{1, 1, 511, 511};
		int StartX{static_cast<int>((GameConfig().InternalWidth - 510) / 2)};
		int EndX{StartX + 510};
		sRECT DstRect{StartX, 128 + 1, EndX, 640 - 2};
		float Transp{1.0f};
		float GreyColor{1.0f};

		// плавно делаем появление
		if (ShowLogoLife > ShowLogoTime / 2.0f)
			Transp = (ShowLogoTime / 2.0f - (ShowLogoLife - ShowLogoTime / 2.0f)) / (ShowLogoTime / 2.0f);
		else {
			Transp = 1.0f;
			// относительно быстро исчезаем
			if (ShowLogoLife < ShowLogoTime / 4.0f) {
				GreyColor = ShowLogoLife / (ShowLogoTime / 4.0f);
				vw_SetClearColor(GreyColor, GreyColor, GreyColor, 1.0f);
			}
		}
		Transp -= 0.01f; // чтобы всегда был немного прозрачным
		if (Transp < 0.0f)
			Transp = 0.0f;

		// рисуем
		vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
		vw_Start2DMode(-1, 1);

		vw_Draw2D(DstRect, SrcRect, ViewizardLogoTexture, true, Transp, 0.0f,
			  sRGBCOLOR{GreyColor, GreyColor, GreyColor});

		vw_End2DMode();
		vw_EndRendering();

		// проверка времени
		ShowLogoLife -= SDL_GetTicks() - ShowLogoPrevTime;
		ShowLogoPrevTime = SDL_GetTicks();
		if (ShowLogoLife <= 0)
			ShowLogoLife = 0;

		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_KEYDOWN:
			case SDL_JOYBUTTONDOWN:
				ShowLogoLife = 0;
				break;
			default:
				break;
			}
		}

		SDL_Delay(2);

		// important, update music buffers
		Audio_LoopProc();
	}

	vw_SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------------
// процедура прорисовки процента загрузки данных
//------------------------------------------------------------------------------------
static void DrawLoading(int Current, int AllDrawLoading, uint32_t &LastDrawTime, GLtexture LoadImageTexture)
{
	if ((Current != AllDrawLoading) && // the last one (with 100%) must be rendered
	    (LastDrawTime + 10 >= SDL_GetTicks())) // 100 per second - is good enough frame rate, no need more
		return;

	vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
	vw_Start2DMode(-1, 1);

	sRECT SrcRect{0, 0, 1024, 512};
	sRECT DstRect{0, 64+32, static_cast<int>(GameConfig().InternalWidth), 64+32+512};
	vw_Draw2D(DstRect, SrcRect, LoadImageTexture, false, 1.0f, 0.0f);

	vw_DrawText(GameConfig().InternalWidth / 2 - vw_TextWidth(vw_GetText("LOADING")) / 2,
		    768-128, 0, 0, 1.0f, eRGBCOLOR::white, 1.0f, vw_GetText("LOADING"));

	SrcRect(0, 0, 256, 32);
	int StartX = (GameConfig().InternalWidth - 256) / 2;
	DstRect(StartX, 768-64-8-32, StartX + SrcRect.right - SrcRect.left, 768-64-8-32 + SrcRect.bottom - SrcRect.top);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("loading/loading_back.tga"), true, 1.0f, 0.0f);

	int loaded = (int)(256.0f * Current / AllDrawLoading);
	SrcRect(0, 0, loaded, 16);
	DstRect(StartX, 768-64-1-32, StartX + SrcRect.right - SrcRect.left, 768-64-1-32 + SrcRect.bottom - SrcRect.top);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("loading/loading_line.tga"), true, 1.0f, 0.0f);

	vw_End2DMode();
	vw_EndRendering();

	// important, care about system events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			Quit = true;
			break;
		default:
			break;
		}
	}

	// important, update music buffers
	Audio_LoopProc();

	LastDrawTime = SDL_GetTicks();

	// small delay for last call with 100%
	if (Current == AllDrawLoading)
		SDL_Delay(100);
}

static void PreLoadGameData(eLoading LoadType)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// удаляем данные из памяти
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ReleaseAllObject3D();
	vw_ReleaseAllParticleSystems();
	vw_ReleaseAllLights();
	StarSystemRelease();

	// если это не переход меню-игра, снимаем звук
	vw_StopAllSoundsIfAllowed();

	// FIXME should be moved to proper sources
	switch(LoadType) {
	// меню, загрузка в самом начале
	case eLoading::MenuWithLogo:
		MenuStatus = eMenuStatus::MAIN_MENU;
		Audio_LoopProc();
		StartMusicWithFade(eMusicTheme::MENU, 4000, 4000);
		break;
	// переход игра-меню
	case eLoading::Menu:
		MenuStatus = eMenuStatus::MISSION;
		StartMusicWithFade(eMusicTheme::MENU, 2000, 2000);
		break;
	// переход меню-игра
	case eLoading::Game:
		SaveXMLConfigFile();
		break;
	default:
		break;
	}
}

static void PostLoadGameData(eLoading LoadType)
{
	// инициализируем шадов меп, делаем это постоянно т.к. у нас разные размеры карт для меню и игры
	if (GameConfig().ShadowMap > 0) {
		int ShadowMapSize = vw_GetDevCaps().MaxTextureWidth;

		auto MenuShadowMap = [&ShadowMapSize] () {
			// since we need "soft" shadows for less price, reduce shadow map size
			if (ShadowMapSize > 2048)
				ShadowMapSize = 2048;
			if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize / 2))
				ChangeGameConfig().ShadowMap = 0;
		};

		switch(LoadType) {
		case eLoading::Menu:   // меню (выходим из игры)
			ShadowMap_Release();
			MenuShadowMap();
			break;

		case eLoading::MenuWithLogo:  // меню (только запустили)
			MenuShadowMap();
			break;

		case eLoading::Game: // переход на уровни игры
			ShadowMap_Release();
			// since we need "soft" shadows for less price, reduce shadow map size
			if (ShadowMapSize > 4096)
				ShadowMapSize = 4096;
			if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize))
				ChangeGameConfig().ShadowMap = 0;
			break;

		default:
			break;
		}
	}

	// переходим в нужное место...
	switch(LoadType) {
	// меню, первая загрузка, самый старт
	case eLoading::MenuWithLogo:
		InitMenu();
		break;

	// меню, выходим из игры, входим в меню
	case eLoading::Menu:
		InitMenu();
		MenuStatus = eMenuStatus::MISSION; // чтобы не было перехода с основного меню в мисии
		break;

	// уровни игры
	case eLoading::Game:
		InitGame();
		StartMusicWithFade(eMusicTheme::GAME, 2000, 2000);
		// приготовиться к действию (речь)
		Audio_PlayVoice(5, 1.0f);
		break;

	default:
		break;
	}

	// всегда на черном фоне
	vw_SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------------
// процедура загрузки данных
//------------------------------------------------------------------------------------
void LoadGameData(eLoading LoadType)
{
	PreLoadGameData(LoadType);

	// load assets only on game start (or restart)
	if (LoadType != eLoading::MenuWithLogo) {
		PostLoadGameData(LoadType);
		return;
	}

	int AllDrawLoading{0};
	// получаем значение (реальное, по весам)
	for (unsigned i = 0; i < LoadListCount; i++) {
		AllDrawLoading += LoadList[i].Value;
	}

	// если будем загружать шейдеры - делаем поправку общего кол-ва
	bool NeedLoadShaders = false;
	if (vw_GetDevCaps().OpenGL_2_0_supported &&
	    vw_GetDevCaps().OpenGL_2_1_supported &&
	    GameConfig().UseGLSL120 &&
	    vw_ShadersMapEmpty()) {
		AllDrawLoading += GLSLLoadListCount*100;
		NeedLoadShaders = true;
	}

	if (LoadType == eLoading::MenuWithLogo)
		GenerateFonts();

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем логотип компании
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (LoadType == eLoading::MenuWithLogo) {
		// выводим логотип Viewizard
		vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 0, eTextureWrapMode::CLAMP_TO_EDGE,
				  false, eAlphaCreateMode::EQUAL, false);
		GLtexture ViewizardLogoTexture = vw_LoadTexture("loading/viewizardlogo.tga");

		DrawViewizardLogo(ViewizardLogoTexture);
		vw_ReleaseTexture(ViewizardLogoTexture);
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем список
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем картинки вывода загрузки
	vw_SetTextureAlpha(0, 0, 0);
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 0, eTextureWrapMode::CLAMP_TO_EDGE,
			  true, eAlphaCreateMode::GREYSC, false);
	vw_LoadTexture("loading/loading_line.tga");
	vw_LoadTexture("loading/loading_back.tga");
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 0, eTextureWrapMode::CLAMP_TO_EDGE,
			  false, eAlphaCreateMode::GREYSC, false);

	GLtexture LoadImageTexture{0};
	switch (1 + vw_iRandNum(3)) {
	case 1:
		LoadImageTexture = vw_LoadTexture("loading/loading01.tga");
		break;
	case 2:
		LoadImageTexture = vw_LoadTexture("loading/loading02.tga");
		break;
	case 3:
		LoadImageTexture = vw_LoadTexture("loading/loading03.tga");
		break;
	case 4:
		LoadImageTexture = vw_LoadTexture("loading/loading04.tga");
		break;
	// на всякий случай
	default:
		LoadImageTexture = vw_LoadTexture("loading/loading01.tga");
		break;
	}

	// ставим время последней прорисовки
	uint32_t LastDrawTime = SDL_GetTicks();

	// если нужно, загрузка всех шейдеров (!) обязательно это делать до загрузки моделей
	int RealLoadedAssets{0};
	if (NeedLoadShaders) {
		for (unsigned i = 0; i < GLSLLoadListCount; i++) {
			if (GameConfig().UseGLSL120) {

				std::weak_ptr<cGLSL> Program = vw_CreateShader(GLSLLoadList[i].Name,
									       GLSLLoadList[i].VertexShaderFileName,
									       GLSLLoadList[i].FragmentShaderFileName);

				if (!Program.expired()) {
					// получаем сразу состояние, смогли прилинковать или нет
					if (!vw_LinkShaderProgram(Program))
						ChangeGameConfig().UseGLSL120 = false;
				} else
					ChangeGameConfig().UseGLSL120 = false;

				RealLoadedAssets += 100;
				// рисуем текущее состояние загрузки, если не рисуем логотип
				DrawLoading(RealLoadedAssets, AllDrawLoading, LastDrawTime, LoadImageTexture);
			}
		}

		// find all shaders by name
		GLSLShaderType1 = vw_FindShaderByName("PerPixelLight");
		GLSLShaderType2 = vw_FindShaderByName("PerPixelLight_Explosion");
		GLSLShaderType3 = vw_FindShaderByName("PerPixelLight_ShadowMap");

		// find and store uniform location for all shaders, in real, we don' need
		// store internal storage number for uniforms usage, since we load them
		// one-by-one, and we know sequence, we could use 0-1-2-3-4 numbers directly
		vw_FindShaderUniformLocation(GLSLShaderType1, "Texture1");
		vw_FindShaderUniformLocation(GLSLShaderType1, "Texture2");
		vw_FindShaderUniformLocation(GLSLShaderType1, "DirectLightCount");
		vw_FindShaderUniformLocation(GLSLShaderType1, "PointLightCount");
		vw_FindShaderUniformLocation(GLSLShaderType1, "NeedMultitexture");
		vw_FindShaderUniformLocation(GLSLShaderType1, "NormalMap");
		vw_FindShaderUniformLocation(GLSLShaderType1, "NeedNormalMapping");

		vw_FindShaderUniformLocation(GLSLShaderType2, "Texture1");
		vw_FindShaderUniformLocation(GLSLShaderType2, "DirectLightCount");
		vw_FindShaderUniformLocation(GLSLShaderType2, "PointLightCount");
		vw_FindShaderUniformLocation(GLSLShaderType2, "SpeedData1");
		vw_FindShaderUniformLocation(GLSLShaderType2, "SpeedData2");

		vw_FindShaderUniformLocation(GLSLShaderType3, "Texture1");
		vw_FindShaderUniformLocation(GLSLShaderType3, "Texture2");
		vw_FindShaderUniformLocation(GLSLShaderType3, "DirectLightCount");
		vw_FindShaderUniformLocation(GLSLShaderType3, "PointLightCount");
		vw_FindShaderUniformLocation(GLSLShaderType3, "NeedMultitexture");
		vw_FindShaderUniformLocation(GLSLShaderType3, "ShadowMap");
		vw_FindShaderUniformLocation(GLSLShaderType3, "xPixelOffset");
		vw_FindShaderUniformLocation(GLSLShaderType3, "yPixelOffset");
		vw_FindShaderUniformLocation(GLSLShaderType3, "NormalMap");
		vw_FindShaderUniformLocation(GLSLShaderType3, "NeedNormalMapping");
		vw_FindShaderUniformLocation(GLSLShaderType3, "PCFMode");
	}
	// еще одна проверка перед тем как будем использовать шадовмеп
	// если не смогли загрузить шейдеры, то делать с шадовмеп нечего
	if (!GameConfig().UseGLSL120)
		ChangeGameConfig().ShadowMap = 0;

	// инициализация менеджера частиц (обязательно после загрузки шейдеров)
	// VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
	vw_InitParticleSystems(GameConfig().UseGLSL120, GameConfig().VisualEffectsQuality + 1.0f);

	for (unsigned i = 0; i < LoadListCount; i++) {
		switch (LoadList[i].FileType) {
		// 2d текстуры
		case 0:
			// установки параметров
			vw_SetTextureAlpha(LoadList[i].Red, LoadList[i].Green, LoadList[i].Blue);
			vw_SetTextureProp(LoadList[i].TextFilter, LoadList[i].TextAnisotropy * GameConfig().AnisotropyLevel,
					  LoadList[i].TextWrap, LoadList[i].Alpha, LoadList[i].AlphaMode, LoadList[i].MipMap);

			vw_LoadTexture(LoadList[i].FileName);
			break;

		// текстуры
		case 1:
			// установки параметров
			vw_SetTextureAlpha(LoadList[i].Red, LoadList[i].Green, LoadList[i].Blue);
			vw_SetTextureProp(LoadList[i].TextFilter, LoadList[i].TextAnisotropy * GameConfig().AnisotropyLevel,
					  LoadList[i].TextWrap, LoadList[i].Alpha, LoadList[i].AlphaMode, LoadList[i].MipMap);

			// если это карта нормалей, но у нас не включены шейдеры - пропускаем
			// (если эту опцию убрать - можно объеденить 2д текстуры и 3д текстуры)
			if (!GameConfig().UseGLSL120 &&
			    (LoadList[i].FileName.find("models/normalmap") != std::string::npos))
				break;

			vw_LoadTexture(LoadList[i].FileName);
			break;

		// предварит. загрузка моделей
		case 2:
			vw_LoadModel3D(LoadList[i].FileName, LoadList[i].TriangleSizeLimit,
				       LoadList[i].NeedTangentAndBinormal && GameConfig().UseGLSL120);
			break;

		// загрузка sfx
		case 4:
			// если вообще можем играть звуки
			if (vw_GetAudioStatus())
				vw_LoadSoundBuffer(LoadList[i].FileName);
			break;
		}

		RealLoadedAssets += LoadList[i].Value;

		// рисуем текущее состояние загрузки, если не рисуем логотип
		DrawLoading(RealLoadedAssets, AllDrawLoading, LastDrawTime, LoadImageTexture);
	}

	// убираем картинку загрузки
	vw_ReleaseTexture(LoadImageTexture);

	PostLoadGameData(LoadType);
}
