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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "../core/core.h"
#include "../struct.h"


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

unsigned int Audio_PlaySound3D(int SoundID, float LocalVolume, sVECTOR3D Location, int AtType = 1);

struct sTimeSheet;

//-----------------------------------------------------------------------------
// Класс cObject3D
//-----------------------------------------------------------------------------
class cObject3D
{
public:
	virtual ~cObject3D();

	// Установка AABB, OBB и габаритов по геометрии объекта
	virtual void	InitByDrawObjectList();
	// Прорисовка объектa Object3D
	virtual void	Draw(bool VertexOnlyPass, bool ShadowMap=false);
	bool		NeedCullFaces{true}; // нужно резать бэк фейсы
	bool		NeedAlphaTest{false}; // нужно включить альфа тест
	// Обновление данных объектa Object3D
	virtual bool	Update(float Time);

	// Установка положения 1 объекта модели
	void	SetObjectLocation(sVECTOR3D NewLocation, int ObjectNum);
	// Установка углов поворота 1 объекта модели
	void	SetObjectRotation(sVECTOR3D NewRotation, int ObjectNum);
	// Установка положения модели
	virtual void	SetLocation(sVECTOR3D NewLocation);
	// Установка углов поворота модели
	virtual void	SetRotation(sVECTOR3D NewRotation);

	// статус объекта свой-чужой (1враг-2друг-3игрок)
	int	ObjectStatus{2};
	// флаг показать - удалить -1 - не задействован, 0-ждем показа, 1-показали, 2-нужно удалить
	int	ShowDeleteOnHide{-1};
	// уникальный идентифакационный номер, или 0, если он не нужен
	int	ID{0};
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
	int	ObjectType{0};

	// тип (номер при создании)
	int	ObjectCreationType{0};

	// время жизни объекта в секундах, оставшееся
	float	Lifetime{-1.0f};

	// направление, куда ориентирован объект
	sVECTOR3D	Orientation{0.0f, 0.0f, 1.0f};
	// углы поворота объекта по трем осям на данный момент
	sVECTOR3D	Rotation{0.0f, 0.0f, 0.0f};
	// обратный угол поворота, старого
	sVECTOR3D	OldRotationInv{0.0f, 0.0f, 0.0f};
	// положение объекта
	sVECTOR3D	Location{0.0f, 0.0f, 0.0f};
	sVECTOR3D	GeometryCenterLocation{0.0f, 0.0f, 0.0f}; // точка "центра геометрии" внутри модели
	// предыдущее положение объекта
	sVECTOR3D	PrevLocation{0.0f, 0.0f, 0.0f};

	// коробки объекта для проверки прорисовки и коллизий
	sVECTOR3D	AABB[8]{{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f}}; // в AABB всегда абсолютные координаты (относ. коорд. модели)
	sVECTOR3D	OBB[8]{{0.0f, 0.0f, 0.0f},
			       {0.0f, 0.0f, 0.0f},
			       {0.0f, 0.0f, 0.0f},
			       {0.0f, 0.0f, 0.0f},
			       {0.0f, 0.0f, 0.0f},
			       {0.0f, 0.0f, 0.0f},
			       {0.0f, 0.0f, 0.0f},
			       {0.0f, 0.0f, 0.0f}};
	sVECTOR3D	OBBLocation{0.0f, 0.0f, 0.0f}; // положение OBB относительно координат модели
	// для каждого объекта в моделе
	sVECTOR3D	**HitBB{nullptr};
	sVECTOR3D	*HitBBLocation{nullptr}; // положение HitBB относительно координат модели
	float		*HitBBRadius2{nullptr}; 	// квадрат радиуса HitBB
	sVECTOR3D	*HitBBSize{nullptr}; // размеры HitBB
	// радиус, для первой проверки коллизий
	float		Radius{0.0f};

	// последнее время, когда проверяли-обновляли объект
	float	TimeLastUpdate{-1.0f};
	// время между апдейтами
	float	TimeDelta{0.0f};

	// небольшая девиация-болтание модели или каки-то объектов
	bool		DeviationOn{false};
	int		DeviationObjQuantity{0};
	sVECTOR3D	*Deviation{nullptr};
	float		*CurentDeviation{nullptr};
	float		*NeedDeviation{nullptr};
	float		*CurentDeviationSum{nullptr};
	// ассоциированный объект (если нужно)
	int		*DeviationObjNum{nullptr};

	// текстура для объектов геометрии
	std::vector<GLtexture> Texture{};
	std::vector<GLtexture> TextureIllum{};
	std::vector<GLtexture> NormalMap{};

	// список с геометрией модели, объекты для прорисовки
	std::vector<sObjectBlock> ObjectBlocks{};
	std::shared_ptr<float> GlobalVertexArray{}; // float[], make sure, that custom deleter are used
	GLuint GlobalVBO{0};
	std::shared_ptr<unsigned> GlobalIndexArray{}; // unsigned[], make sure, that custom deleter are used
	GLuint GlobalIBO{0};
	GLuint GlobalVAO{0};
	// минимальное расстояние (квадрат) с которого надо рисовать пообъектно, если -1 всегда рисовать только пообъектно
	float		PromptDrawDist2{-1.0f};
	int		InternalLights{0}; // кол-во внутренних источников света

	// данные материала объекта
	float	Diffuse[4]{1.0f, 1.0f, 1.0f, 1.0f};
	float	Specular[4]{1.0f, 1.0f, 1.0f, 1.0f};
	float	Ambient[4]{0.1f, 0.1f, 0.1f, 0.1f};
	float	Power[1]{64.0f};

	// вес объекта
	float	Weight{1.0f};
	// ширина
	float	Width{1.0f};
	// длина
	float	Length{1.0f};
	// высота
	float	Height{1.0f};
	// прочность
	float	Strength{0.0f};
	float	StrengthStart{0.0f};
	// прочность щита
	float	ShieldStrength{0.0f};
	float	ShieldStrengthStart{0.0f};
	float	ShieldRecharge{0.0f}; // перезарядка щита в секунду
	// показывать полосу или нет
	bool	ShowStrength{true};
	// этот параметр устанавливается автоматически!
	// если попали, будем всегда показывать (даже если щит перезарядился)
	bool	NeedShowStrengthNow{false};

	// максимально возможное ослабление воздействий на корпус
	float	ResistanceHull{1.0f};
	// максимально возможное ослабление воздействий на системы
	float	ResistanceSystems{1.0f};

	// матрицы поворота, для оптимизации просчетов
	float	CurrentRotationMat[9]{1.0f, 0.0f, 0.0f,
				      0.0f, 1.0f, 0.0f,
				      0.0f, 0.0f, 1.0f};
	float	OldInvRotationMat[9]{1.0f, 0.0f, 0.0f,
				     0.0f, 1.0f, 0.0f,
				     0.0f, 0.0f, 1.0f};

	// отладочная информация... номер линии в скрипте
	char	*DebugInfo{nullptr};

	// данные по текущему сценарию действий объекта
	sTimeSheet	*StartTimeSheet{nullptr}; // - этот текущий
	sTimeSheet	*EndTimeSheet{nullptr};
	// действия
	void	AttachTimeSheet(sTimeSheet *TimeSheet);
	void	DetachTimeSheet(sTimeSheet *TimeSheet);
};


//-----------------------------------------------------------------------------
// Менеджер cObject3D
//-----------------------------------------------------------------------------

// Удаляем все объекты в списке
void ReleaseAllObject3D();
// Прорисовываем все объекты
void DrawAllObject3D(eDrawType DrawType);
// Проверяем все объекты на столкновение
void DetectCollisionAllObject3D();
//	Проверяем все объекты, обновляем данные
void UpdateAllObject3D(float Time);


//-----------------------------------------------------------------------------
// Дополнительные функции для cObject3D
//-----------------------------------------------------------------------------

// Проверяем, нужно ли для данного объекта проверка коллизии и наведение на него
bool NeedCheckCollision(cObject3D* Object3D);
// Загрузка в модель нужной геометрии
void LoadObjectData(const char *Name, cObject3D* Object3D, int ObjectNum,
		    float TriangleSizeLimit, bool NeedTangentAndBinormal=false);
// Получение угла поворота оружия на врага
void GetShipOnTargetOrientateion(int ObjectStatus, sVECTOR3D Location, sVECTOR3D CurrentObjectRotation,
				 float MinDistance, float (&RotationMatrix)[9], sVECTOR3D *NeedAngle,
				 float ObjectWidth, bool NeedCenterOrientation,
				 bool NeedByWeaponOrientation, sVECTOR3D WeponLocation, int WeaponType);
// Получение угла поворота оружия на врага для противника
void GetEnemyShipOnTargetOrientateion(int ObjectStatus, sVECTOR3D Location, sVECTOR3D CurrentObjectRotation,
				      float (&RotationMatrix)[9], sVECTOR3D *NeedAngle, int WeaponType);
// Получение угла поворота турели на врага
bool GetTurretOnTargetOrientateion(int ObjectStatus, sVECTOR3D Location, sVECTOR3D CurrentObjectRotation,
				   float (&RotationMatrix)[9], sVECTOR3D *NeedAngle, int WeaponType);
// Получение данных для наведение ракет
cObject3D *GetMissileOnTargetOrientateion(int ObjectStatus, sVECTOR3D Location,
					  sVECTOR3D CurrentObjectRotation, float (&RotationMatrix)[9],
					  sVECTOR3D *NeedAngle, float MaxDistance);
bool GetMissileOnTargetOrientateion(sVECTOR3D Location, sVECTOR3D CurrentObjectRotation, float (&RotationMatrix)[9],
				    cObject3D *TargetObject, sVECTOR3D *NeedAngle);
bool GetMissileTargetStatus(cObject3D *TargetObject, sVECTOR3D Location, float (&RotationMatrix)[9]);
// Получение положения ближайшего врага, для мин
cObject3D *GetCloserTargetPosition(int ObjectStatus, sVECTOR3D Location);

#endif // OBJECT3D_H
