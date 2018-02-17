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


/// подключаем нужные файлы
#include "alien_space_mothership.h"



struct AlienSpaceMotherShipData
{

	int		WeaponQuantity;
	int		BossWeaponQuantity;
	int		EngineQuantity;
	float	Strength;
	float	ShieldStrength;
	const	char *Name;
	const	char *Texture;
	const	char *TextureIllum;
};

static AlienSpaceMotherShipData PresetAlienSpaceMotherShipData[] =
{
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
void CAlienSpaceMotherShip::Create(int	SpaceShipNum)
{
	if ((SpaceShipNum <= 0) || ((unsigned int)SpaceShipNum > PresetAlienSpaceMotherShipDataCount))
	{
		fprintf(stderr, "!!! Couldn't init CAlienSpaceMotherShip object with Number %i.\n", SpaceShipNum);
		return;
	}

	ObjectStatus = 1; // чужой
	ObjectType = 3;
	ObjectCreationType = SpaceShipNum;
	PromptDrawDist2 = 20000.0f;

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	MaxSpeed = 20.0f;
	MaxAcceler = 20.0f;
	MaxSpeedRotate = 80.0f;
	//EngineSoundNum = 0;
	EngineLeftQuantity = 0;
	EngineRightQuantity = 0;
	Strength = StrengthStart = PresetAlienSpaceMotherShipData[SpaceShipNum-1].Strength/GameNPCArmorPenalty;
	ShieldStrength = ShieldStrengthStart = PresetAlienSpaceMotherShipData[SpaceShipNum-1].ShieldStrength/GameNPCArmorPenalty;
	ShieldRecharge = ShieldStrengthStart/15.0f;

	WeaponQuantity = PresetAlienSpaceMotherShipData[SpaceShipNum-1].WeaponQuantity;
	BossWeaponQuantity = PresetAlienSpaceMotherShipData[SpaceShipNum-1].BossWeaponQuantity;
	EngineQuantity = PresetAlienSpaceMotherShipData[SpaceShipNum-1].EngineQuantity;


	LoadObjectData(PresetAlienSpaceMotherShipData[SpaceShipNum-1].Name, this, 0, 2.0f, Setup.UseGLSL);

	// всегда только эти текстуры
	for (int i=0; i<DrawObjectQuantity; i++)
	{
		Texture[i] =vw_FindTextureByName(PresetAlienSpaceMotherShipData[SpaceShipNum-1].Texture);
		TextureIllum[i] =vw_FindTextureByName(PresetAlienSpaceMotherShipData[SpaceShipNum-1].TextureIllum);
		// если шейдеры выключены - вернет ноль (не загружаем текстуры нормал мепов если нет шейдеров)
		// в LoadObjectData указываем Setup.UseGLSL
		NormalMap[i] = vw_FindTextureByName("models/normalmap/alien_mothership_nm.tga");
	}



	// начальные установки для оружия
	WeaponSetFire = new bool[WeaponQuantity];
	WeaponLocation = new VECTOR3D[WeaponQuantity];
	WeaponType = new int[WeaponQuantity];
	Weapon = new CWeapon*[WeaponQuantity];
	for (int i=0; i<WeaponQuantity; i++)
	{
		WeaponSetFire[i] = false;
		WeaponType[i] = 1;
		Weapon[i] = 0;
	}

	BossWeaponSetFire = new bool[BossWeaponQuantity];
	BossWeaponLocation = new VECTOR3D[BossWeaponQuantity];
	BossWeaponType = new int[BossWeaponQuantity];
	BossWeapon = new CWeapon*[BossWeaponQuantity];
	for (int i=0; i<BossWeaponQuantity; i++)
	{
		BossWeaponSetFire[i] = false;
		BossWeaponType[i] = 1;
		BossWeapon[i] = 0;
	}

	// начальные установки для двигателей
	EngineLocation = new VECTOR3D[EngineQuantity];
	Engine = new eParticleSystem*[EngineQuantity];
	for (int i=0; i<EngineQuantity; i++) Engine[i] = 0;










	// перебираем и ставим нужные данные
	switch (SpaceShipNum)
	{
		case 1:
			// оружие
			WeaponLocation[0] = VECTOR3D(-1.6f, 1.0f, 19.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(102);
			WeaponLocation[1] = VECTOR3D(1.6f, 1.0f, 19.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(102);
			WeaponLocation[2] = VECTOR3D(-1.6f, -1.0f, 19.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(102);
			WeaponLocation[3] = VECTOR3D(1.6f, -1.0f, 19.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(102);

			WeaponLocation[4] = VECTOR3D(-1.6f, 1.5f, 18.0f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(103);
			WeaponLocation[5] = VECTOR3D(1.6f, 1.5f, 18.0f);
			Weapon[5] = new CWeapon;
			Weapon[5]->Create(103);
			WeaponLocation[6] = VECTOR3D(-1.6f, -1.5f, 18.0f);
			Weapon[6] = new CWeapon;
			Weapon[6]->Create(103);
			WeaponLocation[7] = VECTOR3D(1.6f, -1.5f, 18.0f);
			Weapon[7] = new CWeapon;
			Weapon[7]->Create(103);

			WeaponLocation[8] = VECTOR3D(-1.6f, 1.0f, 19.0f);
			Weapon[8] = new CWeapon;
			Weapon[8]->Create(104);
			WeaponLocation[9] = VECTOR3D(1.6f, 1.0f, 19.0f);
			Weapon[9] = new CWeapon;
			Weapon[9]->Create(104);
			WeaponLocation[10] = VECTOR3D(-1.6f, -1.0f, 19.0f);
			Weapon[10] = new CWeapon;
			Weapon[10]->Create(104);
			WeaponLocation[11] = VECTOR3D(1.6f, -1.0f, 19.0f);
			Weapon[11] = new CWeapon;
			Weapon[11]->Create(104);

			WeaponLocation[12] = VECTOR3D(0.0f, -8.0f, 0.0f);
			Weapon[12] = new CWeapon;
			Weapon[12]->Create(104);
			WeaponLocation[13] = VECTOR3D(0.0f, -8.0f, 0.0f);
			Weapon[13] = new CWeapon;
			Weapon[13]->Create(104);

			BossWeaponLocation[0] = VECTOR3D(0.0f, 0.0f, 0.0f);

			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(8.4f, 5.2f, -24.0f);
			SetAlienSpaceMotherShipEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-8.4f, 5.2f, -24.0f);
			SetAlienSpaceMotherShipEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(8.4f, -5.6f, -24.0f);
			SetAlienSpaceMotherShipEngine(Engine[2], 1);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-8.4f, -5.6f, -24.0f);
			SetAlienSpaceMotherShipEngine(Engine[3], 1);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, 8.2f, -15.0f);
			SetAlienSpaceMotherShipEngine(Engine[4], 2);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(0.0f, -8.2f, -15.0f);
			SetAlienSpaceMotherShipEngine(Engine[5], 2);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(0.0f, 0.0f, -8.0f);
			SetAlienSpaceMotherShipEngine(Engine[6], 3);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(0.0f, 0.0f, -8.0f);
			SetAlienSpaceMotherShipEngine(Engine[7], 4);
			Engine[8] = new eParticleSystem;
			EngineLocation[8] = VECTOR3D(0.0f, 0.0f, 15.0f);
			SetAlienSpaceMotherShipEngine(Engine[8], 5);
			Engine[9] = new eParticleSystem;
			EngineLocation[9] = VECTOR3D(0.0f, 0.0f, 15.0f);
			SetAlienSpaceMotherShipEngine(Engine[9], 6);
			break;
		case 2:
			// оружие
			WeaponLocation[0] = VECTOR3D(-10.7f, 0.0f, -18.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(103);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
			WeaponLocation[1] = VECTOR3D(10.7f, 0.0f, -18.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(103);
			Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
			WeaponLocation[2] = VECTOR3D(-10.7f, 0.0f, -18.5f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(103);
			Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
			WeaponLocation[3] = VECTOR3D(10.7f, 0.0f, -18.5f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(103);
			Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;

			WeaponLocation[4] = VECTOR3D(-10.7f, 0.0f, -18.5f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(106);
			Weapon[4]->NextFireTime = Weapon[4]->NextFireTime/2.0f;
			WeaponLocation[5] = VECTOR3D(10.7f, 0.0f, -18.5f);
			Weapon[5] = new CWeapon;
			Weapon[5]->Create(106);
			Weapon[5]->NextFireTime = Weapon[5]->NextFireTime/2.0f;
			WeaponLocation[6] = VECTOR3D(-4.0f, 0.0f, 24.0f);
			Weapon[6] = new CWeapon;
			Weapon[6]->Create(106);
			Weapon[6]->NextFireTime = Weapon[6]->NextFireTime/2.0f;
			WeaponLocation[7] = VECTOR3D(4.0f, 0.0f, 24.0f);
			Weapon[7] = new CWeapon;
			Weapon[7]->Create(106);
			Weapon[7]->NextFireTime = Weapon[7]->NextFireTime/2.0f;


			BossWeaponLocation[0] = VECTOR3D(-1.5f, 1.5f, 25.0f);
			BossWeapon[0] = new CWeapon;
			BossWeapon[0]->Create(102);
			BossWeaponLocation[1] = VECTOR3D(1.5f, 1.5f, 25.0f);
			BossWeapon[1] = new CWeapon;
			BossWeapon[1]->Create(102);
			BossWeaponLocation[2] = VECTOR3D(-1.5f, -1.5f, 25.0f);
			BossWeapon[2] = new CWeapon;
			BossWeapon[2]->Create(102);
			BossWeaponLocation[3] = VECTOR3D(1.5f, -1.5f, 25.0f);
			BossWeapon[3] = new CWeapon;
			BossWeapon[3]->Create(102);


			BossWeaponLocation[4] = VECTOR3D(-4.0f, 2.0f, 24.0f);
			BossWeapon[4] = new CWeapon;
			BossWeapon[4]->Create(102);
			BossWeaponLocation[5] = VECTOR3D(4.0f, 2.0f, 24.0f);
			BossWeapon[5] = new CWeapon;
			BossWeapon[5]->Create(102);
			BossWeaponLocation[6] = VECTOR3D(-4.0f, -2.0f, 24.0f);
			BossWeapon[6] = new CWeapon;
			BossWeapon[6]->Create(102);
			BossWeaponLocation[7] = VECTOR3D(4.0f, -2.0f, 24.0f);
			BossWeapon[7] = new CWeapon;
			BossWeapon[7]->Create(102);


			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(5.6f, 7.0f, -27.6f);
			SetAlienSpaceMotherShipEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-5.6f, 7.0f, -27.6f);
			SetAlienSpaceMotherShipEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(5.6f, -7.0f, -27.6f);
			SetAlienSpaceMotherShipEngine(Engine[2], 1);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-5.6f, -7.0f, -27.6f);
			SetAlienSpaceMotherShipEngine(Engine[3], 1);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(10.7f, 0.0f, -21.5f);
			SetAlienSpaceMotherShipEngine(Engine[4], 2);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-10.7f, 0.0f, -21.5f);
			SetAlienSpaceMotherShipEngine(Engine[5], 2);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(0.0f, 0.0f, -14.0f);
			SetAlienSpaceMotherShipEngine(Engine[6], 3);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(0.0f, 0.0f, -14.0f);
			SetAlienSpaceMotherShipEngine(Engine[7], 4);
			Engine[8] = new eParticleSystem;
			EngineLocation[8] = VECTOR3D(0.0f, 0.0f, 15.0f);
			SetAlienSpaceMotherShipEngine(Engine[8], 5);
			Engine[9] = new eParticleSystem;
			EngineLocation[9] = VECTOR3D(0.0f, 0.0f, 15.0f);
			SetAlienSpaceMotherShipEngine(Engine[9], 6);
			break;
		case 3:
			// оружие
			WeaponLocation[0] = VECTOR3D(3.0f, -2.0f, 13.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(104);
			WeaponLocation[1] = VECTOR3D(-3.0f, -2.0f, 13.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(104);
			WeaponLocation[2] = VECTOR3D(2.0f, -1.0f, 13.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(104);
			WeaponLocation[3] = VECTOR3D(-2.0f, -1.0f, 13.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(104);
			WeaponLocation[4] = VECTOR3D(2.0f, -3.0f, 13.0f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(104);
			WeaponLocation[5] = VECTOR3D(-2.0f, -3.0f, 13.0f);
			Weapon[5] = new CWeapon;
			Weapon[5]->Create(104);

			WeaponLocation[6] = VECTOR3D(7.0f, -2.0f, 13.0f);
			Weapon[6] = new CWeapon;
			Weapon[6]->Create(109);
			WeaponLocation[7] = VECTOR3D(-7.0f, -2.0f, 13.0f);
			Weapon[7] = new CWeapon;
			Weapon[7]->Create(109);


			BossWeaponLocation[0] = VECTOR3D(0.0f, -2.0f, 27.0f);
			BossWeapon[0] = new CWeapon;
			BossWeapon[0]->Create(110);

			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(5.5f, 8.1f, -26.1f);
			SetAlienSpaceMotherShipEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-5.5f, 8.1f, -26.1f);
			SetAlienSpaceMotherShipEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(5.6f, -7.2f, -28.6f);
			SetAlienSpaceMotherShipEngine(Engine[2], 1);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-5.6f, -7.2f, -28.6f);
			SetAlienSpaceMotherShipEngine(Engine[3], 1);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(11.9f, -1.0f, -14.8f);
			SetAlienSpaceMotherShipEngine(Engine[4], 1);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-11.9f, -1.0f, -14.8f);
			SetAlienSpaceMotherShipEngine(Engine[5], 1);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(0.0f, -1.0f, -10.0f);
			SetAlienSpaceMotherShipEngine(Engine[6], 3);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(0.0f, -1.0f, -10.0f);
			SetAlienSpaceMotherShipEngine(Engine[7], 4);
			break;
		case 4:
			// оружие
			WeaponLocation[0] = VECTOR3D(1.0f, -1.0f, 23.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(104);
			WeaponLocation[1] = VECTOR3D(-1.0f, -1.0f, 23.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(104);
			WeaponLocation[2] = VECTOR3D(1.0f, 0.0f, 23.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(104);
			WeaponLocation[3] = VECTOR3D(-1.0f, 0.0f, 23.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(104);
			WeaponLocation[4] = VECTOR3D(1.0f, -2.0f, 23.0f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(104);
			WeaponLocation[5] = VECTOR3D(-1.0f, -2.0f, 23.0f);
			Weapon[5] = new CWeapon;
			Weapon[5]->Create(104);
			WeaponLocation[6] = VECTOR3D(0.0f, -1.0f, 23.0f);
			Weapon[6] = new CWeapon;
			Weapon[6]->Create(109);
			WeaponLocation[7] = VECTOR3D(0.0f, -1.0f, 23.0f);
			Weapon[7] = new CWeapon;
			Weapon[7]->Create(109);

			BossWeaponLocation[0] = VECTOR3D(8.9f, -0.6f, 18.0f);
			BossWeapon[0] = new CWeapon;
			BossWeapon[0]->Create(108);
			BossWeapon[0]->SetRotation(VECTOR3D(0.0f, -15.0f, 0.0f));
			BossWeaponLocation[1] = VECTOR3D(-8.9f, -0.6f, 18.0f);
			BossWeapon[1] = new CWeapon;
			BossWeapon[1]->Create(108);
			BossWeapon[1]->SetRotation(VECTOR3D(0.0f, 15.0f, 0.0f));
			BossWeaponLocation[2] = VECTOR3D(10.0f, -5.6f, 18.0f);
			BossWeapon[2] = new CWeapon;
			BossWeapon[2]->Create(108);
			BossWeaponLocation[3] = VECTOR3D(-10.0f, -5.6f, 18.0f);
			BossWeapon[3] = new CWeapon;
			BossWeapon[3]->Create(108);
			BossWeaponLocation[4] = VECTOR3D(8.9f, -0.6f, 18.0f);
			BossWeapon[4] = new CWeapon;
			BossWeapon[4]->Create(108);
			BossWeapon[4]->SetRotation(VECTOR3D(0.0f, -5.0f, 0.0f));
			BossWeaponLocation[5] = VECTOR3D(-8.9f, -0.6f, 18.0f);
			BossWeapon[5] = new CWeapon;
			BossWeapon[5]->Create(108);
			BossWeapon[5]->SetRotation(VECTOR3D(0.0f, 5.0f, 0.0f));
			BossWeaponLocation[6] = VECTOR3D(10.0f, -5.6f, 18.0f);
			BossWeapon[6] = new CWeapon;
			BossWeapon[6]->Create(108);
			BossWeapon[6]->SetRotation(VECTOR3D(0.0f, -10.0f, 0.0f));
			BossWeaponLocation[7] = VECTOR3D(-10.0f, -5.6f, 18.0f);
			BossWeapon[7] = new CWeapon;
			BossWeapon[7]->Create(108);
			BossWeapon[7]->SetRotation(VECTOR3D(0.0f, 10.0f, 0.0f));


			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(1.9f, 5.9f, -24.6f);
			SetAlienSpaceMotherShipEngine(Engine[0], 2);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-1.9f, 5.9f, -24.6f);
			SetAlienSpaceMotherShipEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(1.9f, -2.9f, -24.6f);
			SetAlienSpaceMotherShipEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-1.9f, -2.9f, -24.6f);
			SetAlienSpaceMotherShipEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(2.9f, 1.6f, -24.6f);
			SetAlienSpaceMotherShipEngine(Engine[4], 1);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-2.9f, 1.6f, -24.6f);
			SetAlienSpaceMotherShipEngine(Engine[5], 1);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(8.9f, -0.6f, -12.6f);
			SetAlienSpaceMotherShipEngine(Engine[6], 1);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(-8.9f, -0.6f, -12.6f);
			SetAlienSpaceMotherShipEngine(Engine[7], 1);
			Engine[8] = new eParticleSystem;
			EngineLocation[8] = VECTOR3D(10.0f, -5.6f, -5.2f);
			SetAlienSpaceMotherShipEngine(Engine[8], 1);
			Engine[9] = new eParticleSystem;
			EngineLocation[9] = VECTOR3D(-10.0f, -5.6f, -5.2f);
			SetAlienSpaceMotherShipEngine(Engine[9], 1);
			Engine[10] = new eParticleSystem;
			EngineLocation[10] = VECTOR3D(0.0f, -6.0f, 10.0f);
			SetAlienSpaceMotherShipEngine(Engine[10], 5);
			Engine[10]->CreationSize = VECTOR3D(6.0f,6.0f,2.0f);
			Engine[10]->DeadZone = 5.9f;
			Engine[11] = new eParticleSystem;
			EngineLocation[11] = VECTOR3D(0.0f, -6.0f, 10.0f);
			SetAlienSpaceMotherShipEngine(Engine[11], 6);
			Engine[11]->CreationSize = VECTOR3D(6.0f,6.0f,2.0f);
			Engine[11]->DeadZone = 5.9f;
			break;
		case 5:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(109);
			WeaponLocation[1] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(109);
			Weapon[1]->SetRotation(VECTOR3D(0.0f, 3.0f, 0.0f));
			WeaponLocation[2] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(109);
			Weapon[2]->SetRotation(VECTOR3D(0.0f, -3.0f, 0.0f));
			WeaponLocation[3] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(109);
			Weapon[3]->SetRotation(VECTOR3D(0.0f, 6.0f, 0.0f));
			WeaponLocation[4] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(109);
			WeaponLocation[5] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[5] = new CWeapon;
			Weapon[5]->Create(109);
			Weapon[5]->SetRotation(VECTOR3D(0.0f, -6.0f, 0.0f));
			WeaponLocation[6] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[6] = new CWeapon;
			Weapon[6]->Create(109);
			Weapon[6]->SetRotation(VECTOR3D(0.0f, 9.0f, 0.0f));
			WeaponLocation[7] = VECTOR3D(0.0f, -2.4f, 20.0f);
			Weapon[7] = new CWeapon;
			Weapon[7]->Create(109);
			Weapon[7]->SetRotation(VECTOR3D(0.0f, -9.0f, 0.0f));

			BossWeaponLocation[0] = VECTOR3D(35.7f, -3.0f, -13.0f);
			BossWeapon[0] = new CWeapon;
			BossWeapon[0]->Create(106);
			BossWeaponLocation[1] = VECTOR3D(-35.7f, -3.0f, -13.0f);
			BossWeapon[1] = new CWeapon;
			BossWeapon[1]->Create(106);
			BossWeaponLocation[2] = VECTOR3D(0.0f, -2.4f, 20.0f);
			BossWeapon[2] = new CWeapon;
			BossWeapon[2]->Create(106);
			BossWeaponLocation[3] = VECTOR3D(0.0f, -2.4f, 20.0f);
			BossWeapon[3] = new CWeapon;
			BossWeapon[3]->Create(106);
			BossWeaponLocation[4] = VECTOR3D(8.85f, 5.65f, -10.2f);
			BossWeapon[4] = new CWeapon;
			BossWeapon[4]->Create(104);
			BossWeaponLocation[5] = VECTOR3D(-8.85f, 5.65f, -10.2f);
			BossWeapon[5] = new CWeapon;
			BossWeapon[5]->Create(104);

			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(25.1f, 0.65f, -18.8f);
			SetAlienSpaceMotherShipEngine(Engine[0], 9);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-25.1f, 0.65f, -18.8f);
			SetAlienSpaceMotherShipEngine(Engine[1], 9);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(20.6f, 0.65f, -18.8f);
			SetAlienSpaceMotherShipEngine(Engine[2], 9);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-20.6f, 0.65f, -18.8f);
			SetAlienSpaceMotherShipEngine(Engine[3], 9);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(22.9f, 0.65f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[4], 10);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-22.9f, 0.65f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[5], 10);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(22.9f, -5.1f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[6], 10);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(-22.9f, -5.1f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[7], 10);
			Engine[8] = new eParticleSystem;
			EngineLocation[8] = VECTOR3D(8.85f, 5.65f, -15.2f);
			SetAlienSpaceMotherShipEngine(Engine[8], 10);
			Engine[9] = new eParticleSystem;
			EngineLocation[9] = VECTOR3D(-8.85f, 5.65f, -15.2f);
			SetAlienSpaceMotherShipEngine(Engine[9], 10);
			Engine[10] = new eParticleSystem;
			EngineLocation[10] = VECTOR3D(4.0f, 7.0f, -21.2f);
			SetAlienSpaceMotherShipEngine(Engine[10], 11);
			Engine[11] = new eParticleSystem;
			EngineLocation[11] = VECTOR3D(-4.0f, 7.0f, -21.2f);
			SetAlienSpaceMotherShipEngine(Engine[11], 11);
			Engine[12] = new eParticleSystem;
			EngineLocation[12] = VECTOR3D(20.4f, -2.4f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[12], 11);
			Engine[13] = new eParticleSystem;
			EngineLocation[13] = VECTOR3D(-20.4f, -2.4f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[13], 11);
			Engine[14] = new eParticleSystem;
			EngineLocation[14] = VECTOR3D(25.2f, -2.4f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[14], 11);
			Engine[15] = new eParticleSystem;
			EngineLocation[15] = VECTOR3D(-25.2f, -2.4f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[15], 11);
			Engine[16] = new eParticleSystem;
			EngineLocation[16] = VECTOR3D(35.7f, -3.0f, -13.0f);
			SetAlienSpaceMotherShipEngine(Engine[16], 12);
			Engine[16]->Direction = VECTOR3D(0.4f, 0.0f, -0.8f);
			Engine[17] = new eParticleSystem;
			EngineLocation[17] = VECTOR3D(-35.7f, -3.0f, -13.0f);
			SetAlienSpaceMotherShipEngine(Engine[17], 12);
			Engine[17]->Direction = VECTOR3D(-0.4f, 0.0f, -0.8f);
			Engine[18] = new eParticleSystem;
			EngineLocation[18] = VECTOR3D(0.0f, -2.4f, -23.0f);
			SetAlienSpaceMotherShipEngine(Engine[18], 13);
			break;


		case 6:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, -4.4f, 18.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(103);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
			WeaponLocation[1] = VECTOR3D(0.0f, -4.4f, 18.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(103);
			Weapon[1]->SetRotation(VECTOR3D(0.0f, 3.0f, 0.0f));
			Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
			WeaponLocation[2] = VECTOR3D(0.0f, -4.4f, 18.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(103);
			Weapon[2]->SetRotation(VECTOR3D(0.0f, -3.0f, 0.0f));
			Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
			WeaponLocation[3] = VECTOR3D(0.0f, -4.4f, 18.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(103);
			Weapon[3]->SetRotation(VECTOR3D(0.0f, 6.0f, 0.0f));
			Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;
			WeaponLocation[4] = VECTOR3D(0.0f, -4.4f, 18.0f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(103);
			Weapon[4]->NextFireTime = Weapon[4]->NextFireTime/2.0f;
			WeaponLocation[5] = VECTOR3D(0.0f, -4.4f, 18.0f);
			Weapon[5] = new CWeapon;
			Weapon[5]->Create(103);
			Weapon[5]->SetRotation(VECTOR3D(0.0f, -6.0f, 0.0f));
			Weapon[5]->NextFireTime = Weapon[5]->NextFireTime/2.0f;

			WeaponLocation[6] = VECTOR3D(7.0f, -4.4f, 13.0f);
			Weapon[6] = new CWeapon;
			Weapon[6]->Create(104);
			Weapon[6]->NextFireTime = Weapon[6]->NextFireTime/2.0f;
			WeaponLocation[7] = VECTOR3D(-7.0f, -4.4f, 13.0f);
			Weapon[7] = new CWeapon;
			Weapon[7]->Create(104);
			Weapon[7]->NextFireTime = Weapon[7]->NextFireTime/2.0f;
			WeaponLocation[8] = VECTOR3D(12.0f, -4.4f, 10.0f);
			Weapon[8] = new CWeapon;
			Weapon[8]->Create(104);
			Weapon[8]->NextFireTime = Weapon[8]->NextFireTime/2.0f;
			WeaponLocation[9] = VECTOR3D(-12.0f, -4.4f, 10.0f);
			Weapon[9] = new CWeapon;
			Weapon[9]->Create(104);
			Weapon[9]->NextFireTime = Weapon[9]->NextFireTime/2.0f;
			WeaponLocation[10] = VECTOR3D(17.0f, -4.4f, 8.0f);
			Weapon[10] = new CWeapon;
			Weapon[10]->Create(104);
			Weapon[10]->NextFireTime = Weapon[10]->NextFireTime/2.0f;
			WeaponLocation[11] = VECTOR3D(-17.0f, -4.4f, 8.0f);
			Weapon[11] = new CWeapon;
			Weapon[11]->Create(104);
			Weapon[11]->NextFireTime = Weapon[11]->NextFireTime/2.0f;

			BossWeaponLocation[0] = VECTOR3D(10.0f, -6.4f, 10.0f);
			BossWeapon[0] = new CWeapon;
			BossWeapon[0]->Create(106);
			BossWeaponLocation[1] = VECTOR3D(-10.0f, -6.4f, 10.0f);
			BossWeapon[1] = new CWeapon;
			BossWeapon[1]->Create(106);
			BossWeaponLocation[2] = VECTOR3D(15.0f, -6.4f, 8.0f);
			BossWeapon[2] = new CWeapon;
			BossWeapon[2]->Create(106);
			BossWeaponLocation[3] = VECTOR3D(-15.0f, -6.4f, 8.0f);
			BossWeapon[3] = new CWeapon;
			BossWeapon[3]->Create(106);
			BossWeaponLocation[4] = VECTOR3D(20.0f, -6.4f, 5.0f);
			BossWeapon[4] = new CWeapon;
			BossWeapon[4]->Create(106);
			BossWeaponLocation[5] = VECTOR3D(-20.0f, -6.4f, 5.0f);
			BossWeapon[5] = new CWeapon;
			BossWeapon[5]->Create(106);

			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, -7.0f, -23.0f);
			SetAlienSpaceMotherShipEngine(Engine[0], 13);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-25.4f, -4.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[1], 11);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(20.6f, -4.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[2], 11);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-20.6f, -4.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[3], 11);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(23.4f, -1.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[4], 10);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-23.4f, -1.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[5], 10);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(23.4f, -7.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[6], 10);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(-23.4f, -7.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[7], 10);
			Engine[8] = new eParticleSystem;
			EngineLocation[8] = VECTOR3D(5.3f, 4.7f, -22.9f);
			SetAlienSpaceMotherShipEngine(Engine[8], 10);
			Engine[9] = new eParticleSystem;
			EngineLocation[9] = VECTOR3D(-5.3f, 4.7f, -22.9f);
			SetAlienSpaceMotherShipEngine(Engine[9], 10);
			Engine[10] = new eParticleSystem;
			EngineLocation[10] = VECTOR3D(2.6f, 6.6f, -21.5f);
			SetAlienSpaceMotherShipEngine(Engine[10], 11);
			Engine[11] = new eParticleSystem;
			EngineLocation[11] = VECTOR3D(-2.6f, 6.6f, -21.5f);
			SetAlienSpaceMotherShipEngine(Engine[11], 11);
			Engine[12] = new eParticleSystem;
			EngineLocation[12] = VECTOR3D(2.6f, 2.0f, -21.5f);
			SetAlienSpaceMotherShipEngine(Engine[12], 11);
			Engine[13] = new eParticleSystem;
			EngineLocation[13] = VECTOR3D(-2.6f, 2.0f, -21.5f);
			SetAlienSpaceMotherShipEngine(Engine[13], 11);
			Engine[14] = new eParticleSystem;
			EngineLocation[14] = VECTOR3D(25.4f, -4.0f, -20.0f);
			SetAlienSpaceMotherShipEngine(Engine[14], 11);
			break;

		case 7:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, -1.4f, 15.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(108);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
			WeaponLocation[1] = VECTOR3D(2.0f, -1.4f, 15.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(108);
			Weapon[1]->SetRotation(VECTOR3D(0.0f, 3.0f, 0.0f));
			Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
			WeaponLocation[2] = VECTOR3D(-2.0f, -1.4f, 15.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(108);
			Weapon[2]->SetRotation(VECTOR3D(0.0f, -3.0f, 0.0f));
			Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
			WeaponLocation[3] = VECTOR3D(4.0f, -1.4f, 15.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(108);
			Weapon[3]->SetRotation(VECTOR3D(0.0f, 6.0f, 0.0f));
			Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;
			WeaponLocation[4] = VECTOR3D(-4.0f, -1.4f, 15.0f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(108);
			Weapon[4]->SetRotation(VECTOR3D(0.0f, -6.0f, 0.0f));
			Weapon[4]->NextFireTime = Weapon[4]->NextFireTime/2.0f;

			BossWeaponLocation[0] = VECTOR3D(12.5f, 1.6f, -15.3f);
			BossWeapon[0] = new CWeapon;
			BossWeapon[0]->Create(110);
			BossWeaponLocation[1] = VECTOR3D(-12.5f, 1.6f, -15.3f);
			BossWeapon[1] = new CWeapon;
			BossWeapon[1]->Create(110);

			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 2.3f, -25.0f);
			SetAlienSpaceMotherShipEngine(Engine[0], 15);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(12.5f, 1.6f, -20.3f);
			SetAlienSpaceMotherShipEngine(Engine[1], 16);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(-12.5f, 1.6f, -20.3f);
			SetAlienSpaceMotherShipEngine(Engine[2], 16);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(0.0f, -8.0f, 0.0f);
			SetAlienSpaceMotherShipEngine(Engine[3], 17);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, -8.0f, 0.0f);
			SetAlienSpaceMotherShipEngine(Engine[4], 18);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(0.0f, -8.0f, 0.0f);
			SetAlienSpaceMotherShipEngine(Engine[5], 19);
			break;




		case 8:
			// оружие
			WeaponLocation[0] = VECTOR3D(2.0f, -2.4f, 15.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(104);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/2.0f;
			WeaponLocation[1] = VECTOR3D(-2.0f, -2.4f, 15.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(104);
			Weapon[1]->NextFireTime = Weapon[1]->NextFireTime/2.0f;
			WeaponLocation[2] = VECTOR3D(2.0f, -4.4f, 15.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(104);
			Weapon[2]->NextFireTime = Weapon[2]->NextFireTime/2.0f;
			WeaponLocation[3] = VECTOR3D(-2.0f, -4.4f, 15.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(104);
			Weapon[3]->NextFireTime = Weapon[3]->NextFireTime/2.0f;

			BossWeaponLocation[0] = VECTOR3D(8.0f, -1.0f, 15.0f);
			BossWeapon[0] = new CWeapon;
			BossWeapon[0]->Create(110);
			BossWeaponLocation[1] = VECTOR3D(-8.0f, -1.0f, 15.0f);
			BossWeapon[1] = new CWeapon;
			BossWeapon[1]->Create(110);
			BossWeaponLocation[2] = VECTOR3D(10.0f, -6.4f, 8.0f);
			BossWeapon[2] = new CWeapon;
			BossWeapon[2]->Create(107);
			BossWeaponLocation[3] = VECTOR3D(-10.0f, -6.4f, 8.0f);
			BossWeapon[3] = new CWeapon;
			BossWeapon[3]->Create(107);
			BossWeaponLocation[4] = VECTOR3D(15.0f, -6.4f, 5.0f);
			BossWeapon[4] = new CWeapon;
			BossWeapon[4]->Create(107);
			BossWeaponLocation[5] = VECTOR3D(-15.0f, -6.4f, 5.0f);
			BossWeapon[5] = new CWeapon;
			BossWeapon[5]->Create(107);

			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(15.0f, 4.4f, -29.0f);
			SetAlienSpaceMotherShipEngine(Engine[0], 15);
			Engine[0]->CreationSize = VECTOR3D(4.0f,6.0f,3.0f);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-15.0f, 4.4f, -29.0f);
			SetAlienSpaceMotherShipEngine(Engine[1], 15);
			Engine[1]->CreationSize = VECTOR3D(4.0f,6.0f,3.0f);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(5.2f, 5.4f, -20.6f);
			SetAlienSpaceMotherShipEngine(Engine[2], 20);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-5.2f, 5.4f, -20.6f);
			SetAlienSpaceMotherShipEngine(Engine[3], 20);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(5.2f, -8.8f, -22.6f);
			SetAlienSpaceMotherShipEngine(Engine[4], 20);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-5.2f, -8.8f, -22.6f);
			SetAlienSpaceMotherShipEngine(Engine[5], 20);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(11.2f, -2.8f, -10.0f);
			SetAlienSpaceMotherShipEngine(Engine[6], 20);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(-11.2f, -2.8f, -10.0f);
			SetAlienSpaceMotherShipEngine(Engine[7], 20);
			Engine[8] = new eParticleSystem;
			EngineLocation[8] = VECTOR3D(0.0f, -10.0f, -3.0f);
			SetAlienSpaceMotherShipEngine(Engine[8], 17);
			Engine[8]->CreationSize = VECTOR3D(17.0f,1.0f,17.0f);
			Engine[8]->DeadZone = 16.9f;
			Engine[9] = new eParticleSystem;
			EngineLocation[9] = VECTOR3D(0.0f, -10.0f, -3.0f);
			SetAlienSpaceMotherShipEngine(Engine[9], 18);
			Engine[9]->CreationSize = VECTOR3D(17.0f,1.0f,17.0f);
			Engine[9]->DeadZone = 16.9f;
			break;



		default:
			fprintf(stderr, "Wrong SpaceShipNum!");
			return;
	}

	for (int i=0; i< EngineQuantity; i++)
	{
		Engine[i]->SetStartLocation(EngineLocation[i]);
		// находим кол-во внутренних источников света
		if (Engine[i]->Light != 0) InternalLights++;
	}




	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();
}

