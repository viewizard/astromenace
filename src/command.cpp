/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

#include "command.h"
#include "config/config.h"
#include "object3d/object3d.h"
#include "gfx/star_system.h"
#include "assets/audio.h"
#include "game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Prepare to switch status.
 */
static void PrepareToSwitchStatus() {
	SaveXMLConfigFile();
	ReleaseAllObject3D();
	vw_ReleaseAllParticleSystems();
	vw_ReleaseAllLights();
	StarSystemRelease();
	vw_StopAllSoundsIfAllowed();
}

/*
 * Proceed command.
 */
void cCommand::Proceed()
{
	if (Command_ == eCommand::DO_NOTHING)
		return;

	switch (Command_) {
	case eCommand::SWITCH_TO_MAIN_MENU:
		SetMenu(eMenuStatus::MAIN_MENU);
		break;

	case eCommand::SWITCH_TO_TOP_SCORES:
		SetMenu(eMenuStatus::TOP_SCORES);
		break;

	case eCommand::SWITCH_TO_INTERFACE:
		SetMenu(eMenuStatus::INTERFACE);
		break;

	case eCommand::SWITCH_TO_OPTIONS:
		SetMenu(eMenuStatus::OPTIONS);
		break;

	case eCommand::SWITCH_TO_OPTIONS_ADVANCED:
		SetMenu(eMenuStatus::OPTIONS_ADVANCED);
		break;

	case eCommand::SWITCH_TO_INFORMATION:
		SetMenu(eMenuStatus::INFORMATION);
		break;

	case eCommand::SWITCH_TO_CREDITS:
		SetMenu(eMenuStatus::CREDITS);
		break;

	case eCommand::SWITCH_TO_CONFCONTROL:
		SetMenu(eMenuStatus::CONFCONTROL);
		break;

	case eCommand::SWITCH_TO_PROFILE:
		SetMenu(eMenuStatus::PROFILE);
		break;

	case eCommand::SWITCH_TO_DIFFICULTY:
		SetMenu(eMenuStatus::DIFFICULTY);
		break;

	case eCommand::SWITCH_TO_MISSION:
		SetMenu(eMenuStatus::MISSION);
		break;

	case eCommand::SWITCH_TO_WORKSHOP:
		SetMenu(eMenuStatus::WORKSHOP);
		break;

	case eCommand::SWITCH_FROM_MENU_TO_GAME: // also used for mission restart
		PrepareToSwitchStatus();
		InitGame();
		PlayMusicTheme(eMusicTheme::GAME, 2000, 2000);
		PlayVoicePhrase(eVoicePhrase::PrepareForAction, 1.0f);
		break;

	case eCommand::SWITCH_FROM_GAME_TO_MISSION_MENU:
		PrepareToSwitchStatus();
		InitMenu(eMenuStatus::MISSION);
		PlayMusicTheme(eMusicTheme::MENU, 2000, 2000);
		// FIXME code duplication, see SetMenu()
		vw_ResetWheelStatus();
		// mission list
		StartMission = 0;
		EndMission = 4;
		if (CurrentMission != -1)
			if (CurrentMission > 2) { // should be centered
				StartMission = CurrentMission - 2;
				EndMission = CurrentMission + 2;

				if (CurrentMission >= AllMission - 2) {
					StartMission = AllMission - 5;
					EndMission = AllMission - 1;
				}
			}
		break;

	case eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU:
		PrepareToSwitchStatus();
		InitMenu(eMenuStatus::MAIN_MENU);
		PlayMusicTheme(eMusicTheme::MENU, 2000, 2000);
		break;

	case eCommand::SWITCH_FROM_GAME_TO_CREDITS:
		PrepareToSwitchStatus();
		InitMenu(eMenuStatus::CREDITS);
		PlayMusicTheme(eMusicTheme::CREDITS, 2000, 2000);
		// FIXME code duplication, see SetMenu()
		InitCreditsMenu(vw_GetTimeThread(0));
		break;

	default:
		std::cerr << __func__ << "(): " << "ComBuffer = " << static_cast<int>(Command_) << " ... error!\n";
		break;
	}

	Command_ = eCommand::DO_NOTHING;
}

/*
 * Set command.
 */
void cCommand::Set(eCommand Command)
{
	Command_ = Command;
}

} // astromenace namespace
} // viewizard namespace
