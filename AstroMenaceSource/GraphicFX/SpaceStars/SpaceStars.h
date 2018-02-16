/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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


#ifndef SPACESTARS_H
#define SPACESTARS_H


#include "Star.h"




class CSpaceStars
{
public:
	CSpaceStars();
	~CSpaceStars();

	// обновить все частицы в этой системе, по времени
	bool Update(float Time);
	// прорисовка всех частиц
	void Draw();
	// базовая текстура частиц
	eTexture	*Texture;

	// работа с шейдерами
	eGLSL 	*GLSL;
	int		UniformLocations[10];

	// текущее положение частиц в пространстве
	VECTOR3D		Location;

	// размер частиц в мировых координатах
	float Size;


	VECTOR3D	CreationSize;


	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float	DeadZone;


	// возраст системы в секундах
	float Age;

	// последнее время обновления системы
	float TimeLastUpdate;


	// матрицы поворота, для оптимизации просчетов
	float	CurrentRotationMat[9];
	float	OldInvRotationMat[9];


	// подвязка к динамическому массиву
	CStar * Start;
	CStar * End;
	void Attach(CStar * NewParticle);
	void Detach(CStar * OldParticle);


	// данные, храним их, чтобы каждый раз не пересчитывать
	CStar **list;
	float *tmpDATA;
	unsigned int *VBO; // указатель на номер VBO массива
	int PrimitCount;
	float LastCameraAngleX, LastCameraAngleY, LastCameraAngleZ;
};










#endif //SPACESTARS_H
