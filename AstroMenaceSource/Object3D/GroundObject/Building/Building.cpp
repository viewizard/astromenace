/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Building.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


/// подключаем нужные файлы
#include "Building.h"



struct BuildingData
{
	float	Strength;
	const	char *Name;
};

const int	PresetBuildingDataCount = 11;
BuildingData PresetBuildingData[PresetBuildingDataCount] =
{
	{150,	"DATA/MODELS/BUILDING/bld-01.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-02.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-03.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-04.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-05.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-06.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-07.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-08.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-09.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-10.VW3D"},
	{150,	"DATA/MODELS/BUILDING/bld-11.VW3D"}
};



//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CBuilding::Create(int	BuildingNum)
{
	ObjectType = 12;
	ObjectCreationType = BuildingNum;
	// здания неразрушимы, не показываем вообще их жизнь
	ShowStrength = false;
	PromptDrawDist2 = 100.0f;

	LoadObjectData(PresetBuildingData[BuildingNum-1].Name, this, 0, -1.0f, Setup.UseGLSL);

	for (int i=0; i<DrawObjectQuantity; i++)
	{
		Texture[i] = vw_FindTextureByName("DATA/MODELS/BUILDING/bld.VW2D");
		TextureIllum[i] = vw_FindTextureByName("DATA/MODELS/BUILDING/bld_illum.VW2D");
		NormalMap[i] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/buildings_nm.tga");
	}

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;

	Strength = StrengthStart = PresetBuildingData[BuildingNum-1].Strength/GameNPCArmorPenalty;


	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();
}

