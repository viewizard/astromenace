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

// TODO use internal variable instead of CurrentAlert3

// NOTE should be tested with in-game resolution changes

#include "../core/core.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../object3d/space_ship/space_ship.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

constexpr int ProgressBarWidth{8};
constexpr int ProgressBarHeight{64};
constexpr int WeaponIconWidth{128};
constexpr int WeaponIconHeight{64};

constexpr int SlimBorder{2};
constexpr int SlimSeparator{2};

constexpr int FlatBorder{2};
constexpr int FlatSeparator{2};

constexpr int FullPanelWidth{164};
constexpr int FullPanelHeight{88};
constexpr int FullProgressBarHeight{56};

} // unnamed namespace

// FIXME should be fixed, don't allow global scope interaction for local variables
extern float CurrentAlert3;


/*
 * Calculate weapon ammo progress.
 *            -  |---|  -
 *            |  |   |  |
 *            |  |   |  |- should be calculated by this function
 *            |  |   |  |
 * BarHeight -|  |---|  -
 *            |  |---|
 *            |  |---|
 *            -  |---|
 */
static int WeaponAmmoProgress(std::shared_ptr<cWeapon> &sharedWeapon, int BarHeight)
{
	return (BarHeight * (sharedWeapon->AmmoStart - sharedWeapon->Ammo)) / sharedWeapon->AmmoStart;
}

/*
 * Calculate weapon reload progress.
 *            -  |---|  -
 *            |  |   |  |
 *            |  |   |  |- should be calculated by this function
 *            |  |   |  |
 * BarHeight -|  |---|  -
 *            |  |---|
 *            |  |---|
 *            -  |---|
 */
static int WeaponReloadProgress(std::shared_ptr<cWeapon> &sharedWeapon, float TimeLastUpdate, int BarHeight)
{
	int Reload = BarHeight -
		     static_cast<int>((BarHeight * (TimeLastUpdate - sharedWeapon->LastFireTime)) / sharedWeapon->NextFireTime);

	if ((sharedWeapon->InternalType == 17) && // swarm
	    (sharedWeapon->SwarmNum) > 0) // for swarm, reload will be started after last missile fired
		Reload = 64;
	else if (Reload < 0)
		Reload = 0;

	return Reload;
}

/*
 * Draw slim weapon panel.
 */
static void DrawSlimWeaponPanel(int X, int Y, int AmmoOffsetX, int ReloadOffsetX,
				std::shared_ptr<cWeapon> &sharedWeapon, float TimeLastUpdate)
{
	sRECT SrcRect(0, 0, 2, 2);
	sRECT DstRect(X,
		      Y,
		      X + SlimBorder * 2 + SlimSeparator + ProgressBarWidth * 2,
		      Y + SlimBorder * 2 + ProgressBarHeight);
	GLtexture tmpBlackPoint = GetPreloadedTextureAsset("menu/blackpoint.tga");
	vw_Draw2D(DstRect, SrcRect, tmpBlackPoint, true, 0.2f);

	DstRect(X + SlimBorder + AmmoOffsetX,
		Y + SlimBorder,
		X + SlimBorder + AmmoOffsetX + ProgressBarWidth,
		Y + SlimBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, tmpBlackPoint, true, 0.5f);

	DstRect(X + SlimBorder + ReloadOffsetX,
		Y + SlimBorder,
		X + SlimBorder + ReloadOffsetX + ProgressBarWidth,
		Y + SlimBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, tmpBlackPoint, true, 0.5f);

	bool WeaponDestroyed = (sharedWeapon->ArmorCurrentStatus <= 0.0f);
	if (WeaponDestroyed)
		return;

	int AmmoProgressBar = WeaponAmmoProgress(sharedWeapon, ProgressBarHeight);
	SrcRect(0, AmmoProgressBar, ProgressBarWidth, ProgressBarHeight);
	DstRect(X + SlimBorder + AmmoOffsetX,
		Y + SlimBorder + AmmoProgressBar,
		X + SlimBorder + AmmoOffsetX + ProgressBarWidth,
		Y + SlimBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/ammo.tga"), true, 1.0f);

	int ReloadProgressBar = WeaponReloadProgress(sharedWeapon, TimeLastUpdate, ProgressBarHeight);
	SrcRect(0, ReloadProgressBar, ProgressBarWidth, ProgressBarHeight);
	DstRect(X + SlimBorder + ReloadOffsetX,
		Y + SlimBorder + ReloadProgressBar,
		X + SlimBorder + ReloadOffsetX + ProgressBarWidth,
		Y + SlimBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/energy.tga"), true, 1.0f);
}

/*
 * Draw flat weapon panel.
 */
static void DrawFlatWeaponPanel(int X, int Y, int AmmoOffsetX, int ReloadOffsetX, int IconOffsetX,
				std::shared_ptr<cWeapon> &sharedWeapon, float TimeLastUpdate)
{
	sRECT SrcRect(0, 0, 2, 2);
	sRECT DstRect(X,
		      Y,
		      X + FlatBorder * 2 + FlatSeparator * 2 + ProgressBarWidth * 2 + WeaponIconWidth,
		      Y + FlatBorder * 2 + WeaponIconHeight);
	GLtexture tmpBlackPoint = GetPreloadedTextureAsset("menu/blackpoint.tga");
	vw_Draw2D(DstRect, SrcRect, tmpBlackPoint, true, 0.2f);

	DstRect(X + FlatBorder + AmmoOffsetX,
		Y + FlatBorder,
		X + FlatBorder + AmmoOffsetX + ProgressBarWidth,
		Y + FlatBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, tmpBlackPoint, true, 0.5f);

	DstRect(X + FlatBorder + ReloadOffsetX,
		Y + FlatBorder,
		X + FlatBorder + ReloadOffsetX + ProgressBarWidth,
		Y + FlatBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, tmpBlackPoint, true, 0.5f);

	DstRect(X + FlatBorder + IconOffsetX,
		Y + FlatBorder,
		X + FlatBorder + IconOffsetX + WeaponIconWidth,
		Y + FlatBorder + WeaponIconHeight);
	vw_Draw2D(DstRect, SrcRect, tmpBlackPoint, true, 0.5f);

	SrcRect(0, 0, WeaponIconWidth, WeaponIconHeight);
	GLtexture tmpWeaponStatus = GetPreloadedTextureAsset("menu/weapon_on_icon.tga");

	bool WeaponDestroyed = (sharedWeapon->ArmorCurrentStatus <= 0.0f);
	if (WeaponDestroyed) {
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, CurrentAlert3, 0.0f, sRGBCOLOR{eRGBCOLOR::red});
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);
		return;
	}

	if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse)
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, CurrentAlert3, 0.0f, sRGBCOLOR{0.0f, 1.0f, 1.0f});
	else if (sharedWeapon->Ammo == 0)
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.5f, 0.2f});
	else
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, 1.0f, 0.0f, sRGBCOLOR{eRGBCOLOR::green});

	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);

	int AmmoProgressBar = WeaponAmmoProgress(sharedWeapon, ProgressBarHeight);
	SrcRect(0, AmmoProgressBar, ProgressBarWidth, ProgressBarHeight);
	DstRect(X + FlatBorder + AmmoOffsetX,
		Y + FlatBorder + AmmoProgressBar,
		X + FlatBorder + AmmoOffsetX + ProgressBarWidth,
		Y + FlatBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/ammo.tga"), true, 1.0f);

	int ReloadProgressBar = WeaponReloadProgress(sharedWeapon, TimeLastUpdate, ProgressBarHeight);
	SrcRect(0, ReloadProgressBar, ProgressBarWidth, ProgressBarHeight);
	DstRect(X + FlatBorder + ReloadOffsetX,
		Y + FlatBorder + ReloadProgressBar,
		X + FlatBorder + ReloadOffsetX + ProgressBarWidth,
		Y + FlatBorder + ProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/energy.tga"), true, 1.0f);
}

/*
 * Draw full weapon panel.
 */
static void DrawFullWeaponPanel(int X, int Y, int AmmoOffsetX, int ReloadOffsetX, int IconOffsetX,
				GLtexture PanelBorder,
				std::shared_ptr<cWeapon> &sharedWeapon, float TimeLastUpdate)
{
	sRECT SrcRect(0, 0, FullPanelWidth, FullPanelHeight);
	sRECT DstRect(X,
		      Y,
		      X + FullPanelWidth,
		      Y + FullPanelHeight);
	vw_Draw2D(DstRect, SrcRect, PanelBorder, true, 1.0f);

	SrcRect(0, 0, WeaponIconWidth, WeaponIconHeight);
	DstRect(X + IconOffsetX,
		Y + 12,
		X + IconOffsetX + WeaponIconWidth,
		Y + 12 + WeaponIconHeight);
	GLtexture tmpWeaponStatus = GetPreloadedTextureAsset("menu/weapon_on_icon.tga");

	bool WeaponDestroyed = (sharedWeapon->ArmorCurrentStatus <= 0.0f);
	if (WeaponDestroyed) {
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, CurrentAlert3, 0.0f, sRGBCOLOR{eRGBCOLOR::red});
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);
		return;
	}

	if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse)
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, CurrentAlert3, 0.0f, sRGBCOLOR{0.0f, 1.0f, 1.0f});
	else if (sharedWeapon->Ammo == 0)
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.5f, 0.2f});
	else
		vw_Draw2D(DstRect, SrcRect, tmpWeaponStatus, true, 1.0f, 0.0f, sRGBCOLOR{eRGBCOLOR::green});

	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);

	int AmmoProgressBar = WeaponAmmoProgress(sharedWeapon, FullProgressBarHeight);
	SrcRect(0, AmmoProgressBar, ProgressBarWidth, FullProgressBarHeight);
	DstRect(X + AmmoOffsetX,
		Y + 16 + AmmoProgressBar,
		X + AmmoOffsetX + ProgressBarWidth,
		Y + 16 + FullProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_ammo.tga"), true, 1.0f);

	int ReloadProgressBar = WeaponReloadProgress(sharedWeapon, TimeLastUpdate, FullProgressBarHeight);
	SrcRect(0, ReloadProgressBar, ProgressBarWidth, FullProgressBarHeight);
	DstRect(X + ReloadOffsetX,
		Y + 16 + ReloadProgressBar,
		X + ReloadOffsetX + ProgressBarWidth,
		Y + 16 + FullProgressBarHeight);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_energy.tga"), true, 1.0f);

}

/*
 * Draw left weapon panel.
 */
static void DrawLeftWeaponPanel(std::shared_ptr<cSpaceShip> &sharedSpaceShip,
				std::shared_ptr<cWeapon> &sharedWeapon, int &DrawLevelPos)
{
	switch (GameConfig().WeaponPanelView) {
	case eWeaponPanelView::full:
		DrawFullWeaponPanel(0, 70 + DrawLevelPos * 85,
				    2, 12, 24,
				    GetPreloadedTextureAsset("game/weapon_panel_left.tga"),
				    sharedWeapon, sharedSpaceShip->TimeLastUpdate);
		break;

	case eWeaponPanelView::flat:
		DrawFlatWeaponPanel(0,
				    80 + DrawLevelPos * 70,
				    0,
				    ProgressBarWidth + FlatSeparator,
				    (ProgressBarWidth + FlatSeparator) * 2,
				    sharedWeapon, sharedSpaceShip->TimeLastUpdate);
		break;

	case eWeaponPanelView::slim:
		DrawSlimWeaponPanel(0,
				    80 + DrawLevelPos * 70,
				    0, ProgressBarWidth + SlimSeparator,
				    sharedWeapon, sharedSpaceShip->TimeLastUpdate);
		break;

	case eWeaponPanelView::hide: // suppress warnings, we should never have this case here
		break;
	}

	DrawLevelPos++;
}

/*
 * Draw right weapon panel.
 */
static void DrawRightWeaponPanel(std::shared_ptr<cSpaceShip> &sharedSpaceShip,
				 std::shared_ptr<cWeapon> &sharedWeapon, int &DrawLevelPos)
{
	switch (GameConfig().WeaponPanelView) {
	case eWeaponPanelView::full:
		DrawFullWeaponPanel(GameConfig().InternalWidth - 164,
				    70 + DrawLevelPos * 85,
				    154, 144, 12,
				    GetPreloadedTextureAsset("game/weapon_panel_right.tga"),
				    sharedWeapon, sharedSpaceShip->TimeLastUpdate);
		break;

	case eWeaponPanelView::flat:
		DrawFlatWeaponPanel(GameConfig().InternalWidth -
					(WeaponIconWidth + (ProgressBarWidth + SlimSeparator + SlimBorder) * 2),
				    80 + DrawLevelPos * 70,
				    WeaponIconWidth + ProgressBarWidth + FlatSeparator * 2,
				    WeaponIconWidth + FlatSeparator,
				    0,
				    sharedWeapon, sharedSpaceShip->TimeLastUpdate);
		break;

	case eWeaponPanelView::slim:
		DrawSlimWeaponPanel(GameConfig().InternalWidth - (ProgressBarWidth * 2 + SlimSeparator + SlimBorder * 2),
				    80 + DrawLevelPos * 70,
				    ProgressBarWidth + SlimSeparator,
				    0,
				    sharedWeapon, sharedSpaceShip->TimeLastUpdate);
		break;

	case eWeaponPanelView::hide: // suppress warnings, we should never have this case here
		break;
	}

	DrawLevelPos++;
}

/*
 * Draw in-game weapon panels (part of HUD).
 */
void DrawWeaponPanels(std::weak_ptr<cSpaceShip> &SpaceShip)
{
	auto sharedSpaceShip = SpaceShip.lock();
	if (!sharedSpaceShip ||
	    (static_cast<int>(GameConfig().WeaponPanelView) < static_cast<int>(eWeaponPanelView::hide)) ||
	    (static_cast<int>(GameConfig().WeaponPanelView) > static_cast<int>(eWeaponPanelView::full)))
		return;

	// FIXME move this calculation to the "weapon panels" init, should be called one time only
	float tmpInvMatrix[9];
	memcpy(tmpInvMatrix,
	       sharedSpaceShip->CurrentRotationMat,
	       9 * sizeof(sharedSpaceShip->CurrentRotationMat[0]));
	vw_Matrix33InverseRotate(tmpInvMatrix);

	int RightDrawLevelPos{0};
	int LeftDrawLevelPos{0};
	for (auto &tmpWeaponSlot : sharedSpaceShip->WeaponSlots) {
		// revert back rotation + ship shaking effect, since we need initial weapon slot location
		// FIXME move this calculation to the "weapon panels" init, should be called one time only
		sVECTOR3D tmpInitialLocation = tmpWeaponSlot.Location;
		vw_Matrix33CalcPoint(tmpInitialLocation, tmpInvMatrix);

		if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
			if (tmpInitialLocation.x > -0.01f) // denote a small quantity, which will be taken to zero
				DrawLeftWeaponPanel(sharedSpaceShip, sharedWeapon, LeftDrawLevelPos);
			else
				DrawRightWeaponPanel(sharedSpaceShip, sharedWeapon, RightDrawLevelPos);
		}
	}
}

/*
 * Find next weapon panel view (cycled).
 */
void WeaponPanelViewNext(eWeaponPanelView &Value)
{
	int tmpWeaponPanelView = static_cast<int>(Value);
	tmpWeaponPanelView++;
	if (tmpWeaponPanelView > static_cast<int>(eWeaponPanelView::full))
		tmpWeaponPanelView = static_cast<int>(eWeaponPanelView::hide);
	Value = static_cast<eWeaponPanelView>(tmpWeaponPanelView);
}

/*
 * Find previous weapon panel view (cycled).
 */
void WeaponPanelViewPrev(eWeaponPanelView &Value)
{
	int tmpWeaponPanelView = static_cast<int>(Value);
	tmpWeaponPanelView--;
	if (tmpWeaponPanelView < static_cast<int>(eWeaponPanelView::hide))
		tmpWeaponPanelView = static_cast<int>(eWeaponPanelView::full);
	Value = static_cast<eWeaponPanelView>(tmpWeaponPanelView);
}

} // astromenace namespace
} // viewizard namespace
