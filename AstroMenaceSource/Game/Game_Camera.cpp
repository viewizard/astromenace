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


#include "../Game.h"




//-----------------------------------------------------------------------------
// локальные/защищенные переменные
//-----------------------------------------------------------------------------

// время последнего обновления камеры
float		GameCameraLastUpdate = 0.0f;
// скорость движения камеры
float		GameCameraSpeed = 10.0f;
// угол поворота камеры над плоскостью с точкой GamePoint
float		GameCameraAngle = 0.0f;
// расстояние от точки GamePoint до положения камеры
float		GameCameraDistance = 0.0f;
// болтание камеры при взрывах
float	GameCameraDeviation = 0.0f;
float 	GameCameraDeviationTime = 0.0f;
float 	GameCameraNeedDeviation = 0.0f;
float 	GameCameraDeviationPower = 0.0f;
float   GameCameraNeedStartDeviation = 0.0f;
float   GameCameraDeviationAge = 0.0f;








//-----------------------------------------------------------------------------
// инициализация переменных камеры для игры
//-----------------------------------------------------------------------------
void InitGameCamera()
{
	GamePoint = VECTOR3D(0.0f, 0.0f, 0.0f);
	GameCameraLastUpdate = vw_GetTime(1);

	GameCameraDeviation = 0.0f;
	GameCameraDeviationTime = 0.0f;
	GameCameraNeedDeviation = 0.0f;
	GameCameraDeviationPower = 0.0f;
    GameCameraNeedStartDeviation = 0.0f;
    GameCameraDeviationAge = 0.0f;
    vw_SetCameraDeviation(VECTOR3D(0,0,0));
}







//-----------------------------------------------------------------------------
// установка взрыва
//-----------------------------------------------------------------------------
void GameCameraSetExplosion(VECTOR3D Location, float Power)
{
    // если корабля нет, нам тут делать нечего
    if (PlayerFighter == 0) return;

    // вычисляем по дистанции время болтанки....
    // чуствительность начинается со 100 едениц (10000 в квадрате)

    float dist2 = (PlayerFighter->Location.x - Location.x)*(PlayerFighter->Location.x - Location.x)+
                (PlayerFighter->Location.y - Location.y)*(PlayerFighter->Location.y - Location.y)+
                (PlayerFighter->Location.z - Location.z)*(PlayerFighter->Location.z - Location.z);


	// слишком далеко
	if (dist2 > 10000.0f) return;
	// или очень близко
	if (dist2 <= PlayerFighter->Radius*PlayerFighter->Radius)
	{
		dist2 = PlayerFighter->Radius*PlayerFighter->Radius;
	}


	// время болтанки
	GameCameraDeviationAge = GameCameraDeviationTime = (10000.0f - dist2)/10000.0f;

	if (Power>1.0f) // очень большой взрыв
		GameCameraDeviationAge = GameCameraDeviationTime = GameCameraDeviationTime*3.0f;

	GameCameraDeviationPower = Power*(10000.0f- dist2)/25000.0f;


	GameCameraNeedStartDeviation = GameCameraNeedDeviation = GameCameraDeviationPower*vw_Randf0;

}









//-----------------------------------------------------------------------------
// обновление камеры в игре, учитываем (полет, поворот и т.д.)
//-----------------------------------------------------------------------------
void GameCameraUpdate(float Time)
{
	float TimeDelta = Time - GameCameraLastUpdate;
	GameCameraLastUpdate = Time;

	VECTOR3D TmpNeedPos = GameCameraMovement^(GameCameraSpeed*TimeDelta);

	// обновляем данные камеры (+ устанавливаем флаг, чтобы обновить фруструм)
	vw_IncCameraLocation(TmpNeedPos);

	// обновляем точку под камерой (минимальную)
	GamePoint += TmpNeedPos;


	GameCameraDeviationTime -= TimeDelta;
	if (GameCameraDeviationTime < 0.0f) GameCameraDeviationTime = 0.0f;


	if (GameCameraDeviationTime < GameCameraDeviationTime && GameCameraDeviationAge != 0.0f)
        GameCameraNeedDeviation = GameCameraNeedStartDeviation -  GameCameraNeedStartDeviation/((GameCameraDeviationAge-GameCameraDeviationTime)/GameCameraDeviationAge);

	// просчет девиации камеры
	if (GameCameraDeviationTime > 0.0f)
	{
		float Sign = 1.0f;
		// нужно двигать
		if (GameCameraNeedDeviation < 0.0f) Sign = -1.0f;
		if (Sign == 1.0f)
		{if (GameCameraNeedDeviation < GameCameraDeviation) Sign = -1.0f;}
		else
		{if (GameCameraNeedDeviation > GameCameraDeviation) Sign = 1.0f;}


		float CurrentDeviation = Sign*5.0f*TimeDelta;

		if (Sign == 1.0f)
		{
			if (GameCameraNeedDeviation <= GameCameraDeviation+CurrentDeviation)
			{
				GameCameraDeviation = GameCameraNeedDeviation;
				GameCameraNeedStartDeviation = GameCameraNeedDeviation = GameCameraDeviationPower*vw_Randf0;
			}
			else GameCameraDeviation += CurrentDeviation;
		}
		else
		{
			if (GameCameraNeedDeviation >= GameCameraDeviation+CurrentDeviation)
			{
				GameCameraDeviation = GameCameraNeedDeviation;
				GameCameraNeedStartDeviation = GameCameraNeedDeviation = GameCameraDeviationPower*vw_Randf0;
			}
			else GameCameraDeviation += CurrentDeviation;
		}
	}
	else // нужно остановить ровно, уже не надо болтать
		if (GameCameraDeviation != 0.0f)
		{
			GameCameraNeedStartDeviation = GameCameraNeedDeviation = 0.0f;
			float Sign = 1.0f;
			if (GameCameraNeedDeviation < GameCameraDeviation) Sign = -1.0f;
			if (GameCameraNeedDeviation > GameCameraDeviation) Sign = 1.0f;

			float CurrentDeviation = Sign*5.0f*TimeDelta;

			if (Sign == 1.0f)
			{
				if (GameCameraNeedDeviation <= GameCameraDeviation+CurrentDeviation)
				{
					GameCameraDeviation = GameCameraNeedDeviation;
				}
				else GameCameraDeviation += CurrentDeviation;
			}
			else
			{
				if (GameCameraNeedDeviation >= GameCameraDeviation+CurrentDeviation)
				{
					GameCameraDeviation = GameCameraNeedDeviation;
				}
				else GameCameraDeviation += CurrentDeviation;
			}
		}


	vw_SetCameraDeviation(VECTOR3D(GameCameraDeviation,GameCameraDeviation/(-2.0f),0.0f));


	// если поворачиваем
	/*
	GameCameraMovement - меняем угол


	GamePoint - делаем учет
!!! - учесть поворот в скрипте!!!void SetRotation(CObject3D *Object, TiXmlElement *Element)
	*/


	// делаем действия над кораблем игрока, если он есть
	if (PlayerFighter != 0)
	{
		// нужно сместить корабль на расстояние
		PlayerFighter->SetLocationArcadePlayer(PlayerFighter->Location+TmpNeedPos);
	}

}







//-----------------------------------------------------------------------------
// передача параметров
//-----------------------------------------------------------------------------
float GameCameraGetDeviation(){return GameCameraDeviation;}
float GameCameraGetSpeed(){return GameCameraSpeed;}


