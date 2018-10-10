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

// FIXME if we change game resolution/window size, we should also update particle systems position

// FIXME provide adaptive HUD size for high resolution, care about display dpi

#include "../core/core.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../object3d/space_ship/space_ship.h"
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

std::weak_ptr<cParticleSystem2D> EnergyEmblem{};
std::weak_ptr<cParticleSystem2D> ArmorEmblemCircle{};
std::weak_ptr<cParticleSystem2D> ArmorEmblemHoriz{};
std::weak_ptr<cParticleSystem2D> ArmorEmblemVert{};

float DrawBuffer[(2 + 2 + 4) * 6 * 16]; // RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR = (2 + 2 + 4) * 6 vertices * 16 characters
unsigned int DrawBufferCurrentPosition{0};
GLtexture HUDFontTexture{0};
float HUDFontImageWidth{0.0f};
float HUDFontImageHeight{0.0f};

constexpr unsigned ProgressBarSegmentCount{19};
unsigned int ProgressBarDrawSegments{0};
// RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR  = (2 + 2 + 4) * 6 vertices * (Armor Segments + Energy Segments)
float ProgressBarDrawBuffer[(2 + 2 + 4) * 6 * (ProgressBarSegmentCount + ProgressBarSegmentCount)];
GLtexture ProgressBarTexture{0};
float ProgressBarImageHeight{0.0f};
float ProgressBarImageWidth{0.0f};
float CurrentDrawEnergyStatus{0.0f};
float CurrentDrawArmorStatus{0.0f};

float TimeDelta{0.0f};
uint32_t LastUpdateTick{0};
float Blinking{1.0f};

} // unnamed namespace

// FIXME should be fixed, don't allow global scope interaction for local variables
extern int GamePowerSystem;
extern float CurrentPlayerShipEnergy;

float GetShipMaxEnergy(int Num);


/*
 * Init head-up display particle systems.
 * Make sure we re-init all particle systems data all the time in order to avoid
 * issues with future code changes.
 */
static void InitHUDParticleSystems()
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
static void UpdateHUDParticleSystems(std::weak_ptr<cSpaceShip> &SpaceShip)
{
	if (auto sharedSpaceShip = SpaceShip.lock()) {
		if (auto sharedEnergyEmblem = EnergyEmblem.lock()) {
			sharedEnergyEmblem->ParticlesPerSec =
				1 + static_cast<unsigned>(49 * (CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem)));
		}

		float tmpArmorPercentage = sharedSpaceShip->ArmorCurrentStatus / sharedSpaceShip->ArmorInitialStatus;
		bool tmpLowArmor = (sharedSpaceShip->ArmorCurrentStatus < sharedSpaceShip->ArmorInitialStatus / 10.0f);

		auto BlinkingOnLowArmor = [&] (std::weak_ptr<cParticleSystem2D> &ParticleSystem) {
			auto sharedParticleSystem = ParticleSystem.lock();
			if (!sharedParticleSystem)
				return;

			sharedParticleSystem->ColorStart.r = 1.0f;
			sharedParticleSystem->ColorStart.g = 0.6f * tmpArmorPercentage;
			sharedParticleSystem->ColorStart.b = 0.2f * tmpArmorPercentage;

			if (tmpLowArmor) {
				if (Blinking > 0.6f) {
					sharedParticleSystem->AlphaStart = Blinking;
					sharedParticleSystem->AlphaEnd = Blinking;
				} else {
					sharedParticleSystem->AlphaStart = 0.0f;
					sharedParticleSystem->AlphaEnd = 0.0f;
				}
			} else { // armor could be repaired in-game
				sharedParticleSystem->AlphaStart = 1.0f;
				sharedParticleSystem->AlphaEnd = 1.0f;
			}
		};

		BlinkingOnLowArmor(ArmorEmblemCircle);
		BlinkingOnLowArmor(ArmorEmblemHoriz);
		BlinkingOnLowArmor(ArmorEmblemVert);
	} else {
		auto FadeOut = [] (std::weak_ptr<cParticleSystem2D> &ParticleSystem) {
			auto sharedParticleSystem = ParticleSystem.lock();
			if (!sharedParticleSystem)
				return;

			sharedParticleSystem->AlphaStart = 0.0f;
			sharedParticleSystem->AlphaEnd = 0.0f;
			sharedParticleSystem->ParticlesPerSec = 1;
		};

		FadeOut(EnergyEmblem);
		FadeOut(ArmorEmblemCircle);
		FadeOut(ArmorEmblemHoriz);
		FadeOut(ArmorEmblemVert);
	}

	vw_UpdateAllParticleSystems2D(vw_GetTimeThread(0));
}

/*
 * Draw head-up display particle systems.
 */
static void DrawHUDParticleSystems()
{
	vw_DrawAllParticleSystems2D();
}

/*
 * Resize head-up display particle systems.
 */
static void ResizeHUDParticleSystems()
{
	if (auto sharedArmorEmblemVert = ArmorEmblemVert.lock())
		sharedArmorEmblemVert->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});

	if (auto sharedArmorEmblemHoriz = ArmorEmblemHoriz.lock())
		sharedArmorEmblemHoriz->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});

	if (auto sharedArmorEmblemCircle = ArmorEmblemCircle.lock())
		sharedArmorEmblemCircle->MoveSystem(sVECTOR3D{GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f});
}

/*
 * Draw head-up display border.
 */
static void DrawHUDBorder()
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

/*
 * Sub-image rectangle in head-up display font texture for character.
 */
static void GetHUDCharacterRectangle(char Char, sRECT &Rect)
{
	switch (Char) {
	case '0':
		Rect(232, 4, 245, 25);
		break;
	case '1':
		Rect(71, 4, 84, 25);
		break;
	case '2':
		Rect(88, 4, 101, 25);
		break;
	case '3':
		Rect(106, 4, 119, 25);
		break;
	case '4':
		Rect(124, 4, 137, 25);
		break;
	case '5':
		Rect(142, 4, 155, 25);
		break;
	case '6':
		Rect(160, 4, 173, 25);
		break;
	case '7':
		Rect(178, 4, 191, 25);
		break;
	case '8':
		Rect(196, 4,209,25);
		break;
	case '9':
		Rect(214,4, 227, 25);
		break;

	case 'E': // star, experience symbol
		Rect(47, 4, 66, 25);
		break;
	case 'S': // second $ symbol, not in use
		Rect(4, 4, 21, 25);
		break;
	case '$':
		Rect(25, 4, 41 ,25);
		break;

	case ' ':
		Rect(0, 0, 13, 0);
		break;
	}
}

/*
 * Add data to draw buffer.
 */
static void AddToDrawBuffer(float CoordX, float CoordY,
			    float Alpha, float TextureU, float TextureV,
			    float Buffer[], unsigned &BufferPosition)
{
	static sRGBCOLOR tmpColor{eRGBCOLOR::white};

	Buffer[BufferPosition++] = CoordX;
	Buffer[BufferPosition++] = CoordY;
	Buffer[BufferPosition++] = tmpColor.r;
	Buffer[BufferPosition++] = tmpColor.g;
	Buffer[BufferPosition++] = tmpColor.b;
	Buffer[BufferPosition++] = Alpha;
	Buffer[BufferPosition++] = TextureU;
	Buffer[BufferPosition++] = TextureV;
}

/*
 * Add quad to draw buffer.
 */
static void AddQuadToDrawBuffer(const sRECT &SrcRect, const sRECT &DstRect,
				float Alpha, float ImageWidth, float ImageHeight,
				float Buffer[], unsigned &BufferPosition)
{
	// texture's UV coordinates
	float U_Left{SrcRect.left / ImageWidth};
	float V_Top{SrcRect.top/ImageHeight};
	float U_Right{SrcRect.right / ImageWidth};
	float V_Bottom{SrcRect.bottom / ImageHeight};

	// first triangle
	AddToDrawBuffer(DstRect.left, DstRect.top, Alpha, U_Left, V_Top,
			Buffer, BufferPosition);
	AddToDrawBuffer(DstRect.left, DstRect.bottom, Alpha, U_Left, V_Bottom,
			Buffer, BufferPosition);
	AddToDrawBuffer(DstRect.right, DstRect.bottom, Alpha, U_Right, V_Bottom,
			Buffer, BufferPosition);
	// second triangle
	AddToDrawBuffer(DstRect.right, DstRect.bottom, Alpha, U_Right, V_Bottom,
			Buffer, BufferPosition);
	AddToDrawBuffer(DstRect.right, DstRect.top, Alpha, U_Right, V_Top,
			Buffer, BufferPosition);
	AddToDrawBuffer(DstRect.left, DstRect.top, Alpha, U_Left, V_Top,
			Buffer, BufferPosition);
}

/*
 * Add character data to local draw buffer.
 * Return character width.
 */
static int AddCharToDrawBuffer(char Character, float Xstart, int Ystart,
			       float Alpha, float ImageWidth, float ImageHeight)
{
	sRECT SrcRect;
	GetHUDCharacterRectangle(Character, SrcRect);
	sRECT DstRect{static_cast<int>(Xstart), Ystart,
		      static_cast<int>(Xstart) + SrcRect.right - SrcRect.left, Ystart + SrcRect.bottom - SrcRect.top};

	AddQuadToDrawBuffer(SrcRect, DstRect, Alpha, ImageWidth, ImageHeight,
			    DrawBuffer, DrawBufferCurrentPosition);

	return SrcRect.right - SrcRect.left;
}

/*
 * Add string data to local draw buffer.
 */
static void AddStringToDrawBuffer(const std::string &String, float Xstart, int Ystart,
				  float ImageWidth, float ImageHeight)
{
	// first '0' characters should be transparent for more nice look
	float Transp{0.2f};
	for (auto &tmpCharacter : String) {
		if (tmpCharacter != '0')
			Transp = 1.0f;

		Xstart += AddCharToDrawBuffer(tmpCharacter, Xstart, Ystart,
					      Transp, ImageWidth, ImageHeight);
	}
}

/*
 * Update head-up display experience and money.
 */
void UpdateHUDExpMoney(const int Experience, const int Money)
{
	DrawBufferCurrentPosition = 0;
	float Transp{1.0f};

	AddCharToDrawBuffer('E', GameConfig().InternalWidth / 2 - 57.0f, 5,
			    Transp, HUDFontImageWidth, HUDFontImageHeight);
	AddCharToDrawBuffer('$', GameConfig().InternalWidth / 2 - 56.0f, 31,
			    Transp, HUDFontImageWidth, HUDFontImageHeight);

	std::ostringstream tmpStream;
	tmpStream << std::fixed << std::setprecision(0)
		  << std::setfill('0') << std::setw(7)
		  << Experience;
	AddStringToDrawBuffer(tmpStream.str(),
			      GameConfig().InternalWidth / 2 - 57 + 23.0f, 5,
			      HUDFontImageWidth, HUDFontImageHeight);

	tmpStream.clear();
	tmpStream.str(std::string{});
	tmpStream << std::setfill('0') << std::setw(7)
		  << Money;
	AddStringToDrawBuffer(tmpStream.str(),
			      GameConfig().InternalWidth / 2 - 57 + 23.0f, 31,
			      HUDFontImageWidth, HUDFontImageHeight);
}

/*
 * Init head-up display experience and money.
 */
static void InitHUDExpMoney(const int Experience, const int Money)
{
	HUDFontTexture = GetPreloadedTextureAsset("game/game_num.tga");
	if (HUDFontTexture &&
	    vw_FindTextureSizeByID(HUDFontTexture, &HUDFontImageWidth, &HUDFontImageHeight))
		UpdateHUDExpMoney(Experience, Money);
}

/*
 * Draw head-up display experience and money.
 */
static void DrawHUDExpMoney()
{
	if (!HUDFontTexture ||
	    !DrawBufferCurrentPosition)
		return;

	vw_BindTexture(0, HUDFontTexture);
	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);

	vw_Draw3D(ePrimitiveType::TRIANGLES, 6 * 16, RI_2f_XY | RI_1_TEX | RI_4f_COLOR,
		  DrawBuffer, 8 * sizeof(DrawBuffer[0]));

	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_BindTexture(0, 0);
}

/*
 * Init head-up display energy and armor progress bars.
 */
static void InitHUDProgressBars(std::weak_ptr<cSpaceShip> &SpaceShip)
{
	CurrentDrawEnergyStatus = 1.0f;
	if (GamePowerSystem == 0)
		CurrentDrawEnergyStatus = 0.0f;
	if (auto sharedSpaceShip = SpaceShip.lock())
		CurrentDrawArmorStatus = sharedSpaceShip->ArmorCurrentStatus / sharedSpaceShip->ArmorInitialStatus;

	ProgressBarTexture = GetPreloadedTextureAsset("game/game_panel_el.tga");
	if (!ProgressBarTexture)
		return;

	ProgressBarImageHeight = 0.0f;
	ProgressBarImageWidth = 0.0f;
	vw_FindTextureSizeByID(ProgressBarTexture, &ProgressBarImageWidth, &ProgressBarImageHeight);
}

/*
 * Update head-up display energy and armor progress bars.
 */
static void UpdateHUDProgressBars(std::weak_ptr<cSpaceShip> &SpaceShip)
{
	float EnergyStatus{0.0f};
	float ArmorStatus{0.0f};
	if (auto sharedSpaceShip = SpaceShip.lock()) {
		EnergyStatus = CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem);
		ArmorStatus = sharedSpaceShip->ArmorCurrentStatus / sharedSpaceShip->ArmorInitialStatus;
	}

	// in case of armor and energy progress bars we provide animation,
	// looks much better then instant progress bar status changes
	auto ProgressBarAnimation = [] (float Status, float &CurrentDrawStatus, float AnimationSpeed) {
		if (Status > CurrentDrawStatus) {
			CurrentDrawStatus += GamePowerSystem * AnimationSpeed * TimeDelta;
			if (CurrentDrawStatus > Status)
				CurrentDrawStatus = Status;
		} else if (Status < CurrentDrawStatus) {
			CurrentDrawStatus -= GamePowerSystem * AnimationSpeed * TimeDelta;
			if (CurrentDrawStatus < Status)
				CurrentDrawStatus = Status;
		}
	};
	ProgressBarAnimation(EnergyStatus, CurrentDrawEnergyStatus, 0.5f);
	ProgressBarAnimation(ArmorStatus, CurrentDrawArmorStatus, 0.5f);

	int LastFilledEnergySegment = static_cast<int>(ceil(CurrentDrawEnergyStatus * ProgressBarSegmentCount));
	int LastFilledArmorSegment = static_cast<int>(ceil(CurrentDrawArmorStatus * ProgressBarSegmentCount));

	if (LastFilledArmorSegment + LastFilledEnergySegment <= 0)
		return;

	unsigned int tmpBufferPosition{0};

	for (int i = 0; i < LastFilledEnergySegment; i++) {
		sRECT SrcRect{67 + i * 20, 0, 85 + i * 20, 64};
		sRECT DstRect = SrcRect;

		float Transp = CurrentDrawEnergyStatus * ProgressBarSegmentCount - i;
		if (Transp > 1.0f)
			Transp = 1.0f;

		AddQuadToDrawBuffer(SrcRect, DstRect, Transp,
				    ProgressBarImageWidth, ProgressBarImageHeight,
				    ProgressBarDrawBuffer, tmpBufferPosition);
	}

	for (int i = 0; i < LastFilledArmorSegment; i++) {
		sRECT SrcRect(582 + i * 20, 0, 599 + i * 20, 64);
		sRECT DstRect = SrcRect;
		if (GameConfig().InternalWidth == 1228)
			DstRect(204 + 582 + i * 20, 0, 204 + 599 + i * 20, 64);

		float Transp = CurrentDrawArmorStatus * ProgressBarSegmentCount - i;
		if (Transp > 1.0f)
			Transp = 1.0f;

		AddQuadToDrawBuffer(SrcRect, DstRect, Transp,
				    ProgressBarImageWidth, ProgressBarImageHeight,
				    ProgressBarDrawBuffer, tmpBufferPosition);
	}

	ProgressBarDrawSegments = LastFilledArmorSegment + LastFilledEnergySegment;
}

/*
 * Draw head-up display energy and armor progress bars.
 */
static void DrawHUDProgressBars()
{
	if (!ProgressBarTexture ||
	    !ProgressBarDrawSegments)
		return;

	vw_BindTexture(0, ProgressBarTexture);
	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);

	vw_Draw3D(ePrimitiveType::TRIANGLES, 6 * ProgressBarDrawSegments,
		  RI_2f_XY | RI_1_TEX | RI_4f_COLOR, ProgressBarDrawBuffer, 8 * sizeof(ProgressBarDrawBuffer[0]));

	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_BindTexture(0, 0);
}

/*
 * Init HUD.
 */
void InitHUD(std::weak_ptr<cSpaceShip> &SpaceShip, const int Experience, const int Money)
{
	TimeDelta = 0.0f;
	LastUpdateTick = SDL_GetTicks();

	InitHUDParticleSystems();
	InitHUDExpMoney(Experience, Money);
	InitHUDProgressBars(SpaceShip);
}

/*
 * Draw HUD.
 */
void DrawHUD()
{
	DrawHUDBorder();
	DrawHUDParticleSystems();
	DrawHUDProgressBars();
	DrawHUDExpMoney();
}

/*
 * Update HUD.
 */
void UpdateHUD(std::weak_ptr<cSpaceShip> &SpaceShip)
{
	uint32_t CurrentTick = SDL_GetTicks();
	constexpr uint32_t TicksInSecond{1000}; // connected to SDL_GetTicks()
	TimeDelta = static_cast<float>(CurrentTick - LastUpdateTick) / TicksInSecond;
	Blinking -= 1.9f * TimeDelta;
	if ((Blinking < 0.1f) ||
	    (Blinking > 1.0f))
		Blinking = 1.0f;
	LastUpdateTick = CurrentTick;

	UpdateHUDParticleSystems(SpaceShip);
	UpdateHUDProgressBars(SpaceShip);
}

/*
 * Resize HUD (need this on resolution/window size change).
 */
void ResizeHUD()
{
	ResizeHUDParticleSystems();
}

} // astromenace namespace
} // viewizard namespace
