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

#include "space_object.h"
#include "../../config/config.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Setup gfx (engine).
 */
static void SetBaseGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXType)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
	ParticleSystem->Direction = sVECTOR3D{0.0f, -1.0f, 0.0f};

	switch(GFXType) {
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
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 5.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{0.8f, 0.1f, 0.8f};
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D{0.0f, -1.0f, 0.0f};
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.025f);
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
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 180.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D{0.0f, 1.0f, 0.0f};
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.3f);
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong GFXType.\n";
		break;
	}
}

/*
 * Constructor.
 */
cBasePart::cBasePart(const int BasePartNum)
{
	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::BasePart;
	NeedAlphaTest = true;

	// for base part type 1, 2, 6 or 8 we could pick random block or provide particular id
	if ((BasePartNum == 1) ||
	    (BasePartNum == 2) ||
	    (BasePartNum == 6) ||
	    (BasePartNum == 8))
		InternalType = BasePartNum * 10 + 1 + vw_iRandNum(4);
	else
		InternalType = BasePartNum;

	auto Setup1Type = [&] () {
		Texture[0] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		Texture[1] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[1] = 0;
		Texture[2] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[2] = 0;

		GraphicFXLocation.resize(8);
		GraphicFX.resize(8);

		GraphicFX[0] = vw_CreateParticleSystem();
		GraphicFXLocation[0] = sVECTOR3D{14.3f, -4.0f, -14.3f};
		if (auto sharedGFX = GraphicFX[0].lock())
			SetBaseGFX(sharedGFX, 1);
		GraphicFX[1] = vw_CreateParticleSystem();
		GraphicFXLocation[1] = sVECTOR3D{14.3f, -4.0f, 14.3f};
		if (auto sharedGFX = GraphicFX[1].lock())
			SetBaseGFX(sharedGFX, 1);
		GraphicFX[2] = vw_CreateParticleSystem();
		GraphicFXLocation[2] = sVECTOR3D{-14.3f, -4.0f, -14.3f};
		if (auto sharedGFX = GraphicFX[2].lock())
			SetBaseGFX(sharedGFX, 1);
		GraphicFX[3] = vw_CreateParticleSystem();
		GraphicFXLocation[3] = sVECTOR3D{-14.3f, -4.0f, 14.3f};
		if (auto sharedGFX = GraphicFX[3].lock())
			SetBaseGFX(sharedGFX, 1);

		GraphicFX[4] = vw_CreateParticleSystem();
		GraphicFXLocation[4] = sVECTOR3D{14.3f, 3.2f, -14.3f};
		if (auto sharedGFX = GraphicFX[4].lock())
			SetBaseGFX(sharedGFX, 2);
		GraphicFX[5] = vw_CreateParticleSystem();
		GraphicFXLocation[5] = sVECTOR3D{14.3f, 3.2f, 14.3f};
		if (auto sharedGFX = GraphicFX[5].lock())
			SetBaseGFX(sharedGFX, 2);
		GraphicFX[6] = vw_CreateParticleSystem();
		GraphicFXLocation[6] = sVECTOR3D{-14.3f, 3.2f, -14.3f};
		if (auto sharedGFX = GraphicFX[6].lock())
			SetBaseGFX(sharedGFX, 2);
		GraphicFX[7] = vw_CreateParticleSystem();
		GraphicFXLocation[7] = sVECTOR3D{-14.3f, 3.2f, 14.3f};
		if (auto sharedGFX = GraphicFX[7].lock())
			SetBaseGFX(sharedGFX, 2);
	};

	auto Setup2Type = [&] () {
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[1] = 0;
		NormalMap[1] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		Texture[2] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[2] = 0;
	};

	auto Setup6Type = [&] () {
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[1] = 0;
		NormalMap[1] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
	};

	auto Setup8Type = [&] () {
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[1] = 0;
		Texture[2] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[2] = 0;
		NormalMap[2] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
	};

	switch (InternalType) {
	case 11:
		LoadObjectData("models/spacebase/1/1.vw3d", *this);
		Setup1Type();
		break;
	case 12:
		LoadObjectData("models/spacebase/1/2.vw3d", *this);
		Setup1Type();
		break;
	case 13:
		LoadObjectData("models/spacebase/1/3.vw3d", *this);
		Setup1Type();
		break;
	case 14:
		LoadObjectData("models/spacebase/1/4.vw3d", *this);
		Setup1Type();
		break;
	case 15:
		LoadObjectData("models/spacebase/1/5.vw3d", *this);
		Setup1Type();
		break;

	case 21:
		LoadObjectData("models/spacebase/2/1.vw3d", *this);
		Setup2Type();
		break;
	case 22:
		LoadObjectData("models/spacebase/2/2.vw3d", *this);
		Setup2Type();
		break;
	case 23:
		LoadObjectData("models/spacebase/2/3.vw3d", *this);
		Setup2Type();
		break;
	case 24:
		LoadObjectData("models/spacebase/2/4.vw3d", *this);
		Setup2Type();
		break;
	case 25:
		LoadObjectData("models/spacebase/2/5.vw3d", *this);
		Setup2Type();
		break;

	case 3:
		LoadObjectData("models/spacebase/3/1.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[1] = 0;

		GraphicFXLocation.resize(2);
		GraphicFX.resize(2);

		GraphicFX[0] = vw_CreateParticleSystem();
		GraphicFXLocation[0] = sVECTOR3D{0.0f, -5.0f, 13.7f};
		if (auto sharedGFX = GraphicFX[0].lock())
			SetBaseGFX(sharedGFX, 1);
		GraphicFX[1] = vw_CreateParticleSystem();
		GraphicFXLocation[1] = sVECTOR3D{0.0f, 5.0f, 13.7f};
		if (auto sharedGFX = GraphicFX[1].lock())
			SetBaseGFX(sharedGFX, 2);
		break;

	case 4:
		LoadObjectData("models/spacebase/4/1.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		break;

	case 5:
		LoadObjectData("models/spacebase/5/1.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		break;

	case 61:
		LoadObjectData("models/spacebase/6/1.vw3d", *this);
		Setup6Type();
		break;
	case 62:
		LoadObjectData("models/spacebase/6/2.vw3d", *this);
		Setup6Type();
		break;
	case 63:
		LoadObjectData("models/spacebase/6/3.vw3d", *this);
		Setup6Type();
		break;
	case 64:
		LoadObjectData("models/spacebase/6/4.vw3d", *this);
		Setup6Type();
		break;
	case 65:
		LoadObjectData("models/spacebase/6/5.vw3d", *this);
		Setup6Type();
		break;

	case 7:
		LoadObjectData("models/spacebase/7/1.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		break;

	case 81:
		LoadObjectData("models/spacebase/8/1.vw3d", *this);
		Setup8Type();
		break;
	case 82:
		LoadObjectData("models/spacebase/8/2.vw3d", *this);
		Setup8Type();
		break;
	case 83:
		LoadObjectData("models/spacebase/8/3.vw3d", *this);
		Setup8Type();
		break;
	case 84:
		LoadObjectData("models/spacebase/8/4.vw3d", *this);
		Setup8Type();
		break;
	case 85:
		LoadObjectData("models/spacebase/8/5.vw3d", *this);
		Setup8Type();
		break;
	}
}

/*
 * Destructor.
 */
cBasePart::~cBasePart()
{
	if (!GraphicFX.empty()) {
		for (auto &tmpGFX : GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				sharedGFX->IsSuppressed = true;
				sharedGFX->DestroyIfNoParticles = true;
			}
		}
	}
}

/*
 * Set location.
 */
void cBasePart::SetLocation(const sVECTOR3D &NewLocation)
{
	cObject3D::SetLocation(NewLocation);

	if (!GraphicFX.empty()) {
		for (unsigned int i = 0; i < GraphicFX.size(); i++) {
			if (auto sharedGFX = GraphicFX[i].lock()) {
				sharedGFX->MoveSystem(NewLocation + GraphicFXLocation[i]);
				sharedGFX->SetStartLocation(GraphicFXLocation[i] + NewLocation);
			}
		}
	}
}

/*
 * Set rotation.
 */
void cBasePart::SetRotation(const sVECTOR3D &NewRotation)
{
	cObject3D::SetRotation(NewRotation);

	if (!GraphicFX.empty()) {
		for (unsigned int i = 0; i < GraphicFX.size(); i++) {
			if (auto sharedGFX = GraphicFX[i].lock()) {
				vw_Matrix33CalcPoint(GraphicFXLocation[i], OldInvRotationMat);
				vw_Matrix33CalcPoint(GraphicFXLocation[i], CurrentRotationMat);

				if (sharedGFX->SpeedOnCreation == -1.0f) {
					sharedGFX->MoveSystem(GraphicFXLocation[i] + Location);
					sharedGFX->SetStartLocation(GraphicFXLocation[i] + Location);
					sharedGFX->RotateSystemAndParticlesByAngle(Rotation);
				} else {
					sharedGFX->MoveSystemLocation(GraphicFXLocation[i] + Location);
					sharedGFX->RotateSystemByAngle(Rotation);
				}
			}
		}
	}
}

} // astromenace namespace
} // viewizard namespace
