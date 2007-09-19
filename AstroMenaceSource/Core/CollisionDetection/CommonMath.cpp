/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: CommonMath.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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






