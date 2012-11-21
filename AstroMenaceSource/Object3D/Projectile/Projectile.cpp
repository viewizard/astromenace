/********************************************************************************

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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


/// подключаем нужные файлы
#include "Projectile.h"
#include "../Explosion/BulletExplosion/BulletExplosion.h"

CObject3D *GetMissileOnTargetOrientateion(int	ObjectStatus, VECTOR3D Location,
		VECTOR3D CurrentObjectRotation, float RotationMatrix[9], VECTOR3D *NeedAngle);
bool GetMissileOnTargetOrientateion(VECTOR3D Location, VECTOR3D CurrentObjectRotation,
		float RotationMatrix[9], CObject3D *TargetObject, VECTOR3D *NeedAngle);
bool GetMissileTargetLiveStatus(CObject3D *TargetObject);



struct ProjectileData
{
	float	Radius;
	float	DamageHull;
	float	DamageSystems;
	// 0-обычный снаряд, 1-уничтожаемый, 2-лучевой, 3-мина (не уничтожаемая), 4-мина (уничтожаемая)
	int		ProjectileType;
	float	Speed;
	float	Age;
	int		GraphicFXQuantity;
};




// снаряды для оружия землян 1-99
const int	PresetEarthProjectileDataCount = 19;
ProjectileData PresetEarthProjectileData[PresetEarthProjectileDataCount] =
{
	// Kinetic
	{0.3f, 5, 0, 	0, 50, 4, 1},
	{0.6f, 20, 0, 	0, 45, 4, 1},
	{0.6f, 40, 0, 	0, 45, 4, 1},
	{0.4f, 10, 0, 	0, 50, 4, 1},
	// Ion
	{1.5f, 0, 10, 	0, 40, 4, 1},
	{1.5f, 0, 30, 	0, 40, 4, 2},
	{2.0f, 0, 50, 	0, 40, 4, 2},
	// Plasma
	{1.0f, 30, 5, 	0, 40, 4, 1},
	{1.3f, 40, 25, 	0, 40, 4, 3},
	{1.3f, 50, 50, 	0, 45, 4, 3},
	// Maser
	{0.6f, 50, 0, 	2, 0, 2, 1},
	{1.0f, 80, 0, 	2, 0, 2, 2},
	// Antimatter
	{1.0f, 80, 10, 	0, 70, 3, 1},
	// Laser
	{0.4f, 150, 0, 	2, 0, 1, 1},
	// Gauss
	{1.1f, 100, 0, 	0, 80, 3, 1},
	// Missiles
	{0.2f, 100, 0, 	1, 40, 8, 2},
	{0.2f, 20, 0, 	1, 40, 6, 2},
	{0.2f, 400, 0, 	1, 35, 7, 2},
	{0.2f, 800, 0, 	1, 30, 6, 2},
};




// снаряды для оружия пришельцев 101-199
const int	PresetAlienProjectileDataCount = 10;
ProjectileData PresetAlienProjectileData[PresetAlienProjectileDataCount] =
{
	// как Kinetic1
	{0.3f, 5, 0, 	0, 70, 4, 1},
	// с наведением, как Kinetic2
	{0.6f, 10, 0, 	0, 50, 4, 2},
	// как Kinetic3
	{0.6f, 20, 0, 	0, 60, 4, 1},
	// с наведением, как Kinetic3
	{0.6f, 20, 0, 	0, 60, 4, 2},
	// как Kinetic2
	{0.6f, 10, 0, 	0, 50, 4, 1},
	// энергетическая мина (1-й тип), только позиционируется по высоте (на ближайшего врага)
	{2.0f, 60, 0, 	3, 0, 16, 1},
	// энергетическая мина (2-й тип), позиционируется по высоте (на ближайшего врага) + немного приближается
	{2.0f, 120, 0, 	3, 7, 16, 1},
	// как Plasma3
	{1.3f, 50, 50, 	0, 55, 4, 3},
	// как Plasma2
	{1.3f, 40, 25, 	0, 55, 4, 3},
	// как Laser (для больших кораблей)
	{1.0f, 250, 0, 	2, 0, 1, 1},

};



// снаряды для оружия пиратов 201-299
const int	PresetPirateProjectileDataCount = 17;
ProjectileData PresetPirateProjectileData[PresetPirateProjectileDataCount] =
{
	// стрельба турели 1
	{0.3f, 5, 0, 	0, 60, 4, 1},
	// стрельба турели 2
	{0.6f, 20, 0, 	0, 45, 4, 1},
	// фларес
	{3.0f, 5, 0, 	3, 5, 5, 1},

	// как Kinetic1
	{0.3f, 5, 0, 	0, 60, 4, 1},
	// как Missile1
	{0.2f, 40, 0, 	1, 60, 6, 2},
	// как Missile2
	{0.2f, 20, 0, 	1, 60, 6, 2},
	// как Ion2
	{1.5f, 0, 30, 	0, 40, 4, 2},
	// как Antimatter
	{1.0f, 80, 10, 	0, 70, 3, 1},
	// как Missile3 (торпеда)
	{0.2f, 400, 0, 	1, 55, 7, 2},
	// как Missile4 (бомба)
	{0.2f, 1000, 0, 1, 50, 6, 2},
	// как Kinetic2
	{0.6f, 20, 0, 	0, 45, 4, 1},
	// как Kinetic3
	{0.6f, 40, 0, 	0, 45, 4, 1},
	// как Plasma2
	{1.3f, 40, 25, 	0, 40, 4, 3},

	// мина1 (простое наведение по высоте)
	{1.2f, 50, 0, 	4, 0, -1, 1},
	// мина2 (наведение по высоте + приближение)
	{1.2f, 100, 0, 	4, 7, -1, 1},
	// мина3 (наведение по высоте + стрельба снарядами)
	{1.2f, 150, 0, 	4, 0, -1, 1},
	// мина4 (наведение по высоте + стрельба ракетами)
	{1.2f, 200, 0, 	4, 0, -1, 1},
};











// получаем время жизни снаряда
float GetProjectileRange(int Num)
{

	if (Num >= 1 && Num <= 99)
	{
		float Return = PresetEarthProjectileData[Num-1].Age * PresetEarthProjectileData[Num-1].Speed;
		if (Num == 11) Return = 100.0f;
		if (Num == 12) Return = 110.0f;
		if (Num == 14) Return = 120.0f;
		return Return;
	}
	else
	if (Num >= 101 && Num <= 199)
	{
		if (Num == 110) return 200.0f;
		return PresetAlienProjectileData[Num-101].Age * PresetAlienProjectileData[Num-101].Speed;
	}
	else
	if (Num >= 201 && Num <= 299)
	{
		return PresetPirateProjectileData[Num-201].Age * PresetPirateProjectileData[Num-201].Speed;
	}

	return 0.0f;
}
int GetWeaponHullDamage(int Num)
{
	if (Num >= 1 && Num <= 99)
	{
		// если система залпового огня
		if (Num == 17) return 10 * (int)PresetEarthProjectileData[Num-1].DamageHull;
		return (int)PresetEarthProjectileData[Num-1].DamageHull;
	}
	else
	if (Num >= 101 && Num <= 199)
	{
		return (int)PresetAlienProjectileData[Num-101].DamageHull;
	}
	else
	if (Num >= 201 && Num <= 299)
	{
		return (int)PresetPirateProjectileData[Num-201].DamageHull;
	}
	return 0;
}
int GetWeaponSystemsDamage(int Num)
{
	if (Num >= 1 && Num <= 99)
	{
		// если система залпового огня
		if (Num == 17) return 10 * (int)PresetEarthProjectileData[Num-1].DamageSystems;
		return (int)PresetEarthProjectileData[Num-1].DamageSystems;
	}
	else
	if (Num >= 101 && Num <= 199)
	{
		return (int)PresetAlienProjectileData[Num-101].DamageSystems;
	}
	else
	if (Num >= 201 && Num <= 299)
	{
		return (int)PresetPirateProjectileData[Num-201].DamageSystems;
	}
	return 0;
}
float GetProjectileSpeed(int Num)
{
	if (Num >= 1 && Num <= 99)
	{
		return PresetEarthProjectileData[Num-1].Speed;
	}
	else
	if (Num >= 101 && Num <= 199)
	{
		return PresetAlienProjectileData[Num-101].Speed;
	}
	else
	if (Num >= 201 && Num <= 299)
	{
		return PresetPirateProjectileData[Num-201].Speed;
	}
	return 0.0f;
}











//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
CProjectile::CProjectile(void)
{
	// друг
	ObjectStatus = 2;
	ObjectType = 10;

	// проверяем на столкновения только по радиусу
	Radius = 0.0f;

	// нет цели
	Target = 0;

	// пока не повреждает вообще
	DamageHull = 0.0f;
	DamageSystems = 0.0f;

	// нет графических эффектов
	GraphicFXQuantity = 0;
	GraphicFXLocation = 0;
	GraphicFX = 0;
	GraphicFXDestroyType = false;

	NeedStopPartic = false;

	// не можем сбивать снаряд
	ProjectileType = 0;

	ProjectileCenter = VECTOR3D(0.0f, 0.0f, 0.0f);

	Age = 0.0f;
	NeedDeadSound = true;

	SpeedStart = SpeedEnd = Speed = 0.0f;

	MineReloadTime = MineNextFireTime = 1.0f;


	Ambient[0] = Ambient[1] = Ambient[2] = Ambient[3] = 0.35f;

	Next = Prev = 0;
	AttachProjectile(this);
}



//-----------------------------------------------------------------------------
// выполнение действий
//-----------------------------------------------------------------------------
void CProjectile::Create(int ProjectileNum)
{
	Num = ProjectileNum;

	// внутренний номер, номер в таблицах
	int IntNum = ProjectileNum;


	if (Num >= 1 && Num <= 99)
	{
		Radius = PresetEarthProjectileData[IntNum-1].Radius;
		DamageHull = PresetEarthProjectileData[IntNum-1].DamageHull;
		DamageSystems = PresetEarthProjectileData[IntNum-1].DamageSystems;
		ProjectileType = PresetEarthProjectileData[IntNum-1].ProjectileType;
		SpeedStart = SpeedEnd = Speed = PresetEarthProjectileData[IntNum-1].Speed;
		Age = Lifetime = PresetEarthProjectileData[IntNum-1].Age;
		GraphicFXQuantity = PresetEarthProjectileData[IntNum-1].GraphicFXQuantity;
	}
	else
	if (Num >= 101 && Num <= 199)
	{
		IntNum = ProjectileNum - 100;
		Radius = PresetAlienProjectileData[IntNum-1].Radius;
		DamageHull = PresetAlienProjectileData[IntNum-1].DamageHull;
		DamageSystems = PresetAlienProjectileData[IntNum-1].DamageSystems;
		ProjectileType = PresetAlienProjectileData[IntNum-1].ProjectileType;
		SpeedStart = SpeedEnd = Speed = PresetAlienProjectileData[IntNum-1].Speed;
		Age = Lifetime = PresetAlienProjectileData[IntNum-1].Age;
		GraphicFXQuantity = PresetAlienProjectileData[IntNum-1].GraphicFXQuantity;
	}
	else
	if (Num >= 201 && Num <= 299)
	{
		IntNum = ProjectileNum - 200;
		// радиус для мин потом изменим при инициализации геометрии
		Radius = PresetPirateProjectileData[IntNum-1].Radius;
		DamageHull = PresetPirateProjectileData[IntNum-1].DamageHull;
		DamageSystems = PresetPirateProjectileData[IntNum-1].DamageSystems;
		ProjectileType = PresetPirateProjectileData[IntNum-1].ProjectileType;
		SpeedStart = SpeedEnd = Speed = PresetPirateProjectileData[IntNum-1].Speed;
		Age = Lifetime = PresetPirateProjectileData[IntNum-1].Age;
		GraphicFXQuantity = PresetPirateProjectileData[IntNum-1].GraphicFXQuantity;
	}



	// начальные установки
	GraphicFXLocation = new VECTOR3D[GraphicFXQuantity];
	GraphicFX = new eParticleSystem*[GraphicFXQuantity];
	for (int i=0; i<GraphicFXQuantity; i++)
	{
		GraphicFXLocation[i] = VECTOR3D(0.0f, 0.0f, 0.0f);
		GraphicFX[i] = 0;
	}

	if (ProjectileType == 1)
	{
		 Strength = StrengthStart = 1.0f;
	}


	switch (ProjectileNum)
	{
		// снаряды землян

		// Kinetic
		case 1:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 1);
			break;
		case 2:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 2);
			break;
		case 3:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 3);
			break;
		case 4:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 4);
			break;
		// Ion
		case 5:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 5);
			break;
		case 6:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 6);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 5);
			GraphicFX[1]->CreationSize = VECTOR3D(2.5f,2.5f,0.5f);
			GraphicFX[1]->DeadZone = 1.9f;
			break;
		case 7:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 12);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 5);
			break;
		// Plasma
		case 8:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 7);
			break;
		case 9:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 10);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 8);
			GraphicFX[2] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[2], 9);
			break;
		case 10:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 8);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 9);
			GraphicFX[2] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[2], 11);
			break;
		// Maser
		case 11:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 17);
			GraphicFX[0]->CreationSize = VECTOR3D(0.8f,0.8f,100.0f);
			ProjectileCenter = VECTOR3D(0.0f,0.0f,50.0f);
			NeedStopPartic = true;
			{
				float MinX = -GraphicFX[0]->CreationSize.x/2;
				float MaxX = GraphicFX[0]->CreationSize.x/2;
				float MinY = -GraphicFX[0]->CreationSize.y/2;
				float MaxY = GraphicFX[0]->CreationSize.y/2;
				float MinZ = -GraphicFX[0]->CreationSize.z/2;
				float MaxZ = GraphicFX[0]->CreationSize.z/2;
				// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
				OBB[0] = AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
				OBB[1] = AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
				OBB[2] = AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
				OBB[3] = AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
				OBB[4] = AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
				OBB[5] = AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
				OBB[6] = AABB[6] = VECTOR3D(MinX, MinY, MinZ);
				OBB[7] = AABB[7] = VECTOR3D(MaxX, MinY, MinZ);
				OBBLocation = VECTOR3D(0,0,0);
				Width = fabsf(MaxX-MinX);
				Height = fabsf(MaxY-MinY);
				Length = fabsf(MaxZ-MinZ);
			}
			break;
		case 12:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 17);
			GraphicFX[0]->CreationSize = VECTOR3D(0.8f,0.8f,110.0f);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 18);
			GraphicFX[1]->CreationSize = VECTOR3D(1.6f,1.6f,110.0f);
			ProjectileCenter = VECTOR3D(0.0f,0.0f,55.0f);
			NeedStopPartic = true;
			{
				float MinX = -GraphicFX[0]->CreationSize.x/2;
				float MaxX = GraphicFX[0]->CreationSize.x/2;
				float MinY = -GraphicFX[0]->CreationSize.y/2;
				float MaxY = GraphicFX[0]->CreationSize.y/2;
				float MinZ = -GraphicFX[0]->CreationSize.z/2;
				float MaxZ = GraphicFX[0]->CreationSize.z/2;
				// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
				OBB[0] = AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
				OBB[1] = AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
				OBB[2] = AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
				OBB[3] = AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
				OBB[4] = AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
				OBB[5] = AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
				OBB[6] = AABB[6] = VECTOR3D(MinX, MinY, MinZ);
				OBB[7] = AABB[7] = VECTOR3D(MaxX, MinY, MinZ);
				OBBLocation = VECTOR3D(0,0,0);
				Width = fabsf(MaxX-MinX);
				Height = fabsf(MaxY-MinY);
				Length = fabsf(MaxZ-MinZ);
			}
			break;
		// Antimatter
		case 13:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 19);
			break;
		// Laser
		case 14:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 20);
			GraphicFX[0]->CreationSize = VECTOR3D(0.4f,0.4f,120.0f);
			ProjectileCenter = VECTOR3D(0.0f,0.0f,60.0f);
			NeedStopPartic = true;
			{
				float MinX = -GraphicFX[0]->CreationSize.x/2;
				float MaxX = GraphicFX[0]->CreationSize.x/2;
				float MinY = -GraphicFX[0]->CreationSize.y/2;
				float MaxY = GraphicFX[0]->CreationSize.y/2;
				float MinZ = -GraphicFX[0]->CreationSize.z/2;
				float MaxZ = GraphicFX[0]->CreationSize.z/2;
				// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
				OBB[0] = AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
				OBB[1] = AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
				OBB[2] = AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
				OBB[3] = AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
				OBB[4] = AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
				OBB[5] = AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
				OBB[6] = AABB[6] = VECTOR3D(MinX, MinY, MinZ);
				OBB[7] = AABB[7] = VECTOR3D(MaxX, MinY, MinZ);
				OBBLocation = VECTOR3D(0,0,0);
				Width = fabsf(MaxX-MinX);
				Height = fabsf(MaxY-MinY);
				Length = fabsf(MaxZ-MinZ);
			}
			break;
		// Gauss
		case 15:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 21);
			break;
		// ракета
		case 16:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/missile.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 13);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;
		// рой
		case 17:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/swarm.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 16);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;
		// торпеда
		case 18:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/torpedo.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 14);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;
		// бомба
		case 19:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/nuke.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 15);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;






		// снаряды пришельцев



		// как Kinetic1
		case 101:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 22);
			break;
		// с наведением, как Kinetic2
		case 102:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 23);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 102);
			break;
		// как Kinetic3
		case 103:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 24);
			break;
		// с наведением, как Kinetic3
		case 104:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 24);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 102);
			break;
		// как Kinetic2
		case 105:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 23);
			break;
		// энергетическая мина (1-й тип)
		case 106:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 36);
			break;
		// энергетическая мина (2-й тип)
		case 107:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 37);
			break;
		// как Plasma3
		case 108:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 8);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 9);
			GraphicFX[2] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[2], 11);
			break;
		// как Plasma2
		case 109:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 10);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 8);
			GraphicFX[2] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[2], 9);
			break;
		// как Laser
		case 110:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 38);
			GraphicFX[0]->CreationSize = VECTOR3D(2.0f,2.0f,110.0f);
			ProjectileCenter = VECTOR3D(0.0f,0.0f,55.0f);
			NeedStopPartic = true;
			{
				float MinX = -GraphicFX[0]->CreationSize.x/2;
				float MaxX = GraphicFX[0]->CreationSize.x/2;
				float MinY = -GraphicFX[0]->CreationSize.y/2;
				float MaxY = GraphicFX[0]->CreationSize.y/2;
				float MinZ = -GraphicFX[0]->CreationSize.z/2;
				float MaxZ = GraphicFX[0]->CreationSize.z/2;
				// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
				OBB[0] = AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
				OBB[1] = AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
				OBB[2] = AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
				OBB[3] = AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
				OBB[4] = AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
				OBB[5] = AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
				OBB[6] = AABB[6] = VECTOR3D(MinX, MinY, MinZ);
				OBB[7] = AABB[7] = VECTOR3D(MaxX, MinY, MinZ);
				OBBLocation = VECTOR3D(0,0,0);
				Width = fabsf(MaxX-MinX);
				Height = fabsf(MaxY-MinY);
				Length = fabsf(MaxZ-MinZ);
			}
			break;




		// снаряды пиратов


		// стрельба турели 1
		case 201:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 26);
			break;
		// стрельба турели 2
		case 202:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 34);
			break;
		// фларес
		case 203:
			// смотрит вверх
			Orientation = VECTOR3D(0.0f, 0.5f, 0.5f);
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 25);
			break;
		// как Kinetic1
		case 204:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 26);
			break;
		// как Missile1
		case 205:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/missile.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 27);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;
		// как Missile2
		case 206:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/swarm.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 31);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;
		// как Ion2
		case 207:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 6);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 5);
			GraphicFX[1]->CreationSize = VECTOR3D(2.5f,2.5f,0.5f);
			GraphicFX[1]->DeadZone = 1.9f;
			break;
		// как Antimatter
		case 208:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 19);
			break;
		// как торпеда
		case 209:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/torpedo.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 32);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;
		// как бомба
		case 210:
			LoadObjectData("DATA/MODELS/EARTHFIGHTER/nuke.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/rockets.tga");
			::CObject3D::InitByDrawObjectList();
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 33);
			GraphicFXLocation[0] = VECTOR3D(0.0f, 0.0f, -Length/2.0f);
			NeedStopPartic = true;
			// шлейф
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 101);
			GraphicFXLocation[1] = GraphicFXLocation[0];
			break;
		// как Kinetic2
		case 211:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 34);
			break;
		// как Kinetic3
		case 212:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 35);
			break;
		// как Plasma2
		case 213:
			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 10);
			GraphicFX[1] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[1], 8);
			GraphicFX[2] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[2], 9);
			break;



		// мина1 (простое наведение по высоте)
		case 214:
			MineIData = 0.0f;
			Strength = StrengthStart = 10.0f;
			LoadObjectData("DATA/MODELS/MINE/mine-01.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine1.tga");
			TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine1i.tga");
			::CObject3D::InitByDrawObjectList();
			break;
		// мина2 (наведение по высоте + приближение)
		case 215:
			MineIData = 0.0f;
			Strength = StrengthStart = 15.0f;
			LoadObjectData("DATA/MODELS/MINE/mine-02.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine2.tga");
			TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine2i.tga");
			::CObject3D::InitByDrawObjectList();

			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 28);
			GraphicFXLocation[0] = VECTOR3D(0.0f, -0.8f, 0.0f);
			NeedStopPartic = true;
			break;
		// мина3 (наведение по высоте + стрельба снарядами)
		case 216:
			MineIData = 0.0f;
			Strength = StrengthStart = 20.0f;
			MineReloadTime = MineNextFireTime = 1.0f;
			LoadObjectData("DATA/MODELS/MINE/mine-03.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine3.tga");
			TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine3i.tga");
			::CObject3D::InitByDrawObjectList();

			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 29);
			GraphicFXLocation[0] = VECTOR3D(0.0f, -2.5f, 0.0f);
			NeedStopPartic = true;
			break;
		// мина4 (наведение по высоте + стрельба ракетами)
		case 217:
			MineIData = 0.0f;
			Strength = StrengthStart = 40.0f;
			MineReloadTime = MineNextFireTime = 3.0f;
			LoadObjectData("DATA/MODELS/MINE/mine-04.VW3D", this, 0, 2.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine4.tga");
			TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine4i.tga");
			::CObject3D::InitByDrawObjectList();

			GraphicFX[0] = new eParticleSystem;
			SetProjectileGFX(GraphicFX[0], 30);
			GraphicFXLocation[0] = VECTOR3D(0.0f, -0.7f, 0.0f);
			NeedStopPartic = true;
			break;





		default:
			fprintf(stderr, "Wrong ProjectileNum!");
	}




}










//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
CProjectile::~CProjectile(void)
{

	if (GraphicFX != 0)
	{
		for (int i=0; i<GraphicFXQuantity; i++)
		if (GraphicFX[i] != 0)
		{
			if (GraphicFXDestroyType)
			{
				delete GraphicFX[i]; GraphicFX[i] = 0;
			}
			else
			{
				if (!NeedStopPartic)
					GraphicFX[i]->ChangeSpeed(Orientation^Speed);
				else
					GraphicFX[i]->StopAllParticles();
				GraphicFX[i]->IsSuppressed = true;
				GraphicFX[i]->DestroyIfNoParticles = true;
			}
		}
		delete [] GraphicFX; GraphicFX = 0;
	}

	if (GraphicFXLocation != 0){delete [] GraphicFXLocation; GraphicFXLocation = 0;}


	DetachProjectile(this);
}











//-----------------------------------------------------------------------------
// Установка положения
//-----------------------------------------------------------------------------
void CProjectile::SetRotation(VECTOR3D NewRotation)
{

	// вызываем родительскую функцию
	::CObject3D::SetRotation(NewRotation);


	if (GraphicFX != 0)
	for (int i=0; i<GraphicFXQuantity; i++)
	{
		if (GraphicFX[i] != 0)
		{
			Matrix33CalcPoint(&(GraphicFXLocation[i]), OldInvRotationMat);
			Matrix33CalcPoint(&(GraphicFXLocation[i]), CurrentRotationMat);
			// если лучевое оружие, нужно вращать все, и частицы тоже
			if (ProjectileType == 2)
				GraphicFX[i]->RotateSystemAndParticlesByAngle(Rotation);
			else
				GraphicFX[i]->RotateSystemByAngle(Rotation);
		}
	}

}







//-----------------------------------------------------------------------------
// Установка положения
//-----------------------------------------------------------------------------
void CProjectile::SetLocation(VECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	::CObject3D::SetLocation(NewLocation);

	if (GraphicFX != 0)
	for (int i=0; i<GraphicFXQuantity; i++)
	{

		switch (Num)
		{

			// снаряды землян


			// Kinetic
			case 1:
			case 2:
			case 3:
			case 4:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// Ion
			case 5:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				break;
			case 6:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				break;
			case 7:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				break;
			// Plasma
			case 8:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				break;
			case 9:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				if (GraphicFX[2] != 0) GraphicFX[2]->MoveSystem(GraphicFXLocation[2] + Location);
				break;
			case 10:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				if (GraphicFX[2] != 0) GraphicFX[2]->MoveSystemLocation(GraphicFXLocation[2] + Location);
				break;
			// Maser
			case 11:
				if (GraphicFX[0] != 0)
				{
					GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
					GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
				}
				break;
			case 12:
				if (GraphicFX[0] != 0)
				{
					GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
					GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
				}
				if (GraphicFX[1] != 0)
				{
					GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
					GraphicFX[1]->SetStartLocation(GraphicFXLocation[1] + Location);
				}
				break;
			// Antimatter
			case 13:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				break;
			// Laser
			case 14:
				if (GraphicFX[0] != 0)
				{
					GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
					GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
				}
				break;
			// Gauss
			case 15:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				break;
			// Missiles
			case 16:
			case 17:
			case 18:
			case 19:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystemLocation(GraphicFXLocation[1] + Location);
				break;



			// снаряды пришельцев


			// как Kinetic1
			case 101:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// с наведением, как Kinetic2
			case 102:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystemLocation(GraphicFXLocation[1] + Location);
				break;
			case 103:
			// как Kinetic3
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// с наведением, как Kinetic3
			case 104:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystemLocation(GraphicFXLocation[1] + Location);
				break;
			// как Kinetic2
			case 105:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// энергетическая мина (1-й тип)
			case 106:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
				break;
			// энергетическая мина (2-й тип)
			case 107:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// как Plasma3
			case 108:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				if (GraphicFX[2] != 0) GraphicFX[2]->MoveSystemLocation(GraphicFXLocation[2] + Location);
				break;
			// как Plasma2
			case 109:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				if (GraphicFX[2] != 0) GraphicFX[2]->MoveSystem(GraphicFXLocation[2] + Location);
				break;
			// как Laser
			case 110:
				if (GraphicFX[0] != 0)
				{
					GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
					GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
				}
				break;



			// снаряды пиратов


			// стрельба турели 1
			case 201:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// стрельба турели 2
			case 202:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// фларес
			case 203:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// как Kinetic1
			case 204:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			case 205: // как Missile1
			case 206: // как Missile2
			case 209: // как Missile3
			case 210: // как Missile4
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystemLocation(GraphicFXLocation[1] + Location);
				break;
			// как Ion2
			case 207:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				break;
			// Antimatter
			case 208:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				break;
			// как Kinetic2
			case 211:
			// как Kinetic3
			case 212:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystemLocation(GraphicFXLocation[0] + Location);
				break;
			// как Plasma2
			case 213:
				if (GraphicFX[0] != 0) GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
				if (GraphicFX[1] != 0) GraphicFX[1]->MoveSystem(GraphicFXLocation[1] + Location);
				if (GraphicFX[2] != 0) GraphicFX[2]->MoveSystem(GraphicFXLocation[2] + Location);
				break;


			// снаряды без оружия


			// мины
			case 214: break;
			case 215:
				if (GraphicFX[0] != 0)
				{
					GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
					GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
				}
				break;
			case 216:
				if (GraphicFX[0] != 0)
				{
					GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
					GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
				}
				break;
			case 217:
				if (GraphicFX[0] != 0)
				{
					GraphicFX[0]->MoveSystem(GraphicFXLocation[0] + Location);
					GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
				}
				break;


		}


	}
}








//-----------------------------------------------------------------------------
// выполнение действий
//-----------------------------------------------------------------------------
bool CProjectile::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!::CObject3D::Update(Time))
	{
		// если лучевое оружие, никогда не уничтожаем его!!!
		// это сделаем в пушке...
		if (ProjectileType == 2) return true;


		// передаем на самоуничтожение... там все сделаем правильно
		CBulletExplosion *TMPBulletExplosion;
		TMPBulletExplosion = new CBulletExplosion;
		TMPBulletExplosion->Create(0, this, -Num, Location, Speed);

		return false;
	}

	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;



	Speed = SpeedStart*(Lifetime/Age) + SpeedEnd*((Age-Lifetime)/Age);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Общее перемещение снаряда...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	VECTOR3D Velocity = Orientation^(Speed*TimeDelta);
	SetLocation(Location+Velocity);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// общий - пенальти, если не игрок
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	float CurrentPenalty = GameNPCWeaponPenalty*1.0f;
	// если игрок или свои - ничего не надо...
	if (ObjectStatus >= 2) CurrentPenalty = 1.0f;




	float RotationSpeed;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Если что-то надо делать со снарядом (наводить к примеру)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch (Num)
	{
		// Plasma
		case 9:
		case 109:
			GraphicFX[1]->RotateSystemAndParticlesByAngle(VECTOR3D(GraphicFX[1]->Angle.x-360.0f*TimeDelta,GraphicFX[1]->Angle.y,GraphicFX[1]->Angle.z));
			GraphicFX[2]->RotateSystemAndParticlesByAngle(VECTOR3D(GraphicFX[2]->Angle.x-360.0f*TimeDelta,GraphicFX[2]->Angle.y,GraphicFX[2]->Angle.z));
			break;
		case 10:
		case 108:
		case 213:
			GraphicFX[0]->RotateSystemAndParticlesByAngle(VECTOR3D(GraphicFX[0]->Angle.x-360.0f*TimeDelta,GraphicFX[0]->Angle.y,GraphicFX[0]->Angle.z));
			GraphicFX[1]->RotateSystemAndParticlesByAngle(VECTOR3D(GraphicFX[1]->Angle.x-360.0f*TimeDelta,GraphicFX[1]->Angle.y,GraphicFX[1]->Angle.z));
			break;

		// Maser
		case 11:
			GraphicFX[0]->RotateParticlesByAngle(VECTOR3D(0.0f,	0.0f, 360.0f*TimeDelta*3.0f));
			if (Lifetime <= GraphicFX[0]->Life/1.5f) GraphicFX[0]->IsSuppressed = true;
			break;
		case 12:
			GraphicFX[0]->RotateParticlesByAngle(VECTOR3D(0.0f,0.0f,360.0f*TimeDelta*4.0f));
			if (Lifetime <= GraphicFX[0]->Life/1.5f) GraphicFX[0]->IsSuppressed = true;
			GraphicFX[1]->RotateParticlesByAngle(VECTOR3D(0.0f,0.0f,-360.0f*TimeDelta*2.0f));
			if (Lifetime <= GraphicFX[1]->Life/1.5f) GraphicFX[1]->IsSuppressed = true;
			break;

		// Antimatter
		case 13:
		case 208:
			GraphicFX[0]->RotateSystemAndParticlesByAngle(VECTOR3D(GraphicFX[0]->Angle.x,GraphicFX[0]->Angle.y-360.0f*TimeDelta*2.0f,GraphicFX[0]->Angle.z));
			break;

		// Laser
		case 14:
		case 110:
			if (Lifetime <= GraphicFX[0]->Life/1.5f) GraphicFX[0]->IsSuppressed = true;
			break;

		// Gauss
		case 15:
			GraphicFX[0]->RotateParticlesByAngle(VECTOR3D(0.0f,0.0f,360.0f*TimeDelta*6.0f));
			break;




		// ракеты



		// ракеты землян
		case 16:
			RotationSpeed = 50.0f;
			goto missile;
		case 17:
			RotationSpeed = 60.0f;
			goto missile;
		case 18:
			RotationSpeed = 30.0f;
			goto missile;
		case 19:
			RotationSpeed = 10.0f;
			goto missile;
		// ракеты пришельцев
		case 102:
			RotationSpeed = 20.0f;
			goto missile;
		case 104:
			RotationSpeed = 40.0f;
			goto missile;
		// ракеты пиратов
		case 205:
			RotationSpeed = 35.0f;
			goto missile;
		case 206:
			RotationSpeed = 40.0f;
			goto missile;
		case 209:
			RotationSpeed = 30.0f;
			goto missile;
		case 210:
			RotationSpeed = 10.0f;
			{
missile:
				VECTOR3D NeedAngle = Rotation;
				// устанавливаем в Target на что наведен этот снаряд, если еще ничего не выбрано
				if (Target == 0)
				{
					Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, &NeedAngle);
				}
				else // если уже что-то выбрали
				{
					// 1. надо проверить, есть ли еще вообще этот объект, если он есть - летим к нему
					if (GetMissileTargetLiveStatus(Target))
					{
						// получаем углы, возвращает false - значит цель уже проскочили, и надо навестись на новую
						if (!GetMissileOnTargetOrientateion(Location, Rotation, CurrentRotationMat, Target, &NeedAngle))
							Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, &NeedAngle);
					}
					// 2. если объекта нет (уже взорвали), надо наводить на другой
					else
					{
						Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, &NeedAngle);
					}
				}



				// учитываем скорость поворота по вертикали
				if (Rotation.y < NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
					if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;

				}
				if (Rotation.y > NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
					if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;
				}

				if (Rotation.x < NeedAngle.x)
				{
					float NeedAngle_x = Rotation.x+RotationSpeed*TimeDelta;
					if (NeedAngle_x > NeedAngle.x) NeedAngle_x = NeedAngle.x;
					NeedAngle.x = NeedAngle_x;

				}
				if (Rotation.x > NeedAngle.x)
				{
					float NeedAngle_x = Rotation.x-RotationSpeed*TimeDelta;
					if (NeedAngle_x < NeedAngle.x) NeedAngle_x = NeedAngle.x;
					NeedAngle.x = NeedAngle_x;
				}


				VECTOR3D TTT = NeedAngle-Rotation;
				SetRotation(TTT);

			}
			break;



		// мины



		// мина пиратов (1-й тип)
		case 214:
			MineIData += TimeDelta;
			if (MineIData >= 0.1f)
			{
				if (TextureIllum[0] == 0) TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine1i.tga");
				else TextureIllum[0] = 0;
				MineIData = 0.0f;
			}
			DrawObjectList[0].Rotation.y += 90.0f*TimeDelta;
			while (DrawObjectList[0].Rotation.y > 360.0f) DrawObjectList[0].Rotation.y -= 360.0f;
		// мина пришельцев, энергетическая (1-й тип)
		case 106:
			{
				float MineSpeed = 5.0f;

				// получаем положение ближайшего врага
				CObject3D *tmpTarget = GetCloserTargetPosition(ObjectStatus, Location);

				// !!! упращенно, не учитываем положение плоскости
				if (tmpTarget != 0)
				{
					float SpeedTmp = MineSpeed*TimeDelta;
					if (SpeedTmp > fabs(Location.y-tmpTarget->Location.y)) SpeedTmp = fabs(Location.y-tmpTarget->Location.y);

					if (SpeedTmp != 0.0f)
					{
						// находим направление (если нужно вниз, меняем знак)
						if (Location.y > tmpTarget->Location.y) SpeedTmp *= -1.0f;

						VECTOR3D VelocityUp = VECTOR3D(0.0f,SpeedTmp,0.0f);
						SetLocation(Location+VelocityUp);
					}
				}

				if (GraphicFXQuantity >= 1)
				if (GraphicFX[0] != 0) GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);
			}
			break;




		// мина пиратов (2-й тип)
		case 215:
			MineIData += TimeDelta;
			if (MineIData >= 0.1f)
			{
				if (TextureIllum[0] == 0) TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine2i.tga");
				else TextureIllum[0] = 0;
				MineIData = 0.0f;
			}
			DrawObjectList[0].Rotation.y += 90.0f*TimeDelta;
			while (DrawObjectList[0].Rotation.y > 360.0f) DrawObjectList[0].Rotation.y -= 360.0f;
		// мина пришельцев, энергетическая (2-й тип)
		case 107:
			RotationSpeed = 180.0f;
			{
				VECTOR3D NeedAngle = Rotation;
				// устанавливаем в Target на что наведен этот снаряд
				Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation,
					CurrentRotationMat, &NeedAngle);


				// учитываем скорость поворота по вертикали
				if (Rotation.y < NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
					if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;

				}
				if (Rotation.y > NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
					if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;
				}

				NeedAngle.z = Rotation.z;
				NeedAngle.x = Rotation.x;

				VECTOR3D TTT = NeedAngle-Rotation;
				SetRotation(TTT);


				// если есть цель, поднимаемся на ее уровень
				if (Target != 0)
				{
					float MineSpeed = 5.0f;

					// получаем положение ближайшего врага
					VECTOR3D NeedPoint = Target->Location;

					// !!! упращенно, не учитываем положение плоскости

					float SpeedTmp = MineSpeed*TimeDelta;
					if (SpeedTmp > fabs(Location.y-NeedPoint.y)) SpeedTmp = fabs(Location.y-NeedPoint.y);

					if (SpeedTmp != 0.0f)
					{
						// находим направление (если нужно вниз, меняем знак)
						if (Location.y > NeedPoint.y) SpeedTmp *= -1.0f;

						VECTOR3D VelocityUp = VECTOR3D(0.0f,SpeedTmp,0.0f);
						SetLocation(Location+VelocityUp);
					}
				}

				if (GraphicFXQuantity >= 1)
				if (GraphicFX[0] != 0) GraphicFX[0]->SetStartLocation(GraphicFXLocation[0] + Location);

				// сбрасываем установку, чтобы не было голосового предупреждения
				Target = 0;
			}
			break;



		case 216:
			MineIData += TimeDelta;
			if (MineIData >= 0.1f)
			{
				if (TextureIllum[0] == 0) TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine3i.tga");
				else TextureIllum[0] = 0;
				MineIData = 0.0f;
			}
			DrawObjectList[0].Rotation.y += 120.0f*TimeDelta;
			while (DrawObjectList[0].Rotation.y > 360.0f) DrawObjectList[0].Rotation.y -= 360.0f;

			RotationSpeed = 180.0f;
			{
				VECTOR3D NeedAngle = Rotation;
				// устанавливаем в Target на что наведен этот снаряд
				Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation,
					CurrentRotationMat, &NeedAngle);


				// учитываем скорость поворота по вертикали
				if (Rotation.y < NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
					if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;

				}
				if (Rotation.y > NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
					if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;
				}

				NeedAngle.z = Rotation.z;
				NeedAngle.x = Rotation.x;

				VECTOR3D TTT = NeedAngle-Rotation;
				SetRotation(TTT);


				// если есть цель, поднимаемся на ее уровень
				if (Target != 0)
				{
					float MineSpeed = 5.0f;

					// получаем положение ближайшего врага
					VECTOR3D NeedPoint = Target->Location;

					// !!! упращенно, не учитываем положение плоскости

					float SpeedTmp = MineSpeed*TimeDelta;
					if (SpeedTmp > fabs(Location.y-2.0f-NeedPoint.y)) SpeedTmp = fabs(Location.y-2.0f-NeedPoint.y);

					if (SpeedTmp != 0.0f)
					{
						// находим направление (если нужно вниз, меняем знак)
						if (Location.y-2.0f > NeedPoint.y) SpeedTmp *= -1.0f;

						VECTOR3D VelocityUp = VECTOR3D(0.0f,SpeedTmp,0.0f);
						SetLocation(Location+VelocityUp);
					}

					// стрельба
					MineNextFireTime -= TimeDelta;
					if (MineNextFireTime <= 0.0f)
					{
						// создаем снаряд
						CProjectile *Projectile  = 0;
						Projectile  = new CProjectile;
						Projectile->Create(204);
						Projectile->SetLocation(Location+VECTOR3D(0.0f, -2.0f, 0.0f));


						// общий - пенальти, если не игрок
						float CurrentPenalty = GameNPCWeaponPenalty*1.0f;
						// если игрок или свои - ничего не надо...
						if (ObjectStatus >= 2) CurrentPenalty = 1.0f;


						Projectile->SetRotation(Rotation);
						for (int i=0; i<Projectile->GraphicFXQuantity; i++)
						{
							Projectile->GraphicFX[i]->Direction = Orientation;
							// учитываем пенальти для визуальных эффектов
							Projectile->GraphicFX[i]->ParticlesPerSec = (int)(Projectile->GraphicFX[i]->ParticlesPerSec/CurrentPenalty);

							Projectile->GraphicFX[i]->Speed = Projectile->GraphicFX[i]->Speed/CurrentPenalty;
							Projectile->GraphicFX[i]->Life = Projectile->GraphicFX[i]->Life*CurrentPenalty;
							Projectile->GraphicFX[i]->AttractiveValue = Projectile->GraphicFX[i]->AttractiveValue/(CurrentPenalty*CurrentPenalty);
						}
						Projectile->ObjectStatus = ObjectStatus;
						// учитываем пенальти для снаряда
						Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
						Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
						Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
						Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;


						MineNextFireTime = MineReloadTime;
					}

				}


				// сбрасываем установку, чтобы не было голосового предупреждения
				Target = 0;
			}
			break;


		case 217:
			MineIData += TimeDelta;
			if (MineIData >= 0.1f)
			{
				if (TextureIllum[0] == 0) TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/MINE/mine4i.tga");
				else TextureIllum[0] = 0;
				MineIData = 0.0f;
			}
			DrawObjectList[0].Rotation.y += 90.0f*TimeDelta;
			while (DrawObjectList[0].Rotation.y > 360.0f) DrawObjectList[0].Rotation.y -= 360.0f;

			RotationSpeed = 180.0f;
			{
				VECTOR3D NeedAngle = Rotation;
				// устанавливаем в Target на что наведен этот снаряд
				Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation,
					CurrentRotationMat, &NeedAngle);


				// учитываем скорость поворота по вертикали
				if (Rotation.y < NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
					if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;

				}
				if (Rotation.y > NeedAngle.y)
				{
					float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
					if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
					NeedAngle.y = NeedAngle_y;
				}

				NeedAngle.z = Rotation.z;
				NeedAngle.x = Rotation.x;

				VECTOR3D TTT = NeedAngle-Rotation;
				SetRotation(TTT);


				// если есть цель, поднимаемся на ее уровень
				if (Target != 0)
				{
					float MineSpeed = 5.0f;

					// получаем положение ближайшего врага
					VECTOR3D NeedPoint = Target->Location;

					// !!! упращенно, не учитываем положение плоскости

					float SpeedTmp = MineSpeed*TimeDelta;
					if (SpeedTmp > fabs(Location.y+2.0f-NeedPoint.y)) SpeedTmp = fabs(Location.y+2.0f-NeedPoint.y);

					if (SpeedTmp != 0.0f)
					{
						// находим направление (если нужно вниз, меняем знак)
						if (Location.y+2.0f > NeedPoint.y) SpeedTmp *= -1.0f;

						VECTOR3D VelocityUp = VECTOR3D(0.0f,SpeedTmp,0.0f);
						SetLocation(Location+VelocityUp);
					}

					// стрельба
					MineNextFireTime -= TimeDelta;
					if (MineNextFireTime <= 0.0f)
					{
						// создаем снаряд
						CProjectile *Projectile  = 0;
						Projectile  = new CProjectile;
						Projectile->Create(206);
						Projectile->SetLocation(Location+VECTOR3D(0.0f, 0.0f, 0.0f));


						// общий - пенальти, если не игрок
						float CurrentPenalty = GameNPCWeaponPenalty*1.0f;
						// если игрок или свои - ничего не надо...
						if (ObjectStatus >= 2) CurrentPenalty = 1.0f;


						Projectile->SetRotation(Rotation);
						for (int i=0; i<Projectile->GraphicFXQuantity; i++)
						{
							Projectile->GraphicFX[i]->Direction = Orientation;
							// учитываем пенальти для визуальных эффектов
							Projectile->GraphicFX[i]->ParticlesPerSec = (int)(Projectile->GraphicFX[i]->ParticlesPerSec/CurrentPenalty);

							Projectile->GraphicFX[i]->Speed = Projectile->GraphicFX[i]->Speed/CurrentPenalty;
							Projectile->GraphicFX[i]->Life = Projectile->GraphicFX[i]->Life*CurrentPenalty;
							Projectile->GraphicFX[i]->AttractiveValue = Projectile->GraphicFX[i]->AttractiveValue/(CurrentPenalty*CurrentPenalty);
						}
						Projectile->ObjectStatus = ObjectStatus;
						// учитываем пенальти для снаряда
						Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
						Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
						Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
						Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;


						MineNextFireTime = MineReloadTime;
					}

				}


				// сбрасываем установку, чтобы не было голосового предупреждения
				Target = 0;
			}
			break;
	}









	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// понемногу сводим снаряд на нет... так красивее
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /*  for (int i=0; i<GraphicFXQuantity; i++)
	{
	//	GraphicFX[i]->Theta = 1.0f + 20.0f*((Age-Lifetime)/Age);
		if (Lifetime <= GraphicFX[i]->Life-0.1f) GraphicFX[i]->IsSuppressed = true;
	}	*/


	// объект в порядке - удалять не нужно
	return true;
}
