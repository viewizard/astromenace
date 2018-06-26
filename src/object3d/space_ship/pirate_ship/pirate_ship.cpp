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

#include "pirate_ship.h"
#include "../../../config/config.h"
#include "../../../assets/audio.h"
#include "../../../assets/texture.h"

struct sPirateShipData {
	unsigned int WeaponQuantity;
	unsigned int BossWeaponQuantity;
	unsigned int EngineQuantity;
	float Strength;
	const std::string Name;
	const std::string TextureName;
	const std::string NormalMapName;
};

const sPirateShipData PresetPirateShipData[] = {
	{2,	0, 2,	20,	"models/pirateship/gunship-01.vw3d",	"models/gr-04.vw2d", ""},
	{1,	0, 2,	30,	"models/pirateship/bomber-07.vw3d",	"models/gr-04.vw2d", ""},
	{4,	0, 2,	100,	"models/pirateship/gunship-03.vw3d",	"models/gr-03.vw2d", ""},
	{4,	0, 2,	200,	"models/pirateship/gunship-04.vw3d",	"models/gr-03.vw2d", ""},
	{4,	0, 6,	400,	"models/pirateship/gunship-02.vw3d",	"models/gr-03.vw2d", ""},

	{1,	2, 2,	600,	"models/pirateship/bomber-03.vw3d",	"models/gr-05.vw2d", ""},
	{1,	2, 4,	1200,	"models/pirateship/bomber-02.vw3d",	"models/gr-05.vw2d", ""},
	{1,	6, 4,	1000,	"models/pirateship/bomber-04.vw3d",	"models/gr-03.vw2d", ""},
	{3,	2, 6,	2000,	"models/pirateship/bomber-05.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"},
	{4,	4, 8,	2500,	"models/pirateship/bomber-06.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"},

	{0,	2, 2,	300,	"models/pirateship/bomber-03.vw3d",	"models/gr-05.vw2d", ""},
	{0,	2, 4,	600,	"models/pirateship/bomber-02.vw3d",	"models/gr-05.vw2d", ""},
	{0,	6, 4,	500,	"models/pirateship/bomber-04.vw3d",	"models/gr-03.vw2d", ""},
	{0,	2, 6,	1000,	"models/pirateship/bomber-05.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"},
	{0,	4, 8,	1500,	"models/pirateship/bomber-06.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"}
};
#define PresetPirateShipDataCount sizeof(PresetPirateShipData)/sizeof(PresetPirateShipData[0])




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cPirateShip::cPirateShip(int PirateShipNum)
{
	if ((PirateShipNum <= 0) || ((unsigned int)PirateShipNum > PresetPirateShipDataCount)) {
		std::cerr << __func__ << "(): "
			  << "Could not init cPirateShip object with Number " << PirateShipNum << "\n";
		return;
	}

	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::PirateShip;
	InternalType = PirateShipNum;
	PromptDrawDist2 = 100.0f;
	if (PirateShipNum > 5)
		PromptDrawDist2 = 20000.0f; // большие корабли пиратов

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	MaxSpeed = 20.0f;
	MaxAcceler = 20.0f;
	MaxSpeedRotate = 40.0f;
	Strength = StrengthStart = PresetPirateShipData[PirateShipNum - 1].Strength / GameEnemyArmorPenalty;
	WeaponQuantity = PresetPirateShipData[PirateShipNum - 1].WeaponQuantity;
	BossWeaponQuantity = PresetPirateShipData[PirateShipNum - 1].BossWeaponQuantity;
	unsigned int EngineQuantity = PresetPirateShipData[PirateShipNum - 1].EngineQuantity;


	LoadObjectData(PresetPirateShipData[PirateShipNum - 1].Name, this);

	// всегда только эти текстуры
	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetPirateShipData[PirateShipNum - 1].TextureName);
		if (!PresetPirateShipData[PirateShipNum - 1].NormalMapName.empty() && GameConfig().UseGLSL120)
			NormalMap[i] = GetPreloadedTextureAsset(PresetPirateShipData[PirateShipNum - 1].NormalMapName);
	}


	// начальные установки для оружия
	if (WeaponQuantity > 0) {
		WeaponSetFire = new bool[WeaponQuantity];
		WeaponLocation = new sVECTOR3D[WeaponQuantity];
		WeaponType = new int[WeaponQuantity];
		Weapon = new cWeapon*[WeaponQuantity];
		for (int i = 0; i < WeaponQuantity; i++) {
			WeaponSetFire[i] = false;
			WeaponType[i] = 1;
			Weapon[i] = nullptr;
		}
	}
	if (BossWeaponQuantity > 0) {
		BossWeaponSetFire = new bool[BossWeaponQuantity];
		BossWeaponLocation = new sVECTOR3D[BossWeaponQuantity];
		BossWeaponType = new int[BossWeaponQuantity];
		BossWeapon = new cWeapon*[BossWeaponQuantity];
		for (int i = 0; i < BossWeaponQuantity; i++) {
			BossWeaponSetFire[i] = false;
			BossWeaponType[i] = 1;
			BossWeapon[i] = nullptr;
		}
	}
	// начальные установки для двигателей
	EnginesLocation.resize(EngineQuantity);
	Engines.resize(EngineQuantity);







	// перебираем и ставим нужные данные
	switch (PirateShipNum) {
	case 1:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(1.8f, -1.2f, 6.0f);
		Weapon[0] = new cWeapon(204);
		WeaponLocation[1] = sVECTOR3D(-1.8f, -1.2f, 6.0f);
		Weapon[1] = new cWeapon(204);
		Weapon[1]->SFX = eGameSFX::none;
		WeaponFireType = 1;

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(0.9f, 1.6f, -5.4f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-0.9f, 1.6f, -5.4f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 1);
		break;

	case 2:
		MaxSpeed = 50.0f;
		MaxAcceler = 50.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, -2.0f, 2.0f);
		Weapon[0] = new cWeapon(205);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.1f, 0.5f, -5.7f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.1f, 0.5f, -5.7f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 3:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(3.0f, -0.2f, -0.5f);
		Weapon[0] = new cWeapon(204);
		WeaponLocation[1] = sVECTOR3D(-3.0f, -0.2f, -0.5f);
		Weapon[1] = new cWeapon(204);
		WeaponLocation[2] = sVECTOR3D(2.0f, -0.2f, 0.0f);
		Weapon[2] = new cWeapon(206);
		WeaponLocation[3] = sVECTOR3D(-2.0f, -0.2f, 0.0f);
		Weapon[3] = new cWeapon(206);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.0f, 1.8f, -1.2f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.0f, 1.8f, -1.2f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 1);
		break;
	case 4:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(4.3f, -0.4f, -0.5f);
		Weapon[0] = new cWeapon(211);
		WeaponLocation[1] = sVECTOR3D(-4.3f, -0.4f, -0.5f);
		Weapon[1] = new cWeapon(211);
		WeaponLocation[2] = sVECTOR3D(3.0f, -0.4f, 0.5f);
		Weapon[2] = new cWeapon(205);
		WeaponLocation[3] = sVECTOR3D(-3.0f, -0.4f, 0.5f);
		Weapon[3] = new cWeapon(205);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.3f, 1.9f, -1.7f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.3f, 1.9f, -1.7f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 5:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(1.0f, 4.2f, -5.5f);
		Weapon[0] = new cWeapon(211);
		WeaponLocation[1] = sVECTOR3D(-1.0f, 4.2f, -5.5f);
		Weapon[1] = new cWeapon(211);
		WeaponLocation[2] = sVECTOR3D(0.5f, -1.8f, 5.0f);
		Weapon[2] = new cWeapon(211);
		WeaponLocation[3] = sVECTOR3D(-0.5f, -1.8f, 5.0f);
		Weapon[3] = new cWeapon(211);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(5.8f, 6.5f-4.6f, 0.0f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 3);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-5.8f, 6.5f-4.6f, 0.0f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 3);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.8f, 7.3f-4.6f, 0.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 4);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.8f, 7.3f-4.6f, 0.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 4);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(5.8f, 6.1f-4.6f, -2.3f);
		if (auto sharedEngine = Engines[4].lock()) {
			SetPirateShipEngine(sharedEngine, 1);
			sharedEngine->Direction = sVECTOR3D(0.0f, -0.2f, -0.8f);
		}
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-5.8f, 6.1f-4.6f, -2.3f);
		if (auto sharedEngine = Engines[5].lock()) {
			SetPirateShipEngine(sharedEngine, 1);
			sharedEngine->Direction = sVECTOR3D(0.0f, -0.2f, -0.8f);
		}
		break;

	case 6:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 2.2f, -0.2f);
		Weapon[0] = new cWeapon(202);

		BossWeaponLocation[0] = sVECTOR3D(5.4f, 1.0f, 2.0f);
		BossWeapon[0] = new cWeapon(209);
		BossWeaponLocation[1] = sVECTOR3D(-5.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon(209);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.7f, 6.6f-3.83f, -14.2f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.7f, 6.6f-3.83f, -14.2f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 7:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 2.4f, -0.2f);
		Weapon[0] = new cWeapon(202);

		BossWeaponLocation[0] = sVECTOR3D(9.5f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon(210);
		BossWeaponLocation[1] = sVECTOR3D(-9.5f, 1.3f, 2.0f);
		BossWeapon[1] = new cWeapon(210);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(2.2f, 7.6f-5.25f, -14.8f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-2.2f, 7.6f-5.25f, -14.8f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(17.2f, 6.8f-5.25f, -9.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-17.2f, 6.8f-5.25f, -9.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 8:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 2.4f, -0.2f);
		Weapon[0] = new cWeapon(202);

		BossWeaponLocation[0] = sVECTOR3D(9.1f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon(206);
		BossWeaponLocation[1] = sVECTOR3D(9.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon(206);
		BossWeaponLocation[2] = sVECTOR3D(9.1f, 0.7f, 2.0f);
		BossWeapon[2] = new cWeapon(206);

		BossWeaponLocation[3] = sVECTOR3D(-9.1f, 1.3f, 2.0f);
		BossWeapon[3] = new cWeapon(206);
		BossWeaponLocation[4] = sVECTOR3D(-9.4f, 1.0f, 2.0f);
		BossWeapon[4] = new cWeapon(206);
		BossWeaponLocation[5] = sVECTOR3D(-9.1f, 0.7f, 2.0f);
		BossWeapon[5] = new cWeapon(206);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 6.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(4.5f, 6.3f-3.62f, -12.5f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-4.5f, 6.3f-3.62f, -12.5f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(9.2f, 4.4f-3.62f, -5.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-9.2f, 4.4f-3.62f, -5.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 9:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 4.2f, 7.0f);
		Weapon[0] = new cWeapon(201);
		WeaponLocation[1] = sVECTOR3D(0.0f, 4.2f, 3.5f);
		Weapon[1] = new cWeapon(201);
		Weapon[1]->SetRotation(sVECTOR3D(0,180,0));
		WeaponLocation[2] = sVECTOR3D(0.0f, 3.4f, -5.0f);
		Weapon[2] = new cWeapon(202);

		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 9.0f);
		BossWeapon[0] = new cWeapon(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 9.0f);
		BossWeapon[1] = new cWeapon(205);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.4f, 8.7f-4.9f, -20.4f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.4f, 8.7f-4.9f, -20.4f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(4.7f, 7.0f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-4.7f, 7.0f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(7.5f, 6.8f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-7.5f, 6.8f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 10:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 4.4f, 6.8f);
		Weapon[0] = new cWeapon(201);
		WeaponLocation[1] = sVECTOR3D(0.0f, 4.4f, 2.6f);
		Weapon[1] = new cWeapon(201);
		Weapon[1]->SetRotation(sVECTOR3D(0,180,0));
		WeaponLocation[2] = sVECTOR3D(0.0f, 2.6f, -7.0f);
		Weapon[2] = new cWeapon(202);
		WeaponLocation[3] = sVECTOR3D(0.0f, 2.6f, -16.0f);
		Weapon[3] = new cWeapon(201);
		Weapon[3]->SetRotation(sVECTOR3D(0,180,0));

		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 8.0f);
		BossWeapon[0] = new cWeapon(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 8.0f);
		BossWeapon[1] = new cWeapon(205);
		BossWeaponLocation[2] = sVECTOR3D(16.5f, 2.7f, 7.5f);
		BossWeapon[2] = new cWeapon(205);
		BossWeaponLocation[3] = sVECTOR3D(-16.5f, 2.7f, 7.5f);
		BossWeapon[3] = new cWeapon(205);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(3.8f, 5.4f-4.29f, -19.5f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-3.8f, 5.4f-4.29f, -19.5f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.8f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.8f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(8.0f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-8.0f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(20.0f, 5.0f-4.29f, -3.6f);
		if (auto sharedEngine = Engines[6].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-20.0f, 5.0f-4.29f, -3.6f);
		if (auto sharedEngine = Engines[7].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;





	case 11:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(5.4f, 1.0f, 2.0f);
		BossWeapon[0] = new cWeapon(209);
		BossWeaponLocation[1] = sVECTOR3D(-5.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon(209);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.7f, 6.6f-3.83f, -14.2f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.7f, 6.6f-3.83f, -14.2f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 12:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(9.5f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon(210);
		BossWeaponLocation[1] = sVECTOR3D(-9.5f, 1.3f, 2.0f);
		BossWeapon[1] = new cWeapon(210);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(2.2f, 7.6f-5.25f, -14.8f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-2.2f, 7.6f-5.25f, -14.8f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(17.2f, 6.8f-5.25f, -9.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-17.2f, 6.8f-5.25f, -9.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 13:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(9.1f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon(206);
		BossWeaponLocation[1] = sVECTOR3D(9.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon(206);
		BossWeaponLocation[2] = sVECTOR3D(9.1f, 0.7f, 2.0f);
		BossWeapon[2] = new cWeapon(206);
		BossWeaponLocation[3] = sVECTOR3D(-9.1f, 1.3f, 2.0f);
		BossWeapon[3] = new cWeapon(206);
		BossWeaponLocation[4] = sVECTOR3D(-9.4f, 1.0f, 2.0f);
		BossWeapon[4] = new cWeapon(206);
		BossWeaponLocation[5] = sVECTOR3D(-9.1f, 0.7f, 2.0f);
		BossWeapon[5] = new cWeapon(206);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 6.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(4.5f, 6.3f-3.62f, -12.5f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-4.5f, 6.3f-3.62f, -12.5f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(9.2f, 4.4f-3.62f, -5.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-9.2f, 4.4f-3.62f, -5.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 14:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 9.0f);
		BossWeapon[0] = new cWeapon(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 9.0f);
		BossWeapon[1] = new cWeapon(205);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.4f, 8.7f-4.9f, -20.4f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.4f, 8.7f-4.9f, -20.4f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(4.7f, 7.0f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-4.7f, 7.0f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(7.5f, 6.8f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-7.5f, 6.8f-4.9f, 0.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 15:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 8.0f);
		BossWeapon[0] = new cWeapon(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 8.0f);
		BossWeapon[1] = new cWeapon(205);
		BossWeaponLocation[2] = sVECTOR3D(16.5f, 2.7f, 7.5f);
		BossWeapon[2] = new cWeapon(205);
		BossWeaponLocation[3] = sVECTOR3D(-16.5f, 2.7f, 7.5f);
		BossWeapon[3] = new cWeapon(205);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon(203);
		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(3.8f, 5.4f-4.29f, -19.5f);
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-3.8f, 5.4f-4.29f, -19.5f);
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.8f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.8f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(8.0f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-8.0f, 6.0f-4.29f, -0.9f);
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(20.0f, 5.0f-4.29f, -3.6f);
		if (auto sharedEngine = Engines[6].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-20.0f, 5.0f-4.29f, -3.6f);
		if (auto sharedEngine = Engines[7].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;





	default:
		std::cerr << __func__ << "(): " << "wrong PirateShipNum.\n";
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
