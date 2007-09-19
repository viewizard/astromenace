/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: StarSystem.cpp

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


#include "../Game.h"



//-----------------------------------------------------------------------------
// local/protected variables
//-----------------------------------------------------------------------------

// StarSystem initialization status
bool	StarSystem_InitedAll = false;
bool	StarSystem_Inited = false;
// StarSystem rotation
VECTOR3D StarSystem_BaseRotation(0.0f,0.0f,0.0f);






//------------------------------------------------------------------------------------
// StarSystem initialiation
//------------------------------------------------------------------------------------
void StarSystemInit(int Num, VECTOR3D SetBaseRotation)
{
	// SkyBox setup
	switch (Num)
	{
		case 1:
			SkyBoxCreate(0.0f, 0.0f, 0.0f, 7000.0f, 7000.0f, 7000.0f);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_BK.jpg"), 4);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_DN.jpg"), 2);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_FR.jpg"), 5);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_LF.jpg"), 1);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_RT.jpg"), 0);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_UP.jpg"), 3);
			StarSystem_Inited = true;
			break;
		case 2:
			SkyBoxCreate(0.0f, 0.0f, 0.0f, 7000.0f, 7000.0f, 7000.0f);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_BK.jpg"), 4);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_DN.jpg"), 2);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_FR.jpg"), 5);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_LF.jpg"), 1);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_RT.jpg"), 0);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_UP.jpg"), 3);
			StarSystem_Inited = true;
			break;
		case 3:
			SkyBoxCreate(0.0f, 0.0f, 0.0f, 7000.0f, 7000.0f, 7000.0f);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/3/skybox_BK.jpg"), 4);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/3/skybox_DN.jpg"), 2);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/3/skybox_FR.jpg"), 5);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/3/skybox_LF.jpg"), 1);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/3/skybox_RT.jpg"), 0);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/3/skybox_UP.jpg"), 3);
			StarSystem_Inited = true;
			break;
	}


	// StarSystem setup
	StarSystem_InitedAll = true;
	StarSystem_BaseRotation = SetBaseRotation;

	// static space stars initialization
	if (psSpaceStatic != 0){delete psSpaceStatic; psSpaceStatic = 0;}
	psSpaceStatic = new CSpaceStars; if (psSpaceStatic == 0) return;
}




//------------------------------------------------------------------------------------
// StarSystem data release function
//------------------------------------------------------------------------------------
void StarSystemRelease()
{
	for (int i=0; i<6; i++)
		SkyBoxSetTexture(0, i);

	StarSystem_Inited = false;

	// static space stars
	if (psSpaceStatic != 0){delete psSpaceStatic; psSpaceStatic = 0;}
}







//------------------------------------------------------------------------------------
// StarSystem draw function
//------------------------------------------------------------------------------------
void StarSystemDraw()
{
	if (!StarSystem_InitedAll) return;

	// current camera location
	VECTOR3D CurrentCameraLocation;
	vw_GetCameraLocation(&CurrentCameraLocation);


	if (StarSystem_Inited)
	{
		// SkyBox
		vw_PushMatrix();
		vw_Translate(CurrentCameraLocation);
		vw_Rotate(StarSystem_BaseRotation.x, StarSystem_BaseRotation.y, StarSystem_BaseRotation.z);
		SkyBoxDraw();
		vw_PopMatrix();
	}

	// static space stars
	if (psSpaceStatic!=0) psSpaceStatic->Draw();
}




//------------------------------------------------------------------------------------
// Update for all StarSystem parts
//------------------------------------------------------------------------------------
void StarSystemUpdate()
{
	if (!StarSystem_InitedAll) return;

	// static space stars
	if (psSpaceStatic!=0) psSpaceStatic->Update(vw_GetTime());
}
