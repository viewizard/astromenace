/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: AlienSpaceFighter.cpp

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
#include "AlienSpaceFighter.h"



struct AlienSpaceFighterData
{

	int		WeaponQuantity;
	int		EngineQuantity;
	float	Strength;
	float	ShieldStrength;
	const	char *Name;
};

const int	PresetAlienSpaceFighterDataCount = 17;
AlienSpaceFighterData PresetAlienSpaceFighterData[PresetAlienSpaceFighterDataCount] =
{
	{1,	4,	5,	0,	"DATA/MODELS/ALIENFIGHTER/al-01.VW3D"},
	{1,	5,	15,	0,	"DATA/MODELS/ALIENFIGHTER/al-02.VW3D"},
	{1,	5,	60,	0,	"DATA/MODELS/ALIENFIGHTER/al-03.VW3D"},
	{4,	8,	40,	0, 	"DATA/MODELS/ALIENFIGHTER/al-04.VW3D"},
	{1,	7,	30,	0,	"DATA/MODELS/ALIENFIGHTER/al-05.VW3D"},
	{1,	6,	120,0,	"DATA/MODELS/ALIENFIGHTER/al-06.VW3D"},
	{1,	8,	250,100,"DATA/MODELS/ALIENFIGHTER/al-07.VW3D"},
	{1,	7,	200,50,	"DATA/MODELS/ALIENFIGHTER/al-08.VW3D"},
	{2,	11,	500,200,"DATA/MODELS/ALIENFIGHTER/al-09.VW3D"},
	{1,	7,	250,50,	"DATA/MODELS/ALIENFIGHTER/al-10.VW3D"},
	{2,	6,	150,100,"DATA/MODELS/ALIENFIGHTER/al-11.VW3D"},
	{1,	5,	200,100,"DATA/MODELS/ALIENFIGHTER/al-12.VW3D"},
	{2,	5,	300,200,"DATA/MODELS/ALIENFIGHTER/al-13.VW3D"},
	{2,	8,	400,300,"DATA/MODELS/ALIENFIGHTER/al-14.VW3D"},
	{6,	4,	50,100,	"DATA/MODELS/ALIENFIGHTER/al-15.VW3D"},
	{3,	4,	100,150,"DATA/MODELS/ALIENFIGHTER/al-16.VW3D"},
	{5,	6,	150,200,"DATA/MODELS/ALIENFIGHTER/al-17.VW3D"}
};




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CAlienSpaceFighter::Create(int	SpaceShipNum)
{
	ObjectStatus = 1; // чужой
	ObjectType = 2;
	ObjectCreationType = SpaceShipNum;
	PromptDrawDist2 = 100.0f;

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	MaxSpeed = 40.0f;
	MaxAcceler = 20.0f;
	MaxSpeedRotate = 40.0f;
	//EngineSoundNum = 0;
	EngineLeftQuantity = 0;
	EngineRightQuantity = 0;
	Strength = StrengthStart = PresetAlienSpaceFighterData[SpaceShipNum-1].Strength/GameNPCArmorPenalty;
	ShieldStrength = ShieldStrengthStart = PresetAlienSpaceFighterData[SpaceShipNum-1].ShieldStrength/GameNPCArmorPenalty;
	ShieldRecharge = ShieldStrengthStart/10.0f;

	WeaponQuantity = PresetAlienSpaceFighterData[SpaceShipNum-1].WeaponQuantity;
	EngineQuantity = PresetAlienSpaceFighterData[SpaceShipNum-1].EngineQuantity;


	LoadObjectData(PresetAlienSpaceFighterData[SpaceShipNum-1].Name, this, 0, 2.0f);

	// всегда только эти текстуры и 1 объект
	for (int i=0; i<DrawObjectQuantity; i++)
	{
		Texture[i] = vw_FindTextureByName("DATA/MODELS/ALIENFIGHTER/al-text04.VW2D");
		TextureIllum[i] = vw_FindTextureByName("DATA/MODELS/ALIENFIGHTER/al-illum04.VW2D");
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

	// начальные установки для двигателей
	EngineLocation = new VECTOR3D[EngineQuantity];
	Engine = new eParticleSystem*[EngineQuantity];
	for (int i=0; i<EngineQuantity; i++) Engine[i] = 0;










	// перебираем и ставим нужные данные
	switch (SpaceShipNum)
	{
		case 1:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, 0.25f, 1.85f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(101);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 0.65f, -2.3f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-1.9f, 0.45f, -3.8f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(1.9f, 0.45f, -3.8f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(0.0f, 0.25f, 1.85f);
			SetAlienSpaceFighterEngine(Engine[3], 3);
			break;
		case 2:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, -0.30f, 3.95f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(101);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 0.8f, -3.1f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -1.0f, -4.3f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(2.85f, 1.1f, -5.8f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-2.85f, 1.1f, -5.8f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, -0.30f, 3.95f);
			SetAlienSpaceFighterEngine(Engine[4], 7);
			Engine[4]->Direction = VECTOR3D(0.0f, 0.3f, -1.0f);
			break;
		case 3:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, -0.35f, 3.9f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(102);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 0.8f, -5.5f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -1.1f, -6.5f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(5.6f, 1.1f, -3.3f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-5.6f, 1.1f, -3.3f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, -0.35f, 3.9f);
			SetAlienSpaceFighterEngine(Engine[4], 7);
			Engine[4]->SizeStart = 0.40f;
			Engine[4]->SizeVar = 0.20f;
			Engine[4]->Speed = 9.0f;
			Engine[4]->Direction = VECTOR3D(0.0f, -0.3f, 1.0f);
			break;
		case 4:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.75f, -0.15f, 5.1f);
			WeaponLocation[1] = VECTOR3D(-0.75f, -0.15f, 5.1f);
			WeaponLocation[2] = VECTOR3D(0.85f, -1.4f, 6.5f);
			WeaponLocation[3] = VECTOR3D(-0.85f, -1.4f, 6.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(101);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(101);
			Weapon[1]->SoundNum = 0;
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(101);
			Weapon[2]->SoundNum = 0;
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(101);
			Weapon[3]->SoundNum = 0;
			WeaponFireType = 1;

			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 1.4f, -3.7f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -0.6f, -5.0f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(1.5f, 1.9f, -7.1f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-1.5f, 1.9f, -7.1f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.75f, -0.15f, 5.1f);
			SetAlienSpaceFighterEngine(Engine[4], 3);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-0.75f, -0.15f, 5.1f);
			SetAlienSpaceFighterEngine(Engine[5], 3);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(0.85f, -1.4f, 6.5f);
			SetAlienSpaceFighterEngine(Engine[6], 3);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(-0.85f, -1.4f, 6.5f);
			SetAlienSpaceFighterEngine(Engine[7], 3);
			break;
		case 5:
			MaxSpeedRotate = 720.0f;
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, 0.0f, 0.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(103);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(1.0f, 0.0f, -4.0f);
			SetAlienSpaceFighterEngine(Engine[0], 2);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-1.0f, 0.0f, -4.0f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(0.0f, 0.0f, -2.2f);
			SetAlienSpaceFighterEngine(Engine[2], 4);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(1.0f, 0.0f, -2.0f);
			SetAlienSpaceFighterEngine(Engine[3], 4);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(-1.0f, 0.0f, -2.0f);
			SetAlienSpaceFighterEngine(Engine[4], 4);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(1.7f, 0.0f, -1.3f);
			SetAlienSpaceFighterEngine(Engine[5], 4);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(-1.7f, 0.0f, -1.3f);
			SetAlienSpaceFighterEngine(Engine[6], 4);
			break;
		case 6:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, 0.35f, 2.65f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(104);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 2.4f, -7.4f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, 2.4f, -7.4f);
			SetAlienSpaceFighterEngine(Engine[1], 5);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(0.0f, -1.3f, -5.0f);
			SetAlienSpaceFighterEngine(Engine[2], 1);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(4.05f, 1.4f, -6.8f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(-4.05f, 1.4f, -6.8f);
			SetAlienSpaceFighterEngine(Engine[4], 2);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(0.0f, 0.35f, 2.65f);
			SetAlienSpaceFighterEngine(Engine[5], 3);
			Engine[5]->SizeStart = 0.50f;
			break;
		case 7:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, -0.70f, 7.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(105);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 1.8f, -10.2f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -1.8f, -3.1f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(3.4f, 1.0f, -5.6f);
			SetAlienSpaceFighterEngine(Engine[2], 6);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-3.4f, 1.0f, -5.6f);
			SetAlienSpaceFighterEngine(Engine[3], 6);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, -0.70f, 7.0f);
			SetAlienSpaceFighterEngine(Engine[4], 7);
			Engine[4]->SizeStart = 0.40f;
			Engine[4]->SizeVar = 0.20f;
			Engine[4]->Direction = VECTOR3D(0.0f, 0.3f, -1.0f);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(1.6f, -2.2f, 6.5f);
			SetAlienSpaceFighterEngine(Engine[5], 7);
			Engine[5]->Direction = VECTOR3D(-0.3f, 0.0f, 1.0f);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(-1.6f, -2.2f, 6.5f);
			SetAlienSpaceFighterEngine(Engine[6], 7);
			Engine[6]->Direction = VECTOR3D(0.3f, 0.0f, 1.0f);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(0.0f, 1.8f, -10.2f);
			SetAlienSpaceFighterEngine(Engine[7], 5);
			break;
		case 8:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, 1.10f, -0.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(103);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/4.0f;
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 1.4f, -8.6f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -0.2f, -9.7f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(5.0f, 2.25f, -7.3f);
			SetAlienSpaceFighterEngine(Engine[2], 6);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-5.0f, 2.25f, -7.3f);
			SetAlienSpaceFighterEngine(Engine[3], 6);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, 1.10f, -0.5f);
			SetAlienSpaceFighterEngine(Engine[4], 7);
			Engine[4]->SizeStart = 0.60f;
			Engine[4]->SizeVar = 0.10f;
			Engine[4]->Direction = VECTOR3D(0.0f, -0.3f, 1.0f);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(1.25f, -0.95f, 5.4f);
			SetAlienSpaceFighterEngine(Engine[5], 7);
			Engine[5]->Direction = VECTOR3D(-0.22f, 0.25f, 1.0f);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(-1.25f, -0.95f, 5.4f);
			SetAlienSpaceFighterEngine(Engine[6], 7);
			Engine[6]->Direction = VECTOR3D(0.22f, 0.25f, 1.0f);
			break;
		case 9:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, 0.10f, 4.9f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(103);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime/3.0f;
			WeaponLocation[1] = VECTOR3D(0.0f, 0.0f, 0.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(107);
			WeaponFireType = 1;
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 0.2f, -5.8f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -1.4f, -6.6f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(3.0f, 1.5f, -8.0f);
			SetAlienSpaceFighterEngine(Engine[2], 1);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-3.0f, 1.5f, -8.0f);
			SetAlienSpaceFighterEngine(Engine[3], 1);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(3.0f, 1.5f, -8.0f);
			SetAlienSpaceFighterEngine(Engine[4], 5);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-3.0f, 1.5f, -8.0f);
			SetAlienSpaceFighterEngine(Engine[5], 5);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(7.1f, 0.6f, -4.9f);
			SetAlienSpaceFighterEngine(Engine[6], 6);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(-7.1f, 0.6f, -4.9f);
			SetAlienSpaceFighterEngine(Engine[7], 6);
			Engine[8] = new eParticleSystem;
			EngineLocation[8] = VECTOR3D(0.0f, 0.10f, 4.9f);
			SetAlienSpaceFighterEngine(Engine[8], 7);
			Engine[8]->SizeStart = 0.50f;
			Engine[8]->Direction = VECTOR3D(0.0f, 0.3f, -1.0f);
			Engine[9] = new eParticleSystem;
			EngineLocation[9] = VECTOR3D(2.05f, -1.15f, 4.15f);
			SetAlienSpaceFighterEngine(Engine[9], 7);
			Engine[9]->Direction = VECTOR3D(-0.6f, 0.0f, 0.5f);
			Engine[9]->SizeVar = Engine[9]->SizeEnd = 0.20f;
			Engine[10] = new eParticleSystem;
			EngineLocation[10] = VECTOR3D(-2.05f, -1.15f, 4.15f);
			SetAlienSpaceFighterEngine(Engine[10], 7);
			Engine[10]->Direction = VECTOR3D(0.6f, 0.0f, 0.5f);
			Engine[10]->SizeVar = Engine[10]->SizeEnd = 0.20f;
			break;
		case 10:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, 0.0f, 0.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(106);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 0.8f, -4.0f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -1.2f, -4.8f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(3.8f, 0.7f, -5.7f);
			SetAlienSpaceFighterEngine(Engine[2], 1);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-3.8f, 0.7f, -5.7f);
			SetAlienSpaceFighterEngine(Engine[3], 1);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(3.8f, 0.7f, -5.7f);
			SetAlienSpaceFighterEngine(Engine[4], 5);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-3.8f, 0.7f, -5.7f);
			SetAlienSpaceFighterEngine(Engine[5], 5);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(0.0f, 0.65f, 4.65f);
			SetAlienSpaceFighterEngine(Engine[6], 4);
			Engine[6]->CreationSize = VECTOR3D(2.4f,0.6f,0.1f);
			Engine[6]->ParticlesPerSec = 50;
			break;
		case 11:
			// оружие
			WeaponLocation[0] = VECTOR3D(3.9f, 1.3f, 1.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(104);
			WeaponLocation[1] = VECTOR3D(-3.9f, 1.3f, 1.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(104);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 1.0f, -5.4f);
			SetAlienSpaceFighterEngine(Engine[0], 1);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -0.3f, -6.2f);
			SetAlienSpaceFighterEngine(Engine[1], 1);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(2.25f, -1.1f, -4.1f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-2.25f, -1.1f, -4.1f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, 0.50f, 0.15f);
			SetAlienSpaceFighterEngine(Engine[4], 3);
			Engine[4]->SizeStart = 0.2f;
			Engine[4]->SizeVar = 0.1f;
			Engine[4]->Speed = 2.0f;
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(0.0f, 1.15f, -1.1f);
			SetAlienSpaceFighterEngine(Engine[5], 7);
			Engine[5]->Direction = VECTOR3D(0.0f, -0.3f, 1.0f);
			Engine[5]->Speed = 5.0f;
			break;
		case 12:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, 0.50f, 0.8f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(108);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(1.1f, 1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[0], 2);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-1.1f, 1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(1.1f, -1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-1.1f, -1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, 0.50f, 1.1f);
			SetAlienSpaceFighterEngine(Engine[4], 4);
			Engine[4]->CreationSize = VECTOR3D(1.0f,0.1f,0.1f);
			break;
		case 13:
			// оружие
			WeaponLocation[0] = VECTOR3D(3.0f, 0.0f, 0.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(109);
			WeaponLocation[1] = VECTOR3D(-3.0f, 0.0f, 0.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(109);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(1.1f, 1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[0], 2);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-1.1f, 1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(1.1f, -1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-1.1f, -1.0f, -4.4f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.0f, 0.00f, 1.1f);
			SetAlienSpaceFighterEngine(Engine[4], 4);
			Engine[4]->CreationSize = VECTOR3D(0.1f,1.2f,0.1f);
			break;
		case 14:
			// оружие
			WeaponLocation[0] = VECTOR3D(3.0f, 0.0f, 0.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(108);
			WeaponLocation[1] = VECTOR3D(-3.0f, 0.0f, 0.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(108);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(1.1f, 1.1f, -4.9f);
			SetAlienSpaceFighterEngine(Engine[0], 2);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-1.1f, 1.1f, -4.9f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(1.1f, -1.1f, -4.9f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-1.1f, -1.1f, -4.9f);
			SetAlienSpaceFighterEngine(Engine[3], 2);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(0.75f, -0.25f, 3.8f);
			SetAlienSpaceFighterEngine(Engine[4], 3);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-0.75f, -0.25f, 3.8f);
			SetAlienSpaceFighterEngine(Engine[5], 3);
			Engine[6] = new eParticleSystem;
			EngineLocation[6] = VECTOR3D(0.8f, -1.7f, 4.4f);
			SetAlienSpaceFighterEngine(Engine[6], 3);
			Engine[7] = new eParticleSystem;
			EngineLocation[7] = VECTOR3D(-0.8f, -1.7f, 4.4f);
			SetAlienSpaceFighterEngine(Engine[7], 3);
			break;
		case 15:
			// оружие
			WeaponLocation[0] = VECTOR3D(3.8f, -1.8f, 0.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(102);
			WeaponLocation[1] = VECTOR3D(-3.8f, -1.8f, 0.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(102);
			WeaponLocation[2] = VECTOR3D(0.2f, 1.9f, -3.0f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(101);
			WeaponLocation[3] = VECTOR3D(-0.2f, 1.6f, -3.0f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(101);
			WeaponLocation[4] = VECTOR3D(0.2f, 1.4f, -3.0f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(101);
			WeaponLocation[5] = VECTOR3D(-0.2f, 1.1f, -3.0f);
			Weapon[5] = new CWeapon;
			Weapon[5]->Create(101);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(0.0f, 2.4f, -4.0f);
			SetAlienSpaceFighterEngine(Engine[0], 2);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(0.0f, -0.1f, -4.0f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(0.0f, 1.2f, -4.0f);
			SetAlienSpaceFighterEngine(Engine[2], 2);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(0.0f, 1.0f, -2.1f);
			SetAlienSpaceFighterEngine(Engine[3], 4);
			Engine[3]->CreationSize = VECTOR3D(1.0f,2.4f,0.1f);
			Engine[3]->ParticlesPerSec = 40;
			break;
		case 16:
			// оружие
			WeaponLocation[0] = VECTOR3D(3.7f, -1.9f, 1.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(102);
			WeaponLocation[1] = VECTOR3D(-3.7f, -1.9f, 1.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(102);
			WeaponLocation[2] = VECTOR3D(0.0f, 0.2f, -0.8f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(108);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(2.8f, 0.2f, -7.2f);
			SetAlienSpaceFighterEngine(Engine[0], 2);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-2.8f, 0.2f, -7.2f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(0.0f, 1.2f, -0.8f);
			SetAlienSpaceFighterEngine(Engine[2], 4);
			Engine[2]->CreationSize = VECTOR3D(0.6f,1.7f,0.4f);
			Engine[2]->ParticlesPerSec = 40;
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(0.0f, 0.2f, -0.8f);
			SetAlienSpaceFighterEngine(Engine[3], 4);
			Engine[3]->CreationSize = VECTOR3D(3.0f,0.4f,0.8f);
			Engine[3]->ParticlesPerSec = 40;
			break;
		case 17:
			// оружие
			WeaponLocation[0] = VECTOR3D(0.0f, -0.85f, 1.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(108);
			WeaponLocation[1] = VECTOR3D(1.7f, -0.85f, 1.9f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(105);
			WeaponLocation[2] = VECTOR3D(-1.7f, -0.85f, 1.9f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(105);
			WeaponLocation[3] = VECTOR3D(1.0f, -0.85f, 1.1f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(105);
			WeaponLocation[4] = VECTOR3D(-1.0f, -0.85f, 1.1f);
			Weapon[4] = new CWeapon;
			Weapon[4]->Create(105);
			// двигатели
			Engine[0] = new eParticleSystem;
			EngineLocation[0] = VECTOR3D(1.8f, 1.0f, -7.2f);
			SetAlienSpaceFighterEngine(Engine[0], 2);
			Engine[0]->CreationSize = VECTOR3D(0.4f,0.4f,0.1f);
			Engine[1] = new eParticleSystem;
			EngineLocation[1] = VECTOR3D(-1.8f, 1.0f, -7.2f);
			SetAlienSpaceFighterEngine(Engine[1], 2);
			Engine[1]->CreationSize = VECTOR3D(0.4f,0.4f,0.1f);
			Engine[2] = new eParticleSystem;
			EngineLocation[2] = VECTOR3D(1.0f, -0.85f, 1.1f);
			SetAlienSpaceFighterEngine(Engine[2], 4);
			Engine[3] = new eParticleSystem;
			EngineLocation[3] = VECTOR3D(-1.0f, -0.85f, 1.1f);
			SetAlienSpaceFighterEngine(Engine[3], 4);
			Engine[4] = new eParticleSystem;
			EngineLocation[4] = VECTOR3D(1.7f, -0.85f, 1.9f);
			SetAlienSpaceFighterEngine(Engine[4], 4);
			Engine[5] = new eParticleSystem;
			EngineLocation[5] = VECTOR3D(-1.7f, -0.85f, 1.9f);
			SetAlienSpaceFighterEngine(Engine[5], 4);
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

