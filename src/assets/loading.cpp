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

#include "../core/core.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../audio/audio.h"
#include "../main.h"
#include "audio.h"
#include "model3d.h"


struct sLoadList {
	// имя файла
	std::string FileName;
	// тип файла (0-2д текстура, 1-текстура, 2-VW3D, 3-music, 4-sfx)
	int FileType;
	// вес данного объекта при загрузке (для текстур берем кб)
	int Value;
	// альфа канал, если нужно
	bool Alpha;
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
	{"menu/astromenace.tga",	0, 512, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button384_back.tga",	0, 192, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button384_in.tga",	0, 96, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button384_out.tga",	0, 96, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_back.tga",	0, 192, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_in.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_out.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button256_off.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/blackpoint.tga",		0, 0, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/whitepoint.tga",		0, 0, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/line.tga",		0, 4, true,  eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/perc.tga",		0, 2, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/perc_none.tga",		0, 2, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/checkbox_main.tga",	0, 5, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/checkbox_in.tga",	0, 5, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// панельки и кнопки вывода информации в меню модернизации корабля
	{"menu/workshop_panel1.tga",			0, 270, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel1+.tga",			0, 270, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel2.tga",			0, 492, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel2+.tga",			0, 492, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel3.tga",			0, 110, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel4.tga",			0, 54, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/workshop_panel5.tga",			0, 899, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/ammo.tga",				0, 8, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/en/menu/button_weaponry_out.tga",	0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/en/menu/button_weaponry_in.tga",		0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/de/menu/button_weaponry_out.tga",	0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/de/menu/button_weaponry_in.tga",		0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/ru/menu/button_weaponry_out.tga",	0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/ru/menu/button_weaponry_in.tga",		0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/pl/menu/button_weaponry_out.tga",	0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/pl/menu/button_weaponry_in.tga",		0, 6, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/back_spot.tga",				0, 256, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/back_spot2.tga",				0, 256, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// диалоги
	{"menu/dialog512_256.tga",	0, 706, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/dialog512_512.tga",	0, 1242, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/dialog768_600.tga",	0, 2131, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// панельки
	{"menu/panel444_333_back.tga",		0, 639, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/panel444_333_border.tga",	0, 705, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/panel800_444_back.tga",		0, 1631, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// малые кнопки для диалогов и панелек
	{"menu/button_dialog200_out.tga",	0, 57, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog200_in.tga",	0, 57, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog200_off.tga",	0, 57, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog128_out.tga",	0, 39, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog128_in.tga",	0, 39, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/button_dialog128_off.tga",	0, 39, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/arrow_list_up.tga",		0, 69, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"menu/arrow_list_down.tga",		0, 69, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
// иконки отображения в меню выбора миссий
	{"script/mission1_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission2_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission3_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission4_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission5_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission6_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission7_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission8_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission9_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission10_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission11_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission12_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission13_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission14_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"script/mission15_icon.tga",	0, 12, false,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// иконки систем корабля
	{"menu/system_engine1.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_engine2.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_engine3.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_engine4.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan1.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan2.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan3.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_mechan4.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power1.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power2.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power3.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_power4.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect1.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect2.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect3.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_protect4.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target1.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target2.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target3.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_target4.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/system_empty.tga",	0, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// иконки оружия для передаскивания-отображения в слотах оружия
	{"menu/weapon1_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon2_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon3_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon4_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon5_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon6_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon7_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon8_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon9_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon10_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon11_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon12_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon13_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon14_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon15_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon16_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon17_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon18_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon19_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon_on_icon.tga",	0, 32, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/weapon_empty_icon.tga",	0, 256, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// курсор
	{"menu/cursor.tga",		0, 16, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"menu/cursor_shadow.tga",	0, 16, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
// спец эффекты
	{"gfx/flare.tga",	1, 16, true,  eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/flare1.tga",	1, 16, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/flare2.tga",	1, 16, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/flare3.tga",	1, 16, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail1.tga",	1, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail2.tga",	1, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail3.tga",	1, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail4.tga",	1, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"gfx/trail5.tga",	1, 64, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},

// 2д часть
	{"game/nums.tga",			0, 104, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/ammo.tga",			0, 2, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/energy.tga",			0, 2, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/game_panel.tga",			0, 296, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/game_panel2.tga",		0, 347, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/game_panel_el.tga",		0, 256, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/game_num.tga",			0, 31, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/weapon_panel_left.tga",		0, 56, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/weapon_panel_right.tga",		0, 56, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"game/weapon_ammo.tga",		0, 1, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"game/weapon_energy.tga",		0, 1, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, false, -1.0f, false},
	{"lang/en/game/mission.tga",		0, 64, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/en/game/missionfailed.tga",	0, 168, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/en/game/pause.tga",		0, 64, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/de/game/mission.tga",		0, 64, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/de/game/missionfailed.tga",	0, 168, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/de/game/pause.tga",		0, 64, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/ru/game/mission.tga",		0, 64, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/ru/game/missionfailed.tga",	0, 168, true,  eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"lang/ru/game/pause.tga",		0, 64, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/tile_back.tga",		1, 4096/2, true,  eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"skybox/tile_stars.tga",		1, 4096/2, true,  eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// текстура брони для кораблей землян
	{"models/earthfighter/sf-text00.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text04.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text05.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text06.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text07.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text08.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text09.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-text10.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum01.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum02.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum03.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/sf-illum04.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/lnch12.tga",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/lnch34.tga",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/earthfighter/rockets.tga",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
// мины
	{"models/mine/mine1.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine1i.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine2.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine2i.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine3.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine3i.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine4.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/mine/mine4i.tga",	1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},

// AlienFighter – load alien fighters textures.
	{"models/alienfighter/al-text04.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienfighter/al-illum04.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},

// Pirate – load all pirate data (vehicles, military buildings, ships...) testures.
	{"models/gr-01.vw2d",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-02.vw2d",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-03.vw2d",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-04.vw2d",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/bomber_nm.tga",	1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-05.vw2d",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-06.vw2d",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/gr-07.vw2d",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/track.vw2d",			1, 48, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/turret/turrets.tga",		1, 192, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},

// BasePart – load pirate base textures.
	{"models/spacebase/allalpha.tga",	1, 768, true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/spacebase/metal.tga",		1, 1024, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},

// Asteroid – load asteroids (for AsteroidField) textures.
	{"models/space/asteroid-01.tga",	1, 96, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},

// Planet – load planets textures.
	{"models/planet/asteroid.tga",			1, 512, true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/clouds.tga",			1, 1024, true, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, false, -1.0f, false},
	{"models/planet/q_class2.tga",			1, 192, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/a_class4.tga",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/m_class7.tga",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/planet/d_class3.tga",			1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/m_class7_nm.tga",		1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/d_class3_nm.tga",		1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/a_class4_nm.tga",		1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/planet_asteroids_nm.tga",	1, 384, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/q_class2_nm.tga",		1, 192, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},

// AlienMotherShip – load alien motherships textures.
	{"models/alienmothership/alm-text02.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-text03.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-text04.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-text08.vw2d",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum02.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum03.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum04.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/alienmothership/alm-illum08.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/alien_mothership_nm.tga",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},

// Building – load buildings textures.
	{"models/building/bld.vw2d",		1, 768, false, eAlphaCreateMode::GREYSC, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/normalmap/buildings_nm.tga",	1, 768, false, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},
	{"models/building/bld_illum.vw2d",	1, 1024, true, eAlphaCreateMode::EQUAL, eTextureWrapMode::REPEAT, eTextureBasicFilter::TRILINEAR, 1, true, true, -1.0f, false},

// StarSystem1 – load StarSystem 1 SkyBox textures.
	{"skybox/1/skybox_back6.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_bottom4.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_front5.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_left2.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_right1.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/1/skybox_top3.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},

//StarSystem2 – load StarSystem 2 SkyBox textures.
	{"skybox/2/skybox_back6.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_bottom4.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_front5.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_left2.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_right1.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
	{"skybox/2/skybox_top3.tga",	1, 3072/2, false,  eAlphaCreateMode::GREYSC, eTextureWrapMode::CLAMP_TO_EDGE, eTextureBasicFilter::BILINEAR, 0, false, true, -1.0f, false},
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
		AudioLoop();
	}

	vw_SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------------
// процедура прорисовки процента загрузки данных
//------------------------------------------------------------------------------------
static void DrawLoading(unsigned int Current, unsigned int AllDrawLoading, uint32_t &LastDrawTime, GLtexture LoadImageTexture)
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
			QuitFromMainLoop();
			break;
		default:
			break;
		}
	}

	LastDrawTime = SDL_GetTicks();

	// small delay for last call with 100%
	if (Current == AllDrawLoading)
		SDL_Delay(100);
}

void ChangeTexturesAnisotropyLevel()
{
	for (unsigned i = 0; i < LoadListCount; i++) {
		if (((LoadList[i].FileType == 0) || // "2D" texture
		     (LoadList[i].FileType == 1)) && // "3D" texture
		    LoadList[i].TextAnisotropy) {
			GLtexture tmpTexture = vw_FindTextureByName(LoadList[i].FileName);
			vw_BindTexture(0, tmpTexture);
			vw_SetTextureAnisotropy(GameConfig().AnisotropyLevel);
		}
	}
	vw_BindTexture(0, 0);
}

/*
 * Load game assets.
 */
void LoadGameData()
{
	unsigned int AllDrawLoading{0};
	// получаем значение (реальное, по весам)
	for (unsigned i = 0; i < LoadListCount; i++) {
		AllDrawLoading += LoadList[i].Value;
	}

	AllDrawLoading += GetAudioAssetsValue();
	AllDrawLoading += GetModel3DAssetsValue();

	// если будем загружать шейдеры - делаем поправку общего кол-ва
	bool NeedLoadShaders = false;
	if (vw_GetDevCaps().OpenGL_2_0_supported &&
	    vw_GetDevCaps().OpenGL_2_1_supported &&
	    GameConfig().UseGLSL120) {
		AllDrawLoading += GLSLLoadListCount * 100;
		NeedLoadShaders = true;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем логотип компании
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выводим логотип Viewizard
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1, eTextureWrapMode::CLAMP_TO_EDGE,
			  false, eAlphaCreateMode::EQUAL, false);
	GLtexture ViewizardLogoTexture = vw_LoadTexture("loading/viewizardlogo.tga");
	DrawViewizardLogo(ViewizardLogoTexture);
	vw_ReleaseTexture(ViewizardLogoTexture);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем список
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем картинки вывода загрузки
	vw_SetTextureAlpha(0, 0, 0);
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1, eTextureWrapMode::CLAMP_TO_EDGE,
			  true, eAlphaCreateMode::GREYSC, false);
	vw_LoadTexture("loading/loading_line.tga");
	vw_LoadTexture("loading/loading_back.tga");
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1, eTextureWrapMode::CLAMP_TO_EDGE,
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
	unsigned int RealLoadedAssets{0};
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
				// important, update music buffers
				AudioLoop();
			}
		}

		// find all shaders by name
		GLSLShaderType1 = vw_FindShaderByName("PerPixelLight");
		GLSLShaderType2 = vw_FindShaderByName("PerPixelLight_Explosion");
		GLSLShaderType3 = vw_FindShaderByName("PerPixelLight_ShadowMap");

		// find and store uniform location for all shaders, in real, we don't need
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


	auto UpdateLoadStatus = [&] (unsigned AssetValue) {
		RealLoadedAssets += AssetValue;
		DrawLoading(RealLoadedAssets, AllDrawLoading, LastDrawTime, LoadImageTexture);
		// important, update music buffers
		AudioLoop();
	};
	ForEachAudioAssetLoad(UpdateLoadStatus);
	ForEachModel3DAssetLoad(UpdateLoadStatus);

	for (unsigned i = 0; i < LoadListCount; i++) {
		switch (LoadList[i].FileType) {
		// "2D" texture
		case 0:
			// установки параметров
			// vw_SetTextureAlpha(0, 0, 0) called in code above
			vw_SetTextureProp(LoadList[i].TextFilter,
					  LoadList[i].TextAnisotropy ? GameConfig().AnisotropyLevel : 1,
					  LoadList[i].TextWrap, LoadList[i].Alpha, LoadList[i].AlphaMode, LoadList[i].MipMap);

			vw_LoadTexture(LoadList[i].FileName);
			break;

		// "3D" texture
		case 1:
			// установки параметров
			// vw_SetTextureAlpha(0, 0, 0) called in code above
			vw_SetTextureProp(LoadList[i].TextFilter,
					  LoadList[i].TextAnisotropy ? GameConfig().AnisotropyLevel : 1,
					  LoadList[i].TextWrap, LoadList[i].Alpha, LoadList[i].AlphaMode, LoadList[i].MipMap);

			// если это карта нормалей, но у нас не включены шейдеры - пропускаем
			// (если эту опцию убрать - можно объеденить 2д текстуры и 3д текстуры)
			if (!GameConfig().UseGLSL120 &&
			    (LoadList[i].FileName.find("models/normalmap") != std::string::npos))
				break;

			vw_LoadTexture(LoadList[i].FileName);
			break;
		}

		UpdateLoadStatus(LoadList[i].Value);
	}

	// убираем картинку загрузки
	vw_ReleaseTexture(LoadImageTexture);
}
