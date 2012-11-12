/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: ProjectileGFX.cpp

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
#include "Projectile.h"





//-----------------------------------------------------------------------------
// Создание графического эффекта
//-----------------------------------------------------------------------------
void SetProjectileGFX(eParticleSystem *ParticleSystem, int GFXNum)
{

	ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");


	switch(GFXNum)
	{
		case 1:	// Kinetic
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 0.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.15f;
			ParticleSystem->ParticlesPerSec = 300;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
			break;
		case 2:	// Kinetic
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.15f);
			break;
		case 3:	// Kinetic
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.60f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.1f);
			break;
		case 4:	// Kinetic
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedVar   = 0.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.20f;
			ParticleSystem->ParticlesPerSec = 300;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.17f);
			break;
		case 5:	// Ion
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.70f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.20f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 60;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(2.0f,2.0f,0.5f);
			ParticleSystem->DeadZone = 1.5f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 25.0f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.1f);
			break;
		case 6:	// Ion
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.70f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 30;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -20.0f;
			break;
		case 7:	// Plasma
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.30f;
			ParticleSystem->Speed      = 6.40f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->NeedStop = true;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.15f);
			break;
		case 8:	// Plasma
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.80f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(1.3f,1.3f,0.2f);
			ParticleSystem->DeadZone = 1.2f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 2.0f;
			break;
		case 9:	// Plasma
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.80f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(1.3f,0.2f,1.3f);
			ParticleSystem->DeadZone = 1.2f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 2.0f;
			break;
		case 10:	// Plasma
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.80f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 4.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 30;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.10f);
			break;
		case 11:	// Plasma
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.60f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.20f;
			ParticleSystem->ParticlesPerSec = 150;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.08f);
			break;
		case 12:	// Ion
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.70f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.20f;
			ParticleSystem->ParticlesPerSec = 200;
			break;


		case 13:	// Missile
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.35f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.1f);
			break;
		case 14:	// Torpedo
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.35f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 1.0f, 0.15f, 0.0f, 0.075f);
			break;
		case 15:	// Nuke
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.15f, 0.35f, 1.0f, 0.0f, 0.05f);
			break;
		case 16:	// Swarm
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.25f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
			break;



		case 17:	// maser
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 600;
			ParticleSystem->CreationType = 11;
			ParticleSystem->CreationSize = VECTOR3D(0.4f,0.4f,0.2f);
			ParticleSystem->AlphaShowHide = true;
			break;
		case 18:	// maser2
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 400;
			ParticleSystem->CreationType = 11;
			ParticleSystem->CreationSize = VECTOR3D(0.8f,0.8f,0.2f);
			ParticleSystem->AlphaShowHide = true;
			break;
		case 19:	// Antimatter
			ParticleSystem->ColorStart.r = 0.50f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(0.8f,0.1f,0.8f);
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -20.0f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.5f, 1.0f, 0.0f, 0.0f, 0.05f);
			break;
		case 20:	// Laser
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.20f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.20f;
			ParticleSystem->ParticlesPerSec = 3000;
			ParticleSystem->CreationType = 11;
			ParticleSystem->CreationSize = VECTOR3D(0.2f,0.2f,0.1f);
			break;
		case 21:	// Antimatter
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 0.00f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.20f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.30f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 150;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(1.0f,1.0f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -40.0f;
			break;



		case 22: // Оружие пришельцев 1
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 0.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.15f;
			ParticleSystem->ParticlesPerSec = 300;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.2f);
			break;
		case 23:	// Оружие пришельцев 2
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.1f);
			break;
		case 24:	// Оружие пришельцев 2
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.60f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.1f);
			break;

		case 25:	// фларес
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 0.10f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 1.00f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(4.0f,4.0f,4.0f);
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.25f, 0.05f, 0.0f, 0.05f);
			break;
		case 26:	// стрельба 1-го пирата Kinetic
			ParticleSystem->ColorStart.r = 0.60f;
			ParticleSystem->ColorStart.g = 0.60f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.30f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.30f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 0.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.15f;
			ParticleSystem->ParticlesPerSec = 300;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.2f);
			break;
		case 27:	// стрельба 2-го пирата Missile
			ParticleSystem->ColorStart.r = 0.60f;
			ParticleSystem->ColorStart.g = 0.60f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.30f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.30f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.35f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
			break;


		case 28:	// 2-я мина
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
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
			ParticleSystem->Direction = VECTOR3D(0.0f, -1.0f, 0.0f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
			break;
		case 29:	// 3-я мина
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 3.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->SpeedVar   = 2.00f;
			ParticleSystem->Theta      = 60.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->AlphaShowHide= true;
			ParticleSystem->Direction = VECTOR3D(0.0f, -1.0f, 0.0f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.15f);
			break;
		case 30:	// 4-я мина
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
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
			ParticleSystem->ParticlesPerSec = 150;
			ParticleSystem->AlphaShowHide= true;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(1.5f,0.1f,1.5f);
			ParticleSystem->Direction = VECTOR3D(0.0f, -1.0f, 0.0f);
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.1f);
			break;
		case 31:	// Swarm для пиратов
			ParticleSystem->ColorStart.r = 0.60f;
			ParticleSystem->ColorStart.g = 0.60f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.30f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.30f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.25f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.2f);
			break;
		case 32:	// Torpedo пиратов
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.35f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 1.0f, 0.15f, 0.0f, 0.075f);
			break;
		case 33:	// Nuke пиратов
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 1.00f;
			ParticleSystem->SpeedVar   = 1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 250;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.15f, 0.35f, 1.0f, 0.0f, 0.05f);
			break;
		case 34:	// Kinetic2 пиратов
			ParticleSystem->ColorStart.r = 0.60f;
			ParticleSystem->ColorStart.g = 0.60f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.30f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.30f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.00f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.15f);
			break;
		case 35:	// Kinetic3 пиратов
			ParticleSystem->ColorStart.r = 0.60f;
			ParticleSystem->ColorStart.g = 0.60f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.30f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.30f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.60f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 2.00f;
			ParticleSystem->SpeedVar   = 3.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
			break;

		case 36:	// Оружие пришельцев, мина 1 тип
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.01f;
			ParticleSystem->SizeVar    = 0.02f;
			ParticleSystem->SizeEnd    = 0.40f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(2.0,2.0f,2.0f);
			ParticleSystem->DeadZone = 1.9f;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 25.0f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.1f);
			break;

		case 37:	// Оружие пришельцев, мина 2 тип
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.20f;
			ParticleSystem->ColorStart.b = 0.20f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.20f;
			ParticleSystem->ColorEnd.b = 0.20f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 1.00f;
			ParticleSystem->SizeStart  = 0.3f;
			ParticleSystem->SizeVar    = 0.02f;
			ParticleSystem->SizeEnd    = 0.1f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.40f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(2.0,0.2f,2.0f);
			ParticleSystem->DeadZone = 1.9f;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 2.0f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.2f, 0.2f, 0.0f, 0.1f);
			break;
		case 38:	// Оружие пришельцев, Laser
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.30f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.20f;
			ParticleSystem->ParticlesPerSec = 9000;
			ParticleSystem->CreationType = 11;
			ParticleSystem->CreationSize = VECTOR3D(0.2f,0.2f,0.1f);
			ParticleSystem->AlphaShowHide = true;
			break;










		case 101:	// шлейф ракеты землян и пиратов

			switch (vw_iRandNum(4))
			{
				case 1: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail2.tga"); break;
				case 2: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail3.tga"); break;
				case 3: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail4.tga"); break;
				case 4: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail5.tga"); break;
				default: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail1.tga"); break;
			}

			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.05f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.20f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 2.00f;
			ParticleSystem->Speed      = 0.20f;
			ParticleSystem->SpeedVar   = 0.30f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 5.00f;
			ParticleSystem->LifeVar    = 5.00f;
			ParticleSystem->ParticlesPerSec = 150;
			ParticleSystem->BlendType = 1;
			break;

		case 102:	// шлейф ракет пришельцев

			switch (vw_iRandNum(4))
			{
				case 1: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail2.tga"); break;
				case 2: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail3.tga"); break;
				case 3: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail4.tga"); break;
				case 4: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail5.tga"); break;
				default: ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/trail1.tga"); break;
			}

			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.07f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.20f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 2.00f;
			ParticleSystem->Speed      = 0.20f;
			ParticleSystem->SpeedVar   = 0.30f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 5.00f;
			ParticleSystem->LifeVar    = 5.00f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->BlendType = 1;
			break;

	}
}

