/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Wheeled.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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
#include "Wheeled.h"



struct WheeledData
{
	float	Strength;
	int		WeaponQuantity;
	float 	SpeedToRotate;
	const	char *Name;
	const	char *TextureName;
};

const int	PresetWheeledDataCount = 8;
WheeledData PresetWheeledData[PresetWheeledDataCount] =
{
	{120, 4,	50.0f,	"DATA/MODELS/WHEELED/jeep-01.VW3D",	"DATA/MODELS/gr-02.jpg"},
	{70, 2,		50.0f,	"DATA/MODELS/WHEELED/jeep-02.VW3D",	"DATA/MODELS/gr-02.jpg"},
	{100, 2,	50.0f,	"DATA/MODELS/WHEELED/jeep-03.VW3D",	"DATA/MODELS/gr-02.jpg"},
	{100, 1,	50.0f,	"DATA/MODELS/WHEELED/jeep-04.VW3D",	"DATA/MODELS/gr-07.jpg"},
	{150, 1,	50.0f,	"DATA/MODELS/WHEELED/jeep-05.VW3D",	"DATA/MODELS/gr-03.jpg"},
	{250,1,		30.0f,	"DATA/MODELS/WHEELED/apc-02.VW3D",	"DATA/MODELS/gr-03.jpg"},
	{200, 2,	30.0f,	"DATA/MODELS/WHEELED/apc-04.VW3D",	"DATA/MODELS/gr-01.jpg"},
	{400,2,		34.0f,	"DATA/MODELS/WHEELED/r-launcher-01.VW3D",	"DATA/MODELS/gr-02.jpg"}
};



//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CWheeled::Create(int WheeledNum)
{
	ObjectType = 5;
	ObjectCreationType = WheeledNum;
	MaxSpeed = 20.0f;
	MaxAcceler = 5.0f;
	MaxSpeedRotate = 20.0f;


	LoadObjectData(PresetWheeledData[WheeledNum-1].Name, this, 0);

	for (int i=0; i<DrawObjectQuantity; i++)
	{
		Texture[i] = vw_FindTextureByName(PresetWheeledData[WheeledNum-1].TextureName);
	}
	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	SpeedToRotate = PresetWheeledData[WheeledNum-1].SpeedToRotate;


	Strength = StrengthStart = PresetWheeledData[WheeledNum-1].Strength/GameNPCArmorPenalty;


	WeaponQuantity = PresetWheeledData[WheeledNum-1].WeaponQuantity;
	// начальные установки для оружия
	WeaponSetFire = new bool[WeaponQuantity];
	WeaponLocation = new VECTOR3D[WeaponQuantity];
	Weapon = new CWeapon*[WeaponQuantity];
	WeaponBound = new VECTOR3D[WeaponQuantity];
	for (int i=0; i<WeaponQuantity; i++)
	{
		WeaponSetFire[i] = false;
		Weapon[i] = 0;
	}



	// установка доп. текстуры и других настроек для каждой модели
	switch (WheeledNum)
	{
		case 1:
			WeaponLocation[0] = VECTOR3D(0.3f, 4.5f, -1.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(206);
			WeaponLocation[1] = VECTOR3D(-0.3f, 4.5f, -1.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(206);
			WeaponLocation[2] = VECTOR3D(0.9f, 4.5f, -1.5f);
			Weapon[2] = new CWeapon;
			Weapon[2]->Create(206);
			WeaponLocation[3] = VECTOR3D(-0.9f, 4.5f, -1.5f);
			Weapon[3] = new CWeapon;
			Weapon[3]->Create(206);
			WeaponFireType = 3;

			WheelQuantity = 4;
			WheelObjectsNum = new int[4];
			WheelObjectsNum[0] = 0;
			WheelObjectsNum[1] = 2;
			WheelObjectsNum[2] = 3;
			WheelObjectsNum[3] = 4;

			WheelRotateQuantity = 2;
			WheelRotateObjectsNum = new int[2];
			WheelRotateObjectsNum[0] = 3;
			WheelRotateObjectsNum[1] = 4;
			MaxWheelRotateAngle = 30.0f;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 5;
			TargetVertObjectMaxAngle = 30.0f;
			break;

		case 2:
			WeaponLocation[0] = VECTOR3D(1.3f, 3.5f, -1.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(206);
			WeaponLocation[1] = VECTOR3D(-1.3f, 3.5f, -1.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(206);
			WeaponFireType = 3;
			DoNotCalculateRotation = true;

			WheelQuantity = 4;
			WheelObjectsNum = new int[4];
			WheelObjectsNum[0] = 1;
			WheelObjectsNum[1] = 2;
			WheelObjectsNum[2] = 3;
			WheelObjectsNum[3] = 4;

			WheelRotateQuantity = 2;
			WheelRotateObjectsNum = new int[2];
			WheelRotateObjectsNum[0] = 1;
			WheelRotateObjectsNum[1] = 4;
			MaxWheelRotateAngle = 20.0f;
			break;

		case 3:
			WeaponLocation[0] = VECTOR3D(0.8f, 4.2f, -1.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);
			WeaponLocation[1] = VECTOR3D(-0.8f, 4.2f, -1.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(204);
			WeaponFireType = 2;

			WheelQuantity = 4;
			WheelObjectsNum = new int[4];
			WheelObjectsNum[0] = 0;
			WheelObjectsNum[1] = 1;
			WheelObjectsNum[2] = 2;
			WheelObjectsNum[3] = 5;

			WheelRotateQuantity = 2;
			WheelRotateObjectsNum = new int[2];
			WheelRotateObjectsNum[0] = 0;
			WheelRotateObjectsNum[1] = 1;
			MaxWheelRotateAngle = 20.0f;

			TargetHorizObjectQuantity = 2;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 3;
			TargetHorizObject[1] = 6;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 6;
			TargetVertObjectMaxAngle = 60.0f;
			break;

		case 4:
			WeaponLocation[0] = VECTOR3D(0.0f, 4.5f, -4.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(207);

			WheelQuantity = 4;
			WheelObjectsNum = new int[4];
			WheelObjectsNum[0] = 0;
			WheelObjectsNum[1] = 1;
			WheelObjectsNum[2] = 3;
			WheelObjectsNum[3] = 4;

			WheelRotateQuantity = 2;
			WheelRotateObjectsNum = new int[2];
			WheelRotateObjectsNum[0] = 0;
			WheelRotateObjectsNum[1] = 3;
			MaxWheelRotateAngle = 20.0f;
			break;

		case 5:
			WeaponLocation[0] = VECTOR3D(0.0f, 4.5f, -4.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(208);

			WheelQuantity = 4;
			WheelObjectsNum = new int[4];
			WheelObjectsNum[0] = 1;
			WheelObjectsNum[1] = 2;
			WheelObjectsNum[2] = 3;
			WheelObjectsNum[3] = 4;

			WheelRotateQuantity = 2;
			WheelRotateObjectsNum = new int[2];
			WheelRotateObjectsNum[0] = 1;
			WheelRotateObjectsNum[1] = 2;
			MaxWheelRotateAngle = 30.0f;
			break;

		case 6:
			WeaponLocation[0] = VECTOR3D(0.0f, 6.0f, -3.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);

			WheelQuantity = 4;
			WheelObjectsNum = new int[4];
			WheelObjectsNum[0] = 1;
			WheelObjectsNum[1] = 2;
			WheelObjectsNum[2] = 3;
			WheelObjectsNum[3] = 4;

			WheelRotateQuantity = 2;
			WheelRotateObjectsNum = new int[2];
			WheelRotateObjectsNum[0] = 1;
			WheelRotateObjectsNum[1] = 4;
			MaxWheelRotateAngle = 30.0f;

			TargetHorizObjectQuantity = 2;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 5;
			TargetHorizObject[1] = 6;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 6;
			TargetVertObjectMaxAngle = 40.0f;
			break;

		case 7:
			WeaponLocation[0] = VECTOR3D(0.1f, 5.0f, -1.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);
			WeaponLocation[1] = VECTOR3D(-0.1f, 5.0f, -1.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(204);
			WeaponFireType = 2;

			WheelQuantity = 4;
			WheelObjectsNum = new int[4];
			WheelObjectsNum[0] = 1;
			WheelObjectsNum[1] = 2;
			WheelObjectsNum[2] = 3;
			WheelObjectsNum[3] = 4;

			WheelRotateQuantity = 2;
			WheelRotateObjectsNum = new int[2];
			WheelRotateObjectsNum[0] = 1;
			WheelRotateObjectsNum[1] = 4;
			MaxWheelRotateAngle = 20.0f;

			TargetHorizObjectQuantity = 2;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 5;
			TargetHorizObject[1] = 6;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 6;
			TargetVertObjectMaxAngle = 40.0f;
			break;

		case 8:
			WeaponLocation[0] = VECTOR3D(1.5f, 5.2f, 7.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(210);
			WeaponLocation[1] = VECTOR3D(-1.5f, 5.2f, 7.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(210);
			WeaponFireType = 3;

			WheelQuantity = 8;
			WheelObjectsNum = new int[8];
			WheelObjectsNum[0] = 1;
			WheelObjectsNum[1] = 3;
			WheelObjectsNum[2] = 4;
			WheelObjectsNum[3] = 5;
			WheelObjectsNum[4] = 6;
			WheelObjectsNum[5] = 7;
			WheelObjectsNum[6] = 8;
			WheelObjectsNum[7] = 9;

			WheelRotateQuantity = 4;
			WheelRotateObjectsNum = new int[4];
			WheelRotateObjectsNum[0] = 1;
			WheelRotateObjectsNum[1] = 3;
			WheelRotateObjectsNum[2] = 6;
			WheelRotateObjectsNum[3] = 7;
			MaxWheelRotateAngle = 20.0f;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 2;
			TargetVertObjectMaxAngle = 90.0f;
			break;
	}


	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();


	// установка остальных параметров девиации
	DeviationOn = false;
	DeviationObjQuantity = WheelQuantity;
	Deviation = new VECTOR3D[DeviationObjQuantity];
	NeedDeviation = new float[DeviationObjQuantity];
	CurentDeviation = new float[DeviationObjQuantity];
	CurentDeviationSum = new float[DeviationObjQuantity];
	DeviationObjNum = new int[DeviationObjQuantity];

	for (int i=0; i<DeviationObjQuantity; i++)
	{
		Deviation[i] = VECTOR3D(0.0f, 1.0f, 0.0f);
		NeedDeviation[i] = vw_Randf0*0.1f;
		CurentDeviation[i] = CurentDeviationSum[i] = 0.0f;
		DeviationObjNum[i] = WheelObjectsNum[i];
	}

	// делаем сдвиг поворота колес, чтобы смотрелось естественнее
	for (int i=0; i<WheelQuantity; i++)
	{
		DrawObjectList[WheelObjectsNum[i]].Rotation.x = vw_RandNum(360.0f);
		if (DrawObjectList[WheelObjectsNum[i]].Rotation.x < 0.0f) DrawObjectList[WheelObjectsNum[i]].Rotation.x = 0.0f;
		if (DrawObjectList[WheelObjectsNum[i]].Rotation.x > 360.0f) DrawObjectList[WheelObjectsNum[i]].Rotation.x = 360.0f;
	}



	// вычисляем данные для нахождения точки стрельбы
	if (TargetHorizObject != 0)
	{
		BaseBound = DrawObjectList[TargetHorizObject[0]].Location;
	}

	if (TargetVertObject != 0)
	{
		if (TargetHorizObject != 0)
		{
			MiddleBound = DrawObjectList[TargetVertObject[0]].Location - DrawObjectList[TargetHorizObject[0]].Location;
		}
		else
		{
			MiddleBound = DrawObjectList[TargetVertObject[0]].Location;
		}
	}

	if (WeaponBound != 0)
	for (int i=0; i<WeaponQuantity; i++)
	{
		if (TargetVertObject != 0)
		{
			WeaponBound[i] = WeaponLocation[i] - DrawObjectList[TargetVertObject[0]].Location;
		}
		else
		if (TargetHorizObject != 0)
		{
			WeaponBound[i] = WeaponLocation[i] - DrawObjectList[TargetHorizObject[0]].Location;
		}
		else
		{
			WeaponBound[i] = WeaponLocation[i];
		}
	}

}

