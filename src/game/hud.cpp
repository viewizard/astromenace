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

// FIXME split "draw" and "update" code

// TODO translate comments

#include "../core/core.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../object3d/space_ship/space_ship.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

std::weak_ptr<cParticleSystem2D> EnergyParticleSystem2D{};
std::weak_ptr<cParticleSystem2D> LifeParticleSystem2D{};
std::weak_ptr<cParticleSystem2D> Life2ParticleSystem2D{};
std::weak_ptr<cParticleSystem2D> Life3ParticleSystem2D{};

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
	if (EnergyParticleSystem2D.expired())
		EnergyParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedEnergyParticleSystem2D = EnergyParticleSystem2D.lock()) {
		sharedEnergyParticleSystem2D->ColorStart = sRGBCOLOR{0.7f, 0.8f, 1.0f};
		sharedEnergyParticleSystem2D->ColorEnd = sRGBCOLOR{0.0f, 0.0f, 1.0f};
		sharedEnergyParticleSystem2D->AlphaStart = 1.0f;
		sharedEnergyParticleSystem2D->AlphaEnd = 1.0f;
		sharedEnergyParticleSystem2D->SizeStart = 12.0f;
		sharedEnergyParticleSystem2D->SizeVar = 10.0f;
		sharedEnergyParticleSystem2D->SizeEnd = 0.0f;
		sharedEnergyParticleSystem2D->Speed = 70.0f;
		sharedEnergyParticleSystem2D->SpeedVar = 20.0f;
		sharedEnergyParticleSystem2D->Theta = 360.0f;
		sharedEnergyParticleSystem2D->Life = 2.1f;
		sharedEnergyParticleSystem2D->LifeVar = 0.05f;
		sharedEnergyParticleSystem2D->ParticlesPerSec = 50;
		sharedEnergyParticleSystem2D->IsMagnet = true;
		sharedEnergyParticleSystem2D->MagnetFactor = 150.0f;
		sharedEnergyParticleSystem2D->CreationType = eParticle2DCreationType::Point;
		sharedEnergyParticleSystem2D->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
		sharedEnergyParticleSystem2D->MoveSystem(sVECTOR3D{33.0f, 29.0f, 0.0f});
	}

	if (Life3ParticleSystem2D.expired())
		Life3ParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedLife3ParticleSystem2D = Life3ParticleSystem2D.lock()) {
		sharedLife3ParticleSystem2D->ColorStart = sRGBCOLOR{1.0f, 0.4f, 0.1f};
		sharedLife3ParticleSystem2D->ColorEnd = sRGBCOLOR{0.5f, 0.0f, 0.0f};
		sharedLife3ParticleSystem2D->AlphaStart = 1.0f;
		sharedLife3ParticleSystem2D->AlphaEnd = 1.0f;
		sharedLife3ParticleSystem2D->SizeStart = 13.0f;
		sharedLife3ParticleSystem2D->SizeVar = 5.0f;
		sharedLife3ParticleSystem2D->SizeEnd = 0.0f;
		sharedLife3ParticleSystem2D->Speed = 0.0f;
		sharedLife3ParticleSystem2D->SpeedOnCreation = 8.0f;
		sharedLife3ParticleSystem2D->SpeedVar = 0.0f;
		sharedLife3ParticleSystem2D->Theta = 360.0f;
		sharedLife3ParticleSystem2D->Life = 2.0f;
		sharedLife3ParticleSystem2D->LifeVar = 0.05f;
		sharedLife3ParticleSystem2D->ParticlesPerSec = 50;
		sharedLife3ParticleSystem2D->CreationType = eParticle2DCreationType::Quad;
		sharedLife3ParticleSystem2D->CreationSize(1.0f, 18.0f, 0.0f);
		sharedLife3ParticleSystem2D->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
		sharedLife3ParticleSystem2D->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});
	}

	if (Life2ParticleSystem2D.expired())
		Life2ParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedLife2ParticleSystem2D = Life2ParticleSystem2D.lock()) {
		sharedLife2ParticleSystem2D->ColorStart = sRGBCOLOR{1.0f, 0.4f, 0.1f};
		sharedLife2ParticleSystem2D->ColorEnd = sRGBCOLOR{0.5f, 0.0f, 0.0f};
		sharedLife2ParticleSystem2D->AlphaStart = 1.0f;
		sharedLife2ParticleSystem2D->AlphaEnd = 1.0f;
		sharedLife2ParticleSystem2D->SizeStart = 13.0f;
		sharedLife2ParticleSystem2D->SizeVar = 5.0f;
		sharedLife2ParticleSystem2D->SizeEnd = 0.0f;
		sharedLife2ParticleSystem2D->Speed = 0.0f;
		sharedLife2ParticleSystem2D->SpeedOnCreation = 8.0f;
		sharedLife2ParticleSystem2D->SpeedVar = 0.0f;
		sharedLife2ParticleSystem2D->Theta = 360.0f;
		sharedLife2ParticleSystem2D->Life = 2.0f;
		sharedLife2ParticleSystem2D->LifeVar = 0.05f;
		sharedLife2ParticleSystem2D->ParticlesPerSec = 50;
		sharedLife2ParticleSystem2D->CreationType = eParticle2DCreationType::Quad;
		sharedLife2ParticleSystem2D->CreationSize(18.0f, 1.0f, 0.0f);
		sharedLife2ParticleSystem2D->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
		sharedLife2ParticleSystem2D->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});
	}

	if (LifeParticleSystem2D.expired())
		LifeParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedLifeParticleSystem2D = LifeParticleSystem2D.lock()) {
		sharedLifeParticleSystem2D->ColorStart = sRGBCOLOR{1.0f, 0.6f, 0.2f};
		sharedLifeParticleSystem2D->ColorEnd = sRGBCOLOR{0.5f, 0.0f, 0.0f};
		sharedLifeParticleSystem2D->AlphaStart = 1.0f;
		sharedLifeParticleSystem2D->AlphaEnd = 1.0f;
		sharedLifeParticleSystem2D->SizeStart = 25.0f;
		sharedLifeParticleSystem2D->SizeVar = 5.0f;
		sharedLifeParticleSystem2D->SizeEnd = 0.0f;
		sharedLifeParticleSystem2D->Speed = 0.0f;
		sharedLifeParticleSystem2D->SpeedOnCreation = 8.0f;
		sharedLifeParticleSystem2D->SpeedVar = 10.0f;
		sharedLifeParticleSystem2D->Theta = 360.0f;
		sharedLifeParticleSystem2D->Life = 1.5f;
		sharedLifeParticleSystem2D->LifeVar = 0.05f;
		sharedLifeParticleSystem2D->ParticlesPerSec = 70;
		sharedLifeParticleSystem2D->Direction(1.0f, 0.0f, 0.0f);
		sharedLifeParticleSystem2D->CreationType = eParticle2DCreationType::Circle;
		sharedLifeParticleSystem2D->CreationSize(25.0f, 25.0f, 0.0f);
		sharedLifeParticleSystem2D->DeadZone = 24.0f;
		sharedLifeParticleSystem2D->IsMagnet = true;
		sharedLifeParticleSystem2D->MagnetFactor = 25.0f;
		sharedLifeParticleSystem2D->Texture = GetPreloadedTextureAsset("gfx/flare.tga");
		sharedLifeParticleSystem2D->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});
		sharedLifeParticleSystem2D->SetRotation(sVECTOR3D{0.0f, 0.0f, 90.0f});
	}
}

/*
 * Draw head-up display particle systems.
 */
void DrawHUDParticleSystems(std::weak_ptr<cSpaceShip> &PlayerFighter)
{
	if (auto sharedEnergyParticleSystem2D = EnergyParticleSystem2D.lock()) {
		// учитываем в эмблеме энергии, сколько у нас ее (визуально меняем вид эмблемы)
		sharedEnergyParticleSystem2D->ParticlesPerSec =
				(unsigned int)(50 * (CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem)));
		if (sharedEnergyParticleSystem2D->ParticlesPerSec == 0)
			sharedEnergyParticleSystem2D->ParticlesPerSec = 1;
	}
	if (auto sharedPlayerFighter = PlayerFighter.lock()) {
		if (auto sharedLifeParticleSystem2D = LifeParticleSystem2D.lock()) {
			sharedLifeParticleSystem2D->ColorStart.r = 1.00f;
			sharedLifeParticleSystem2D->ColorStart.g = 0.60f * (sharedPlayerFighter->ArmorCurrentStatus / sharedPlayerFighter->ArmorInitialStatus);
			sharedLifeParticleSystem2D->ColorStart.b = 0.20f * (sharedPlayerFighter->ArmorCurrentStatus / sharedPlayerFighter->ArmorInitialStatus);
			// если меньше 10% нужно бить тревогу
			if (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus / 10.0f) {
				sharedLifeParticleSystem2D->AlphaStart = CurrentAlert2;
				sharedLifeParticleSystem2D->AlphaEnd = CurrentAlert2;
			} else { // подчинились, восстанавливаем данные
				sharedLifeParticleSystem2D->AlphaStart = 1.0f;
				sharedLifeParticleSystem2D->AlphaEnd = 1.0f;
			}
		}

		if (auto sharedLife2ParticleSystem2D = Life2ParticleSystem2D.lock()) {
			sharedLife2ParticleSystem2D->ColorStart.r = 1.00f;
			sharedLife2ParticleSystem2D->ColorStart.g = 0.60f * (sharedPlayerFighter->ArmorCurrentStatus / sharedPlayerFighter->ArmorInitialStatus);
			sharedLife2ParticleSystem2D->ColorStart.b = 0.20f * (sharedPlayerFighter->ArmorCurrentStatus / sharedPlayerFighter->ArmorInitialStatus);
			// если меньше 10% нужно бить тревогу
			if (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus / 10.0f) {
				if (CurrentAlert2 > 0.6f) {
					sharedLife2ParticleSystem2D->AlphaStart = CurrentAlert2;
					sharedLife2ParticleSystem2D->AlphaEnd = CurrentAlert2;
				} else {
					sharedLife2ParticleSystem2D->AlphaStart = 0.00f;
					sharedLife2ParticleSystem2D->AlphaEnd = 0.00f;
				}
			} else { // подчинились, восстанавливаем данные
				sharedLife2ParticleSystem2D->AlphaStart = 1.00f;
				sharedLife2ParticleSystem2D->AlphaEnd = 1.00f;
			}
		}

		if (auto sharedLife3ParticleSystem2D = Life3ParticleSystem2D.lock()) {
			sharedLife3ParticleSystem2D->ColorStart.r = 1.00f;
			sharedLife3ParticleSystem2D->ColorStart.g = 0.60f * (sharedPlayerFighter->ArmorCurrentStatus / sharedPlayerFighter->ArmorInitialStatus);
			sharedLife3ParticleSystem2D->ColorStart.b = 0.20f * (sharedPlayerFighter->ArmorCurrentStatus / sharedPlayerFighter->ArmorInitialStatus);

			// если меньше 10% нужно бить тревогу
			if (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus / 10.0f) {
				if (CurrentAlert2 > 0.6f) {
					sharedLife3ParticleSystem2D->AlphaStart = CurrentAlert2;
					sharedLife3ParticleSystem2D->AlphaEnd = CurrentAlert2;
				} else {
					sharedLife3ParticleSystem2D->AlphaStart = 0.00f;
					sharedLife3ParticleSystem2D->AlphaEnd = 0.00f;
				}
			} else { // подчинились, восстанавливаем данные
				sharedLife3ParticleSystem2D->AlphaStart = 1.00f;
				sharedLife3ParticleSystem2D->AlphaEnd = 1.00f;
			}
		}
	}

	vw_UpdateAllParticleSystems2D(vw_GetTimeThread(0));
	vw_DrawAllParticleSystems2D();
}

} // astromenace namespace
} // viewizard namespace
