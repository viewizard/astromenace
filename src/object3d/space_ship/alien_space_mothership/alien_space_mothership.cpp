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

#include "alien_space_mothership.h"
#include "../../../config/config.h"


struct sAlienSpaceMotherShipData {

	unsigned int WeaponQuantity;
	unsigned int BossWeaponQuantity;
	unsigned int EngineQuantity;
	float	Strength;
	float	ShieldStrength;
	const	char *Name;
	const	char *Texture;
	const	char *TextureIllum;
};

static sAlienSpaceMotherShipData PresetAlienSpaceMotherShipData[] = {
	{14,1,	10,	3000, 1500,	"models/alienmothership/alm-01.vw3d", "models/alienmothership/alm-text04.vw2d", "models/alienmothership/alm-illum04.vw2d"},
	{8,	8,	10,	4000, 3000,	"models/alienmothership/alm-02.vw3d", "models/alienmothership/alm-text04.vw2d", "models/alienmothership/alm-illum04.vw2d"},
	{8,	1,	8,	5000, 3300,	"models/alienmothership/alm-03.vw3d", "models/alienmothership/alm-text02.vw2d", "models/alienmothership/alm-illum02.vw2d"},
	{8,	8,	12,	6000, 3500,	"models/alienmothership/alm-04.vw3d", "models/alienmothership/alm-text02.vw2d", "models/alienmothership/alm-illum02.vw2d"},
	{8,	6,	19,	7000, 3800,	"models/alienmothership/alm-05.vw3d", "models/alienmothership/alm-text08.vw2d", "models/alienmothership/alm-illum08.vw2d"},
	{12,6,	15,	8000, 4000,	"models/alienmothership/alm-06.vw3d", "models/alienmothership/alm-text08.vw2d", "models/alienmothership/alm-illum08.vw2d"},
	{5,	2,	6,	9000, 4300,	"models/alienmothership/alm-07.vw3d", "models/alienmothership/alm-text03.vw2d", "models/alienmothership/alm-illum03.vw2d"},
	{4,	6,	10,	10000,4500,	"models/alienmothership/alm-08.vw3d", "models/alienmothership/alm-text03.vw2d", "models/alienmothership/alm-illum03.vw2d"}
};
#define PresetAlienSpaceMotherShipDataCount sizeof(PresetAlienSpaceMotherShipData)/sizeof(PresetAlienSpaceMotherShipData[0])




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cAlienSpaceMotherShip::Create(int	SpaceShipNum)
{
	if ((SpaceShipNum <= 0) || ((unsigned int)SpaceShipNum > PresetAlienSpaceMotherShipDataCount)) {
		std::cerr << __func__ << "(): "
			  << "Could not init cAlienSpaceMotherShip object with Number " << SpaceShipNum << "\n";
		return;
	}

	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::AlienMotherShip;
	InternalType = SpaceShipNum;
	PromptDrawDist2 = 20000.0f;

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	MaxSpeed = 20.0f;
	MaxAcceler = 20.0f;
	MaxSpeedRotate = 80.0f;
	Strength = StrengthStart = PresetAlienSpaceMotherShipData[SpaceShipNum-1].Strength/GameEnemyArmorPenalty;
	ShieldStrength = ShieldStrengthStart = PresetAlienSpaceMotherShipData[SpaceShipNum-1].ShieldStrength/GameEnemyArmorPenalty;
	ShieldRecharge = ShieldStrengthStart/15.0f;

	WeaponQuantity = PresetAlienSpaceMotherShipData[SpaceShipNum-1].WeaponQuantity;
	BossWeaponQuantity = PresetAlienSpaceMotherShipData[SpaceShipNum-1].BossWeaponQuantity;
	unsigned int EngineQuantity = PresetAlienSpaceMotherShipData[SpaceShipNum - 1].EngineQuantity;


	LoadObjectData(PresetAlienSpaceMotherShipData[SpaceShipNum-1].Name, this);

	// всегда только эти текстуры
	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = vw_FindTextureByName(PresetAlienSpaceMotherShipData[SpaceShipNum - 1].Texture);
		TextureIllum[i] = vw_FindTextureByName(PresetAlienSpaceMotherShipData[SpaceShipNum - 1].TextureIllum);
		// если шейдеры выключены - вернет ноль (не загружаем текстуры нормал мепов если нет шейдеров)
		// в LoadObjectData указываем Setup.UseGLSL
		NormalMap[i] = vw_FindTextureByName("models/normalmap/alien_mothership_nm.tga");
	}



	// начальные установки для оружия
	WeaponSetFire = new bool[WeaponQuantity];
	WeaponLocation = new sVECTOR3D[WeaponQuantity];
	WeaponType = new int[WeaponQuantity];
	Weapon = new cWeapon*[WeaponQuantity];
	for (int i=0; i<WeaponQuantity; i++) {
		WeaponSetFire[i] = false;
		WeaponType[i] = 1;
		Weapon[i] = nullptr;
	}

	BossWeaponSetFire = new bool[BossWeaponQuantity];
	BossWeaponLocation = new sVECTOR3D[BossWeaponQuantity];
	BossWeaponType = new int[BossWeaponQuantity];
	BossWeapon = new cWeapon*[BossWeaponQuantity];
	for (int i=0; i<BossWeaponQuantity; i++) {
		BossWeaponSetFire[i] = false;
		BossWeaponType[i] = 1;
		BossWeapon[i] = nullptr;
	}

	// начальные установки для двигателей
	EnginesLocation.resize(EngineQuantity);
	Engines.resize(EngineQuantity);










	// перебираем и ставим нужные данные
	switch (SpaceShipNum) {
	case 1:
		// оружие
		WeaponLocation[0] = sVECTOR3D(-1.6f, 1.0f, 19.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(102);
		WeaponLocation[1] = sVECTOR3D(1.6f, 1.0f, 19.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(102);
		WeaponLocation[2] = sVECTOR3D(-1.6f, -1.0f, 19.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(102);
		WeaponLocation[3] = sVECTOR3D(1.6f, -1.0f, 19.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(102);

		WeaponLocation[4] = sVECTOR3D(-1.6f, 1.5f, 18.0f);
		Weapon[4] = new cWeapon;
		Weapon[4]->Create(103);
		WeaponLocation[5] = sVECTOR3D(1.6f, 1.5f, 18.0f);
		Weapon[5] = new cWeapon;
		Weapon[5]->Create(103);
		WeaponLocation[6] = sVECTOR3D(-1.6f, -1.5f, 18.0f);
		Weapon[6] = new cWeapon;
		Weapon[6]->Create(103);
		WeaponLocation[7] = sVECTOR3D(1.6f, -1.5f, 18.0f);
		Weapon[7] = new cWeapon;
		Weapon[7]->Create(103);

		WeaponLocation[8] = sVECTOR3D(-1.6f, 1.0f, 19.0f);
		Weapon[8] = new cWeapon;
		Weapon[8]->Create(104);
		WeaponLocation[9] = sVECTOR3D(1.6f, 1.0f, 19.0f);
		Weapon[9] = new cWeapon;
		Weapon[9]->Create(104);
		WeaponLocation[10] = sVECTOR3D(-1.6f, -1.0f, 19.0f);
		Weapon[10] = new cWeapon;
		Weapon[10]->Create(104);
		WeaponLocation[11] = sVECTOR3D(1.6f, -1.0f, 19.0f);
		Weapon[11] = new cWeapon;
		Weapon[11]->Create(104);

		WeaponLocation[12] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		Weapon[12] = new cWeapon;
		Weapon[12]->Create(104);
		WeaponLocation[13] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		Weapon[13] = new cWeapon;
		Weapon[13]->Create(104);

		BossWeaponLocation[0] = sVECTOR3D(0.0f, 0.0f, 0.0f);

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(8.4f, 5.2f, -24.0f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-8.4f, 5.2f, -24.0f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(8.4f, -5.6f, -24.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-8.4f, -5.6f, -24.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(0.0f, 8.2f, -15.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(0.0f, -8.2f, -15.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(0.0f, 0.0f, -8.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 3);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(0.0f, 0.0f, -8.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 4);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 5);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 6);
		break;
	case 2:
		// оружие
		WeaponLocation[0] = sVECTOR3D(-10.7f, 0.0f, -18.5f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(103);
		Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
		WeaponLocation[1] = sVECTOR3D(10.7f, 0.0f, -18.5f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(103);
		Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
		WeaponLocation[2] = sVECTOR3D(-10.7f, 0.0f, -18.5f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(103);
		Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
		WeaponLocation[3] = sVECTOR3D(10.7f, 0.0f, -18.5f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(103);
		Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;

		WeaponLocation[4] = sVECTOR3D(-10.7f, 0.0f, -18.5f);
		Weapon[4] = new cWeapon;
		Weapon[4]->Create(106);
		Weapon[4]->NextFireTime = Weapon[4]->NextFireTime/2.0f;
		WeaponLocation[5] = sVECTOR3D(10.7f, 0.0f, -18.5f);
		Weapon[5] = new cWeapon;
		Weapon[5]->Create(106);
		Weapon[5]->NextFireTime = Weapon[5]->NextFireTime/2.0f;
		WeaponLocation[6] = sVECTOR3D(-4.0f, 0.0f, 24.0f);
		Weapon[6] = new cWeapon;
		Weapon[6]->Create(106);
		Weapon[6]->NextFireTime = Weapon[6]->NextFireTime/2.0f;
		WeaponLocation[7] = sVECTOR3D(4.0f, 0.0f, 24.0f);
		Weapon[7] = new cWeapon;
		Weapon[7]->Create(106);
		Weapon[7]->NextFireTime = Weapon[7]->NextFireTime/2.0f;


		BossWeaponLocation[0] = sVECTOR3D(-1.5f, 1.5f, 25.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(102);
		BossWeaponLocation[1] = sVECTOR3D(1.5f, 1.5f, 25.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(102);
		BossWeaponLocation[2] = sVECTOR3D(-1.5f, -1.5f, 25.0f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(102);
		BossWeaponLocation[3] = sVECTOR3D(1.5f, -1.5f, 25.0f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(102);


		BossWeaponLocation[4] = sVECTOR3D(-4.0f, 2.0f, 24.0f);
		BossWeapon[4] = new cWeapon;
		BossWeapon[4]->Create(102);
		BossWeaponLocation[5] = sVECTOR3D(4.0f, 2.0f, 24.0f);
		BossWeapon[5] = new cWeapon;
		BossWeapon[5]->Create(102);
		BossWeaponLocation[6] = sVECTOR3D(-4.0f, -2.0f, 24.0f);
		BossWeapon[6] = new cWeapon;
		BossWeapon[6]->Create(102);
		BossWeaponLocation[7] = sVECTOR3D(4.0f, -2.0f, 24.0f);
		BossWeapon[7] = new cWeapon;
		BossWeapon[7]->Create(102);


		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(5.6f, 7.0f, -27.6f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-5.6f, 7.0f, -27.6f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.6f, -7.0f, -27.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.6f, -7.0f, -27.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(10.7f, 0.0f, -21.5f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-10.7f, 0.0f, -21.5f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(0.0f, 0.0f, -14.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 3);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(0.0f, 0.0f, -14.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 4);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 5);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 6);
		break;
	case 3:
		// оружие
		WeaponLocation[0] = sVECTOR3D(3.0f, -2.0f, 13.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(104);
		WeaponLocation[1] = sVECTOR3D(-3.0f, -2.0f, 13.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(104);
		WeaponLocation[2] = sVECTOR3D(2.0f, -1.0f, 13.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(104);
		WeaponLocation[3] = sVECTOR3D(-2.0f, -1.0f, 13.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(104);
		WeaponLocation[4] = sVECTOR3D(2.0f, -3.0f, 13.0f);
		Weapon[4] = new cWeapon;
		Weapon[4]->Create(104);
		WeaponLocation[5] = sVECTOR3D(-2.0f, -3.0f, 13.0f);
		Weapon[5] = new cWeapon;
		Weapon[5]->Create(104);

		WeaponLocation[6] = sVECTOR3D(7.0f, -2.0f, 13.0f);
		Weapon[6] = new cWeapon;
		Weapon[6]->Create(109);
		WeaponLocation[7] = sVECTOR3D(-7.0f, -2.0f, 13.0f);
		Weapon[7] = new cWeapon;
		Weapon[7]->Create(109);


		BossWeaponLocation[0] = sVECTOR3D(0.0f, -2.0f, 27.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(110);

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(5.5f, 8.1f, -26.1f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-5.5f, 8.1f, -26.1f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.6f, -7.2f, -28.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.6f, -7.2f, -28.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(11.9f, -1.0f, -14.8f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-11.9f, -1.0f, -14.8f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(0.0f, -1.0f, -10.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 3);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(0.0f, -1.0f, -10.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 4);
		break;
	case 4:
		// оружие
		WeaponLocation[0] = sVECTOR3D(1.0f, -1.0f, 23.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(104);
		WeaponLocation[1] = sVECTOR3D(-1.0f, -1.0f, 23.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(104);
		WeaponLocation[2] = sVECTOR3D(1.0f, 0.0f, 23.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(104);
		WeaponLocation[3] = sVECTOR3D(-1.0f, 0.0f, 23.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(104);
		WeaponLocation[4] = sVECTOR3D(1.0f, -2.0f, 23.0f);
		Weapon[4] = new cWeapon;
		Weapon[4]->Create(104);
		WeaponLocation[5] = sVECTOR3D(-1.0f, -2.0f, 23.0f);
		Weapon[5] = new cWeapon;
		Weapon[5]->Create(104);
		WeaponLocation[6] = sVECTOR3D(0.0f, -1.0f, 23.0f);
		Weapon[6] = new cWeapon;
		Weapon[6]->Create(109);
		WeaponLocation[7] = sVECTOR3D(0.0f, -1.0f, 23.0f);
		Weapon[7] = new cWeapon;
		Weapon[7]->Create(109);

		BossWeaponLocation[0] = sVECTOR3D(8.9f, -0.6f, 18.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(108);
		BossWeapon[0]->SetRotation(sVECTOR3D(0.0f, -15.0f, 0.0f));
		BossWeaponLocation[1] = sVECTOR3D(-8.9f, -0.6f, 18.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(108);
		BossWeapon[1]->SetRotation(sVECTOR3D(0.0f, 15.0f, 0.0f));
		BossWeaponLocation[2] = sVECTOR3D(10.0f, -5.6f, 18.0f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(108);
		BossWeaponLocation[3] = sVECTOR3D(-10.0f, -5.6f, 18.0f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(108);
		BossWeaponLocation[4] = sVECTOR3D(8.9f, -0.6f, 18.0f);
		BossWeapon[4] = new cWeapon;
		BossWeapon[4]->Create(108);
		BossWeapon[4]->SetRotation(sVECTOR3D(0.0f, -5.0f, 0.0f));
		BossWeaponLocation[5] = sVECTOR3D(-8.9f, -0.6f, 18.0f);
		BossWeapon[5] = new cWeapon;
		BossWeapon[5]->Create(108);
		BossWeapon[5]->SetRotation(sVECTOR3D(0.0f, 5.0f, 0.0f));
		BossWeaponLocation[6] = sVECTOR3D(10.0f, -5.6f, 18.0f);
		BossWeapon[6] = new cWeapon;
		BossWeapon[6]->Create(108);
		BossWeapon[6]->SetRotation(sVECTOR3D(0.0f, -10.0f, 0.0f));
		BossWeaponLocation[7] = sVECTOR3D(-10.0f, -5.6f, 18.0f);
		BossWeapon[7] = new cWeapon;
		BossWeapon[7]->Create(108);
		BossWeapon[7]->SetRotation(sVECTOR3D(0.0f, 10.0f, 0.0f));


		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.9f, 5.9f, -24.6f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.9f, 5.9f, -24.6f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(1.9f, -2.9f, -24.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-1.9f, -2.9f, -24.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(2.9f, 1.6f, -24.6f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-2.9f, 1.6f, -24.6f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(8.9f, -0.6f, -12.6f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-8.9f, -0.6f, -12.6f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(10.0f, -5.6f, -5.2f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(-10.0f, -5.6f, -5.2f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[10] = vw_CreateParticleSystem();
		EnginesLocation[10] = sVECTOR3D(0.0f, -6.0f, 10.0f);
		if (auto sharedEngine = Engines[10].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 5);
			sharedEngine->CreationSize = sVECTOR3D(6.0f,6.0f,2.0f);
			sharedEngine->DeadZone = 5.9f;
		}
		Engines[11] = vw_CreateParticleSystem();
		EnginesLocation[11] = sVECTOR3D(0.0f, -6.0f, 10.0f);
		if (auto sharedEngine = Engines[11].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 6);
			sharedEngine->CreationSize = sVECTOR3D(6.0f,6.0f,2.0f);
			sharedEngine->DeadZone = 5.9f;
		}
		break;
	case 5:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(109);
		WeaponLocation[1] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(109);
		Weapon[1]->SetRotation(sVECTOR3D(0.0f, 3.0f, 0.0f));
		WeaponLocation[2] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(109);
		Weapon[2]->SetRotation(sVECTOR3D(0.0f, -3.0f, 0.0f));
		WeaponLocation[3] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(109);
		Weapon[3]->SetRotation(sVECTOR3D(0.0f, 6.0f, 0.0f));
		WeaponLocation[4] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[4] = new cWeapon;
		Weapon[4]->Create(109);
		WeaponLocation[5] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[5] = new cWeapon;
		Weapon[5]->Create(109);
		Weapon[5]->SetRotation(sVECTOR3D(0.0f, -6.0f, 0.0f));
		WeaponLocation[6] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[6] = new cWeapon;
		Weapon[6]->Create(109);
		Weapon[6]->SetRotation(sVECTOR3D(0.0f, 9.0f, 0.0f));
		WeaponLocation[7] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		Weapon[7] = new cWeapon;
		Weapon[7]->Create(109);
		Weapon[7]->SetRotation(sVECTOR3D(0.0f, -9.0f, 0.0f));

		BossWeaponLocation[0] = sVECTOR3D(35.7f, -3.0f, -13.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(106);
		BossWeaponLocation[1] = sVECTOR3D(-35.7f, -3.0f, -13.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(106);
		BossWeaponLocation[2] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(106);
		BossWeaponLocation[3] = sVECTOR3D(0.0f, -2.4f, 20.0f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(106);
		BossWeaponLocation[4] = sVECTOR3D(8.85f, 5.65f, -10.2f);
		BossWeapon[4] = new cWeapon;
		BossWeapon[4]->Create(104);
		BossWeaponLocation[5] = sVECTOR3D(-8.85f, 5.65f, -10.2f);
		BossWeapon[5] = new cWeapon;
		BossWeapon[5]->Create(104);

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(25.1f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-25.1f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(20.6f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-20.6f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(22.9f, 0.65f, -20.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-22.9f, 0.65f, -20.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(22.9f, -5.1f, -20.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-22.9f, -5.1f, -20.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(8.85f, 5.65f, -15.2f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(-8.85f, 5.65f, -15.2f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[10] = vw_CreateParticleSystem();
		EnginesLocation[10] = sVECTOR3D(4.0f, 7.0f, -21.2f);
		if (auto sharedEngine = Engines[10].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[11] = vw_CreateParticleSystem();
		EnginesLocation[11] = sVECTOR3D(-4.0f, 7.0f, -21.2f);
		if (auto sharedEngine = Engines[11].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[12] = vw_CreateParticleSystem();
		EnginesLocation[12] = sVECTOR3D(20.4f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[12].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[13] = vw_CreateParticleSystem();
		EnginesLocation[13] = sVECTOR3D(-20.4f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[13].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[14] = vw_CreateParticleSystem();
		EnginesLocation[14] = sVECTOR3D(25.2f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[14].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[15] = vw_CreateParticleSystem();
		EnginesLocation[15] = sVECTOR3D(-25.2f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[15].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[16] = vw_CreateParticleSystem();
		EnginesLocation[16] = sVECTOR3D(35.7f, -3.0f, -13.0f);
		if (auto sharedEngine = Engines[16].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 12);
			sharedEngine->Direction = sVECTOR3D(0.4f, 0.0f, -0.8f);
		}
		Engines[17] = vw_CreateParticleSystem();
		EnginesLocation[17] = sVECTOR3D(-35.7f, -3.0f, -13.0f);
		if (auto sharedEngine = Engines[17].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 12);
			sharedEngine->Direction = sVECTOR3D(-0.4f, 0.0f, -0.8f);
		}
		Engines[18] = vw_CreateParticleSystem();
		EnginesLocation[18] = sVECTOR3D(0.0f, -2.4f, -23.0f);
		if (auto sharedEngine = Engines[18].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 13);
		break;


	case 6:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, -4.4f, 18.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(103);
		Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
		WeaponLocation[1] = sVECTOR3D(0.0f, -4.4f, 18.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(103);
		Weapon[1]->SetRotation(sVECTOR3D(0.0f, 3.0f, 0.0f));
		Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
		WeaponLocation[2] = sVECTOR3D(0.0f, -4.4f, 18.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(103);
		Weapon[2]->SetRotation(sVECTOR3D(0.0f, -3.0f, 0.0f));
		Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
		WeaponLocation[3] = sVECTOR3D(0.0f, -4.4f, 18.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(103);
		Weapon[3]->SetRotation(sVECTOR3D(0.0f, 6.0f, 0.0f));
		Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;
		WeaponLocation[4] = sVECTOR3D(0.0f, -4.4f, 18.0f);
		Weapon[4] = new cWeapon;
		Weapon[4]->Create(103);
		Weapon[4]->NextFireTime = Weapon[4]->NextFireTime/2.0f;
		WeaponLocation[5] = sVECTOR3D(0.0f, -4.4f, 18.0f);
		Weapon[5] = new cWeapon;
		Weapon[5]->Create(103);
		Weapon[5]->SetRotation(sVECTOR3D(0.0f, -6.0f, 0.0f));
		Weapon[5]->NextFireTime = Weapon[5]->NextFireTime/2.0f;

		WeaponLocation[6] = sVECTOR3D(7.0f, -4.4f, 13.0f);
		Weapon[6] = new cWeapon;
		Weapon[6]->Create(104);
		Weapon[6]->NextFireTime = Weapon[6]->NextFireTime/2.0f;
		WeaponLocation[7] = sVECTOR3D(-7.0f, -4.4f, 13.0f);
		Weapon[7] = new cWeapon;
		Weapon[7]->Create(104);
		Weapon[7]->NextFireTime = Weapon[7]->NextFireTime/2.0f;
		WeaponLocation[8] = sVECTOR3D(12.0f, -4.4f, 10.0f);
		Weapon[8] = new cWeapon;
		Weapon[8]->Create(104);
		Weapon[8]->NextFireTime = Weapon[8]->NextFireTime/2.0f;
		WeaponLocation[9] = sVECTOR3D(-12.0f, -4.4f, 10.0f);
		Weapon[9] = new cWeapon;
		Weapon[9]->Create(104);
		Weapon[9]->NextFireTime = Weapon[9]->NextFireTime/2.0f;
		WeaponLocation[10] = sVECTOR3D(17.0f, -4.4f, 8.0f);
		Weapon[10] = new cWeapon;
		Weapon[10]->Create(104);
		Weapon[10]->NextFireTime = Weapon[10]->NextFireTime/2.0f;
		WeaponLocation[11] = sVECTOR3D(-17.0f, -4.4f, 8.0f);
		Weapon[11] = new cWeapon;
		Weapon[11]->Create(104);
		Weapon[11]->NextFireTime = Weapon[11]->NextFireTime/2.0f;

		BossWeaponLocation[0] = sVECTOR3D(10.0f, -6.4f, 10.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(106);
		BossWeaponLocation[1] = sVECTOR3D(-10.0f, -6.4f, 10.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(106);
		BossWeaponLocation[2] = sVECTOR3D(15.0f, -6.4f, 8.0f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(106);
		BossWeaponLocation[3] = sVECTOR3D(-15.0f, -6.4f, 8.0f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(106);
		BossWeaponLocation[4] = sVECTOR3D(20.0f, -6.4f, 5.0f);
		BossWeapon[4] = new cWeapon;
		BossWeapon[4]->Create(106);
		BossWeaponLocation[5] = sVECTOR3D(-20.0f, -6.4f, 5.0f);
		BossWeapon[5] = new cWeapon;
		BossWeapon[5]->Create(106);

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(0.0f, -7.0f, -23.0f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 13);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-25.4f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(20.6f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-20.6f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(23.4f, -1.0f, -20.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-23.4f, -1.0f, -20.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(23.4f, -7.0f, -20.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-23.4f, -7.0f, -20.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(5.3f, 4.7f, -22.9f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(-5.3f, 4.7f, -22.9f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[10] = vw_CreateParticleSystem();
		EnginesLocation[10] = sVECTOR3D(2.6f, 6.6f, -21.5f);
		if (auto sharedEngine = Engines[10].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[11] = vw_CreateParticleSystem();
		EnginesLocation[11] = sVECTOR3D(-2.6f, 6.6f, -21.5f);
		if (auto sharedEngine = Engines[11].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[12] = vw_CreateParticleSystem();
		EnginesLocation[12] = sVECTOR3D(2.6f, 2.0f, -21.5f);
		if (auto sharedEngine = Engines[12].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[13] = vw_CreateParticleSystem();
		EnginesLocation[13] = sVECTOR3D(-2.6f, 2.0f, -21.5f);
		if (auto sharedEngine = Engines[13].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[14] = vw_CreateParticleSystem();
		EnginesLocation[14] = sVECTOR3D(25.4f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[14].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		break;

	case 7:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, -1.4f, 15.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(108);
		Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
		WeaponLocation[1] = sVECTOR3D(2.0f, -1.4f, 15.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(108);
		Weapon[1]->SetRotation(sVECTOR3D(0.0f, 3.0f, 0.0f));
		Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
		WeaponLocation[2] = sVECTOR3D(-2.0f, -1.4f, 15.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(108);
		Weapon[2]->SetRotation(sVECTOR3D(0.0f, -3.0f, 0.0f));
		Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
		WeaponLocation[3] = sVECTOR3D(4.0f, -1.4f, 15.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(108);
		Weapon[3]->SetRotation(sVECTOR3D(0.0f, 6.0f, 0.0f));
		Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;
		WeaponLocation[4] = sVECTOR3D(-4.0f, -1.4f, 15.0f);
		Weapon[4] = new cWeapon;
		Weapon[4]->Create(108);
		Weapon[4]->SetRotation(sVECTOR3D(0.0f, -6.0f, 0.0f));
		Weapon[4]->NextFireTime = Weapon[4]->NextFireTime/2.0f;

		BossWeaponLocation[0] = sVECTOR3D(12.5f, 1.6f, -15.3f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(110);
		BossWeaponLocation[1] = sVECTOR3D(-12.5f, 1.6f, -15.3f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(110);

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(0.0f, 2.3f, -25.0f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 15);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(12.5f, 1.6f, -20.3f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 16);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(-12.5f, 1.6f, -20.3f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 16);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 17);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 18);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 19);
		break;




	case 8:
		// оружие
		WeaponLocation[0] = sVECTOR3D(2.0f, -2.4f, 15.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(104);
		Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
		WeaponLocation[1] = sVECTOR3D(-2.0f, -2.4f, 15.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(104);
		Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
		WeaponLocation[2] = sVECTOR3D(2.0f, -4.4f, 15.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(104);
		Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
		WeaponLocation[3] = sVECTOR3D(-2.0f, -4.4f, 15.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(104);
		Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;

		BossWeaponLocation[0] = sVECTOR3D(8.0f, -1.0f, 15.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(110);
		BossWeaponLocation[1] = sVECTOR3D(-8.0f, -1.0f, 15.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(110);
		BossWeaponLocation[2] = sVECTOR3D(10.0f, -6.4f, 8.0f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(107);
		BossWeaponLocation[3] = sVECTOR3D(-10.0f, -6.4f, 8.0f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(107);
		BossWeaponLocation[4] = sVECTOR3D(15.0f, -6.4f, 5.0f);
		BossWeapon[4] = new cWeapon;
		BossWeapon[4]->Create(107);
		BossWeaponLocation[5] = sVECTOR3D(-15.0f, -6.4f, 5.0f);
		BossWeapon[5] = new cWeapon;
		BossWeapon[5]->Create(107);

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(15.0f, 4.4f, -29.0f);
		if (auto sharedEngine = Engines[0].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 15);
			sharedEngine->CreationSize = sVECTOR3D(4.0f,6.0f,3.0f);
		}
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-15.0f, 4.4f, -29.0f);
		if (auto sharedEngine = Engines[1].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 15);
			sharedEngine->CreationSize = sVECTOR3D(4.0f,6.0f,3.0f);
		}
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.2f, 5.4f, -20.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.2f, 5.4f, -20.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(5.2f, -8.8f, -22.6f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-5.2f, -8.8f, -22.6f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(11.2f, -2.8f, -10.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-11.2f, -2.8f, -10.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(0.0f, -10.0f, -3.0f);
		if (auto sharedEngine = Engines[8].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 17);
			sharedEngine->CreationSize = sVECTOR3D(17.0f,1.0f,17.0f);
			sharedEngine->DeadZone = 16.9f;
		}
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(0.0f, -10.0f, -3.0f);
		if (auto sharedEngine = Engines[9].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 18);
			sharedEngine->CreationSize = sVECTOR3D(17.0f,1.0f,17.0f);
			sharedEngine->DeadZone = 16.9f;
		}
		break;



	default:
		std::cerr << __func__ << "(): " << "wrong SpaceShipNum.\n";
		return;
	}

	for (unsigned int i = 0; i < EngineQuantity; i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			sharedEngine->SetStartLocation(EnginesLocation[i]);
			// находим кол-во внутренних источников света
			if (!sharedEngine->Light.expired())
				InternalLights++;
		}
	}
}
