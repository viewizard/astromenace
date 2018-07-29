/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO add initialization via XML file, hard coded initialization should be removed
// TODO remove GetPreloadedTextureAsset() call from main loop

// TODO probably, we could use GL_EXT_draw_instanced here in future
//      render 'space dust' instanced (send position and size in texture), instead of render 3 layer

/*
In order to show movement and more 'live' space, star system render "space dust" with 3 tile-animated layers:
1. Small and far space dust, looks more like 'clouds'. This layer rendered first, before all objects in the foreground.
2. Far 'big' space dust. This layer rendered second with higher speed, before all objects in the foreground.
3. Close space dust. This layer rendered with highest speed, after all 3D objects.
*/

#include "../enum.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../object3d/space_object/space_object.h"
#include "skybox.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// StarSystem initialization status
bool StarSystem_InitedAll{false};
bool StarSystem_Inited{false};
// StarSystem rotation
sVECTOR3D StarSystem_BaseRotation(0.0f, 0.0f, 0.0f);

// particle system for space dust
std::weak_ptr<cParticleSystem> psSpace;

// tile animation for space dust (2 layers)
float StarsTile{0.0f};
float StarsTile2{0.0f};
float StarsTileUpdateTime{0.0f};
float StarsTileUpdateTime2{0.0f};
float StarsTileStartTransparentLayer1{0.0f};
float StarsTileEndTransparentLayer1{0.0f};
float StarsTileStartTransparentLayer2{0.0f};
float StarsTileEndTransparentLayer2{0.0f};

sVECTOR3D InGameInitialLocation{0, 10, 250};

// vertex array for rendering
float VertexArray[36]; // 4 * 9 = 4 vertices * (RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX);
unsigned int VertexArrayPosition{0};

} // unnamed namespace

// FIXME should be fixed, don't allow global scope interaction for local variables
extern sVECTOR3D GamePoint;
// FIXME should be fixed, use 'include' instead
float GameCameraGetDeviation();


/*
 * Star system initialization.
 */
void StarSystemInit(int Num, sVECTOR3D SetBaseRotation)
{
	// SkyBox setup
	switch (Num) {
	case 1:
		SkyBoxCreate(0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 100.0f);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/1/skybox_back6.tga"), eSide::BACK);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/1/skybox_bottom4.tga"), eSide::BOTTOM);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/1/skybox_front5.tga"), eSide::FRONT);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/1/skybox_left2.tga"), eSide::LEFT);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/1/skybox_right1.tga"), eSide::RIGHT);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/1/skybox_top3.tga"), eSide::TOP);
		StarSystem_Inited = true;
		break;
	case 2:
		SkyBoxCreate(0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 100.0f);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/2/skybox_back6.tga"), eSide::BACK);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/2/skybox_bottom4.tga"), eSide::BOTTOM);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/2/skybox_front5.tga"), eSide::FRONT);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/2/skybox_left2.tga"), eSide::LEFT);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/2/skybox_right1.tga"), eSide::RIGHT);
		SkyBoxSetTexture(GetPreloadedTextureAsset("skybox/2/skybox_top3.tga"), eSide::TOP);
		StarSystem_Inited = true;
		break;
	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	// StarSystem setup
	StarSystem_InitedAll = true;
	StarSystem_BaseRotation = SetBaseRotation;
}

/*
 * Star system initialization by game's part type (menu/game).
 */
void StarSystemInitByType(eDrawType DrawType)
{
	StarsTileStartTransparentLayer1 = 0.2f;
	StarsTileEndTransparentLayer1 = 0.7f;
	StarsTileStartTransparentLayer2 = 0.9f;
	StarsTileEndTransparentLayer2 = 0.7f;

	switch (DrawType) {
	case eDrawType::MENU:
		StarsTileUpdateTime = vw_GetTimeThread(0);
		StarsTileUpdateTime2 = vw_GetTimeThread(0);
		psSpace = vw_CreateParticleSystem();
		if (auto sharedSpace = psSpace.lock()) {
			sharedSpace->ColorStart.r = 0.80f;
			sharedSpace->ColorStart.g = 0.80f;
			sharedSpace->ColorStart.b = 1.00f;
			sharedSpace->ColorEnd.r = 0.70f;
			sharedSpace->ColorEnd.g = 0.70f;
			sharedSpace->ColorEnd.b = 1.00f;
			sharedSpace->AlphaStart = 1.00f;
			sharedSpace->AlphaEnd   = 0.00f;
			sharedSpace->SizeStart = 0.10f;
			sharedSpace->SizeVar = 0.05f;
			sharedSpace->SizeEnd = 0.30f;
			sharedSpace->Speed = 4.00f;
			sharedSpace->SpeedVar = 8.00f;
			sharedSpace->Theta = 0.00f;
			sharedSpace->Life = 10.00f;
			sharedSpace->LifeVar = 0.00f;
			sharedSpace->CreationType = eParticleCreationType::Cube;
			sharedSpace->CreationSize = sVECTOR3D{2.0f, 50.0f, 30.0f};
			sharedSpace->ParticlesPerSec = 140;
			sharedSpace->Texture = GetPreloadedTextureAsset("gfx/flare3.tga");
			sharedSpace->Direction = sVECTOR3D{1.0f, 0.0f, 0.0f};
			sharedSpace->CameraDistResize = 0.1f;
			sharedSpace->SetStartLocation(sVECTOR3D{-50.0f, 10.0f, -20.0f});

			// emulate time flow, particles should fill the screen
			float Time = sharedSpace->TimeLastUpdate;
			for (float i = Time; i < (Time + 20); i += 1.0f) {
				sharedSpace->Update(i);
			}
			sharedSpace->TimeLastUpdate = Time;
		}
	break;
	case eDrawType::GAME:
		StarsTileUpdateTime = vw_GetTimeThread(1);
		StarsTileUpdateTime2 = vw_GetTimeThread(1);
		psSpace = vw_CreateParticleSystem();
		if (auto sharedSpace = psSpace.lock()) {
			sharedSpace->ColorStart.r = 0.80f;
			sharedSpace->ColorStart.g = 0.80f;
			sharedSpace->ColorStart.b = 1.00f;
			sharedSpace->ColorEnd.r = 0.70f;
			sharedSpace->ColorEnd.g = 0.70f;
			sharedSpace->ColorEnd.b = 1.00f;
			sharedSpace->AlphaStart = 0.50f;
			sharedSpace->AlphaEnd = 1.00f;
			sharedSpace->SizeStart = 0.40f;
			sharedSpace->SizeEnd = 0.05f;
			sharedSpace->Speed = 25.00f;
			sharedSpace->SpeedVar = 5.00f;
			sharedSpace->Theta = 0.00f;
			sharedSpace->Life = 14.00f;
			sharedSpace->LifeVar = 0.00f;
			sharedSpace->CreationType = eParticleCreationType::Cube;
			sharedSpace->CreationSize = sVECTOR3D{200.0f, 30.0f, 10.0f};
			sharedSpace->ParticlesPerSec = 100;
			sharedSpace->Texture = GetPreloadedTextureAsset("gfx/flare3.tga");
			sharedSpace->Direction = sVECTOR3D{0.0f, 0.0f, -1.0f};
			sharedSpace->SetStartLocation(InGameInitialLocation);

			// emulate time flow, particles should fill the screen
			float Time = sharedSpace->TimeLastUpdate;
			for (float i = Time; i < (Time + 25); i += 1.0f) {
				sharedSpace->Update(i);
			}
			sharedSpace->TimeLastUpdate = Time;
		}
	break;
	}
}

/*
 * Setup first and second layers transparency.
 */
void StarSystemLayer1Transp(float Start, float End)
{
	StarsTileStartTransparentLayer1 = Start;
	StarsTileEndTransparentLayer1 = End;
}

/*
 * Setup third layer transparency.
 */
void StarSystemLayer3Transp(float Start, float End)
{
	StarsTileStartTransparentLayer2 = Start;
	StarsTileEndTransparentLayer2 = End;
}

/*
 * Release star system.
 */
void StarSystemRelease()
{
	for (unsigned i = 0; i < static_cast<unsigned>(eSide::size); i++)
		SkyBoxSetTexture(0, static_cast<eSide>(i));

	StarSystem_Inited = false;
}

/*
 * Draw local vertex array.
 */
static inline void DrawVertexArray()
{
	vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4,
		  RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX,
		  VertexArray, 9 * sizeof(VertexArray[0]));
}

/*
 * Add vertex to  to local vertex array.
 */
static inline void AddToVertexArray(float CoordX, float CoordY, float CoordZ,
				    sRGBCOLOR Color, float Alpha,
				    float TextureU, float TextureV)
{
	VertexArray[VertexArrayPosition++] = CoordX;
	VertexArray[VertexArrayPosition++] = CoordY;
	VertexArray[VertexArrayPosition++] = CoordZ;
	VertexArray[VertexArrayPosition++] = Color.r;
	VertexArray[VertexArrayPosition++] = Color.g;
	VertexArray[VertexArrayPosition++] = Color.b;
	VertexArray[VertexArrayPosition++] = Alpha;
	VertexArray[VertexArrayPosition++] = TextureU;
	VertexArray[VertexArrayPosition++] = TextureV;
}

/*
 * Draw star system.
 */
void StarSystemDraw(eDrawType DrawType)
{
	if (!StarSystem_InitedAll) return;

	// current camera location
	sVECTOR3D CurrentCameraLocation;
	vw_GetCameraLocation(&CurrentCameraLocation);

	if (StarSystem_Inited) {
		vw_DepthTest(false, eCompareFunc::LESS);

		// SkyBox
		vw_PushMatrix();
		vw_Translate(CurrentCameraLocation);
		vw_Rotate(StarSystem_BaseRotation.x, 1.0f, 0.0f, 0.0f);
		vw_Rotate(StarSystem_BaseRotation.y, 0.0f, 1.0f, 0.0f);
		vw_Rotate(StarSystem_BaseRotation.z, 0.0f, 0.0f, 1.0f);
		SkyBoxDraw();
		vw_PopMatrix();

		vw_DepthTest(true, eCompareFunc::LEQUAL);
	}

	// FIXME should be moved to 'space_object' code
	// planets and big asteroids should be rendered before 'space dust'
	ForEachSpaceObject([&] (cSpaceObject &tmpSpace) {
		if (tmpSpace.ObjectType == eObjectType::Planet) {
			if (DrawType == eDrawType::GAME) {
				vw_PushMatrix();
				vw_Translate(sVECTOR3D{CurrentCameraLocation.x * 0.90f - GameCameraGetDeviation() * 4.0f,
						       GameCameraGetDeviation() * 2.0f,
						       0.0f});
			}
			tmpSpace.Draw(false);
			if (DrawType == eDrawType::GAME)
				vw_PopMatrix();
		} else if ((tmpSpace.ObjectType == eObjectType::BigAsteroid) &&
			   ((tmpSpace.InternalType > 10) && (tmpSpace.InternalType < 20))) {
			if (DrawType == eDrawType::GAME) {
				vw_PushMatrix();
				vw_Translate(sVECTOR3D{CurrentCameraLocation.x * 0.70f - GameCameraGetDeviation() * 4.0f,
						       GameCameraGetDeviation() * 2.0f,
						       0.0f});
			}
			tmpSpace.Draw(false);
			if (DrawType == eDrawType::GAME)
				vw_PopMatrix();
		}
	});

	// 'space dust' much more closer to player, than planets, clear the depth buffer
	vw_Clear(RI_DEPTH_BUFFER);

	float width_2{0.0f};
	float heigh_2{110.0f};
	float length_2{110.0f};
	float x{GamePoint.x};
	float y{GamePoint.y};
	float z{GamePoint.z};
	float StartTransparentLayer1{0.7f};
	float EndTransparentLayer1{0.7f};
	sRGBCOLOR Color{1.0f, 1.0f, 1.0f};

	if (DrawType == eDrawType::GAME) {
		width_2 = length_2 = 175.0f;
		heigh_2 = 0.0f;

		x = GamePoint.x + GameCameraGetDeviation() + CurrentCameraLocation.x * 0.8f;
		y = GamePoint.y - GameCameraGetDeviation() * 0.5f;
		z = GamePoint.z + 25.0f;

		StartTransparentLayer1 = StarsTileStartTransparentLayer1;
		EndTransparentLayer1 = StarsTileEndTransparentLayer1;
	}

	// first 'space dust' layer (clouds)
	VertexArrayPosition = 0;
	AddToVertexArray(x + width_2, y + heigh_2, z + length_2 + length_2 / 2.0f,
			 Color, StartTransparentLayer1,
			 1.0f, 0.0f + StarsTile / 3.0f);
	AddToVertexArray(x + width_2, y + heigh_2, z - length_2 / 2.0f,
			 Color, EndTransparentLayer1,
			 1.0f, 1.0f + StarsTile / 3.0f);
	AddToVertexArray(x - width_2, y - heigh_2, z + length_2 + length_2 / 2.0f,
			 Color, StartTransparentLayer1,
			 0.0f, 0.0f + StarsTile / 3.0f);
	AddToVertexArray(x - width_2, y - heigh_2, z - length_2/2,
			 Color, EndTransparentLayer1,
			 0.0f, 1.0f + StarsTile / 3.0f);

	if (DrawType == eDrawType::MENU) {
		StarsTile -= 0.015f * (vw_GetTimeThread(0) - StarsTileUpdateTime);
		StarsTileUpdateTime = vw_GetTimeThread(0);
	} else {
		StarsTile -= 0.035f * (vw_GetTimeThread(1) - StarsTileUpdateTime);
		StarsTileUpdateTime = vw_GetTimeThread(1);
	}

	if (StarsTile < -3.0f)
		StarsTile += 3.0f;

	GLtexture TileTexture = GetPreloadedTextureAsset("skybox/tile_back.tga");
	vw_BindTexture(0, TileTexture);
	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE);

	vw_DepthTest(false, eCompareFunc::LESS);

	if (DrawType == eDrawType::MENU) {
		vw_PushMatrix();
		vw_Rotate(-20.0f, 0.0f, 0.0f, 1.0f);
		vw_Rotate(-45.0f, 0.0f, 1.0f, 0.0f);
		vw_Rotate(30.0f, 1.0f, 0.0f, 0.0f);
	}

	DrawVertexArray();

	// second 'space dust' layer
	VertexArrayPosition = 0;
	AddToVertexArray(x + width_2, y + heigh_2, z + length_2 + length_2 / 2.0f,
			 Color, StartTransparentLayer1,
			 3.0f, 0.0f + StarsTile);
	AddToVertexArray(x + width_2, y + heigh_2, z - length_2 / 2.0f,
			 Color, EndTransparentLayer1,
			 3.0f, 3.0f + StarsTile);
	AddToVertexArray(x - width_2, y - heigh_2, z + length_2 + length_2 / 2.0f,
			 Color, StartTransparentLayer1,
			 0.0f, 0.0f + StarsTile);
	AddToVertexArray(x - width_2, y - heigh_2, z - length_2/2,
			 Color, EndTransparentLayer1,
			 0.0f, 3.0f + StarsTile);

	vw_BindTexture(0, GetPreloadedTextureAsset("skybox/tile_stars.tga"));

	DrawVertexArray();

	if (DrawType == eDrawType::MENU)
		vw_PopMatrix();

	vw_DepthTest(true, eCompareFunc::LEQUAL);

	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_BindTexture(0, 0);

	// care about in-game camera movements
	if (DrawType == eDrawType::GAME) {
		if (auto sharedSpace = psSpace.lock()) {
			sharedSpace->SetStartLocation(sharedSpace->GetLocation());
			sharedSpace->MoveSystemLocation(InGameInitialLocation + GamePoint);
		}
	}
}

/*
 * Draw third layer of space dust.
 */
void StarSystemDrawThirdLayer(eDrawType DrawType)
{
	// VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
	if (GameConfig().VisualEffectsQuality > 1)
		return;

	float width_2{0.0f};
	float heigh_2{110.0f};
	float length_2{110.0f};
	float x{GamePoint.x};
	float y{GamePoint.y};
	float z{GamePoint.z};
	float StartTransparentLayer2{0.9f};
	float EndTransparentLayer2{0.7f};
	sRGBCOLOR Color{1.0f, 1.0f, 1.0f};

	if (DrawType == eDrawType::GAME) {
		width_2 = length_2 = 175.0f;
		heigh_2 = 0.0f;

		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);

		x = GamePoint.x+GameCameraGetDeviation() * 2.0f + CurrentCameraLocation.x * 0.5f;
		y = GamePoint.y-GameCameraGetDeviation();
		z = GamePoint.z+25.0f;

		StartTransparentLayer2 = StarsTileStartTransparentLayer2;
		EndTransparentLayer2 = StarsTileEndTransparentLayer2;
	}

	VertexArrayPosition = 0;
	AddToVertexArray(x + width_2, y + heigh_2, z + length_2 + length_2/ 2.0f,
			 Color, StartTransparentLayer2,
			 3.2f, 0.0f + StarsTile2);
	AddToVertexArray(x + width_2, y + heigh_2, z - length_2 / 2.0f,
			 Color, EndTransparentLayer2,
			 3.2f, 3.0f + StarsTile2);
	AddToVertexArray(x - width_2, y - heigh_2, z + length_2 + length_2 / 2.0f,
			 Color, StartTransparentLayer2,
			 0.2f, 0.0f + StarsTile2);
	AddToVertexArray(x - width_2, y - heigh_2, z - length_2 / 2.0f,
			 Color, EndTransparentLayer2,
			 0.2f, 3.0f + StarsTile2);

	if (DrawType == eDrawType::MENU) {
		StarsTile2 -= 0.04f * (vw_GetTimeThread(0) - StarsTileUpdateTime2);
		StarsTileUpdateTime2 = vw_GetTimeThread(0);
	} else {
		StarsTile2 -= 0.12f * (vw_GetTimeThread(1) - StarsTileUpdateTime2);
		StarsTileUpdateTime2 = vw_GetTimeThread(1);
	}
	if (StarsTile2 < -3.0f)
		StarsTile2 += 3.0f;

	GLtexture TileTexture = GetPreloadedTextureAsset("skybox/tile_stars.tga");
	vw_BindTexture(0, TileTexture);
	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE);
	vw_DepthTest(false, eCompareFunc::LESS);

	if (DrawType == eDrawType::MENU) {
		vw_PushMatrix();
		vw_Rotate(-20.0f, 0.0f, 0.0f, 1.0f);
		vw_Rotate(-45.0f, 0.0f, 1.0f, 0.0f);
		vw_Rotate(30.0f, 1.0f, 0.0f, 0.0f);
	}

	DrawVertexArray();

	if (DrawType == eDrawType::MENU)
		vw_PopMatrix();

	vw_DepthTest(true, eCompareFunc::LEQUAL);
	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_BindTexture(0, 0);
}

} // astromenace namespace
} // viewizard namespace
