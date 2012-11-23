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
extern CSpaceObject *StartSpaceObject;
extern eParticleSystem *psSpace;

//-----------------------------------------------------------------------------
// local/protected variables
//-----------------------------------------------------------------------------

// StarSystem initialization status
bool	StarSystem_InitedAll = false;
bool	StarSystem_Inited = false;
// StarSystem rotation
VECTOR3D StarSystem_BaseRotation(0.0f,0.0f,0.0f);

// для прорисовки подложки с тайловой анимацией
float StarsTile=0.0f;
float StarsTileUpdateTime = 0.0f;
float StarsTile2=0.0f;
float StarsTileUpdateTime2 = 0.0f;
float StarsTileStartTransparentLayer1 = 0.0f;
float StarsTileEndTransparentLayer1 = 0.0f;
float StarsTileStartTransparentLayer2 = 0.0f;
float StarsTileEndTransparentLayer2 = 0.0f;




//------------------------------------------------------------------------------------
// StarSystem initialiation
//------------------------------------------------------------------------------------
void StarSystemInit(int Num, VECTOR3D SetBaseRotation)
{
	// SkyBox setup
	switch (Num)
	{
		case 1:
			SkyBoxCreate(0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 100.0f);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_back6.tga"), 4);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_bottom4.tga"), 2);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_front5.tga"), 5);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_left2.tga"), 1);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_right1.tga"), 0);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/1/skybox_top3.tga"), 3);
			StarSystem_Inited = true;
			break;
		case 2:
			SkyBoxCreate(0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 100.0f);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_back6.tga"), 4);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_bottom4.tga"), 2);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_front5.tga"), 5);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_left2.tga"), 1);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_right1.tga"), 0);
			SkyBoxSetTexture(vw_FindTextureByName("DATA/SKYBOX/2/skybox_top3.tga"), 3);
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
void StarSystemDraw(int DrawType)
{
	if (!StarSystem_InitedAll) return;

	// current camera location
	VECTOR3D CurrentCameraLocation;
	vw_GetCameraLocation(&CurrentCameraLocation);

	vw_DepthTest(false, -1);

	if (StarSystem_Inited)
	{
		// SkyBox
		vw_PushMatrix();
		vw_Translate(CurrentCameraLocation);
		vw_Rotate(StarSystem_BaseRotation.x, 1.0f, 0.0f, 0.0f);
		vw_Rotate(StarSystem_BaseRotation.y, 0.0f, 1.0f, 0.0f);
		vw_Rotate(StarSystem_BaseRotation.z, 0.0f, 0.0f, 1.0f);
		SkyBoxDraw();
		vw_PopMatrix();
	}

	// static space stars
	if (psSpaceStatic!=0) psSpaceStatic->Draw();

	vw_DepthTest(true, RI_LESSEQUAL);





	// космические объекты
	// рисуем планеты и большие астероиды _до_ тайловой анимации
	CSpaceObject *tmp1 = StartSpaceObject;
	while (tmp1!=0)
	{
		CSpaceObject *tmp2 = tmp1->Next;

		// если это планета на заднем фоне
		if (tmp1->ObjectType == 14)
		{
			if (DrawType == 2)
			{
				vw_PushMatrix();
				vw_Translate(VECTOR3D(CurrentCameraLocation.x*0.90f-GameCameraGetDeviation()*4.0f, GameCameraGetDeviation()*2.0f,0.0f));
			}
			tmp1->Draw(false);
			if (DrawType == 2) vw_PopMatrix();
		}
		else
		// если это большой астероид летящий на заднем фоне
		if (tmp1->ObjectType == 15 && (tmp1->ObjectCreationType>10 && tmp1->ObjectCreationType<20))
		{
			if (DrawType == 2)
			{
				vw_PushMatrix();
				vw_Translate(VECTOR3D(CurrentCameraLocation.x*0.70f-GameCameraGetDeviation()*4.0f, GameCameraGetDeviation()*2.0f,0.0f));
			}
			tmp1->Draw(false);
			if (DrawType == 2) vw_PopMatrix();
		}

		tmp1 = tmp2;
	}
	// очищаем буфер глубины, чтобы "3д подложка" не участвовала в проверке глубины
	vw_Clear(RI_DEPTH_BUFFER);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Прорисовка подложки с тайловой анимацией
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		int VFV = RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX;

		float *buff = 0;
		buff = new float[4*9]; if (buff == 0) return;

		float width_2, heigh_2, length_2;
		width_2 = 0.0f;
		heigh_2 = 110.0f;
		length_2 = 110.0f;
		float x,y,z;
		x = GamePoint.x;
		y = GamePoint.y;
		z = GamePoint.z;
		float StartTransparentLayer1 = 0.7f;
		float EndTransparentLayer1 = 0.7f;


		if (DrawType == 2)
		{
			width_2 = length_2 = 175.0f;
			heigh_2 = 0.0f;
			// чем ниже слой, тем меньше его двигаем при перемещении камеры (при стандартном аспект рейшен)
			x = GamePoint.x+GameCameraGetDeviation() + CurrentCameraLocation.x*0.8f;
			y = GamePoint.y-GameCameraGetDeviation()*0.5f;
			z = GamePoint.z+25.0f;

			StartTransparentLayer1 = StarsTileStartTransparentLayer1;
			EndTransparentLayer1 = StarsTileEndTransparentLayer1;
		}


		int k=0;

		buff[k++] = x + width_2;
		buff[k++] = y + heigh_2;
		buff[k++] = z + length_2+length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = StartTransparentLayer1;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f+StarsTile/3.0f;

		buff[k++] = x + width_2;
		buff[k++] = y + heigh_2;
		buff[k++] = z - length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = EndTransparentLayer1;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f+StarsTile/3.0f;

		buff[k++] = x - width_2;
		buff[k++] = y - heigh_2;
		buff[k++] = z + length_2+length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = StartTransparentLayer1;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f+StarsTile/3.0f;

		buff[k++] = x - width_2;
		buff[k++] = y - heigh_2;
		buff[k++] = z - length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = EndTransparentLayer1;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f+StarsTile/3.0f;


		if (DrawType == 1)
		{
			StarsTile -= 0.005f*(vw_GetTime() - StarsTileUpdateTime);
			StarsTileUpdateTime = vw_GetTime();
		}
		else
		{
			StarsTile -= 0.035f*(vw_GetTime(1) - StarsTileUpdateTime);
			StarsTileUpdateTime = vw_GetTime(1);
		}

		if (StarsTile < -3.0f) StarsTile += 3.0f;


		eTexture *TileTexture = vw_FindTextureByName("DATA/SKYBOX/tile_back.tga");
		vw_SetTexture(0, TileTexture);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		// по умолчанию всегда трилинейная фильтрация, если надо - ставим билинейную
		if (Setup.TextureFilteringMode == 1) vw_SetTextureFiltering(RI_TEXTURE_BILINEAR);

		vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);

		vw_DepthTest(false, -1);

		if (DrawType == 1)
		{
			vw_PushMatrix();
			vw_Rotate(-20.0f, 0.0f, 0.0f, 1.0f);
			vw_Rotate(-45.0f, 0.0f, 1.0f, 0.0f);
			vw_Rotate(30.0f, 1.0f, 0.0f, 0.0f);
		}

		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 9*sizeof(float));



		// звезды рисуем отдельно, четкими (со своими текстурными координатами)

		k=0;

		buff[k++] = x + width_2;
		buff[k++] = y + heigh_2;
		buff[k++] = z + length_2+length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = StartTransparentLayer1;
		buff[k++] = 3.0f;
		buff[k++] = 0.0f+StarsTile;

		buff[k++] = x + width_2;
		buff[k++] = y + heigh_2;
		buff[k++] = z - length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = EndTransparentLayer1;
		buff[k++] = 3.0f;
		buff[k++] = 3.0f+StarsTile;

		buff[k++] = x - width_2;
		buff[k++] = y - heigh_2;
		buff[k++] = z + length_2+length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = StartTransparentLayer1;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f+StarsTile;

		buff[k++] = x - width_2;
		buff[k++] = y - heigh_2;
		buff[k++] = z - length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = EndTransparentLayer1;
		buff[k++] = 0.0f;
		buff[k++] = 3.0f+StarsTile;

		vw_SetTexture(0, vw_FindTextureByName("DATA/SKYBOX/tile_stars.tga"));

		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 9*sizeof(float));




		if (DrawType == 1) vw_PopMatrix();

		vw_DepthTest(true, RI_LESSEQUAL);

		vw_SetTextureBlend(false, 0, 0);
		vw_BindTexture(0, 0);
		if (buff != 0){delete [] buff; buff = 0;}
	}



	// корректируем положение частиц "космической пыли", если в игре и камера движется
	if (DrawType == 2)
	{
		VECTOR3D TMPpsSpace;
		psSpace->GetLocation(&TMPpsSpace);
		psSpace->SetStartLocation(TMPpsSpace);
		psSpace->MoveSystemLocation(VECTOR3D(0,10,250)+GamePoint);
	}
}




//------------------------------------------------------------------------------------
// Прорисовка второго слоя "пыли" с тайловой анимацией
//------------------------------------------------------------------------------------
void StarSystemDrawSecondLayer(int DrawType)
{
	if (Setup.VisualEffectsQuality <= 1)
	{
		int VFV = RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX;

		float *buff;
		buff = new float[4*9]; if (buff == 0) return;

		float width_2, heigh_2, length_2;
		width_2 = 0.0f;
		heigh_2 = 110.0f;
		length_2 = 110.0f;
		float x,y,z;
		x = GamePoint.x;
		y = GamePoint.y;
		z = GamePoint.z;
		float StartTransparentLayer2 = 0.9f;
		float EndTransparentLayer2 = 0.7f;

		if (DrawType == 2)
		{
			width_2 = length_2 = 175.0f;
			heigh_2 = 0.0f;

			VECTOR3D CurrentCameraLocation;
			vw_GetCameraLocation(&CurrentCameraLocation);

			x = GamePoint.x+GameCameraGetDeviation()*2.0f + CurrentCameraLocation.x*0.5f;
			y = GamePoint.y-GameCameraGetDeviation();
			z = GamePoint.z+25.0f;

			StartTransparentLayer2 = StarsTileStartTransparentLayer2;
			EndTransparentLayer2 = StarsTileEndTransparentLayer2;
		}


		int k=0;

		buff[k++] = x + width_2;
		buff[k++] = y + heigh_2;
		buff[k++] = z + length_2+length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = StartTransparentLayer2;
		buff[k++] = 3.2f;
		buff[k++] = 0.0f+StarsTile2;

		buff[k++] = x + width_2;
		buff[k++] = y + heigh_2;
		buff[k++] = z - length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = EndTransparentLayer2;
		buff[k++] = 3.2f;
		buff[k++] = 3.0f+StarsTile2;

		buff[k++] = x - width_2;
		buff[k++] = y - heigh_2;
		buff[k++] = z + length_2+length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = StartTransparentLayer2;
		buff[k++] = 0.2f;
		buff[k++] = 0.0f+StarsTile2;

		buff[k++] = x - width_2;
		buff[k++] = y - heigh_2;
		buff[k++] = z - length_2/2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = EndTransparentLayer2;
		buff[k++] = 0.2f;
		buff[k++] = 3.0f+StarsTile2;

		if (DrawType == 1)
		{
			StarsTile2 -= 0.03f*(vw_GetTime() - StarsTileUpdateTime2);
			StarsTileUpdateTime2 = vw_GetTime();
		}
		else
		{
			StarsTile2 -= 0.12f*(vw_GetTime(1) - StarsTileUpdateTime2);
			StarsTileUpdateTime2 = vw_GetTime(1);
		}
		if (StarsTile2 < -3.0f) StarsTile2 += 3.0f;


		eTexture *TileTexture = vw_FindTextureByName("DATA/SKYBOX/tile_stars.tga");
		vw_SetTexture(0, TileTexture);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		// по умолчанию всегда трилинейная фильтрация, если надо - ставим билинейную
		if (Setup.TextureFilteringMode == 1) vw_SetTextureFiltering(RI_TEXTURE_BILINEAR);


		vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);

		vw_DepthTest(false, -1);

		if (DrawType == 1)
		{
			vw_PushMatrix();
			vw_Rotate(-20.0f, 0.0f, 0.0f, 1.0f);
			vw_Rotate(-45.0f, 0.0f, 1.0f, 0.0f);
			vw_Rotate(30.0f, 1.0f, 0.0f, 0.0f);
		}


		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 9*sizeof(float));

		if (DrawType == 1) vw_PopMatrix();

		vw_DepthTest(true, RI_LESSEQUAL);
		vw_SetTextureBlend(false, 0, 0);
		vw_BindTexture(0, 0);
		if (buff != 0){delete [] buff; buff = 0;}
	}
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
