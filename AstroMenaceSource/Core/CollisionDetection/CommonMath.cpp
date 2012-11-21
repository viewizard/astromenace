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


#include "CollisionDetection.h"







//-----------------------------------------------------------------------------
// проверка на попадание точки в треугольник
//-----------------------------------------------------------------------------
bool vw_PointInTriangle(VECTOR3D point, VECTOR3D pa, VECTOR3D pb, VECTOR3D pc)
{
	float TotalAngel = 0.0f;

	VECTOR3D V1 = point - pa;
	VECTOR3D V2 = point - pb;
	VECTOR3D V3 = point - pc;

	// сдесь применяем высокоточное вычисление, иначе снаряды могут "проскакивать"
	V1.NormalizeHi();
	V2.NormalizeHi();
	V3.NormalizeHi();

	TotalAngel += acosf(V1*V2);
	TotalAngel += acosf(V2*V3);
	TotalAngel += acosf(V3*V1);

	if (fabsf(TotalAngel - 2*3.14159265f) <= 0.005f) return true;

	return false;
}






