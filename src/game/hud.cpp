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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

#include "../core/core.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../object3d/space_ship/space_ship.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

std::weak_ptr<cParticleSystem2D> EnergyEmblem{};
std::weak_ptr<cParticleSystem2D> ArmorEmblemCircle{};
std::weak_ptr<cParticleSystem2D> ArmorEmblemHoriz{};
std::weak_ptr<cParticleSystem2D> ArmorEmblemVert{};

} // unnamed namespace

// FIXME should be fixed, don't allow global scope interaction for local variables
extern int GamePowerSystem;
extern float CurrentPlayerShipEnergy;
extern float CurrentAlert2;

float GetShipMaxEnergy(int Num);


/*
 * Init head-up display particle systems.
 * Make sure we re-init all particle systems data all the time in order to avoid
 * issues with future code changes.
 */
void InitHUDParticleSystems()
{
	if (EnergyEmblem.expired())
		EnergyEmblem = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedEnergyEmblem = EnergyEmblem.lock()) {
		sharedEnergyEmblem->ColorStart = sRGBCOLOR{0.7f, 0.8f, 1.0f};
		sharedEnergyEmblem->ColorEnd = sRGBCOLOR{0.0f, 0.0f, 1.0f};
		sharedEnergyEmblem->AlphaStart = 1.0f;
		sharedEnergyEmblem->AlphaEnd = 1.0f;
		sharedEnergyEmblem->SizeStart = 12.0f;
		sharedEnergyEmblem->SizeVar = 10.0f;
		sharedEnergyEmblem->SizeEnd = 0.0f;
		sharedEnergyEmblem->Speed = 70.0f;
		sharedEnergyEmblem->SpeedVar = 20.0f;
		sharedEnergyEmblem->Theta = 360.0f;
		sharedEnergyEmblem->Life = 2.1f;
		sharedEnergyEmblem->LifeVar = 0.05f;
		sharedEnergyEmblem->ParticlesPerSec = 50;
		sharedEnergyEmblem->IsMagnet = true;
		sharedEnergyEmblem->MagnetFactor = 150.0f;
		sharedEnergyEmblem->CreationType = eParticle2DCreationType::Point;
		sharedEnergyEmblem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
		sharedEnergyEmblem->MoveSystem(sVECTOR3D{33.0f, 29.0f, 0.0f});
	}

	if (ArmorEmblemVert.expired())
		ArmorEmblemVert = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedArmorEmblemVert = ArmorEmblemVert.lock()) {
		sharedArmorEmblemVert->ColorStart = sRGBCOLOR{1.0f, 0.4f, 0.1f};
		sharedArmorEmblemVert->ColorEnd = sRGBCOLOR{0.5f, 0.0f, 0.0f};
		sharedArmorEmblemVert->AlphaStart = 1.0f;
		sharedArmorEmblemVert->AlphaEnd = 1.0f;
		sharedArmorEmblemVert->SizeStart = 13.0f;
		sharedArmorEmblemVert->SizeVar = 5.0f;
		sharedArmorEmblemVert->SizeEnd = 0.0f;
		sharedArmorEmblemVert->Speed = 0.0f;
		sharedArmorEmblemVert->SpeedOnCreation = 8.0f;
		sharedArmorEmblemVert->SpeedVar = 0.0f;
		sharedArmorEmblemVert->Theta = 360.0f;
		sharedArmorEmblemVert->Life = 2.0f;
		sharedArmorEmblemVert->LifeVar = 0.05f;
		sharedArmorEmblemVert->ParticlesPerSec = 50;
		sharedArmorEmblemVert->CreationType = eParticle2DCreationType::Quad;
		sharedArmorEmblemVert->CreationSize(1.0f, 18.0f, 0.0f);
		sharedArmorEmblemVert->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
		sharedArmorEmblemVert->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});
	}

	if (ArmorEmblemHoriz.expired())
		ArmorEmblemHoriz = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedArmorEmblemHoriz = ArmorEmblemHoriz.lock()) {
		sharedArmorEmblemHoriz->ColorStart = sRGBCOLOR{1.0f, 0.4f, 0.1f};
		sharedArmorEmblemHoriz->ColorEnd = sRGBCOLOR{0.5f, 0.0f, 0.0f};
		sharedArmorEmblemHoriz->AlphaStart = 1.0f;
		sharedArmorEmblemHoriz->AlphaEnd = 1.0f;
		sharedArmorEmblemHoriz->SizeStart = 13.0f;
		sharedArmorEmblemHoriz->SizeVar = 5.0f;
		sharedArmorEmblemHoriz->SizeEnd = 0.0f;
		sharedArmorEmblemHoriz->Speed = 0.0f;
		sharedArmorEmblemHoriz->SpeedOnCreation = 8.0f;
		sharedArmorEmblemHoriz->SpeedVar = 0.0f;
		sharedArmorEmblemHoriz->Theta = 360.0f;
		sharedArmorEmblemHoriz->Life = 2.0f;
		sharedArmorEmblemHoriz->LifeVar = 0.05f;
		sharedArmorEmblemHoriz->ParticlesPerSec = 50;
		sharedArmorEmblemHoriz->CreationType = eParticle2DCreationType::Quad;
		sharedArmorEmblemHoriz->CreationSize(18.0f, 1.0f, 0.0f);
		sharedArmorEmblemHoriz->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
		sharedArmorEmblemHoriz->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});
	}

	if (ArmorEmblemCircle.expired())
		ArmorEmblemCircle = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedArmorEmblemCircle = ArmorEmblemCircle.lock()) {
	sharedArmorEmblemCircle->ColorStart = sRGBCOLOR{1.0f, 0.6f, 0.2f};
		sharedArmorEmblemCircle->ColorEnd = sRGBCOLOR{0.5f, 0.0f, 0.0f};
		sharedArmorEmblemCircle->AlphaStart = 1.0f;
		sharedArmorEmblemCircle->AlphaEnd = 1.0f;
		sharedArmorEmblemCircle->SizeStart = 25.0f;
		sharedArmorEmblemCircle->SizeVar = 5.0f;
		sharedArmorEmblemCircle->SizeEnd = 0.0f;
		sharedArmorEmblemCircle->Speed = 0.0f;
		sharedArmorEmblemCircle->SpeedOnCreation = 8.0f;
		sharedArmorEmblemCircle->SpeedVar = 10.0f;
		sharedArmorEmblemCircle->Theta = 360.0f;
		sharedArmorEmblemCircle->Life = 1.5f;
		sharedArmorEmblemCircle->LifeVar = 0.05f;
		sharedArmorEmblemCircle->ParticlesPerSec = 70;
		sharedArmorEmblemCircle->Direction(1.0f, 0.0f, 0.0f);
		sharedArmorEmblemCircle->CreationType = eParticle2DCreationType::Circle;
		sharedArmorEmblemCircle->CreationSize(25.0f, 25.0f, 0.0f);
		sharedArmorEmblemCircle->DeadZone = 24.0f;
		sharedArmorEmblemCircle->IsMagnet = true;
		sharedArmorEmblemCircle->MagnetFactor = 25.0f;
		sharedArmorEmblemCircle->Texture = GetPreloadedTextureAsset("gfx/flare.tga");
		sharedArmorEmblemCircle->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});
		sharedArmorEmblemCircle->SetRotation(sVECTOR3D{0.0f, 0.0f, 90.0f});
	}
}

/*
 * Update head-up display particle systems.
 */
void UpdateHUDParticleSystems(std::weak_ptr<cSpaceShip> &PlayerFighter)
{
	if (auto sharedPlayerFighter = PlayerFighter.lock()) {
		if (auto sharedEnergyEmblem = EnergyEmblem.lock()) {
			sharedEnergyEmblem->ParticlesPerSec =
				1 + static_cast<unsigned>(49 * (CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem)));
		}

		float tmpArmorPercentage = sharedPlayerFighter->ArmorCurrentStatus / sharedPlayerFighter->ArmorInitialStatus;
		bool tmpLowArmor = (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus / 10.0f);

		if (auto sharedArmorEmblemCircle = ArmorEmblemCircle.lock()) {
			sharedArmorEmblemCircle->ColorStart.r = 1.0f;
			sharedArmorEmblemCircle->ColorStart.g = 0.6f * tmpArmorPercentage;
			sharedArmorEmblemCircle->ColorStart.b = 0.2f * tmpArmorPercentage;

			if (tmpLowArmor) {
				sharedArmorEmblemCircle->AlphaStart = CurrentAlert2;
				sharedArmorEmblemCircle->AlphaEnd = CurrentAlert2;
			} else { // armor could be repaired in-game
				sharedArmorEmblemCircle->AlphaStart = 1.0f;
				sharedArmorEmblemCircle->AlphaEnd = 1.0f;
			}
		}

		if (auto sharedArmorEmblemHoriz = ArmorEmblemHoriz.lock()) {
			sharedArmorEmblemHoriz->ColorStart.r = 1.0f;
			sharedArmorEmblemHoriz->ColorStart.g = 0.6f * tmpArmorPercentage;
			sharedArmorEmblemHoriz->ColorStart.b = 0.2f * tmpArmorPercentage;

			if (tmpLowArmor) {
				if (CurrentAlert2 > 0.6f) {
					sharedArmorEmblemHoriz->AlphaStart = CurrentAlert2;
					sharedArmorEmblemHoriz->AlphaEnd = CurrentAlert2;
				} else { // armor could be repaired in-game
					sharedArmorEmblemHoriz->AlphaStart = 0.0f;
					sharedArmorEmblemHoriz->AlphaEnd = 0.0f;
				}
			} else {
				sharedArmorEmblemHoriz->AlphaStart = 1.0f;
				sharedArmorEmblemHoriz->AlphaEnd = 1.0f;
			}
		}

		if (auto sharedArmorEmblemVert = ArmorEmblemVert.lock()) {
			sharedArmorEmblemVert->ColorStart.r = 1.0f;
			sharedArmorEmblemVert->ColorStart.g = 0.6f * tmpArmorPercentage;
			sharedArmorEmblemVert->ColorStart.b = 0.2f * tmpArmorPercentage;

			if (tmpLowArmor) {
				if (CurrentAlert2 > 0.6f) {
					sharedArmorEmblemVert->AlphaStart = CurrentAlert2;
					sharedArmorEmblemVert->AlphaEnd = CurrentAlert2;
				} else { // armor could be repaired in-game
					sharedArmorEmblemVert->AlphaStart = 0.0f;
					sharedArmorEmblemVert->AlphaEnd = 0.0f;
				}
			} else {
				sharedArmorEmblemVert->AlphaStart = 1.0f;
				sharedArmorEmblemVert->AlphaEnd = 1.0f;
			}
		}
	} else {
		if (auto sharedEnergyEmblem = EnergyEmblem.lock()) {
			sharedEnergyEmblem->AlphaStart = 0.0f;
			sharedEnergyEmblem->AlphaEnd = 0.0f;
			sharedEnergyEmblem->ParticlesPerSec = 1;
		}

		if (auto sharedArmorEmblemCircle = ArmorEmblemCircle.lock()) {
			sharedArmorEmblemCircle->AlphaStart = 0.0f;
			sharedArmorEmblemCircle->AlphaEnd = 0.0f;
			sharedArmorEmblemCircle->ParticlesPerSec = 1;
		}

		if (auto sharedArmorEmblemHoriz = ArmorEmblemHoriz.lock()) {
			sharedArmorEmblemHoriz->AlphaStart = 0.0f;
			sharedArmorEmblemHoriz->AlphaEnd = 0.0f;
			sharedArmorEmblemHoriz->ParticlesPerSec = 1;
		}

		if (auto sharedArmorEmblemVert = ArmorEmblemVert.lock()) {
			sharedArmorEmblemVert->AlphaStart = 0.0f;
			sharedArmorEmblemVert->AlphaEnd = 0.0f;
			sharedArmorEmblemVert->ParticlesPerSec = 1;
		}
	}

	vw_UpdateAllParticleSystems2D(vw_GetTimeThread(0));
}

/*
 * Draw head-up display particle systems.
 */
void DrawHUDParticleSystems()
{
	vw_DrawAllParticleSystems2D();
}

/*
 * Draw head-up display border.
 */
void DrawHUDBorder()
{
	if (GameConfig().InternalWidth == 1024) {
		sRECT SrcRect{0, 0, 1024, 74};
		sRECT DstRect{0, 0, 1024, 74};
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/game_panel.tga"), true, 1.0f);
		return;
	}

	sRECT SrcRect{0, 0, 466, 73};
	sRECT DstRect{0, 0, 466, 73};
	GLtexture tmpHUDBorder = GetPreloadedTextureAsset("game/game_panel2.tga");
	vw_Draw2D(DstRect, SrcRect, tmpHUDBorder, true, 1.0f);

	SrcRect(1, 74, 150, 145);
	DstRect(540, 0, 540 + 149, 71);
	vw_Draw2D(DstRect, SrcRect, tmpHUDBorder, true, 1.0f);

	SrcRect(150, 74, 610, 145);
	DstRect(768, 0, 768 + 460, 71);
	vw_Draw2D(DstRect, SrcRect, tmpHUDBorder, true, 1.0f);
}

} // astromenace namespace
} // viewizard namespace
