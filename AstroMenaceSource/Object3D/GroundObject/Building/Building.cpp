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

