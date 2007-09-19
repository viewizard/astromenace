/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: WeaponFunctions.cpp

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
#include "Weapon.h"






//-----------------------------------------------------------------------------
// Установка нужных данных для вспышки возле ствола
//-----------------------------------------------------------------------------
void SetWeaponFire(eParticleSystem *ParticleSystem, int WeaponNum)
{
	switch (WeaponNum)
	{
		case 1:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.4f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
			break;
		case 2:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 0.10f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 40.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
			break;
		case 3:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 0.10f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.60f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 50.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
			break;
		case 4:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
			break;
		case 5:
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.70f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 4.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 10.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
			break;
		case 6:
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.70f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.20f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 20.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
			break;
		case 7:
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.70f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.20f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 6.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 10.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
			break;
		case 8:
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.2f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.2f;
			ParticleSystem->Speed      = 4.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->CreationType       = 1;
			ParticleSystem->CreationSize       = VECTOR3D(0.4f,0.4f,0.4f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
			break;
		case 9:
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.15f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.15f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->CreationType       = 1;
			ParticleSystem->CreationSize       = VECTOR3D(0.3f,0.3f,0.3f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
			break;
		case 10:
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.30f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->CreationType       = 1;
			ParticleSystem->CreationSize       = VECTOR3D(0.3f,0.3f,0.3f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
			break;



		case 11:
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.60f;
			ParticleSystem->SizeStart  = 0.1f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->ParticlesPerSec = 20;
			ParticleSystem->CreationType       = 1;
			ParticleSystem->CreationSize       = VECTOR3D(0.3f,0.2f,0.3f);
			break;
		case 12:
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.60f;
			ParticleSystem->SizeStart  = 0.1f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->ParticlesPerSec = 20;
			ParticleSystem->CreationType       = 1;
			ParticleSystem->CreationSize       = VECTOR3D(0.3f,0.2f,0.3f);
			break;
		case 13:
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.4f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.7f, 1.0f, 0.0f, 0.0f, 0.02f);
			break;
		case 14:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.10f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.20f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->CreationType       = 1;
			ParticleSystem->CreationSize       = VECTOR3D(0.2f,0.2f,0.8f);
			break;
		case 15:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.4f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.0f, 0.0f, 0.02f);
			break;
		case 16:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.4f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.40f;
			break;
		case 17:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.4f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.40f;
			break;
		case 18:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.4f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.40f;
			break;
		case 19:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.4f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.40f;
			break;
	}


	ParticleSystem->IsSuppressed = true;
	ParticleSystem->ParticlesPerSec = 50;
	ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");

}











