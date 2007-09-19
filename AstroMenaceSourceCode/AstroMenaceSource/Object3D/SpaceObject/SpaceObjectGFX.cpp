/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: SpaceObjectGFX.cpp

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
#include "../../Core/Core.h"





//-----------------------------------------------------------------------------
// Создание эффектов для космических объектов (двигатели для базы)
//-----------------------------------------------------------------------------
void SetSpaceObjectGFX(eParticleSystem *ParticleSystem, int GFXType)
{

	ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
	ParticleSystem->Direction = VECTOR3D(0.0f, -1.0f, 0.0f);

	switch(GFXType)
	{
		// двигатели на базе пиратов
		case 1:
			ParticleSystem->ColorStart.r = 0.60f;
			ParticleSystem->ColorStart.g = 0.60f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.30f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.30f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.60f;
			ParticleSystem->Speed      = 10.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->SpeedVar   = 2.00f;
			ParticleSystem->Theta      = 5.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(0.8f,0.1f,0.8f);
			ParticleSystem->AlphaShowHide= true;
			ParticleSystem->Direction = VECTOR3D(0.0f, -1.0f, 0.0f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.025f);
			break;
		case 2:
			ParticleSystem->ColorStart.r = 0.60f;
			ParticleSystem->ColorStart.g = 0.60f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.30f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.30f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->SpeedVar   = 2.00f;
			ParticleSystem->Theta      = 180.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->AlphaShowHide= true;
			ParticleSystem->Direction = VECTOR3D(0.0f, 1.0f, 0.0f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.3f);
			break;




	}




}

