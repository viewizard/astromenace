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

#include "building.h"
#include "../../../config/config.h"



struct sBuildingData {
	float	Strength;
	const	char *Name;
};

static sBuildingData PresetBuildingData[] = {
	{150,	"models/building/bld-01.vw3d"},
	{150,	"models/building/bld-02.vw3d"},
	{150,	"models/building/bld-03.vw3d"},
	{150,	"models/building/bld-04.vw3d"},
	{150,	"models/building/bld-05.vw3d"},
	{150,	"models/building/bld-06.vw3d"},
	{150,	"models/building/bld-07.vw3d"},
	{150,	"models/building/bld-08.vw3d"},
	{150,	"models/building/bld-09.vw3d"},
	{150,	"models/building/bld-10.vw3d"},
	{150,	"models/building/bld-11.vw3d"}
};
#define PresetBuildingDataCount sizeof(PresetBuildingData)/sizeof(PresetBuildingData[0])



//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cBuilding::Create(int BuildingNum)
{
	if ((BuildingNum <= 0) || ((unsigned int)BuildingNum > PresetBuildingDataCount)) {
		std::cerr << __func__ << "(): " << "Could not init cBuilding object with Number " << BuildingNum << "\n";
		return;
	}

	ObjectType = eObjectType::CivilianBuilding;
	InternalType = BuildingNum;
	// здания неразрушимы, не показываем вообще их жизнь
	ShowStrength = false;
	PromptDrawDist2 = 100.0f;

	LoadObjectData(PresetBuildingData[BuildingNum-1].Name, this);

	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = vw_FindTextureByName("models/building/bld.vw2d");
		TextureIllum[i] = vw_FindTextureByName("models/building/bld_illum.vw2d");
		NormalMap[i] = vw_FindTextureByName("models/normalmap/buildings_nm.tga");
	}

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;

	Strength = StrengthStart = PresetBuildingData[BuildingNum-1].Strength/GameEnemyArmorPenalty;
}

