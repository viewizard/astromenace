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


#ifndef OBJECT3D_H
#define OBJECT3D_H


#include "../Core/Core.h"
#include "../Struct.h"

extern GameSetup Setup;


extern int GameNPCWeaponPenalty;
extern int GameNPCArmorPenalty;
extern int GameNPCTargetingSpeedPenalty;
extern int GameLimitedAmmo;
extern int GameDestroyableWeapon;
extern int GameWeaponTargetingMode;
extern int GameSpaceShipControlMode;
extern int GameEngineSystem;
extern int GameTargetingSystem;
extern int GameAdvancedProtectionSystem;
extern int GamePowerSystem;
extern int GameTargetingMechanicSystem;



int Audio_PlaySound3D(int SoundID, float fVol, VECTOR3D Location, bool Loop, int AtType=1);





//-----------------------------------------------------------------------------
// еденичная запись поведения
//-----------------------------------------------------------------------------
struct CTimeSheet
{
	// флаг, показывает что установлен и задействован этот режим
	bool InUse;
	// кол-во времени (оставшееся), которое работает этот режим
	float Time;
	// включена ли автоматическая настройка поведения по предустановкам
	int AI_Mode;

	// данные, скорость
	float Speed;
	float Acceler;
	float SpeedLR;
	float AccelerLR;
	float SpeedUD;
	float AccelerUD;

	float SpeedByCamFB;
	float AccelerByCamFB;
	float SpeedByCamLR;
	float AccelerByCamLR;
	float SpeedByCamUD;
	float AccelerByCamUD;

	// поворот
	VECTOR3D Rotation;
	VECTOR3D RotationAcceler;
	// стрельба
	bool Fire;
	// стрельба спец оружием боса
	bool BossFire;
	// наведение на цель (для турелей)
	bool Targeting;

	// указатели на цепочку
	CTimeSheet *Next;
	CTimeSheet *Prev;
};




//-----------------------------------------------------------------------------
// Класс CObject3D
//-----------------------------------------------------------------------------
class CObject3D
{
public:

	// базовые конструктор и деструктор объекта
	CObject3D();
	virtual ~CObject3D();

	// Установка AABB, OBB и габаритов по геометрии объекта
	virtual void	InitByDrawObjectList();
	// Прорисовка объектa Object3D
	virtual void	Draw(bool VertexOnlyPass, bool ShadowMap=false);
	bool			NeedCullFaces; // нужно резать бэк фейсы
	bool			NeedAlphaTest; // нужно включить альфа тест
	// Обновление данных объектa Object3D
	virtual bool	Update(float Time);


	// Установка положения 1 объекта модели
	void SetObjectLocation(VECTOR3D NewLocation, int ObjectNum);
	// Установка углов поворота 1 объекта модели
	void SetObjectRotation(VECTOR3D NewRotation, int ObjectNum);
	// Установка положения модели
	virtual void	SetLocation(VECTOR3D NewLocation);
	// Установка углов поворота модели
	virtual void	SetRotation(VECTOR3D NewRotation);


	// статус объекта свой-чужой (1враг-2друг-3игрок)
	int				ObjectStatus;
	// флаг показать - удалить -1 - не задействован, 0-ждем показа, 1-показали, 2-нужно удалить
	int				ShowDeleteOnHide;
	// уникальный идентифакационный номер, или 0, если он не нужен
	int				ID;
	// тип объекта
	// 1 - Earth Fighter
	// 2 - Alien Fighter
	// 3 - Alien MotherShip
	// 4 - Pirate Ship
	// 5 - Pirate Vehicle (Wheeled + Tracked)
	// 6 - Pirate Building
	// 7 - Asteroids
	// 8 - ShipPart
	// 9 - ShipWeapon
	// 10 - Projectile
	// 11 - Explosion
	// 12 - Civilian Building
	// 13 - BasePart
	// 14 - Planet
	// 15 - Big Asteroid
	// добавь в функцию NeedCheckCollision!!!
	int				ObjectType;


	// тип (номер при создании)
	int				ObjectCreationType;


	// время жизни объекта в секундах, оставшееся
	float			Lifetime;

	// направление, куда ориентирован объект
	VECTOR3D		Orientation;
	// углы поворота объекта по трем осям на данный момент
	VECTOR3D		Rotation;
	// обратный угол поворота, старого
	VECTOR3D		OldRotationInv;
	// положение объекта
	VECTOR3D		Location;
	VECTOR3D		GeometryCenterLocation; // точка "центра геометрии" внутри модели
	// предыдущее положение объекта
	VECTOR3D		PrevLocation;


	// коробки объекта для проверки прорисовки и коллизий
	VECTOR3D		AABB[8]; // в AABB всегда абсолютные координаты (относ. коорд. модели)
	VECTOR3D		OBB[8];
	VECTOR3D		OBBLocation; // положение OBB относительно координат модели
	// для каждого объекта в моделе
	VECTOR3D		**HitBB;
	VECTOR3D		*HitBBLocation; // положение HitBB относительно координат модели
	float			*HitBBRadius2; 	// квадрат радиуса HitBB
	VECTOR3D		*HitBBSize; // размеры HitBB
	// радиус, для первой проверки коллизий
	float			Radius;




	// последнее время, когда проверяли-обновляли объект
	float			TimeLastUpdate;
	// время между апдейтами
	float			TimeDelta;


	// небольшая девиация-болтание модели или каки-то объектов
	bool		DeviationOn;
	int			DeviationObjQuantity;
	VECTOR3D	*Deviation;
	float		*CurentDeviation;
	float		*NeedDeviation;
	float		*CurentDeviationSum;
	// ассоциированный объект (если нужно)
	int			*DeviationObjNum;


	// текстура для объектов геометрии
	eTexture		**Texture;
	eTexture		**TextureIllum;
	eTexture		**NormalMap;

	// список с геометрией модели, объекты для прорисовки
	eObjectBlock	*DrawObjectList;
	float			*GlobalVertexBuffer;
	unsigned int	*GlobalVBO;
	unsigned int	*GlobalIndexBuffer;
	unsigned int	*GlobalIBO;
	unsigned int	*GlobalVAO;
	// кол-во объектов в списке
	int				DrawObjectQuantity;
	// минимальное расстояние (квадрат) с которого надо рисовать пообъектно, если -1 всегда рисовать только пообъектно
	float			PromptDrawDist2;
	int				InternalLights; // кол-во внутренних источников света


	// данные материала объекта
	float Diffuse[4];
	float Specular[4];
	float Ambient[4];
	float Power[1];


	// вес объекта
	float			Weight;
	// ширина
	float			Width;
	// длина
	float			Length;
	// высота
	float			Height;
	// прочность
	float			Strength;
	float			StrengthStart;
	// прочность щита
	float			ShieldStrength;
	float			ShieldStrengthStart;
	float			ShieldRecharge; // перезарядка щита в секунду
	// показывать полосу или нет
	bool			ShowStrength;
	// этот параметр устанавливается автоматически!
	// если попали, будем всегда показывать (даже если щит перезарядился)
	bool			NeedShowStrengthNow;




	// максимально возможное ослабление воздействий на корпус
	float			ResistanceHull;
	// максимально возможное ослабление воздействий на системы
	float			ResistanceSystems;


	// матрицы поворота, для оптимизации просчетов
	float	CurrentRotationMat[9];
	float	OldInvRotationMat[9];



	// отладочная информация... номер линии в скрипте
	char			*DebugInfo;


	// данные по текущему сценарию действий объекта
	CTimeSheet *StartTimeSheet; // - этот текущий
	CTimeSheet *EndTimeSheet;
	// действия
	void AttachTimeSheet(CTimeSheet *TimeSheet);
	void DetachTimeSheet(CTimeSheet *TimeSheet);
};



// работа с распаковкой CTimeSheet
void InterAIMode(CObject3D *Object, CTimeSheet *TimeSheet);
// работа с набором AI
void ReleaseGameAI();
void InitGameAI(const char *FileName);




//-----------------------------------------------------------------------------
// Менеджер CObject3D
//-----------------------------------------------------------------------------

// Удаляем все объекты в списке
void	ReleaseAllObject3D();
// Прорисовываем все объекты
void	DrawAllObject3D(int DrawType); // 1- меню, 2- игра
// Проверяем все объекты на столкновение
void	DetectCollisionAllObject3D();
//	Проверяем все объекты, обновляем данные
void	UpdateAllObject3D(float Time);










//-----------------------------------------------------------------------------
// Дополнительные функции для CObject3D
//-----------------------------------------------------------------------------

// Проверяем, нужно ли для данного объекта проверка коллизии и наведение на него
bool NeedCheckCollision(CObject3D* Object3D);
// Загрузка в модель нужной геометрии
void LoadObjectData(const char *Name, CObject3D* Object3D, int ObjectNum, float TriangleSizeLimit, bool NeedTangentAndBinormal=false);
// Получение угла поворота оружия на врага
void GetShipOnTargetOrientateion(int ObjectStatus, VECTOR3D Location, VECTOR3D CurrentObjectRotation,
		float MinDistance, float RotationMatrix[9], VECTOR3D *NeedAngle, float ObjectWidth, bool NeedCenterOrientation,
		bool NeedByWeaponOrientation, VECTOR3D WeponLocation, int WeaponType);
// Получение угла поворота оружия на врага для противника
void GetEnemyShipOnTargetOrientateion(int ObjectStatus, VECTOR3D Location, VECTOR3D CurrentObjectRotation,
		float RotationMatrix[9], VECTOR3D *NeedAngle, int WeaponType);
// Получение угла поворота турели на врага
bool GetTurretOnTargetOrientateion(int ObjectStatus, VECTOR3D Location, VECTOR3D CurrentObjectRotation,
		float RotationMatrix[9], VECTOR3D *NeedAngle, int WeaponType);
// Получение данных для наведение ракет
CObject3D *GetMissileOnTargetOrientateion(int	ObjectStatus, VECTOR3D Location,
		VECTOR3D CurrentObjectRotation, float RotationMatrix[9], VECTOR3D *NeedAngle, float MaxDistance);
bool GetMissileOnTargetOrientateion(VECTOR3D Location, VECTOR3D CurrentObjectRotation, float RotationMatrix[9], CObject3D *TargetObject, VECTOR3D *NeedAngle);
bool GetMissileTargetStatus(CObject3D *TargetObject, VECTOR3D Location, float RotationMatrix[9]);
// Получение положения ближайшего врага, для мин
CObject3D *GetCloserTargetPosition(int ObjectStatus, VECTOR3D Location);






#endif // OBJECT3D_H
