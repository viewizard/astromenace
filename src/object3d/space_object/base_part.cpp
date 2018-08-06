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

// TODO don't call GetPreloadedTextureAsset() all the time, use cached texture instead

#include "space_object.h"
#include "../../config/config.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

enum class eEngineGFX {
	Primary,
	Auxiliary
};

} // unnamed namespace

/*
 * Setup gfx (engine).
 */
static void SetBaseGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, const eEngineGFX EngineType)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");

	sRGBCOLOR MuddyYellow{0.60f, 0.60f, 0.30f};
	sRGBCOLOR LightGreen{0.30f, 1.00f, 0.30f};

	switch (EngineType) {
	case eEngineGFX::Primary:
		ParticleSystem->ColorStart = MuddyYellow;
		ParticleSystem->ColorEnd = LightGreen;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.30f;
		ParticleSystem->SizeEnd = 0.60f;
		ParticleSystem->Speed = 10.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->SpeedVar = 2.00f;
		ParticleSystem->Theta = 5.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize(0.8f, 0.1f, 0.8f);
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->Direction(0.0f, -1.0f, 0.0f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.025f);
		break;

	case eEngineGFX::Auxiliary:
		ParticleSystem->ColorStart = MuddyYellow;
		ParticleSystem->ColorEnd = LightGreen;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.20f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->SpeedVar = 2.00f;
		ParticleSystem->Theta = 180.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->Direction(0.0f, 1.0f, 0.0f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.3f);
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong GFXType.\n";
		break;
	}
}

/*
 * Setup base part type 1.
 */
void cBasePart::SetupBasePartType1()
{
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
		SetBaseGFX(sharedGFX, eEngineGFX::Primary);
	GraphicFX[1] = vw_CreateParticleSystem();
	GraphicFXLocation[1] = sVECTOR3D{14.3f, -4.0f, 14.3f};
	if (auto sharedGFX = GraphicFX[1].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Primary);
	GraphicFX[2] = vw_CreateParticleSystem();
	GraphicFXLocation[2] = sVECTOR3D{-14.3f, -4.0f, -14.3f};
	if (auto sharedGFX = GraphicFX[2].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Primary);
	GraphicFX[3] = vw_CreateParticleSystem();
	GraphicFXLocation[3] = sVECTOR3D{-14.3f, -4.0f, 14.3f};
	if (auto sharedGFX = GraphicFX[3].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Primary);

	GraphicFX[4] = vw_CreateParticleSystem();
	GraphicFXLocation[4] = sVECTOR3D{14.3f, 3.2f, -14.3f};
	if (auto sharedGFX = GraphicFX[4].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Auxiliary);
	GraphicFX[5] = vw_CreateParticleSystem();
	GraphicFXLocation[5] = sVECTOR3D{14.3f, 3.2f, 14.3f};
	if (auto sharedGFX = GraphicFX[5].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Auxiliary);
	GraphicFX[6] = vw_CreateParticleSystem();
	GraphicFXLocation[6] = sVECTOR3D{-14.3f, 3.2f, -14.3f};
	if (auto sharedGFX = GraphicFX[6].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Auxiliary);
	GraphicFX[7] = vw_CreateParticleSystem();
	GraphicFXLocation[7] = sVECTOR3D{-14.3f, 3.2f, 14.3f};
	if (auto sharedGFX = GraphicFX[7].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Auxiliary);

	for (unsigned int i = 0; i < GraphicFX.size(); i++) {
		if (auto sharedGFX = GraphicFX[i].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[i]);
			sharedGFX->SetStartLocation(GraphicFXLocation[i]);
		}
	}
}

/*
 * Setup base part type 2.
 */
void cBasePart::SetupBasePartType2()
{
	Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
	TextureIllum[0] = 0;
	Texture[1] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
	TextureIllum[1] = 0;
	NormalMap[1] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
	Texture[2] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
	TextureIllum[2] = 0;
}

/*
 * Setup base part type 3.
 */
void cBasePart::SetupBasePartType3()
{
	Texture[0] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
	TextureIllum[0] = 0;
	Texture[1] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
	TextureIllum[1] = 0;

	GraphicFXLocation.resize(2);
	GraphicFX.resize(2);

	GraphicFX[0] = vw_CreateParticleSystem();
	GraphicFXLocation[0] = sVECTOR3D{0.0f, -5.0f, 13.7f};
	if (auto sharedGFX = GraphicFX[0].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Primary);
	GraphicFX[1] = vw_CreateParticleSystem();
	GraphicFXLocation[1] = sVECTOR3D{0.0f, 5.0f, 13.7f};
	if (auto sharedGFX = GraphicFX[1].lock())
		SetBaseGFX(sharedGFX, eEngineGFX::Auxiliary);

	for (unsigned int i = 0; i < GraphicFX.size(); i++) {
		if (auto sharedGFX = GraphicFX[i].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[i]);
			sharedGFX->SetStartLocation(GraphicFXLocation[i]);
		}
	}
}

/*
 * Setup base part type 4.
 */
void cBasePart::SetupBasePartType4()
{
	Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
	TextureIllum[0] = 0;
}

/*
 * Setup base part type 5.
 */
void cBasePart::SetupBasePartType5()
{
	Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
	TextureIllum[0] = 0;
}

/*
 * Setup base part type 6.
 */
void cBasePart::SetupBasePartType6()
{
	Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
	TextureIllum[0] = 0;
	Texture[1] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
	TextureIllum[1] = 0;
	NormalMap[1] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
}

/*
 * Setup base part type 7.
 */
void cBasePart::SetupBasePartType7()
{
	Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
	TextureIllum[0] = 0;
}

/*
 * Setup base part type 8.
 */
void cBasePart::SetupBasePartType8()
{
	Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
	TextureIllum[0] = 0;
	Texture[1] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
	TextureIllum[1] = 0;
	Texture[2] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
	TextureIllum[2] = 0;
	NormalMap[2] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
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

	switch (InternalType) {
	case 11:
		LoadObjectData("models/spacebase/1/1.vw3d", *this);
		SetupBasePartType1();
		break;
	case 12:
		LoadObjectData("models/spacebase/1/2.vw3d", *this);
		SetupBasePartType1();
		break;
	case 13:
		LoadObjectData("models/spacebase/1/3.vw3d", *this);
		SetupBasePartType1();
		break;
	case 14:
		LoadObjectData("models/spacebase/1/4.vw3d", *this);
		SetupBasePartType1();
		break;
	case 15:
		LoadObjectData("models/spacebase/1/5.vw3d", *this);
		SetupBasePartType1();
		break;

	case 21:
		LoadObjectData("models/spacebase/2/1.vw3d", *this);
		SetupBasePartType2();
		break;
	case 22:
		LoadObjectData("models/spacebase/2/2.vw3d", *this);
		SetupBasePartType2();
		break;
	case 23:
		LoadObjectData("models/spacebase/2/3.vw3d", *this);
		SetupBasePartType2();
		break;
	case 24:
		LoadObjectData("models/spacebase/2/4.vw3d", *this);
		SetupBasePartType2();
		break;
	case 25:
		LoadObjectData("models/spacebase/2/5.vw3d", *this);
		SetupBasePartType2();
		break;

	case 3:
		LoadObjectData("models/spacebase/3/1.vw3d", *this);
		SetupBasePartType3();
		break;

	case 4:
		LoadObjectData("models/spacebase/4/1.vw3d", *this);
		SetupBasePartType4();
		break;

	case 5:
		LoadObjectData("models/spacebase/5/1.vw3d", *this);
		SetupBasePartType5();
		break;

	case 61:
		LoadObjectData("models/spacebase/6/1.vw3d", *this);
		SetupBasePartType6();
		break;
	case 62:
		LoadObjectData("models/spacebase/6/2.vw3d", *this);
		SetupBasePartType6();
		break;
	case 63:
		LoadObjectData("models/spacebase/6/3.vw3d", *this);
		SetupBasePartType6();
		break;
	case 64:
		LoadObjectData("models/spacebase/6/4.vw3d", *this);
		SetupBasePartType6();
		break;
	case 65:
		LoadObjectData("models/spacebase/6/5.vw3d", *this);
		SetupBasePartType6();
		break;

	case 7:
		LoadObjectData("models/spacebase/7/1.vw3d", *this);
		SetupBasePartType7();
		break;

	case 81:
		LoadObjectData("models/spacebase/8/1.vw3d", *this);
		SetupBasePartType8();
		break;
	case 82:
		LoadObjectData("models/spacebase/8/2.vw3d", *this);
		SetupBasePartType8();
		break;
	case 83:
		LoadObjectData("models/spacebase/8/3.vw3d", *this);
		SetupBasePartType8();
		break;
	case 84:
		LoadObjectData("models/spacebase/8/4.vw3d", *this);
		SetupBasePartType8();
		break;
	case 85:
		LoadObjectData("models/spacebase/8/5.vw3d", *this);
		SetupBasePartType8();
		break;
	}
}

/*
 * Destructor.
 */
cBasePart::~cBasePart()
{
	if (GraphicFX.empty())
		return;

	for (auto &tmpGFX : GraphicFX) {
		if (auto sharedGFX = tmpGFX.lock()) {
			sharedGFX->IsSuppressed = true;
			sharedGFX->DestroyIfNoParticles = true;
		}
	}
}

/*
 * Set location.
 */
void cBasePart::SetLocation(const sVECTOR3D &NewLocation)
{
	cObject3D::SetLocation(NewLocation);

	if (GraphicFX.empty())
		return;

	for (unsigned int i = 0; i < GraphicFX.size(); i++) {
		if (auto sharedGFX = GraphicFX[i].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[i] + NewLocation);
			sharedGFX->SetStartLocation(GraphicFXLocation[i] + NewLocation);
		}
	}
}

/*
 * Set rotation.
 */
void cBasePart::SetRotation(const sVECTOR3D &NewRotation)
{
	cObject3D::SetRotation(NewRotation);

	if (GraphicFX.empty())
		return;

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

} // astromenace namespace
} // viewizard namespace
