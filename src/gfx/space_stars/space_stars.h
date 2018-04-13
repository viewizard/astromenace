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

#ifndef SPACESTARS_H
#define SPACESTARS_H

#include "star.h"


class cSpaceStars
{
public:
	cSpaceStars();
	~cSpaceStars();

	// обновить все частицы в этой системе, по времени
	bool	Update(float Time);
	// прорисовка всех частиц
	void	Draw();
	// базовая текстура частиц
	GLtexture Texture{0};

	// работа с шейдерами
	sGLSL 	*GLSL{nullptr};
	GLint UniformLocations[10];

	// текущее положение частиц в пространстве
	sVECTOR3D	Location{0.0f, 0.0f, 0.0f};

	// размер частиц в мировых координатах
	float		Size{0.003f};

	sVECTOR3D	CreationSize{5.21f,5.21f,5.21f};

	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float	DeadZone{5.2f};

	// возраст системы в секундах
	float	Age{0.0f};

	// последнее время обновления системы
	float	TimeLastUpdate{-1.0f};

	// матрицы поворота, для оптимизации просчетов
	float	CurrentRotationMat[9]{1.0f, 0.0f, 0.0f,
				      0.0f, 1.0f, 0.0f,
				      0.0f, 0.0f, 1.0f};
	float	OldInvRotationMat[9]{1.0f, 0.0f, 0.0f,
				     0.0f, 1.0f, 0.0f,
				     0.0f, 0.0f, 1.0f};

	// подвязка к динамическому массиву
	sStar	*Start{nullptr};
	sStar	*End{nullptr};
	void	Attach(sStar *NewParticle);
	void	Detach(sStar *OldParticle);

	// данные, храним их, чтобы каждый раз не пересчитывать
	sStar	**list{nullptr};
	float	*tmpDATA{nullptr};
	unsigned int VBO{0};
	int	PrimitCount{0};
	float	LastCameraAngleX{0.0f}, LastCameraAngleY{0.0f}, LastCameraAngleZ{0.0f};
};

#endif //SPACESTARS_H
