/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "Game.h"




//------------------------------------------------------------------------------------
// локальные переменные
//------------------------------------------------------------------------------------

// прорисовка хинтов во время загрузки
bool NeedShowHint = false;
const char *LoadingHints[6] =
{"9_16Line1",
"9_16Line2",
"9_16Line3",
"9_16Line4",
"9_16Line5",
"9_16Line6"};




//------------------------------------------------------------------------------------
// локальная структура данных загрузки
//------------------------------------------------------------------------------------
struct LoadList
{
	// имя файла
	char	FileName[MAX_PATH];
	// тип файла (0-2д текстура, 1-текстура, 2-VW3D, 3-music, 4-sfx)
	int		FileType;
	// вес данного объекта при загрузки (для текстур берем кбайты чистые, т.е. если она в рав формате типа бмп)
	int		Value;
	// альфа канал, если нужно
	bool	Alpha;
	// цвет альфа канала
	BYTE Red, Green, Blue;
	// режим создания альфа канала
	int		AlphaMode;
	// фильтр текстуры
	int		TextWrap;
	int		TextFiltr;
	bool	MipMap;
	// нужно ли для этой текстуры сжатие
	bool	NeedCompression;
	// для 3д моделей, если 1.0f делать структуру с мелкими треугольниками
	float	TriangleSizeLimit;
	// для 3д моделей, если надо - делаем тангенты и бинормали
	bool	NeedTangentAndBinormal;
};
// ВАЖНО!!!
// 1) текстуры должны стоять первые, а модели последние
// 2) одна и таже модель не должна вызываться с разными начальными углами и ресайзом. Для таких случаев нужно 2 модели (одинаковые)... иначе будут проблемы с VBO




// подключаем список загрузки
LoadList	*CurrentList = 0;
int CurrentListCount = 0;









//------------------------------------------------------------------------------------
// данные загрузки шейдеров
//------------------------------------------------------------------------------------
eGLSL 	*GLSLShaderType1 = 0;
eGLSL 	*GLSLShaderType2 = 0;
eGLSL 	*GLSLShaderType3 = 0;
int 	UniformLocations[100];

struct sGLSLLoadList
{
	char Name[MAX_PATH];
	char VertexShaderFileName[MAX_PATH];
	char FragmentShaderFileName[MAX_PATH];
};
const int GLSLLoadListCount = 5;
sGLSLLoadList	GLSLLoadList[GLSLLoadListCount] =
{
{"ParticleSystem", "DATA/GLSL/particle.vert", "DATA/GLSL/particle.frag"},
{"SpaceStars", "DATA/GLSL/particle_stars.vert", "DATA/GLSL/particle.frag"},
{"PerPixelLight", "DATA/GLSL/light.vert", "DATA/GLSL/light.frag"},
{"PerPixelLight_ShadowMap", "DATA/GLSL/light_shadowmap.vert", "DATA/GLSL/light_shadowmap.frag"},
{"PerPixelLight_Explosion", "DATA/GLSL/light_explosion.vert", "DATA/GLSL/light_explosion.frag"},
};







//------------------------------------------------------------------------------------
// данные загрузки меню
//------------------------------------------------------------------------------------


#define TEXTURE_NO_MIPMAP	RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE

// сколько нужно загружать в листе меню
const int	MenuLoadListCount = 187;
// лист загрузки меню
LoadList	MenuLoadList[MenuLoadListCount] =
{
// текстуры меню... кнопки, диалоги, название игры
{"DATA/MENU/astromenace.tga",			0, 512, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button384_back.tga",		0, 192, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button384_in.tga",			0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button384_out.tga",			0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button256_back.tga",		0, 192, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button256_in.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button256_out.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button256_off.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/blackpoint.tga",			0, 0, true,  255,255,255, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/whitepoint.tga",			0, 0, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/line.tga",					0, 4, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/perc.tga",					0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/perc_none.tga",				0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/checkbox_main.tga",			0, 5, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/checkbox_in.tga",			0, 5, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// иконки для отображения в меню credits
{"DATA/CREDITS/sdl.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/svn.tga",				0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/opengl.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/openal.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/codeblocks.tga",			0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/gnugcc.tga",				0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/gentoo.tga",				0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/freetype.tga",			0, 16, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/oggvorbis.tga",			0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/CREDITS/gimp.tga",				0, 32, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// панельки и кнопки вывода информации в меню модернизации корабля
{"DATA/MENU/workshop_panel1.tga",		0, 270, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/workshop_panel1+.tga",		0, 270, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/workshop_panel2.tga",		0, 492, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/workshop_panel2+.tga",		0, 492, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/workshop_panel3.tga",		0, 110, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/workshop_panel4.tga",		0, 54, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/workshop_panel5.tga",		0, 899, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/ammo.tga",					0, 8, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/button_weaponry_out.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/button_weaponry_in.tga",	0, 6, false, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/back_spot.tga",				0, 256, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/back_spot2.tga",			0, 256, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// диалоги
{"DATA/MENU/dialog512_256.tga",			0, 706, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/dialog512_512.tga",			0, 1242, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/dialog768_600.tga",			0, 2131, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// панельки
{"DATA/MENU/panel444_333_back.tga",		0, 639, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/panel444_333_border.tga",	0, 705, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/panel800_444_back.tga",		0, 1631, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// малые кнопки для диалогов и панелек
{"DATA/MENU/button_dialog200_out.tga",	0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog200_in.tga",	0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog200_off.tga",	0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog128_out.tga",	0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog128_in.tga",	0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog128_off.tga",	0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/arrows_on.tga",				0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/arrows_off.tga",			0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/arrows_blue.tga",			0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/arrow_list_up.tga",		0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/arrow_list_down.tga",		0, 69, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
// иконки отображения в меню выбора миссий
{"DATA/SCRIPT/mission1_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission2_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission3_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission4_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission5_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission6_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission7_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission8_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission9_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission10_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission11_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission12_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission13_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission14_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/SCRIPT/mission15_icon.tga",			0, 12, false,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// иконки систем корабля
{"DATA/MENU/system_engine1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_engine2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_engine3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_engine4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_mechan1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_mechan2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_mechan3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_mechan4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_power1.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_power2.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_power3.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_power4.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_protect1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_protect2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_protect3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_protect4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_target1.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_target2.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_target3.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_target4.tga",			0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/system_empty.tga",				0, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// иконки оружия для передаскивания-отображения в слотах оружия
{"DATA/MENU/weapon1_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon2_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon3_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon4_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon5_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon6_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon7_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon8_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon9_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon10_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon11_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon12_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon13_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon14_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon15_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon16_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon17_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon18_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon19_icon.tga",			0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon_on_icon.tga",		0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon_empty_icon.tga",		0, 256, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// курсор
{"DATA/MENU/cursor.tga",				0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/cursor_shadow.tga",			0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// текстура брони для кораблей землян
{"DATA/MODELS/EARTHFIGHTER/sf-text00.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text05.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text06.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text07.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text08.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text09.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text04.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text10.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum01.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum02.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum04.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch12.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch34.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/rockets.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура истребителей пришельцев
{"DATA/MODELS/ALIENFIGHTER/al-text04.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-illum04.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура космических объектов
{"DATA/MODELS/SPACE/asteroid-01.tga",			1, 96, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// планеты
{"DATA/MODELS/PLANET/asteroid.tga",			1, 512, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/clouds.tga",			1, 1024, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
{"DATA/MODELS/PLANET/q_class2.tga",				1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/a_class4.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/m_class7.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/d_class3.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/m_class7_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/d_class3_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/a_class4_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/planet_asteroids_nm.tga",	1, 384, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/q_class2_nm.tga",		1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура наземной техники-сооружений
{"DATA/MODELS/gr-01.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-02.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-03.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-04.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/bomber_nm.tga",				1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-05.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-06.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-07.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/track.VW2D",							1, 48, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},

// текстура больших кораблей пришельцев
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text02.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text03.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text04.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text08.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum02.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum03.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum04.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum08.VW2D",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/alien_mothership_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// турели пиратов
{"DATA/MODELS/TURRET/turrets.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// текстура зданий
{"DATA/MODELS/BUILDING/bld.VW2D",				1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/buildings_nm.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/BUILDING/bld_illum.VW2D",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// космическая база
{"DATA/MODELS/SPACEBASE/allalpha.tga",		1, 1024, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/SPACEBASE/metal.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
// мины
{"DATA/MODELS/MINE/mine1.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine1i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine2.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine2i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine3.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine3i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine4.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine4i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},

// скайбокс - космос, загружаем только те, что нужно - экономим видео память
{"DATA/SKYBOX/1/skybox_bottom4.tga",			1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/1/skybox_front5.tga",				1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/tile_back.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/SKYBOX/tile_stars.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// спец эффекты
{"DATA/GFX/flare.tga",							1, 16, true,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/flare1.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/flare2.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/flare3.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail1.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail2.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail3.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail4.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail5.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},

};

















const int	GameLevelsLoadListCount = 87+41+35;
LoadList	GameLevelsLoadList[GameLevelsLoadListCount] =
{
// 2д часть
{"DATA/MENU/cursor.tga",						0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/cursor_shadow.tga",					0, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/blackpoint.tga",					0, 0, true, 255,255,255, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/dialog512_256.tga",					0, 706, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog200_out.tga",			0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog200_in.tga",			0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog200_off.tga",			0, 57, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog128_out.tga",			0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog128_in.tga",			0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button_dialog128_off.tga",			0, 39, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button384_back.tga",				0, 192, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button384_in.tga",					0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/button384_out.tga",					0, 96, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/dialog512_512.tga",					0, 1242, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/MENU/perc.tga",							0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/perc_none.tga",						0, 2, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/GAME/pause.tga",							0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/nums.tga",							0, 104, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/mission.tga",						0, 64, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/ammo.tga",							0, 2, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/GAME/energy.tga",						0, 2, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/GAME/game_panel.tga",					0, 296, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/game_panel2.tga",					0, 347, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/game_panel_el.tga",					0, 256, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/game_num.tga",						0, 31, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/GAME/weapon_panel_left.tga",				0, 56, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/weapon_panel_right.tga",			0, 56, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/GAME/weapon_ammo.tga",					0, 1, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/GAME/weapon_energy.tga",					0, 1, true, 0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon_on_icon.tga",				0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/GAME/missionfailed.tga",					0, 168, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/tile_back.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/SKYBOX/tile_stars.tga",					1, 4096/2, true,  0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// спец эффекты
{"DATA/GFX/flare.tga",							1, 16, true,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/flare1.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/flare2.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/flare3.tga",							1, 16, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail1.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail2.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail3.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail4.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
{"DATA/GFX/trail5.tga",							1, 64, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, RI_TEXTURE_BILINEAR, true, false, -1.0f, false},
// иконки оружия для перетаскивания-отображения в слотах оружия
{"DATA/MENU/weapon1_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon2_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon3_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon4_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon5_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon6_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon7_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon8_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon9_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon10_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon11_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon12_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon13_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon14_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon15_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon16_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon17_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon18_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
{"DATA/MENU/weapon19_icon.tga",					0, 32, true,  0,0,0, TX_ALPHA_EQUAL, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, false, -1.0f, false},
// текстура брони для кораблей землян
{"DATA/MODELS/EARTHFIGHTER/sf-text00.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text04.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text05.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text06.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text07.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text08.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text09.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-text10.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum01.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum02.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-illum04.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch12.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch34.tga",			1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/EARTHFIGHTER/rockets.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// звуки игры (только игры), грузим тут, иначе плохо, когда подгружает во время игры
{"DATA/SFX/weapon1probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weapon2probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weapon3probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weapon4probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weapon5probl.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/explosion1.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/explosion2.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/explosion3.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/explosion4.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire1.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire2.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire3.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire4.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire5.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire6.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire7.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire8.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire9.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire10.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire11.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire12.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire13.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire14.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire15.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire16.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire17.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire18.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/weaponfire19.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/kinetichit.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/ionhit.wav",							4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/plasmahit.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/antimaterhit.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/gausshit.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/SFX/lowlife.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/Attention.wav",					4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/EngineMalfunction.wav",			4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/MissileDetected.wav",				4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/PowerSupplyReestablished.wav",		4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/PrepareForAction.wav",				4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/ReactorMalfunction.wav",			4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/Warning.wav",						4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/WeaponDamaged.wav",				4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/WeaponDestroyed.wav",				4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
{"DATA/VOICE/WeaponMalfunction.wav",			4, 20, false, 0,0,0, 0, 0, 0, true, true, -1.0f, false},
// мины
{"DATA/MODELS/MINE/mine1.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine1i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine2.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine2i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine3.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine3i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine4.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/MINE/mine4i.tga",				1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// модели мин
{"DATA/MODELS/MINE/mine-01.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MINE/mine-02.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MINE/mine-03.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MINE/mine-04.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// корабли землян + их оружие
{"DATA/MODELS/EARTHFIGHTER/sf-01.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-02.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-03.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-04.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-05.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-06.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-07.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-08.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-09.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-10.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-11.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-12.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-13.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-14.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-15.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-16.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-17.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-18.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-19.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-20.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-21.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/sf-22.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/weapons.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch1.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch2.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch3.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/lnch4.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/missile.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/swarm.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/torpedo.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/EARTHFIGHTER/nuke.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
};



const int	AlienFighterLoadListCount = 19;
LoadList	AlienFighterLoadList[AlienFighterLoadListCount] =
{
// AlienFighter – load alien fighters textures.
{"DATA/MODELS/ALIENFIGHTER/al-text04.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-illum04.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// малые корабли пришельцев
{"DATA/MODELS/ALIENFIGHTER/al-01.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-02.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-03.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-04.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-05.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-06.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-07.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-08.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-09.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-10.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-11.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-12.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-13.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-14.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-15.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-16.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/ALIENFIGHTER/al-17.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
};




const int	PirateLoadListCount = 51;
LoadList	PirateLoadList[PirateLoadListCount] =
{
// Pirate – load all pirate data (vehicles, military buildings, ships...) testures.
{"DATA/MODELS/gr-01.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-02.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-03.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-04.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/bomber_nm.tga",				1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-05.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-06.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/gr-07.VW2D",							1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/track.VW2D",							1, 48, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/TURRET/turrets.tga",					1, 192, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// корабли пиратов
{"DATA/MODELS/PIRATESHIP/gunship-01.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/bomber-07.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/gunship-03.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/gunship-04.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/gunship-02.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/bomber-03.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/bomber-02.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/bomber-04.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/PIRATESHIP/bomber-05.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/PIRATESHIP/bomber-06.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
// турелей
{"DATA/MODELS/TURRET/turret-01.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TURRET/turret-02.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// гусенечный транспорт
{"DATA/MODELS/TRACKED/engineering-01.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/apc-aa-02.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/apc-aa-01.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/apc-03.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/apc-01.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-11.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-10.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-09.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-08.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-07.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-06.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-05.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-03.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/TRACKED/tank-01.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// колесный транспорт
{"DATA/MODELS/WHEELED/r-launcher-01.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/WHEELED/apc-04.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/WHEELED/apc-02.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/WHEELED/jeep-05.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/WHEELED/jeep-04.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/WHEELED/jeep-03.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/WHEELED/jeep-02.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/WHEELED/jeep-01.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
// военные сооружения
{"DATA/MODELS/MILITARYBUILDING/artiler-gun-02.VW3D",2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MILITARYBUILDING/artiler-gun-01.VW3D",2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MILITARYBUILDING/aa-gun-05.VW3D",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MILITARYBUILDING/aa-gun-04.VW3D",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MILITARYBUILDING/aa-gun-03.VW3D",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MILITARYBUILDING/aa-gun-02.VW3D",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
{"DATA/MODELS/MILITARYBUILDING/aa-gun-01.VW3D",		2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, false},
};



const int	BasePartLoadListCount = 28;
LoadList	BasePartLoadList[BasePartLoadListCount] =
{
// BasePart – load pirate base textures.
{"DATA/MODELS/SPACEBASE/allalpha.tga",		1, 768, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/SPACEBASE/metal.tga",			1, 1024, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
{"DATA/MODELS/PLANET/d_class3.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/d_class3_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// геометрия базы пиратов
{"DATA/MODELS/SPACEBASE/1/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/1/2.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/1/3.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/1/4.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/1/5.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/2/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/2/2.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/2/3.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/2/4.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/2/5.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/3/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACEBASE/4/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACEBASE/5/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACEBASE/6/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/6/2.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/6/3.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/6/4.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/6/5.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/7/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACEBASE/8/1.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/8/2.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/8/3.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/8/4.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACEBASE/8/5.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
};



const int	AsteroidLoadListCount = 3+25;
LoadList	AsteroidLoadList[AsteroidLoadListCount] =
{
// Asteroid – load asteroids (for AsteroidField) textures.
{"DATA/MODELS/SPACE/asteroid-01.tga",				1, 96, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// большие астероиды
{"DATA/MODELS/PLANET/d_class3.tga",					1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/d_class3_nm.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// геометрия больших астероидов
{"DATA/MODELS/SPACE/bigasteroid-01.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACE/bigasteroid-02.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACE/bigasteroid-03.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACE/bigasteroid-04.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/SPACE/bigasteroid-05.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
// астероиды
{"DATA/MODELS/SPACE/asteroid-010.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-011.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-012.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-013.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-014.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-015.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-016.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-017.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-018.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-019.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0110.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0111.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0112.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0113.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0114.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0115.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0116.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0117.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0118.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
{"DATA/MODELS/SPACE/asteroid-0119.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, false},
};



const int	PlanetLoadListCount = 11+6;
LoadList	PlanetLoadList[PlanetLoadListCount] =
{
// Planet – load planets textures.
{"DATA/MODELS/PLANET/asteroid.tga",			1, 512, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/clouds.tga",			1, 1024, true, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, false, -1.0f, false},
{"DATA/MODELS/PLANET/q_class2.tga",				1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/a_class4.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/m_class7.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/PLANET/d_class3.tga",			1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/m_class7_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/d_class3_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/a_class4_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/planet_asteroids_nm.tga",	1, 384, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/q_class2_nm.tga",		1, 192, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// модели планет
{"DATA/MODELS/PLANET/aplanet.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/PLANET/dplanet.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/PLANET/gplanet.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/PLANET/moon.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/PLANET/planet5.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/PLANET/planet6.VW3D",					2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
};



const int	AlienMotherShipLoadListCount = 9+8;
LoadList	AlienMotherShipLoadList[AlienMotherShipLoadListCount] =
{
// AlienMotherShip – load alien motherships textures.
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text02.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text03.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text04.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-text08.VW2D",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum02.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum03.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum04.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-illum08.VW2D",	1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/alien_mothership_nm.tga",	1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// большие корабли пришельцев
{"DATA/MODELS/ALIENMOTHERSHIP/alm-01.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-02.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-03.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-04.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-05.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-06.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-07.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
{"DATA/MODELS/ALIENMOTHERSHIP/alm-08.VW3D",			2, 20, true,  0,0,0, 0, 0, 0, true, false, 2.0f, true},
};



const int	BuildingLoadListCount = 3+11;
LoadList	BuildingLoadList[BuildingLoadListCount] =
{
// Building – load buildings textures.
{"DATA/MODELS/BUILDING/bld.VW2D",				1, 768, false, 0,0,0, TX_ALPHA_GREYSC, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/NORMALMAP/buildings_nm.tga",		1, 768, false, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
{"DATA/MODELS/BUILDING/bld_illum.VW2D",			1, 1024, true, 0,0,0, TX_ALPHA_EQUAL, RI_WRAP_U | RI_WRAP_V, RI_TEXTURE_TRILINEAR, true, true, -1.0f, false},
// постройки (мирные)
{"DATA/MODELS/BUILDING/bld-01.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-02.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-03.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-04.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-05.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-06.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-07.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-08.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-09.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-10.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
{"DATA/MODELS/BUILDING/bld-11.VW3D",				2, 20, true,  0,0,0, 0, 0, 0, true, false, -1.0f, true},
};



const int	StarSystem1LoadListCount = 6;
LoadList	StarSystem1LoadList[StarSystem1LoadListCount] =
{
// StarSystem1 – load StarSystem 1 SkyBox textures.
{"DATA/SKYBOX/1/skybox_back6.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/1/skybox_bottom4.tga",				1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/1/skybox_front5.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/1/skybox_left2.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/1/skybox_right1.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/1/skybox_top3.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
};



const int	StarSystem2LoadListCount = 6;
LoadList	StarSystem2LoadList[StarSystem2LoadListCount] =
{
//StarSystem2 – load StarSystem 2 SkyBox textures.
{"DATA/SKYBOX/2/skybox_back6.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/2/skybox_bottom4.tga",				1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/2/skybox_front5.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/2/skybox_left2.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/2/skybox_right1.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
{"DATA/SKYBOX/2/skybox_top3.tga",					1, 3072/2, false,  0,0,0, TX_ALPHA_GREYSC, RI_CLAMP_TO_EDGE, TEXTURE_NO_MIPMAP, false, true, -1.0f, false},
};
















//------------------------------------------------------------------------------------
// процедура прорисовки логотипа
//------------------------------------------------------------------------------------
void DrawViewizardLogo(eTexture *ViewizardLogoTexture)
{
	int		ShowLogoTime = 6000; // сколько нужно показывать логотип
	int		ShowLogoLife = ShowLogoTime; // сколько осталось показывать
	Uint32	ShowLogoPrevTime = SDL_GetTicks();

	vw_SetClearColor(1.0f,1.0f,1.0f,1.0f);

	while (ShowLogoLife > 0)
	{
		RECT SrcRect,DstRect;
		SetRect(&SrcRect, 1,1,511,511);
		int StartX = (Setup.iAspectRatioWidth-510)/2;
		int EndX = StartX+510;
		SetRect(&DstRect, StartX, 128+1, EndX, 640-2);
		float Transp = 1.0f;
		float GreyColor = 1.0f;

		// плавно делаем появление
		if (ShowLogoLife > ShowLogoTime/2.0f)
		{
			Transp = (ShowLogoTime/2.0f - (ShowLogoLife-ShowLogoTime/2.0f))/(ShowLogoTime/2.0f);
		}
		else
		{
			Transp = 1.0f;
			// относительно быстро исчезаем
			if (ShowLogoLife < ShowLogoTime/4.0f)
			{
				GreyColor = ShowLogoLife/(ShowLogoTime/4.0f);
				vw_SetClearColor(GreyColor,GreyColor,GreyColor,1.0f);
			}
		}
		Transp-=.01f; // чтобы всегда был немного прозрачным
		if (Transp < 0.0f) Transp = 0.0f;


		// рисуем
		vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
		vw_Start2DMode(-1,1);

		vw_DrawTransparent(&DstRect, &SrcRect, ViewizardLogoTexture, true, Transp, 0.0f, RI_UL_CORNER, GreyColor, GreyColor, GreyColor);

		vw_End2DMode();
		vw_EndRendering();


		// проверка времени
		ShowLogoLife -= SDL_GetTicks() - ShowLogoPrevTime;
		ShowLogoPrevTime = SDL_GetTicks();
		if (ShowLogoLife <= 0) ShowLogoLife = 0;


		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
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

#ifndef multithread
		// ставим и сюда, иначе не сможем играть во время загрузки
		Audio_LoopProc();
#endif
	}


	vw_SetClearColor(0.0f,0.0f,0.0f,1.0f);
}











//------------------------------------------------------------------------------------
// процедура прорисовки процента загрузки данных
//------------------------------------------------------------------------------------
void DrawLoading(int Current, int AllDrawLoading, float *LastDrawTime, eTexture *LoadImageTexture)
{
	// слишком часто не рисуем
	if (Current != AllDrawLoading) // последний (полный) рисуем всегда
		if ((*LastDrawTime) + 0.035 >= vw_GetTime()) return;

	vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
	vw_Start2DMode(-1,1);

	RECT SrcRect, DstRect;

	// выводим картинку
	SetRect(&SrcRect, 0,0,1024,512);
	SetRect(&DstRect, 0, 64+32,Setup.iAspectRatioWidth,64+32+512);
	vw_DrawTransparent(&DstRect, &SrcRect, LoadImageTexture, false, 1.0f, 0.0f);

	// пишем "загрузка"
	vw_DrawFont(Setup.iAspectRatioWidth/2-vw_FontSize(vw_GetText("11_Loading"))/2, 768-128, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f, vw_GetText("11_Loading"));

	// выводим подложку линии загрузки
	SetRect(&SrcRect, 0,0,256,32);
	int StartX = (Setup.iAspectRatioWidth-256)/2;
	vw_Draw(StartX, 768-64-8 -32, &SrcRect, vw_FindTextureByName("DATA/LOADING/loading_back.tga"), true);

	// выводим линию загрузки
	int loaded = (int)(256.0f*Current/AllDrawLoading);
	SetRect(&SrcRect, 0,0,loaded,16);
	vw_Draw(StartX, 768-64-1 -32, &SrcRect, vw_FindTextureByName("DATA/LOADING/loading_line.tga"), true);


	// выводим хинт при загрузке
	if (NeedShowHint)
	{
		int	Size = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText(LoadingHints[Setup.LoadingHint])))/2;
		vw_DrawFont(Size, 740, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, vw_GetText(LoadingHints[Setup.LoadingHint]));
	}

	vw_End2DMode();
	vw_EndRendering();


	// обработчик окна
	SDL_Event event;
	while ( SDL_PollEvent(&event) )
	{
		switch (event.type)
		{
			case SDL_QUIT:
				Quit = true;
				break;
			default:
				break;
		}
	}

#ifndef multithread
	// ставим и сюда, иначе не сможем играть во время загрузки
	Audio_LoopProc();
#endif

	(*LastDrawTime) = vw_GetTime();
}







//------------------------------------------------------------------------------------
// процедура освобождения данных, что удалять определяем по типу загрузки
//------------------------------------------------------------------------------------
int CurretnLoadedData = -2;
bool ReleaseGameData(int LoadType)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// сбрасываем данные глобальных переменных
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CurrentList = 0;
	CurrentListCount = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// удаляем данные из памяти
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ReleaseAllObject3D();
	vw_ReleaseAllParticleSystems();
	ReleaseAllGameLvlText();
	vw_ReleaseAllLights();
	StarSystemRelease();

	// если это не переход меню-игра, снимаем звук
	vw_ReleaseAllSounds(1);

	// если не менее 128 мб видео памяти - выгружать текстуры вообще не нужно
	if (!Setup.EqualOrMore128MBVideoRAM)
	{
		// если выбрали миссию, и была миссия (т.е. рестарт миссии) - ничего не удаляем-чистим
		if (CurretnLoadedData == LoadType && CurretnLoadedData == 1)
		{
			// эту миссию уже загружали
			CurretnLoadedData = LoadType;
			return true;
		}
		else
		{
			vw_ReleaseAllFontChars(); // (!) всегда перед vw_ReleaseAllTextures
			vw_ReleaseAllTextures();
			CurretnLoadedData = LoadType;
			return false;
		}
	}
	else
	{
		// нужно понять, мы конкретно это загружали или нет
		if (LoadType <= 0) // это меню
		{
			if (LoadedTypes[0]) return true;
			else
			{
				LoadedTypes[0] = true;
				return false;
			}
		}
		else // это миссия
		{
			if (CurrentMission >= 0)
			{
				if (LoadedTypes[CurrentMission+1]) return true;
				else
				{
					LoadedTypes[CurrentMission+1] = true;
					return false;
				}
			}
		}
	}

	return false;
}







// отдельный поток для проигывания музыки при загрузке
#ifdef multithread

//------------------------------------------------------------------------------------
// Процедура звука
//------------------------------------------------------------------------------------
bool LoadSoundThreadNeedOff = false;
int LoadSoundThread(void *UNUSED(data))
{
	LoadSoundThreadNeedOff = false;

	while (!LoadSoundThreadNeedOff)
	{
		Audio_LoopProc();
		SDL_Delay(10);
	}

	return 0;
}

#endif //multithread










//------------------------------------------------------------------------------------
// процедура загрузки данных, тип загрузки, с логотипом (-1) или без (0-10)
//------------------------------------------------------------------------------------
void LoadGameData(int LoadType)
{

	// потом передавать в функцию тип загрузки:
	// -1 - загрузка меню с логотипом
	// 0 - загрузка меню без логотипа
	// 1...10 - загрузка 1...10 уровня


	eTexture *LoadImageTexture = 0;
	int RealLoadedTextures = 0;
	bool NeedLoadShaders = false;
	int AllDrawLoading = 0;
	SDL_Thread *SoundThread = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем, если уже что-то было загружено, если данные для этой миссии-меню загружены - тут вообще нечего делать
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool NeedStartGoto = false;
	if (ReleaseGameData(LoadType)) NeedStartGoto = true;


	// ставим время последней прорисовки
	vw_StartTime();
	float LastDrawTime = vw_GetTime();


	NeedLoadShaders = false;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подключаем список загрузки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(LoadType)
	{
		// меню, загрузка в самом начале
		case -1:
			CurrentList = MenuLoadList;
			CurrentListCount = MenuLoadListCount;
			NeedShowHint = false;
			break;
		// переход игра-меню
		case 0:
			CurrentList = MenuLoadList;
			CurrentListCount = MenuLoadListCount;
			NeedShowHint = true;
			break;

		// уровни игры
		case 1:
		{
			CurrentListCount = GameLevelsLoadListCount;
			NeedShowHint = true;

			// флаги нужно загружать или нет...
			bool StarSystem1 = false;
			bool StarSystem2 = false;
			bool Planet = false;
			bool Asteroid = false;
			bool AlienFighter = false;
			bool BasePart = false;
			bool AlienMotherShip = false;
			bool Building = false;
			bool Pirate = false;

			// по скрипту, смотрим что загружать + считаем сколько позиций
			char *FileName = GetMissionFileName();
			if (FileName == 0)
			{
				fprintf(stderr, "Critical error. Can't find this mission script file or mission list file.\n");
				fprintf(stderr, "Please, check your DATA/SCRIPT/ folder for xml files (aimode, list, missions).\n");
				exit(0);
			}

			cXMLDocument *xmlDoc = new cXMLDocument;


			// читаем данные
			if (!xmlDoc->Load(FileName))
			{
				fprintf(stderr, "Can't find script file or file corrupted: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			// проверяем корневой элемент
			if (strcmp("AstroMenaceScript", xmlDoc->RootXMLEntry->Name))
			{
				fprintf(stderr, "Can't find AstroMenaceScript element in the: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			// переходим на загрузку
			cXMLEntry *xmlEntry = xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Load");
			if (xmlEntry == 0)
			{
				fprintf(stderr, "Can't find Load element in the: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			xmlEntry = xmlEntry->FirstChild;
			if (xmlEntry == 0)
			{
				fprintf(stderr, "Can't find Load element in the: %s\n", FileName);
				delete xmlDoc;
				exit(0);
			}

			// установка прозрачности слоев
			StarsTileStartTransparentLayer1 = 0.2f;
			StarsTileEndTransparentLayer1 = 0.7f;
			StarsTileStartTransparentLayer2 = 0.9f;
			StarsTileEndTransparentLayer2 = 0.7f;

			while (xmlEntry)
			{
				if (!strcmp(xmlEntry->Name, "StarSystem1")) StarSystem1 = true;
				else
				if (!strcmp(xmlEntry->Name, "StarSystem2")) StarSystem2 = true;
				else
				if (!strcmp(xmlEntry->Name, "Planet")) Planet = true;
				else
				if (!strcmp(xmlEntry->Name, "Asteroid")) Asteroid = true;
				else
				if (!strcmp(xmlEntry->Name, "AlienFighter")) AlienFighter = true;
				else
				if (!strcmp(xmlEntry->Name, "BasePart")) BasePart = true;
				else
				if (!strcmp(xmlEntry->Name, "AlienMotherShip")) AlienMotherShip = true;
				else
				if (!strcmp(xmlEntry->Name, "Building")) Building = true;
				else
				if (!strcmp(xmlEntry->Name, "Pirate")) Pirate = true;
				else
				if (!strcmp(xmlEntry->Name, "AIFile")) // загружаем данные по AI
				{
					if (strlen(xmlEntry->Content) > 0)
						InitGameAI(xmlEntry->Content); // "DATA/SCRIPT/aimode.xml"
				}
				else
				if (!strcmp(xmlEntry->Name, "LayersTransp"))
				{
					if (xmlDoc->GetEntryAttribute(xmlEntry, "FirstStart") != 0)
							StarsTileStartTransparentLayer1 = xmlDoc->fGetEntryAttribute(xmlEntry, "FirstStart");
					if (xmlDoc->GetEntryAttribute(xmlEntry, "FirstEnd") != 0)
							StarsTileEndTransparentLayer1 = xmlDoc->fGetEntryAttribute(xmlEntry, "FirstEnd");
					if (xmlDoc->GetEntryAttribute(xmlEntry, "SecondStart") != 0)
							StarsTileStartTransparentLayer2 = xmlDoc->fGetEntryAttribute(xmlEntry, "SecondStart");
					if (xmlDoc->GetEntryAttribute(xmlEntry, "SecondEnd") != 0)
							StarsTileStartTransparentLayer2 = xmlDoc->fGetEntryAttribute(xmlEntry, "SecondEnd");
				}

				// берем следующий элемент по порядку
				xmlEntry = xmlEntry->Next;
			}

			// чистим память, со скриптом работать больше не надо
			delete xmlDoc;


			// считаем сколько там элементов
			if (StarSystem1) CurrentListCount += StarSystem1LoadListCount;
			if (StarSystem2) CurrentListCount += StarSystem2LoadListCount;
			if (Planet) CurrentListCount += PlanetLoadListCount;
			if (Asteroid) CurrentListCount += AsteroidLoadListCount;
			if (AlienFighter) CurrentListCount += AlienFighterLoadListCount;
			if (BasePart) CurrentListCount += BasePartLoadListCount;
			if (AlienMotherShip) CurrentListCount += AlienMotherShipLoadListCount;
			if (Building) CurrentListCount += BuildingLoadListCount;
			if (Pirate) CurrentListCount += PirateLoadListCount;

			// выделяем память
			CurrentList = new LoadList[CurrentListCount];

			// составляем список загрузки

			// копируем основную часть
			int Current = 0;
			memcpy(CurrentList+Current, GameLevelsLoadList, GameLevelsLoadListCount*sizeof(LoadList));
			Current += GameLevelsLoadListCount;

			if (StarSystem1)
			{
				memcpy(CurrentList+Current, StarSystem1LoadList, StarSystem1LoadListCount*sizeof(LoadList));
				Current += StarSystem1LoadListCount;
			}
			if (StarSystem2)
			{
				memcpy(CurrentList+Current, StarSystem2LoadList, StarSystem2LoadListCount*sizeof(LoadList));
				Current += StarSystem2LoadListCount;
			}

			if (Planet)
			{
				memcpy(CurrentList+Current, PlanetLoadList, PlanetLoadListCount*sizeof(LoadList));
				Current += PlanetLoadListCount;
			}
			if (Asteroid)
			{
				memcpy(CurrentList+Current, AsteroidLoadList, AsteroidLoadListCount*sizeof(LoadList));
				Current += AsteroidLoadListCount;
			}
			if (AlienFighter)
			{
				memcpy(CurrentList+Current, AlienFighterLoadList, AlienFighterLoadListCount*sizeof(LoadList));
				Current += AlienFighterLoadListCount;
			}
			if (BasePart)
			{
				memcpy(CurrentList+Current, BasePartLoadList, BasePartLoadListCount*sizeof(LoadList));
				Current += BasePartLoadListCount;
			}
			if (AlienMotherShip)
			{
				memcpy(CurrentList+Current, AlienMotherShipLoadList, AlienMotherShipLoadListCount*sizeof(LoadList));
				Current += AlienMotherShipLoadListCount;
			}
			if (Building)
			{
				memcpy(CurrentList+Current, BuildingLoadList, BuildingLoadListCount*sizeof(LoadList));
				Current += BuildingLoadListCount;
			}
			if (Pirate)
			{
				memcpy(CurrentList+Current, PirateLoadList, PirateLoadListCount*sizeof(LoadList));
				Current += PirateLoadListCount;
			}
		}
			break;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// включаем музыку
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(LoadType)
	{
		// меню, загрузка в самом начале
		case -1:
			GameStatus = MAIN_MENU;
			Audio_LoopProc();
			StartMusicWithFade(0, 4.0f, 4.0f);
			break;
		// переход игра-меню
		case 0:
			GameStatus = MISSION;
			StartMusicWithFade(0, 2.0f, 2.0f);
			break;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// идем на переход именно отсюда, иначе не подключим файл с AI
	// и надо запустить нужную музыку
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedStartGoto) goto AllDataLoaded;




	AllDrawLoading = 0;
	// получаем значение (реальное, по весам)
	for (int i=0; i<CurrentListCount; i++)
	{
		AllDrawLoading += CurrentList[i].Value;
	}



	// если будем загружать шейдеры - делаем поправку общего кол-ва
	if (CAPS->GLSL100Supported && Setup.UseGLSL)
	{
		// там нет ни одного
		if (vw_FindShaderByNum(1) == 0)
		{
			AllDrawLoading += GLSLLoadListCount*100;
			NeedLoadShaders = true;
		}
	}


	// загружаем все по списку
	RealLoadedTextures = 0;



#ifdef multithread
	// поток проигрывания звука
	SoundThread = SDL_CreateThread(LoadSoundThread, 0);
#endif //multithread




	// в самом начале () до прорисовки подложки загрузки - генерируем все возможные символы для меню (чтобы в процессе прорисовки меньше подгружать)
	// если памяти мало, мы очищаем текстуры, надо перегенерировать шрифт и создать новые текстуры
	if ((LoadType == -1) || (!Setup.EqualOrMore128MBVideoRAM))
	{
		// задаем размеры текстуры (всегда степерь 2 ставим, чтобы избежать проблем со старым железом)
		vw_GenerateFontChars(256, 256, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\():;%&`'*#$=[]@^{}_~><–—«»“”|абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЧЦШЩЪЫЬЭЮЯ©®ÄÖÜäöüß°§/");
#ifdef gamedebug
		// проверяем все ли символы из текущего языкового файла вошли в прегенерацию, иначе не сможем потом рисовать меню через одну текстуру
		// смысла гонять постоянно такую проверку нет, один раз сводим все символы языка и не замедляем загрузку поиском
		// + есть часть символов прописанных в коде, так что убирать англ и часть символов нельзя (!)
		vw_CheckFontCharsInText();
#endif // gamedebug
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем логотип компании
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (LoadType == -1)
	{
		// выводим логотип Viewizard
		vw_SetTextureProp(TEXTURE_NO_MIPMAP, RI_CLAMP_TO_EDGE, false, TX_ALPHA_EQUAL, false);
		eTexture *ViewizardLogoTexture = vw_LoadTexture("DATA/LOADING/viewizardlogo.tga", NULL, false);

		DrawViewizardLogo(ViewizardLogoTexture);

		vw_ReleaseTexture(ViewizardLogoTexture);
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем список
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загружаем картинки вывода загрузки
	vw_SetTextureAlpha(0, 0, 0);
	vw_SetTextureProp(TEXTURE_NO_MIPMAP, RI_CLAMP_TO_EDGE, true, TX_ALPHA_GREYSC, false);
	vw_LoadTexture("DATA/LOADING/loading_line.tga", NULL, false);
	vw_LoadTexture("DATA/LOADING/loading_back.tga", NULL, false);
	vw_SetTextureProp(TEXTURE_NO_MIPMAP, RI_CLAMP_TO_EDGE, false, TX_ALPHA_GREYSC, false);

	LoadImageTexture = 0;

	switch (1+vw_iRandNum(3))
	{
		case 1:
			LoadImageTexture = vw_LoadTexture("DATA/LOADING/loading01.tga", NULL, false);
			break;
		case 2:
			LoadImageTexture = vw_LoadTexture("DATA/LOADING/loading02.tga", NULL, false);
			break;
		case 3:
			LoadImageTexture = vw_LoadTexture("DATA/LOADING/loading03.tga", NULL, false);
			break;
		case 4:
			LoadImageTexture = vw_LoadTexture("DATA/LOADING/loading04.tga", NULL, false);
			break;
		// на всякий случай
		default:
			LoadImageTexture = vw_LoadTexture("DATA/LOADING/loading01.tga", NULL, false);
			break;
	}



	//	если нужно, загрузка всех шейдеров (!) обязательно это делать до загрузки моделей
	if (NeedLoadShaders)
	if (CAPS->GLSL100Supported)
	{
		for (int i=0; i<GLSLLoadListCount; i++)
		if (Setup.UseGLSL)
		{

			eGLSL *Program = 0;
			Program = vw_CreateShader(GLSLLoadList[i].Name, GLSLLoadList[i].VertexShaderFileName, GLSLLoadList[i].FragmentShaderFileName);

			if (Program != 0)
			{
				// получаем сразу состояние, смогли прилинковать или нет
				if (!vw_LinkShaderProgram(Program)) Setup.UseGLSL = false;
			}
			else
				Setup.UseGLSL = false;

			RealLoadedTextures += 1000;
			// рисуем текущее состояние загрузки, если не рисуем логотип
			DrawLoading(RealLoadedTextures, AllDrawLoading, &LastDrawTime, LoadImageTexture);
		}

		// сразу находим базовые типы шейдеров для прорисовки 3д моделей
		GLSLShaderType1 = vw_FindShaderByName("PerPixelLight");
		GLSLShaderType2 = vw_FindShaderByName("PerPixelLight_Explosion");
		GLSLShaderType3 = vw_FindShaderByName("PerPixelLight_ShadowMap");

		// находим все юниформы GLSLShaderType1
		UniformLocations[0] = vw_GetUniformLocation(GLSLShaderType1, "Texture1");
		UniformLocations[1] = vw_GetUniformLocation(GLSLShaderType1, "Texture2");
		UniformLocations[2] = vw_GetUniformLocation(GLSLShaderType1, "DirectLightCount");
		UniformLocations[3] = vw_GetUniformLocation(GLSLShaderType1, "PointLightCount");
		UniformLocations[4] = vw_GetUniformLocation(GLSLShaderType1, "NeedMultitexture");
		UniformLocations[5] = vw_GetUniformLocation(GLSLShaderType1, "NormalMap");
		UniformLocations[6] = vw_GetUniformLocation(GLSLShaderType1, "NeedNormalMapping");
		// находим все юниформы GLSLShaderType2
		UniformLocations[10] = vw_GetUniformLocation(GLSLShaderType2, "Texture1");
		UniformLocations[11] = vw_GetUniformLocation(GLSLShaderType2, "DirectLightCount");
		UniformLocations[12] = vw_GetUniformLocation(GLSLShaderType2, "PointLightCount");
		UniformLocations[13] = vw_GetUniformLocation(GLSLShaderType2, "SpeedData1");
		UniformLocations[14] = vw_GetUniformLocation(GLSLShaderType2, "SpeedData2");
		// находим все юниформы GLSLShaderType3
		UniformLocations[20] = vw_GetUniformLocation(GLSLShaderType3, "Texture1");
		UniformLocations[21] = vw_GetUniformLocation(GLSLShaderType3, "Texture2");
		UniformLocations[22] = vw_GetUniformLocation(GLSLShaderType3, "DirectLightCount");
		UniformLocations[23] = vw_GetUniformLocation(GLSLShaderType3, "PointLightCount");
		UniformLocations[24] = vw_GetUniformLocation(GLSLShaderType3, "NeedMultitexture");
		UniformLocations[25] = vw_GetUniformLocation(GLSLShaderType3, "ShadowMap");
		UniformLocations[26] = vw_GetUniformLocation(GLSLShaderType3, "xPixelOffset");
		UniformLocations[27] = vw_GetUniformLocation(GLSLShaderType3, "yPixelOffset");
		UniformLocations[28] = vw_GetUniformLocation(GLSLShaderType3, "NormalMap");
		UniformLocations[29] = vw_GetUniformLocation(GLSLShaderType3, "NeedNormalMapping");
		UniformLocations[30] = vw_GetUniformLocation(GLSLShaderType3, "PCFMode");
	}
	// еще одна проверка перед тем как будем использовать шадовмеп
	// если не смогли загрузить шейдеры, то делать с шадовмеп нечего
	if (!Setup.UseGLSL) Setup.ShadowMap = 0;


	// инициализация менеджера частиц (обязательно после загрузки шейдеров)
	vw_InitParticleSystems(Setup.UseGLSL, Setup.VisualEffectsQuality+1.0f);




	for (int i=0; i<CurrentListCount; i++)
	{
		switch (CurrentList[i].FileType)
		{
			// 2d текстуры
			case 0:
				if (vw_FindTextureByName(CurrentList[i].FileName) == 0)
				{
					// установки параметров
					vw_SetTextureAlpha(CurrentList[i].Red, CurrentList[i].Green, CurrentList[i].Blue);
					vw_SetTextureProp(CurrentList[i].TextFiltr, CurrentList[i].TextWrap,
						CurrentList[i].Alpha, CurrentList[i].AlphaMode, CurrentList[i].MipMap);

					vw_LoadTexture(CurrentList[i].FileName, NULL, CurrentList[i].NeedCompression);
				}
				break;


			// текстуры
			case 1:
				if (vw_FindTextureByName(CurrentList[i].FileName) == 0)
				{
					int H = 0;
					int W = 0;
					bool NeedCompression = CurrentList[i].NeedCompression && Setup.TexturesCompression;

					// установки параметров
					vw_SetTextureAlpha(CurrentList[i].Red, CurrentList[i].Green, CurrentList[i].Blue);
					vw_SetTextureProp(CurrentList[i].TextFiltr, CurrentList[i].TextWrap,
						CurrentList[i].Alpha, CurrentList[i].AlphaMode, CurrentList[i].MipMap);

					// мы можем принудительно менять размер текстур через настройки, но надо учитывать их размеры
					// базовый размер почти всех текстур моделей - 512х512 пикселей, небольшая часть текстур 256х256 (мины, турели)
					// текстуры всех планет - 1024х512
					// "неформатные" текстуры - track.VW2D и asteroid-01.tga, им вообще не надо менять размеры

					if (Setup.TexturesQuality == 1)
					{
						// только для текстур в папке MODELS (скайбоксы никогда не трогаем)
						if (!strncmp("DATA/MODELS/", CurrentList[i].FileName, strlen("DATA/MODELS/")) &&
							// не меняем размеры небольших текстур вообще
							strcmp("DATA/MODELS/track.VW2D", CurrentList[i].FileName) &&
							strcmp("DATA/MODELS/SPACE/asteroid-01.tga", CurrentList[i].FileName) &&
							// не меняем размер или ставим спец размер
							strncmp("DATA/MODELS/SPACEBASE/", CurrentList[i].FileName, strlen("DATA/MODELS/SPACEBASE/")) &&
							strncmp("DATA/MODELS/PLANET/", CurrentList[i].FileName, strlen("DATA/MODELS/PLANET/")) &&
							strncmp("DATA/MODELS/NORMALMAP/", CurrentList[i].FileName, strlen("DATA/MODELS/NORMALMAP/")) &&
							// не ставим маленький размер для текстур-подсветки файтеров землян - плохо смотрится
							strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum01.VW2D", CurrentList[i].FileName) &&
							strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum02.VW2D", CurrentList[i].FileName) &&
							strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D", CurrentList[i].FileName) &&
							strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum04.VW2D", CurrentList[i].FileName))
						{
							H = W = 128;
						}
						else
						{
							// для подсветки файтеров землян и частей баз (с решетками на альфа канале) - ставим больше размер
							if (!strncmp("DATA/MODELS/SPACEBASE/", CurrentList[i].FileName, strlen("DATA/MODELS/SPACEBASE/")) ||
								!strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum01.VW2D", CurrentList[i].FileName) ||
								!strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum02.VW2D", CurrentList[i].FileName) ||
								!strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D", CurrentList[i].FileName) ||
								!strcmp("DATA/MODELS/EARTHFIGHTER/sf-illum04.VW2D", CurrentList[i].FileName))
							{
								H = W = 256;
							}
							// текстуры планет не квадратные, учитываем это
							if (!strncmp("DATA/MODELS/PLANET/", CurrentList[i].FileName, strlen("DATA/MODELS/PLANET/")) &&
								strcmp("DATA/MODELS/PLANET/asteroid.tga", CurrentList[i].FileName))
							{
								W = 512; H = 256;
							}
						}
					}
					if (Setup.TexturesQuality == 2)
					{
						// только для текстур в папке MODELS (скайбоксы никогда не трогаем)
						if (!strncmp("DATA/MODELS/", CurrentList[i].FileName, strlen("DATA/MODELS/")) &&
							// не меняем размеры небольших текстур вообще
							strcmp("DATA/MODELS/track.VW2D", CurrentList[i].FileName) &&
							strcmp("DATA/MODELS/SPACE/asteroid-01.tga", CurrentList[i].FileName) &&
							// не меняем размер
							strncmp("DATA/MODELS/SPACEBASE/", CurrentList[i].FileName, strlen("DATA/MODELS/SPACEBASE/")) &&
							strncmp("DATA/MODELS/PLANET/", CurrentList[i].FileName, strlen("DATA/MODELS/PLANET/")) &&
							strncmp("DATA/MODELS/NORMALMAP/", CurrentList[i].FileName, strlen("DATA/MODELS/NORMALMAP/")))
						{
							H = W = 256;
						}
					}

					// если это карта нормалей, но у нас не включены шейдеры - пропускаем
					if (!strncmp("DATA/MODELS/NORMALMAP", CurrentList[i].FileName, strlen("DATA/MODELS/NORMALMAP")) && !Setup.UseGLSL) break;

					vw_LoadTexture(CurrentList[i].FileName, NULL, NeedCompression, AUTO_FILE, W, H);
				}
				break;

			// предварит. загрузка моделей
			case 2:
				vw_LoadModel3D(CurrentList[i].FileName, CurrentList[i].TriangleSizeLimit, CurrentList[i].NeedTangentAndBinormal && Setup.UseGLSL);
				break;

			// загрузка sfx
			case 4:
				// если вообще можем играть звуки
				if (Setup.Sound_check)
				{
					// если еще не загрузили этот звук
					if (vw_FindBufferIDByName(CurrentList[i].FileName) == 0)
					{
						if( vw_TestFileExtension( CurrentList[i].FileName, "wav" ) || vw_TestFileExtension( CurrentList[i].FileName, "WAV" ))
						{
							vw_CreateSoundBufferFromWAV(CurrentList[i].FileName);
						}
						else
						{
							if( vw_TestFileExtension( CurrentList[i].FileName, "ogg" ) || vw_TestFileExtension( CurrentList[i].FileName, "OGG" ))
								vw_CreateSoundBufferFromOGG(CurrentList[i].FileName);
						}
					}
				}
				break;


		}


		RealLoadedTextures += CurrentList[i].Value;

		// рисуем текущее состояние загрузки, если не рисуем логотип
		DrawLoading(RealLoadedTextures, AllDrawLoading, &LastDrawTime, LoadImageTexture);

	}






#ifdef multithread
	//ждем завершение звука
	LoadSoundThreadNeedOff = true;
	if (SoundThread != 0) SDL_WaitThread(SoundThread, NULL);
#endif //multithread







	// убираем картинку загрузки
	if (LoadImageTexture != 0)
	{
		vw_ReleaseTexture(LoadImageTexture);
	}


AllDataLoaded:


	// инициализируем шадов меп, делаем это постоянно т.к. у нас разные размеры карт для меню и игры
	if (Setup.ShadowMap > 0)
	{
		int ShadowMapSize = 1024;
		switch(Setup.ShadowMap)
		{
			case 1:
			case 2:
			case 3:
					ShadowMapSize = CAPS->MaxTextureWidth/4; break;
			case 4:
			case 5:
			case 6:
					ShadowMapSize = CAPS->MaxTextureWidth/2; break;
			case 7:
			case 8:
			case 9:
					ShadowMapSize = CAPS->MaxTextureWidth; break;
		}

		switch(LoadType)
		{
			case -1:  // меню (только запустили)
				if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize/2)) Setup.ShadowMap = 0;
				break;
			case 0:   // меню (выходим из игры)
				ShadowMap_Release();
				if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize/2)) Setup.ShadowMap = 0;
				break;

			case 1: // переход на уровни игры
				ShadowMap_Release();
				if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize)) Setup.ShadowMap = 0;
				break;
		}
	}


	// переходим в нужное место...
	switch(LoadType)
	{
		// меню, первая загрузка, самый старт
		case -1:
			InitMenu();
			break;
		// меню, выходим из игры, входим в меню
		case 0:
			InitMenu();
			GameStatus = MISSION; // чтобы не было перехода с основного меню в мисии
			Setup.LoadingHint++;
			if (Setup.LoadingHint >= 6) Setup.LoadingHint = 0;
			break;

		// уровни игры
		case 1:
			// освобождаем память от того, что загружали
			if (CurrentList != 0){delete [] CurrentList; CurrentList = 0;}
			InitGame();
			StartMusicWithFade(2, 2.0f, 2.0f);
			// приготовиться к действию (речь)
			Audio_PlayVoice(5, 1.0f);
			Setup.LoadingHint++;
			if (Setup.LoadingHint >= 6) Setup.LoadingHint = 0;
			break;
	}


	// всегда на черном фоне
	vw_SetClearColor(0.0f,0.0f,0.0f,1.0f);
}




