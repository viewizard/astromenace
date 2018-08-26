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

// TODO codestyle should be fixed

// TODO a lot of code duplication, should be fixed

#include "space_ship.h"
#include "functions.h"
#include "../weapon/functions.h"
#include "../projectile/projectile.h"
#include "../../game.h"
#include "../../config/config.h"
#include "../../script/script.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// all ship list
std::list<std::shared_ptr<cSpaceShip>> ShipList{};

} // unnamed namespace

// FIXME should be fixed, don't allow global scope interaction for local variables
extern bool PlayerFighterLeftEng;
extern bool PlayerFighterRightEng;


/*
 * Create cAlienSpaceFighter object.
 */
std::weak_ptr<cSpaceShip> CreateAlienSpaceFighter(const int SpaceShipNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	ShipList.emplace_front(new cAlienSpaceFighter{SpaceShipNum}, [](cAlienSpaceFighter *p) {delete p;});
	return ShipList.front();
}

/*
 * Create cAlienSpaceMotherShip object.
 */
std::weak_ptr<cSpaceShip> CreateAlienSpaceMotherShip(const int SpaceShipNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	ShipList.emplace_front(new cAlienSpaceMotherShip{SpaceShipNum}, [](cAlienSpaceMotherShip *p) {delete p;});
	return ShipList.front();
}

/*
 * Create cEarthSpaceFighter object.
 */
std::weak_ptr<cSpaceShip> CreateEarthSpaceFighter(const int SpaceShipNum)
{
	ShipList.emplace_front(new cEarthSpaceFighter{SpaceShipNum}, [](cEarthSpaceFighter *p) {delete p;});

	std::weak_ptr<cSpaceShip> tmpSpaceShip = ShipList.front();

	SetEarthSpaceFighterEngine(tmpSpaceShip, 1);
	for (unsigned int i = 0; i < ShipList.front()->Engines.size(); i++) {
		if (auto sharedEngine = ShipList.front()->Engines[i].lock()) {
			// находим кол-во внутренних источников света
			if (!sharedEngine->Light.expired())
				ShipList.front()->InternalLights++;
		}
	}

	// default armor
	SetEarthSpaceFighterArmour(tmpSpaceShip, 0);

	return ShipList.front();
}

/*
 * Create cPirateShip object.
 */
std::weak_ptr<cSpaceShip> CreatePirateShip(const int SpaceShipNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	ShipList.emplace_front(new cPirateShip{SpaceShipNum}, [](cPirateShip *p) {delete p;});
	return ShipList.front();
}

/*
 * Update and remove (erase) dead objects.
 */
void UpdateAllSpaceShip(float Time)
{
	for (auto iter = ShipList.begin(); iter != ShipList.end();) {
		if (!iter->get()->Update(Time))
			iter = ShipList.erase(iter);
		else
			++iter;
	}
}

/*
 * Draw all objects.
 */
void DrawAllSpaceShips(bool VertexOnlyPass, unsigned int ShadowMap)
{
	for (auto &tmpShip : ShipList) {
		tmpShip.get()->Draw(VertexOnlyPass, ShadowMap);
	}
}

/*
 * Release particular space ship.
 */
void ReleaseSpaceShip(std::weak_ptr<cSpaceShip> &Ship)
{
	auto sharedObject = Ship.lock();
	if (!sharedObject)
		return;

	for (auto iter = ShipList.begin(); iter != ShipList.end();) {
		if (iter->get() == sharedObject.get()) {
			ShipList.erase(iter);
			return;
		}
		++iter;
	}
}

/*
 * Release all objects.
 */
void ReleaseAllSpaceShips()
{
	ShipList.clear();
}

/*
 * Cycle for each space ship.
 * Note, caller must guarantee, that 'Object' will not released in callback function call.
 */
void ForEachSpaceShip(std::function<void (cSpaceShip &Object)> function)
{
	for (auto &tmpShip : ShipList) {
		function(*tmpShip);
	}
}

/*
 * Managed cycle for each space ship.
 * Note, caller must guarantee, that 'Object' will not released in callback function call.
 */
void ForEachSpaceShip(std::function<void (cSpaceShip &Object, eShipCycle &Command)> function)
{
	for (auto iter = ShipList.begin(); iter != ShipList.end();) {
		eShipCycle Command{eShipCycle::Continue};
		function(*iter->get(), Command);

		switch (Command) {
		case eShipCycle::Continue:
			++iter;
			break;
		case eShipCycle::Break:
			return;
		case eShipCycle::DeleteObjectAndContinue:
			iter = ShipList.erase(iter);
			break;
		case eShipCycle::DeleteObjectAndBreak:
			ShipList.erase(iter);
			return;
		}
	}
}

/*
 * Managed cycle for each space ship pair.
 * Note, caller must guarantee, that 'FirstObject' and 'SecondObject' will not released in callback function call.
 */
void ForEachSpaceShipPair(std::function<void (cSpaceShip &FirstObject,
					      cSpaceShip &SecondObject,
					      eShipPairCycle &Command)> function)
{
	for (auto iterFirst = ShipList.begin(); iterFirst != ShipList.end();) {
		eShipPairCycle Command{eShipPairCycle::Continue};

		for (auto iterSecond = std::next(iterFirst, 1); iterSecond != ShipList.end();) {
			Command = eShipPairCycle::Continue;
			function(*iterFirst->get(), *iterSecond->get(), Command);

			if ((Command == eShipPairCycle::DeleteSecondObjectAndContinue) ||
			    (Command == eShipPairCycle::DeleteBothObjectsAndContinue))
				iterSecond = ShipList.erase(iterSecond);
			else
				++iterSecond;

			// break second cycle
			if ((Command == eShipPairCycle::DeleteFirstObjectAndContinue) ||
			    (Command == eShipPairCycle::DeleteBothObjectsAndContinue))
				break;
		}

		if ((Command == eShipPairCycle::DeleteFirstObjectAndContinue) ||
		    (Command == eShipPairCycle::DeleteBothObjectsAndContinue))
			iterFirst = ShipList.erase(iterFirst);
		else
			++iterFirst;
	}
}

/*
 * Get object ptr by reference.
 */
std::weak_ptr<cObject3D> GetSpaceShipPtr(const cSpaceShip &Object)
{
	for (auto &tmpShip : ShipList) {
		if (tmpShip.get() == &Object)
			return tmpShip;
	}

	return std::weak_ptr<cObject3D>{};
}

/*
 * Destructor.
 */
cSpaceShip::~cSpaceShip()
{
	for (auto &tmpEngine : Engines) {
		if (auto sharedEngine = tmpEngine.lock()) {
			if (!EngineDestroyType) {
				sharedEngine->IsSuppressed = true;
				sharedEngine->DestroyIfNoParticles = true;
			} else
				vw_ReleaseParticleSystem(sharedEngine);
		}
	}

	if (!EnginesLeft.empty()) {
		for (auto &tmpEngineLeft : EnginesLeft) {
			if (auto sharedEngineLeft = tmpEngineLeft.lock()) {
				if (!EngineDestroyType) {
					sharedEngineLeft->IsSuppressed = true;
					sharedEngineLeft->DestroyIfNoParticles = true;
				} else
					vw_ReleaseParticleSystem(sharedEngineLeft);
			}
		}
	}

	if (!EnginesRight.empty()) {
		for (auto &tmpEngineRight : EnginesRight) {
			if (auto sharedEngineRight = tmpEngineRight.lock()) {
				if (!EngineDestroyType) {
					sharedEngineRight->IsSuppressed = true;
					sharedEngineRight->DestroyIfNoParticles = true;
				} else
					vw_ReleaseParticleSystem(sharedEngineRight);
			}
		}
	}
}

/*
 * Set location.
 */
void cSpaceShip::SetLocation(const sVECTOR3D &NewLocation)
{
	cObject3D::SetLocation(NewLocation);

	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpWeaponSlot.Location + NewLocation);
		}
	}

	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpBossWeaponSlot.Location + NewLocation);
		}
	}

	if (!FlareWeaponSlots.empty()) {
		for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
			if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpFlareWeaponSlot.Location + NewLocation);
		}
	}

	for (unsigned int i = 0; i < Engines.size(); i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			sharedEngine->MoveSystem(NewLocation + EnginesLocation[i]);
			sharedEngine->SetStartLocation(EnginesLocation[i] + NewLocation);
		}
	}

	if (!EnginesLeft.empty()) {
		for (unsigned int i = 0; i < EnginesLeft.size(); i++) {
			if (auto sharedEngineLeft = EnginesLeft[i].lock()) {
				sharedEngineLeft->MoveSystem(NewLocation + EnginesLeftLocation[i]);
				sharedEngineLeft->SetStartLocation(EnginesLeftLocation[i] + NewLocation);
			}
		}
	}

	if (!EnginesRight.empty()) {
		for (unsigned int i = 0; i < EnginesRight.size(); i++) {
			if (auto sharedEngineRight = EnginesRight[i].lock()) {
				sharedEngineRight->MoveSystem(NewLocation + EnginesRightLocation[i]);
				sharedEngineRight->SetStartLocation(EnginesRightLocation[i] + NewLocation);
			}
		}
	}
}

/*
 * Set location.
 */
void cSpaceShip::SetLocationArcadePlayer(const sVECTOR3D &NewLocation)
{
	cObject3D::SetLocation(NewLocation);

	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpWeaponSlot.Location + NewLocation);
		}
	}

	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpBossWeaponSlot.Location + NewLocation);
		}
	}

	if (!FlareWeaponSlots.empty()) {
		for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
			if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpFlareWeaponSlot.Location + NewLocation);
		}
	}

	for (unsigned int i = 0; i < Engines.size(); i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			sharedEngine->MoveSystem(NewLocation + EnginesLocation[i]);
			sharedEngine->SetStartLocation(NewLocation + EnginesLocation[i]);
		}
	}

	if (!EnginesLeft.empty()) {
		for (unsigned int i = 0; i < EnginesLeft.size(); i++) {
			if (auto sharedEngineLeft = EnginesLeft[i].lock()) {
				sharedEngineLeft->MoveSystem(NewLocation + EnginesLeftLocation[i]);
				sharedEngineLeft->SetStartLocation(NewLocation + EnginesLeftLocation[i]);
			}
		}
	}

	if (!EnginesRight.empty()) {
		for (unsigned int i = 0; i < EnginesRight.size(); i++) {
			if (auto sharedEngineRight = EnginesRight[i].lock()) {
				sharedEngineRight->MoveSystem(NewLocation + EnginesRightLocation[i]);
				sharedEngineRight->SetStartLocation(NewLocation + EnginesRightLocation[i]);
			}
		}
	}
}

/*
 * Set rotation.
 */
void cSpaceShip::SetRotation(const sVECTOR3D &NewRotation)
{
	cObject3D::SetRotation(NewRotation);

	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
				vw_Matrix33CalcPoint(tmpWeaponSlot.Location, OldInvRotationMat);
				vw_Matrix33CalcPoint(tmpWeaponSlot.Location, CurrentRotationMat);

				sharedWeapon->SetRotation(NewRotation);
				sharedWeapon->SetLocation(Location + tmpWeaponSlot.Location);
			}
		}
	}

	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock()) {
				vw_Matrix33CalcPoint(tmpBossWeaponSlot.Location, OldInvRotationMat);
				vw_Matrix33CalcPoint(tmpBossWeaponSlot.Location, CurrentRotationMat);

				sharedWeapon->SetRotation(NewRotation);
				sharedWeapon->SetLocation(Location + tmpBossWeaponSlot.Location);
			}
		}
	}

	if (!FlareWeaponSlots.empty()) {
		for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
			if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock()) {
				vw_Matrix33CalcPoint(tmpFlareWeaponSlot.Location, OldInvRotationMat);
				vw_Matrix33CalcPoint(tmpFlareWeaponSlot.Location, CurrentRotationMat);

				sharedWeapon->SetRotation(NewRotation);
				sharedWeapon->SetLocation(Location + tmpFlareWeaponSlot.Location);
			}
		}
	}

	for (unsigned int i = 0; i < Engines.size(); i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			vw_Matrix33CalcPoint(EnginesLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(EnginesLocation[i], CurrentRotationMat);

			if (sharedEngine->SpeedOnCreation == -1.0f) {
				sharedEngine->MoveSystem(EnginesLocation[i] + Location);
				sharedEngine->SetStartLocation(EnginesLocation[i] + Location);
				sharedEngine->RotateSystemAndParticlesByAngle(Rotation);
			} else {
				sharedEngine->MoveSystemLocation(EnginesLocation[i] + Location);
				sharedEngine->RotateSystemByAngle(Rotation);
			}
		}
	}
	if (!EnginesLeft.empty()) {
		for (unsigned int i = 0; i < EnginesLeft.size(); i++) {
			if (auto sharedEngineLeft = EnginesLeft[i].lock()) {
				vw_Matrix33CalcPoint(EnginesLeftLocation[i], OldInvRotationMat);
				vw_Matrix33CalcPoint(EnginesLeftLocation[i], CurrentRotationMat);

				sharedEngineLeft->MoveSystemLocation(EnginesLeftLocation[i] + Location);
				sharedEngineLeft->SetStartLocation(EnginesLeftLocation[i] + Location);
				sharedEngineLeft->RotateSystemByAngle(Rotation);
			}
		}
	}
	if (!EnginesRight.empty()) {
		for (unsigned int i = 0; i < EnginesRight.size(); i++) {
			if (auto sharedEngineRight = EnginesRight[i].lock()) {
				vw_Matrix33CalcPoint(EnginesRightLocation[i], OldInvRotationMat);
				vw_Matrix33CalcPoint(EnginesRightLocation[i], CurrentRotationMat);

				sharedEngineRight->MoveSystemLocation(EnginesRightLocation[i] + Location);
				sharedEngineRight->SetStartLocation(EnginesRightLocation[i] + Location);
				sharedEngineRight->RotateSystemByAngle(Rotation);
			}
		}
	}
}

/*
 * Update.
 */
bool cSpaceShip::Update(float Time)
{
	if (!cObject3D::Update(Time))
		return false;

	if (TimeDelta == 0.0f)
		return true;

	if (!TimeSheetList.empty() &&
	    !TimeSheetList.front().InUse) {
		TimeSheetList.front().InUse = true;

		NeedSpeed = TimeSheetList.front().Speed;
		NeedAcceler = TimeSheetList.front().Acceler;
		NeedSpeedLR = TimeSheetList.front().SpeedLR;
		NeedAccelerLR = TimeSheetList.front().AccelerLR;
		NeedSpeedUD = TimeSheetList.front().SpeedUD;
		NeedAccelerUD = TimeSheetList.front().AccelerUD;

		NeedSpeedByCamFB = TimeSheetList.front().SpeedByCamFB;
		NeedAccelerByCamFB = TimeSheetList.front().AccelerByCamFB;
		NeedSpeedByCamLR = TimeSheetList.front().SpeedByCamLR;
		NeedAccelerByCamLR = TimeSheetList.front().AccelerByCamLR;
		NeedSpeedByCamUD = TimeSheetList.front().SpeedByCamUD;
		NeedAccelerByCamUD = TimeSheetList.front().AccelerByCamUD;

		NeedRotate = TimeSheetList.front().Rotation;
		RotationSpeed = TimeSheetList.front().RotationAcceler;

		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (!tmpWeaponSlot.Weapon.expired())
					tmpWeaponSlot.SetFire = TimeSheetList.front().Fire;
			}
		}
		if (!BossWeaponSlots.empty()) {
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (!tmpBossWeaponSlot.Weapon.expired())
					tmpBossWeaponSlot.SetFire = TimeSheetList.front().BossFire;
			}
		}
	}

	if (!FlareWeaponSlots.empty()) {
		bool NeedFlare{false};
		ForEachProjectile([&] (cProjectile &Projectile) {
			// homing missile or homing mine targeted on this ship
			auto sharedTarget = Projectile.Target.lock();
			if (sharedTarget.get() == this) {
				NeedFlare = true;
				Projectile.Target.reset(); // reset target, since we will fire flares
			}
		});

		if (NeedFlare) {
			for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
				if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock())
					sharedWeapon->WeaponFire(Time);
			}
		}
	}

	if (ObjectStatus == eObjectStatus::Player) {
		if ((MoveForward > 0.0f) ||
		    (MoveBackward > 0.0f)) {
			NeedSpeed = MaxSpeed * MoveForward - MaxSpeed * MoveBackward;
		} else
			NeedSpeed = 0.0f;

		if ((MoveLeft > 0.0f) ||
		    (MoveRight > 0.0f)) {
			NeedRotate.y = MaxAcceler * MaxSpeed * MoveRight - MaxAcceler * MaxSpeed * MoveLeft;
		} else
			NeedRotate.y = 0.0f;

		if ((MoveUp > 0.0f) ||
		    (MoveDown > 0.0f)) {
			NeedRotate.x = MaxAcceler * MaxSpeed * MoveUp - MaxAcceler * MaxSpeed * MoveDown;
		} else
			NeedRotate.x = 0.0f;
	}

	if (ObjectStatus != eObjectStatus::Player) {

		if (!EnginesLeft.empty()) {
			for (auto &tmpEngineLeft : EnginesLeft) {
				if (auto sharedEngineLeft = tmpEngineLeft.lock())
					sharedEngineLeft->IsSuppressed = true;
			}
		}
		if (!EnginesRight.empty()) {
			for (auto &tmpEngineRight : EnginesRight) {
				if (auto sharedEngineRight = tmpEngineRight.lock())
					sharedEngineRight->IsSuppressed = true;
			}
		}

		if ((NeedRotate.x != 0.0f) ||
		    (NeedRotate.y != 0.0f) ||
		    (NeedRotate.z != 0.0f)) {
			sVECTOR3D tmpRotate(0.0f, 0.0f, 0.0f);

			if (NeedRotate.x != 0.0f) {
				float Sign{1.0f};
				if (NeedRotate.x < 0.0f)
					Sign = -1.0f;

				tmpRotate.x = Sign * MaxSpeedRotate * RotationSpeed.x * TimeDelta;

				if (Sign == 1.0f) {
					if (tmpRotate.x >= NeedRotate.x) {
						tmpRotate.x = NeedRotate.x;
						NeedRotate.x = 0.0f;
					}
				} else {
					if (tmpRotate.x <= NeedRotate.x) {
						tmpRotate.x = NeedRotate.x;
						NeedRotate.x = 0.0f;
					}
				}

				if (NeedRotate.x != 0.0f)
					NeedRotate.x -= tmpRotate.x;
			}

			if (NeedRotate.y != 0.0f) {
				float Sign{1.0f};
				if (NeedRotate.y < 0.0f)
					Sign = -1.0f;

				tmpRotate.y = Sign * MaxSpeedRotate * RotationSpeed.y * TimeDelta;

				if (Sign == 1.0f) {
					if (tmpRotate.y >= NeedRotate.y) {
						tmpRotate.y = NeedRotate.y;
						NeedRotate.y = 0.0f;
					}
				} else {
					if (tmpRotate.y <= NeedRotate.y) {
						tmpRotate.y = NeedRotate.y;
						NeedRotate.y = 0.0f;
					}
				}

				if (NeedRotate.y != 0.0f)
					NeedRotate.y -= tmpRotate.y;

				if (NeedRotate.y < 0.0f) {
					if (!EnginesLeft.empty()) {
						for (auto &tmpEngineLeft : EnginesLeft) {
							if (auto sharedEngineLeft = tmpEngineLeft.lock())
								sharedEngineLeft->IsSuppressed = false;
						}
					}
				} else {
					if (!EnginesRight.empty()) {
						for (auto &tmpEngineRight : EnginesRight) {
							if (auto sharedEngineRight = tmpEngineRight.lock())
								sharedEngineRight->IsSuppressed = false;
						}
					}
				}
			}

			if (NeedRotate.z != 0.0f) {
				float Sign{1.0f};
				if (NeedRotate.z < 0.0f)
					Sign = -1.0f;

				tmpRotate.z = Sign * MaxSpeedRotate * RotationSpeed.z * TimeDelta;

				if (Sign == 1.0f) {
					if (tmpRotate.z >= NeedRotate.z) {
						tmpRotate.z = NeedRotate.z;
						NeedRotate.z = 0.0f;
					}
				} else {
					if (tmpRotate.z <= NeedRotate.z) {
						tmpRotate.z = NeedRotate.z;
						NeedRotate.z = 0.0f;
					}
				}

				if (NeedRotate.z != 0.0f)
					NeedRotate.z -= tmpRotate.z;
			}

			SetRotation(tmpRotate);

		}
	} else {
		if (!EnginesLeft.empty()) {
			for (auto &tmpEngineLeft : EnginesLeft) {
				if (auto sharedEngineLeft = tmpEngineLeft.lock())
					sharedEngineLeft->IsSuppressed = !PlayerFighterLeftEng;
			}
		}
		if (!EnginesRight.empty()) {
			for (auto &tmpEngineRight : EnginesRight) {
				if (auto sharedEngineRight = tmpEngineRight.lock())
					sharedEngineRight->IsSuppressed = !PlayerFighterRightEng;
			}
		}
	}

	if ((NeedSpeed != 0.0f) ||
	    ((ObjectStatus == eObjectStatus::Player) && (Speed != 0.0f))) {
		float Sign{1.0f};
		if (NeedSpeed < 0.0f)
			Sign = -1.0f;

		if (Sign == 1.0f)
			vw_Clamp(NeedSpeed, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeed, -MaxSpeed, 0.0f);

		if (Sign == 1.0f) {
			if (NeedSpeed < Speed)
				Sign = -1.0f;
		} else {
			if (NeedSpeed > Speed)
				Sign = 1.0f;
		}

		Acceler = Sign * MaxAcceler * NeedAcceler;
		Speed = Speed + Acceler * TimeDelta;

		if (Sign == 1.0f) {
			if (NeedSpeed <= Speed) {
				Speed = NeedSpeed;
				NeedSpeed = 0.0f;
			}
		} else {
			if (NeedSpeed >= Speed) {
				Speed = NeedSpeed;
				NeedSpeed = 0.0f;
			}
		}

		if (Sign == -1.0f) {
			if (!EnginesLeft.empty()) {
				for (auto &tmpEngineLeft : EnginesLeft) {
					if (auto sharedEngineLeft = tmpEngineLeft.lock())
						sharedEngineLeft->IsSuppressed = false;
				}
			}
			if (!EnginesRight.empty()) {
				for (auto &tmpEngineRight : EnginesRight) {
					if (auto sharedEngineRight = tmpEngineRight.lock())
						sharedEngineRight->IsSuppressed = false;
				}
			}
		}
	}

	if (NeedSpeedLR != 0.0f) {
		float Sign{1.0f};
		if (NeedSpeedLR < 0.0f)
			Sign = -1.0f;

		if (Sign == 1.0f) {
			vw_Clamp(NeedSpeedLR, 0.0f, MaxSpeed);
		} else {
			vw_Clamp(NeedSpeedLR, -MaxSpeed, 0.0f);
		}

		if (Sign == 1.0f) {
			if (NeedSpeedLR < SpeedLR) Sign = -1.0f;
		} else {
			if (NeedSpeedLR > SpeedLR) Sign = 1.0f;
		}

		AccelerLR = Sign * MaxAcceler * NeedAccelerLR;
		SpeedLR = SpeedLR + AccelerLR * TimeDelta;

		if (Sign == 1.0f) {
			if (NeedSpeedLR <= SpeedLR) {
				SpeedLR = NeedSpeedLR;
				NeedSpeedLR = 0.0f;
			}
		} else {
			if (NeedSpeedLR >= SpeedLR) {
				SpeedLR = NeedSpeedLR;
				NeedSpeedLR = 0.0f;
			}
		}
	}

	if (NeedSpeedUD != 0.0f) {
		float Sign{1.0f};
		if (NeedSpeedUD < 0.0f)
			Sign = -1.0f;

		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedUD, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedUD, -MaxSpeed, 0.0f);

		if (Sign == 1.0f) {
			if (NeedSpeedUD < SpeedUD)
				Sign = -1.0f;
		} else {
			if (NeedSpeedUD > SpeedUD)
				Sign = 1.0f;
		}

		AccelerUD = Sign * MaxAcceler * NeedAccelerUD;
		SpeedUD = SpeedUD + AccelerUD * TimeDelta;

		if (Sign == 1.0f) {
			if (NeedSpeedUD <= SpeedUD) {
				SpeedUD = NeedSpeedUD;
				NeedSpeedUD = 0.0f;
			}
		} else {
			if (NeedSpeedUD >= SpeedUD) {
				SpeedUD = NeedSpeedUD;
				NeedSpeedUD = 0.0f;
			}
		}
	}

	if (NeedSpeedByCamFB != 0.0f) {
		float Sign{1.0f};
		if (NeedSpeedByCamFB < 0.0f)
			Sign = -1.0f;

		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedByCamFB, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedByCamFB, -MaxSpeed, 0.0f);

		if (Sign == 1.0f) {
			if (NeedSpeedByCamFB < SpeedByCamFB)
				Sign = -1.0f;
		} else {
			if (NeedSpeedByCamFB > SpeedByCamFB)
				Sign = 1.0f;
		}

		AccelerByCamFB = Sign * MaxAcceler * NeedAccelerByCamFB;
		SpeedByCamFB = SpeedByCamFB + AccelerByCamFB * TimeDelta;

		if (Sign == 1.0f) {
			if (NeedSpeedByCamFB <= SpeedByCamFB) {
				SpeedByCamFB = NeedSpeedByCamFB;
				NeedSpeedByCamFB = 0.0f;
			}
		} else {
			if (NeedSpeedByCamFB >= SpeedByCamFB) {
				SpeedByCamFB = NeedSpeedByCamFB;
				NeedSpeedByCamFB = 0.0f;
			}
		}
	}
	if (NeedSpeedByCamLR != 0.0f) {
		float Sign{1.0f};
		if (NeedSpeedByCamLR < 0.0f)
			Sign = -1.0f;

		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedByCamLR, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedByCamLR, -MaxSpeed, 0.0f);

		if (Sign == 1.0f) {
			if (NeedSpeedByCamLR < SpeedByCamLR)
				Sign = -1.0f;
		} else {
			if (NeedSpeedByCamLR > SpeedByCamLR)
				Sign = 1.0f;
		}

		AccelerByCamLR = Sign * MaxAcceler * NeedAccelerByCamLR;
		SpeedByCamLR = SpeedByCamLR + AccelerByCamLR * TimeDelta;

		if (Sign == 1.0f) {
			if (NeedSpeedByCamLR <= SpeedByCamLR) {
				SpeedByCamLR = NeedSpeedByCamLR;
				NeedSpeedByCamLR = 0.0f;
			}
		} else {
			if (NeedSpeedByCamLR >= SpeedByCamLR) {
				SpeedByCamLR = NeedSpeedByCamLR;
				NeedSpeedByCamLR = 0.0f;
			}
		}
	}

	if (NeedSpeedByCamUD != 0.0f) {
		float Sign{1.0f};
		if (NeedSpeedByCamUD < 0.0f)
			Sign = -1.0f;

		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedByCamUD, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedByCamUD, -MaxSpeed, 0.0f);

		if (Sign == 1.0f) {
			if (NeedSpeedByCamUD < SpeedByCamUD)
				Sign = -1.0f;
		} else {
			if (NeedSpeedByCamUD > SpeedByCamUD)
				Sign = 1.0f;
		}

		AccelerByCamUD = Sign * MaxAcceler * NeedAccelerByCamUD;
		SpeedByCamUD = SpeedByCamUD + AccelerByCamUD * TimeDelta;

		if (Sign == 1.0f) {
			if (NeedSpeedByCamUD <= SpeedByCamUD) {
				SpeedByCamUD = NeedSpeedByCamUD;
				NeedSpeedByCamUD = 0.0f;
			}
		} else {
			if (NeedSpeedByCamUD >= SpeedByCamUD) {
				SpeedByCamUD = NeedSpeedByCamUD;
				NeedSpeedByCamUD = 0.0f;
			}
		}
	}

	if (!WeaponSlots.empty()) {
		// volley fire
		if ((WeaponFireType == 1) ||
		    (ObjectStatus == eObjectStatus::Player)) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (tmpWeaponSlot.SetFire)
					if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
						sharedWeapon->WeaponFire(Time);
			}
		} else { // alternate fire
			int PrimCount = 0;
			float PrimTime = 0.0f;
			unsigned FirstWeapon = WeaponSlots.size();
			unsigned LastWeapon = 0;

			WeaponGroupCurrentFireDelay -= TimeDelta;

			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					PrimCount++;
					PrimTime += sharedWeapon->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			}
			if (WeaponGroupCurrentFireNum == -1)
				WeaponGroupCurrentFireNum = FirstWeapon;

			for (unsigned i = 0; i < WeaponSlots.size(); i++)
				if (!WeaponSlots[i].Weapon.expired() &&
				    WeaponSlots[i].SetFire &&
				    (WeaponGroupCurrentFireNum == static_cast<int>(i)) &&
				    (WeaponGroupCurrentFireDelay <= 0.0f)) {
					if (auto sharedWeapon = WeaponSlots[i].Weapon.lock())
						sharedWeapon->WeaponFire(Time);

					WeaponGroupCurrentFireDelay = PrimTime / (PrimCount * PrimCount);
					WeaponGroupCurrentFireNum++;
					if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
						WeaponGroupCurrentFireNum = FirstWeapon;

					if (WeaponSlots[WeaponGroupCurrentFireNum].Weapon.expired()) {
						bool exit{false};
						while (!exit) {
							WeaponGroupCurrentFireNum++;
							if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
								WeaponGroupCurrentFireNum = FirstWeapon;
							if (!WeaponSlots[WeaponGroupCurrentFireNum].Weapon.expired())
								exit = true;
						}
					}
				}
		}
	}
	if (!BossWeaponSlots.empty()) {
		if (BossWeaponFireType == 1) { // volley fire
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (tmpBossWeaponSlot.SetFire)
					if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock())
						sharedWeapon->WeaponFire(Time);
			}
		} else { // alternate fire
			int PrimCount = 0;
			float PrimTime = 0.0f;
			unsigned FirstWeapon = BossWeaponSlots.size();
			unsigned LastWeapon = 0;

			BossWeaponGroupCurrentFireDelay -= TimeDelta;

			for (unsigned i = 0; i < BossWeaponSlots.size(); i++) {
				if (auto sharedWeapon = BossWeaponSlots[i].Weapon.lock()) {
					PrimCount++;
					PrimTime += sharedWeapon->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			}
			if (BossWeaponGroupCurrentFireNum == -1)
				BossWeaponGroupCurrentFireNum = FirstWeapon;

			for (unsigned i = 0; i < BossWeaponSlots.size(); i++) {
				if (!BossWeaponSlots[i].Weapon.expired() &&
				    BossWeaponSlots[i].SetFire &&
				    (BossWeaponGroupCurrentFireNum == static_cast<int>(i)) &&
				    (BossWeaponGroupCurrentFireDelay <= 0.0f)) {
					if (auto sharedWeapon = BossWeaponSlots[i].Weapon.lock())
						sharedWeapon->WeaponFire(Time);

					BossWeaponGroupCurrentFireDelay = PrimTime / (PrimCount * PrimCount);
					BossWeaponGroupCurrentFireNum++;
					if (BossWeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
						BossWeaponGroupCurrentFireNum = FirstWeapon;

					if (BossWeaponSlots[BossWeaponGroupCurrentFireNum].Weapon.expired()) {
						bool exit{false};
						while (!exit) {
							BossWeaponGroupCurrentFireNum++;
							if (BossWeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
								BossWeaponGroupCurrentFireNum = FirstWeapon;
							if (!BossWeaponSlots[BossWeaponGroupCurrentFireNum].Weapon.expired())
								exit = true;
						}
					}
				}
			}
		}
	}

	if (!ShipShake.empty()) {
		ShipShake[0].Update(TimeDelta, [&] (const float ShakeIncrement,
						    const sVECTOR3D &ShakeDirection,
						    const unsigned UNUSED(ShakeChunkNum)) {
			SetRotation(ShakeDirection ^ (ShakeIncrement * 50.0f));
		});
	}

	Velocity = Orientation ^ (Speed * TimeDelta);
	if (fabs(SpeedLR) > 0.01f) {
		sVECTOR3D tmp(SpeedLR * TimeDelta, 0.0f, 0.0f);
		vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
		Velocity += tmp;
	}
	if (fabs(SpeedUD) > 0.01f) {
		sVECTOR3D tmp(0.0f, SpeedUD * TimeDelta, 0.0f);
		vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
		Velocity += tmp;
	}

	if (fabs(SpeedByCamFB) > 0.01f) {
		sVECTOR3D tmp = GameCameraMovement ^ (SpeedByCamFB * TimeDelta);
		Velocity += tmp;
	}
	if (fabs(SpeedByCamLR) > 0.01f) {
		sVECTOR3D tmp = GameCameraMovement ^ (SpeedByCamLR * TimeDelta);
		vw_RotatePoint(tmp, sVECTOR3D{0.0, -90.0f ,0.0f});
		Velocity += tmp;
	}
	if (fabs(SpeedByCamUD) > 0.01f) {
		sVECTOR3D tmp = GameCameraMovement ^ (SpeedByCamUD * TimeDelta);
		vw_RotatePoint(tmp, sVECTOR3D{90.0f, 0.0f, 0.0f});
		Velocity += tmp;
	}

	if ((Velocity.x != 0.0f) ||
	    (Velocity.y != 0.0f) ||
	    (Velocity.z != 0.0f) )
		SetLocation(Location + Velocity);

	if (!Engines.empty()) {
		float tmpSpeed = Speed;
		if (tmpSpeed > 6.0f)
			tmpSpeed = 6.0f;
		else if (tmpSpeed < -6.0f)
			tmpSpeed = -6.0f;
		tmpSpeed /= 2.0f;

		for (auto &tmpEngine : Engines) {
			if (auto sharedEngine = tmpEngine.lock())
				if (sharedEngine->SpeedOnCreation != -1.0f)
					sharedEngine->Speed = sharedEngine->SpeedOnCreation + tmpSpeed;
		}
	}

	bool NeedFire{false};
	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (!tmpWeaponSlot.Weapon.expired() &&
			    tmpWeaponSlot.SetFire) {
				NeedFire = true;
				break;
			}
		}
	}
	bool NeedBossFire = false;
	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (!tmpBossWeaponSlot.Weapon.expired() &&
			    tmpBossWeaponSlot.SetFire) {
				NeedBossFire = true;
				break;
			}
		}
	}

	if ((ObjectStatus == eObjectStatus::Enemy) && NeedFire && !WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
				if (sharedWeapon->NeedRotateOnTargeting) {
					sVECTOR3D tmpNeedAngle{};
					sVECTOR3D tmpTargetLocation{};
					sVECTOR3D tmpWeaponLocation = tmpWeaponSlot.Location +
								      sharedWeapon->FireLocation +
								      Location;
					if (FindTargetLocationWithPrediction(ObjectStatus, tmpWeaponLocation,
									     sharedWeapon->InternalType, tmpTargetLocation) &&
					    GetTurretOnTargetOrientation(tmpWeaponLocation, Rotation, CurrentRotationMat,
									 tmpTargetLocation, tmpNeedAngle))
						sharedWeapon->SetRotation(sVECTOR3D{-sharedWeapon->Rotation.x - tmpNeedAngle.x,
										    0.0f,
										    0.0f});
				}
			}
		}
	}
	// FIXME remove this code duplication (same as above)
	if ((ObjectStatus == eObjectStatus::Enemy) && NeedBossFire && !BossWeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : BossWeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
				if (sharedWeapon->NeedRotateOnTargeting) {
					sVECTOR3D tmpNeedAngle{};
					sVECTOR3D tmpTargetLocation{};
					sVECTOR3D tmpWeaponLocation = tmpWeaponSlot.Location +
								      sharedWeapon->FireLocation +
								      Location;
					if (FindTargetLocationWithPrediction(ObjectStatus, tmpWeaponLocation,
									     sharedWeapon->InternalType, tmpTargetLocation) &&
					    GetTurretOnTargetOrientation(tmpWeaponLocation, Rotation, CurrentRotationMat,
									 tmpTargetLocation, tmpNeedAngle))
						sharedWeapon->SetRotation(sVECTOR3D{-sharedWeapon->Rotation.x - tmpNeedAngle.x,
										    0.0f,
										    0.0f});
				}
			}
		}
	}

	if (ObjectStatus == eObjectStatus::Ally) {
		// weapons 'center' point
		sVECTOR3D WeaponAvLocation(0.0f, 0.0f, 0.0f);
		int UsedWeaponQunt = 0;
		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					WeaponAvLocation = WeaponAvLocation + tmpWeaponSlot.Location + sharedWeapon->FireLocation + Location;
					UsedWeaponQunt++;
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;

		sVECTOR3D NeedAngle = Rotation;

		if (!WeaponSlots.empty()) {
			float TargetingSpeed{1.0f};

			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						NeedAngle = Rotation;
						NeedAngle.y += WeaponSlots[i].YAngle;

						GetWeaponOnTargetOrientation(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation,
									     Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, Rotation,
									     Length, CurrentRotationMat, NeedAngle, true, sharedWeapon->InternalType);

						sVECTOR3D NeedAngleTmp = NeedAngle;

						if (sharedWeapon->Rotation.x < NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x + 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_x > NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;

						}
						if (sharedWeapon->Rotation.x > NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x - 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_x < NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;
						}

						float Min{0.0f};
						float Max{0.0f};
						GetShipWeaponSlotAngle(GameConfig().Profile[CurrentProfile].Ship, i, Min, Max);
						if (sharedWeapon->Rotation.y < NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y + 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_y > NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							if (NeedAngle.y > Max + Rotation.y)
								NeedAngle.y = Max + Rotation.y;
						}
						if (sharedWeapon->Rotation.y > NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y - 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_y < NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							if (NeedAngle.y < Min + Rotation.y)
								NeedAngle.y = Min + Rotation.y;
						}

						if (GameWeaponTargetingMode == 1)
							NeedAngle = NeedAngleTmp;

						if (sharedWeapon->InternalType < 16) { // not missile
							sharedWeapon->SetRotation(sharedWeapon->Rotation ^ (-1));
							sharedWeapon->SetRotation(NeedAngle);
						}
					}
				}
			}
		}
	}

	if (ObjectStatus == eObjectStatus::Player) {
		float TargetingSpeed = static_cast<float>(GameTargetingMechanicSystem);

		// weapons 'center' point
		sVECTOR3D WeaponAvLocation(0.0f, 0.0f, 0.0f);
		int UsedWeaponQunt = 0;
		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					WeaponAvLocation = WeaponAvLocation + tmpWeaponSlot.Location + sharedWeapon->FireLocation + Location;
					UsedWeaponQunt++;
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;

		sVECTOR3D NeedAngle = Rotation;

		if (!WeaponSlots.empty()) {
			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						NeedAngle = Rotation;
						NeedAngle.y += WeaponSlots[i].YAngle;

						switch (GameTargetingSystem) {
						case 1: // for all weapon by height only
							GetWeaponOnTargetOrientation(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation,
										     WeaponAvLocation, Rotation, Length, CurrentRotationMat, NeedAngle, false,
										     sharedWeapon->InternalType);
							break;
						case 2: // for all weapon by height, with correction to target center
							GetWeaponOnTargetOrientation(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation,
										     WeaponAvLocation, Rotation, Length, CurrentRotationMat, NeedAngle, true,
										     sharedWeapon->InternalType);
							break;
						case 3: // for each weapon by height only
							GetWeaponOnTargetOrientation(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation,
										     Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, sharedWeapon->Rotation,
										     Length, sharedWeapon->CurrentRotationMat, NeedAngle, false,
										     sharedWeapon->InternalType);
							break;
						case 4: // for each weapon by height, with correction to target center
							GetWeaponOnTargetOrientation(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation,
										     Location + WeaponSlots[i].Location + sharedWeapon->FireLocation,
										     sVECTOR3D{sharedWeapon->Rotation.x, WeaponSlots[i].YAngle, sharedWeapon->Rotation.z},
										     Length, sharedWeapon->CurrentRotationMat, NeedAngle, true,
										     sharedWeapon->InternalType);
							break;
						}

						sVECTOR3D NeedAngleTmp = NeedAngle;

						if (sharedWeapon->Rotation.x < NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x + 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_x > NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;

						}
						if (sharedWeapon->Rotation.x > NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x - 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_x < NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;
						}

						float Min{0.0f};
						float Max{0.0f};
						GetShipWeaponSlotAngle(GameConfig().Profile[CurrentProfile].Ship, i, Min, Max);
						if (sharedWeapon->Rotation.y > NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y + 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_y > NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							if (NeedAngle.y > Max + sharedWeapon->Rotation.y)
								NeedAngle.y = Max + sharedWeapon->Rotation.y;
						}
						if (sharedWeapon->Rotation.y < NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y - 40.0f * TargetingSpeed * TimeDelta;
							if (NeedAngle_y < NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							if (NeedAngle.y < Min + sharedWeapon->Rotation.y)
								NeedAngle.y = Min + sharedWeapon->Rotation.y;
						}

						if (GameWeaponTargetingMode == 1)
							NeedAngle = NeedAngleTmp;

						sharedWeapon->SetRotation(sharedWeapon->Rotation ^ (-1));
						sharedWeapon->SetRotation(NeedAngle);
					}
				}
			}
		}
	}

	return true;
}

} // astromenace namespace
} // viewizard namespace
