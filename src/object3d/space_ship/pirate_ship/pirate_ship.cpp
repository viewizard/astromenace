/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "pirate_ship.h"



struct sPirateShipData {
	int		WeaponQuantity;
	int		BossWeaponQuantity;
	int		EngineQuantity;
	float	Strength;
	const	char *Name;
	const	char *TextureName;
	const	char *NormalMapName;
	bool	NeedTangentAndBinormal;
};

static sPirateShipData PresetPirateShipData[] = {
	{2,	0, 2,	20,	"models/pirateship/gunship-01.vw3d",	"models/gr-04.vw2d", " ", false},
	{1,	0, 2,	30,	"models/pirateship/bomber-07.vw3d",	"models/gr-04.vw2d", " ", false},
	{4,	0, 2,	100,"models/pirateship/gunship-03.vw3d",	"models/gr-03.vw2d", " ", false},
	{4,	0, 2,	200,"models/pirateship/gunship-04.vw3d",	"models/gr-03.vw2d", " ", false},
	{4,	0, 6,	400,"models/pirateship/gunship-02.vw3d",	"models/gr-03.vw2d", " ", false},

	{1,	2, 2,	600,"models/pirateship/bomber-03.vw3d",	"models/gr-05.vw2d", " ", false},
	{1,	2, 4,	1200,"models/pirateship/bomber-02.vw3d",	"models/gr-05.vw2d", " ", false},
	{1,	6, 4,	1000,"models/pirateship/bomber-04.vw3d",	"models/gr-03.vw2d", " ", false},
	{3,	2, 6,	2000,"models/pirateship/bomber-05.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga", true},
	{4,	4, 8,	2500,"models/pirateship/bomber-06.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga", true},

	{0,	2, 2,	300,"models/pirateship/bomber-03.vw3d",	"models/gr-05.vw2d", " ", false},
	{0,	2, 4,	600,"models/pirateship/bomber-02.vw3d",	"models/gr-05.vw2d", " ", false},
	{0,	6, 4,	500,"models/pirateship/bomber-04.vw3d",	"models/gr-03.vw2d", " ", false},
	{0,	2, 6,	1000,"models/pirateship/bomber-05.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga", true},
	{0,	4, 8,	1500,"models/pirateship/bomber-06.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga", true}
};
#define PresetPirateShipDataCount sizeof(PresetPirateShipData)/sizeof(PresetPirateShipData[0])




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cPirateShip::Create(int PirateShipNum)
{
	if ((PirateShipNum <= 0) || ((unsigned int)PirateShipNum > PresetPirateShipDataCount)) {
		std::cerr << __func__ << "(): "
			  << "Could not init cPirateShip object with Number " << PirateShipNum << "\n";
		return;
	}

	ObjectStatus = 1; // чужой
	ObjectType = 4;
	ObjectCreationType = PirateShipNum;
	PromptDrawDist2 = 100.0f;
	if (PirateShipNum > 5) PromptDrawDist2 = 20000.0f; // большие корабли пиратов

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	MaxSpeed = 20.0f;
	MaxAcceler = 20.0f;
	MaxSpeedRotate = 40.0f;
	//EngineSoundNum = 0;
	EngineLeftQuantity = 0;
	EngineRightQuantity = 0;
	Strength = StrengthStart = PresetPirateShipData[PirateShipNum-1].Strength/GameNPCArmorPenalty;
	WeaponQuantity = PresetPirateShipData[PirateShipNum-1].WeaponQuantity;
	BossWeaponQuantity = PresetPirateShipData[PirateShipNum-1].BossWeaponQuantity;
	EngineQuantity = PresetPirateShipData[PirateShipNum-1].EngineQuantity;


	LoadObjectData(PresetPirateShipData[PirateShipNum-1].Name, this, 0, 2.0f, PresetPirateShipData[PirateShipNum-1].NeedTangentAndBinormal && Setup.UseGLSL);

	// всегда только эти текстуры
	for (int i = 0; i < ObjectsListCount; i++) {
		Texture[i] = vw_FindTextureByName(PresetPirateShipData[PirateShipNum-1].TextureName);
		if (PresetPirateShipData[PirateShipNum-1].NeedTangentAndBinormal && Setup.UseGLSL)
			NormalMap[i] = vw_FindTextureByName(PresetPirateShipData[PirateShipNum-1].NormalMapName);
	}


	// начальные установки для оружия
	if (WeaponQuantity > 0) {
		WeaponSetFire = new bool[WeaponQuantity];
		WeaponLocation = new sVECTOR3D[WeaponQuantity];
		WeaponType = new int[WeaponQuantity];
		Weapon = new cWeapon*[WeaponQuantity];
		for (int i=0; i<WeaponQuantity; i++) {
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
		for (int i=0; i<BossWeaponQuantity; i++) {
			BossWeaponSetFire[i] = false;
			BossWeaponType[i] = 1;
			BossWeapon[i] = nullptr;
		}
	}
	// начальные установки для двигателей
	EngineLocation = new sVECTOR3D[EngineQuantity];
	Engine = new cParticleSystem*[EngineQuantity];
	for (int i = 0; i < EngineQuantity; i++) {
		Engine[i] = nullptr;
	}







	// перебираем и ставим нужные данные
	switch (PirateShipNum) {
	case 1:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(1.8f, -1.2f, 6.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(204);
		WeaponLocation[1] = sVECTOR3D(-1.8f, -1.2f, 6.0f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(204);
		Weapon[1]->SoundNum = 0;
		WeaponFireType = 1;

		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(0.9f, 1.6f, -5.4f);
		SetPirateShipEngine(Engine[0], 1);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-0.9f, 1.6f, -5.4f);
		SetPirateShipEngine(Engine[1], 1);
		break;

	case 2:
		MaxSpeed = 50.0f;
		MaxAcceler = 50.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, -2.0f, 2.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(205);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(1.1f, 0.5f, -5.7f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-1.1f, 0.5f, -5.7f);
		SetPirateShipEngine(Engine[1], 2);
		break;

	case 3:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(3.0f, -0.2f, -0.5f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(204);
		WeaponLocation[1] = sVECTOR3D(-3.0f, -0.2f, -0.5f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(204);
		WeaponLocation[2] = sVECTOR3D(2.0f, -0.2f, 0.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(206);
		WeaponLocation[3] = sVECTOR3D(-2.0f, -0.2f, 0.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(206);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(1.0f, 1.8f, -1.2f);
		SetPirateShipEngine(Engine[0], 1);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-1.0f, 1.8f, -1.2f);
		SetPirateShipEngine(Engine[1], 1);
		break;
	case 4:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(4.3f, -0.4f, -0.5f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(211);
		WeaponLocation[1] = sVECTOR3D(-4.3f, -0.4f, -0.5f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(211);
		WeaponLocation[2] = sVECTOR3D(3.0f, -0.4f, 0.5f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(205);
		WeaponLocation[3] = sVECTOR3D(-3.0f, -0.4f, 0.5f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(205);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(1.3f, 1.9f, -1.7f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-1.3f, 1.9f, -1.7f);
		SetPirateShipEngine(Engine[1], 2);
		break;

	case 5:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;
		// оружие
		WeaponLocation[0] = sVECTOR3D(1.0f, 4.2f, -5.5f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(211);
		WeaponLocation[1] = sVECTOR3D(-1.0f, 4.2f, -5.5f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(211);
		WeaponLocation[2] = sVECTOR3D(0.5f, -1.8f, 5.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(211);
		WeaponLocation[3] = sVECTOR3D(-0.5f, -1.8f, 5.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(211);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(5.8f, 6.5f-4.6f, 0.0f);
		SetPirateShipEngine(Engine[0], 3);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-5.8f, 6.5f-4.6f, 0.0f);
		SetPirateShipEngine(Engine[1], 3);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(5.8f, 7.3f-4.6f, 0.0f);
		SetPirateShipEngine(Engine[2], 4);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-5.8f, 7.3f-4.6f, 0.0f);
		SetPirateShipEngine(Engine[3], 4);
		Engine[4] = new cParticleSystem;
		EngineLocation[4] = sVECTOR3D(5.8f, 6.1f-4.6f, -2.3f);
		SetPirateShipEngine(Engine[4], 1);
		Engine[4]->Direction = sVECTOR3D(0.0f, -0.2f, -0.8f);
		Engine[5] = new cParticleSystem;
		EngineLocation[5] = sVECTOR3D(-5.8f, 6.1f-4.6f, -2.3f);
		SetPirateShipEngine(Engine[5], 1);
		Engine[5]->Direction = sVECTOR3D(0.0f, -0.2f, -0.8f);
		break;

	case 6:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 2.2f, -0.2f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(202);

		BossWeaponLocation[0] = sVECTOR3D(5.4f, 1.0f, 2.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(209);
		BossWeaponLocation[1] = sVECTOR3D(-5.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(209);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(1.7f, 6.6f-3.83f, -14.2f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-1.7f, 6.6f-3.83f, -14.2f);
		SetPirateShipEngine(Engine[1], 2);
		break;

	case 7:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 2.4f, -0.2f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(202);

		BossWeaponLocation[0] = sVECTOR3D(9.5f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(210);
		BossWeaponLocation[1] = sVECTOR3D(-9.5f, 1.3f, 2.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(210);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(2.2f, 7.6f-5.25f, -14.8f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-2.2f, 7.6f-5.25f, -14.8f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(17.2f, 6.8f-5.25f, -9.0f);
		SetPirateShipEngine(Engine[2], 5);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-17.2f, 6.8f-5.25f, -9.0f);
		SetPirateShipEngine(Engine[3], 5);
		break;

	case 8:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 2.4f, -0.2f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(202);

		BossWeaponLocation[0] = sVECTOR3D(9.1f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(206);
		BossWeaponLocation[1] = sVECTOR3D(9.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(206);
		BossWeaponLocation[2] = sVECTOR3D(9.1f, 0.7f, 2.0f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(206);

		BossWeaponLocation[3] = sVECTOR3D(-9.1f, 1.3f, 2.0f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(206);
		BossWeaponLocation[4] = sVECTOR3D(-9.4f, 1.0f, 2.0f);
		BossWeapon[4] = new cWeapon;
		BossWeapon[4]->Create(206);
		BossWeaponLocation[5] = sVECTOR3D(-9.1f, 0.7f, 2.0f);
		BossWeapon[5] = new cWeapon;
		BossWeapon[5]->Create(206);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 6.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(4.5f, 6.3f-3.62f, -12.5f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-4.5f, 6.3f-3.62f, -12.5f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(9.2f, 4.4f-3.62f, -5.6f);
		SetPirateShipEngine(Engine[2], 5);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-9.2f, 4.4f-3.62f, -5.6f);
		SetPirateShipEngine(Engine[3], 5);
		break;

	case 9:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 4.2f, 7.0f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(201);
		WeaponLocation[1] = sVECTOR3D(0.0f, 4.2f, 3.5f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(201);
		Weapon[1]->SetRotation(sVECTOR3D(0,180,0));
		WeaponLocation[2] = sVECTOR3D(0.0f, 3.4f, -5.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(202);

		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 9.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 9.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(205);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(1.4f, 8.7f-4.9f, -20.4f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-1.4f, 8.7f-4.9f, -20.4f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(4.7f, 7.0f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[2], 2);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-4.7f, 7.0f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[3], 2);
		Engine[4] = new cParticleSystem;
		EngineLocation[4] = sVECTOR3D(7.5f, 6.8f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[4], 2);
		Engine[5] = new cParticleSystem;
		EngineLocation[5] = sVECTOR3D(-7.5f, 6.8f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[5], 2);
		break;

	case 10:
		// оружие
		WeaponLocation[0] = sVECTOR3D(0.0f, 4.4f, 6.8f);
		Weapon[0] = new cWeapon;
		Weapon[0]->Create(201);
		WeaponLocation[1] = sVECTOR3D(0.0f, 4.4f, 2.6f);
		Weapon[1] = new cWeapon;
		Weapon[1]->Create(201);
		Weapon[1]->SetRotation(sVECTOR3D(0,180,0));
		WeaponLocation[2] = sVECTOR3D(0.0f, 2.6f, -7.0f);
		Weapon[2] = new cWeapon;
		Weapon[2]->Create(202);
		WeaponLocation[3] = sVECTOR3D(0.0f, 2.6f, -16.0f);
		Weapon[3] = new cWeapon;
		Weapon[3]->Create(201);
		Weapon[3]->SetRotation(sVECTOR3D(0,180,0));

		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 8.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 8.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(205);
		BossWeaponLocation[2] = sVECTOR3D(16.5f, 2.7f, 7.5f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(205);
		BossWeaponLocation[3] = sVECTOR3D(-16.5f, 2.7f, 7.5f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(205);

		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(3.8f, 5.4f-4.29f, -19.5f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-3.8f, 5.4f-4.29f, -19.5f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(5.8f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[2], 2);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-5.8f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[3], 2);
		Engine[4] = new cParticleSystem;
		EngineLocation[4] = sVECTOR3D(8.0f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[4], 2);
		Engine[5] = new cParticleSystem;
		EngineLocation[5] = sVECTOR3D(-8.0f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[5], 2);
		Engine[6] = new cParticleSystem;
		EngineLocation[6] = sVECTOR3D(20.0f, 5.0f-4.29f, -3.6f);
		SetPirateShipEngine(Engine[6], 2);
		Engine[7] = new cParticleSystem;
		EngineLocation[7] = sVECTOR3D(-20.0f, 5.0f-4.29f, -3.6f);
		SetPirateShipEngine(Engine[7], 2);
		break;





	case 11:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(5.4f, 1.0f, 2.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(209);
		BossWeaponLocation[1] = sVECTOR3D(-5.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(209);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(1.7f, 6.6f-3.83f, -14.2f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-1.7f, 6.6f-3.83f, -14.2f);
		SetPirateShipEngine(Engine[1], 2);
		break;

	case 12:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(9.5f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(210);
		BossWeaponLocation[1] = sVECTOR3D(-9.5f, 1.3f, 2.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(210);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 5.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(2.2f, 7.6f-5.25f, -14.8f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-2.2f, 7.6f-5.25f, -14.8f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(17.2f, 6.8f-5.25f, -9.0f);
		SetPirateShipEngine(Engine[2], 5);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-17.2f, 6.8f-5.25f, -9.0f);
		SetPirateShipEngine(Engine[3], 5);
		break;

	case 13:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(9.1f, 1.3f, 2.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(206);
		BossWeaponLocation[1] = sVECTOR3D(9.4f, 1.0f, 2.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(206);
		BossWeaponLocation[2] = sVECTOR3D(9.1f, 0.7f, 2.0f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(206);
		BossWeaponLocation[3] = sVECTOR3D(-9.1f, 1.3f, 2.0f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(206);
		BossWeaponLocation[4] = sVECTOR3D(-9.4f, 1.0f, 2.0f);
		BossWeapon[4] = new cWeapon;
		BossWeapon[4]->Create(206);
		BossWeaponLocation[5] = sVECTOR3D(-9.1f, 0.7f, 2.0f);
		BossWeapon[5] = new cWeapon;
		BossWeapon[5]->Create(206);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 6.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(4.5f, 6.3f-3.62f, -12.5f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-4.5f, 6.3f-3.62f, -12.5f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(9.2f, 4.4f-3.62f, -5.6f);
		SetPirateShipEngine(Engine[2], 5);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-9.2f, 4.4f-3.62f, -5.6f);
		SetPirateShipEngine(Engine[3], 5);
		break;

	case 14:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 9.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 9.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(205);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(1.4f, 8.7f-4.9f, -20.4f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-1.4f, 8.7f-4.9f, -20.4f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(4.7f, 7.0f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[2], 2);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-4.7f, 7.0f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[3], 2);
		Engine[4] = new cParticleSystem;
		EngineLocation[4] = sVECTOR3D(7.5f, 6.8f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[4], 2);
		Engine[5] = new cParticleSystem;
		EngineLocation[5] = sVECTOR3D(-7.5f, 6.8f-4.9f, 0.0f);
		SetPirateShipEngine(Engine[5], 2);
		break;

	case 15:
		// оружие
		BossWeaponLocation[0] = sVECTOR3D(11.1f, 2.7f, 8.0f);
		BossWeapon[0] = new cWeapon;
		BossWeapon[0]->Create(205);
		BossWeaponLocation[1] = sVECTOR3D(-11.1f, 2.7f, 8.0f);
		BossWeapon[1] = new cWeapon;
		BossWeapon[1]->Create(205);
		BossWeaponLocation[2] = sVECTOR3D(16.5f, 2.7f, 7.5f);
		BossWeapon[2] = new cWeapon;
		BossWeapon[2]->Create(205);
		BossWeaponLocation[3] = sVECTOR3D(-16.5f, 2.7f, 7.5f);
		BossWeapon[3] = new cWeapon;
		BossWeapon[3]->Create(205);
		// фларе
		WeaponFlareLocation = sVECTOR3D(0.0f, 7.0f, 0.0f);
		WeaponFlare = new cWeapon;
		WeaponFlare->Create(203);
		// двигатели
		Engine[0] = new cParticleSystem;
		EngineLocation[0] = sVECTOR3D(3.8f, 5.4f-4.29f, -19.5f);
		SetPirateShipEngine(Engine[0], 2);
		Engine[1] = new cParticleSystem;
		EngineLocation[1] = sVECTOR3D(-3.8f, 5.4f-4.29f, -19.5f);
		SetPirateShipEngine(Engine[1], 2);
		Engine[2] = new cParticleSystem;
		EngineLocation[2] = sVECTOR3D(5.8f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[2], 2);
		Engine[3] = new cParticleSystem;
		EngineLocation[3] = sVECTOR3D(-5.8f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[3], 2);
		Engine[4] = new cParticleSystem;
		EngineLocation[4] = sVECTOR3D(8.0f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[4], 2);
		Engine[5] = new cParticleSystem;
		EngineLocation[5] = sVECTOR3D(-8.0f, 6.0f-4.29f, -0.9f);
		SetPirateShipEngine(Engine[5], 2);
		Engine[6] = new cParticleSystem;
		EngineLocation[6] = sVECTOR3D(20.0f, 5.0f-4.29f, -3.6f);
		SetPirateShipEngine(Engine[6], 2);
		Engine[7] = new cParticleSystem;
		EngineLocation[7] = sVECTOR3D(-20.0f, 5.0f-4.29f, -3.6f);
		SetPirateShipEngine(Engine[7], 2);
		break;





	default:
		std::cerr << __func__ << "(): " << "wrong PirateShipNum.\n";
		return;
	}


	for (int i = 0; i < EngineQuantity; i++) {
		Engine[i]->SetStartLocation(EngineLocation[i]);
		// находим кол-во внутренних источников света
		if (Engine[i]->Light != nullptr)
			InternalLights++;
	}


	// находим все данные по геометрии
	::cObject3D::InitByDrawObjectList();
}

