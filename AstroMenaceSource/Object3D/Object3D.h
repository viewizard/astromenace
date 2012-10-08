/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Object3D.h

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


#ifndef OBJECT3D_H
#define OBJECT3D_H


#include "../Core/Core.h"
#include "../Struct.h"


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



extern eGLSL 	*GLSLShaderType1[2][7];
extern eGLSL 	*GLSLShaderType2[2][7];
extern eGLSL 	*GLSLShaderType3[2][7];




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
	virtual void	Draw();
	bool			NeedCullFaces; // нужно резать бэк фейсы
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

	// список с геометрией модели, объекты для прорисовки
	eObjectBlock	*DrawObjectList;
	float *GlobalVertexBuffer;
	unsigned int *GlobalVertexBufferVBO;
	unsigned int *GlobalIndexBuffer;
	unsigned int *GlobalIndexBufferVBO;
	unsigned int *GlobalVAO;
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
void	DrawAllObject3D();
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
void LoadObjectData(const char *Name, CObject3D* Object3D, int ObjectNum);
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
// Получение положения ближайшего врага, для мин
CObject3D *GetCloserTargetPosition(int ObjectStatus, VECTOR3D Location);






#endif // OBJECT3D_H
