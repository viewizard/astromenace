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

// TODO we need store previous versions Top Scores and Pilot Profiles,
//      in case player will back to old game version by some reason

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../core/core.h"
#include "../platform/platform.h"
#include "../ui/font.h"
#include "config.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

namespace {

sGameConfig Config;

const std::string ConfigFileName{"config.xml"};
const std::string ProfilesFileName{std::string{"PilotProfiles_"} +
				   std::string{CONFIG_VERSION} +
				   std::string{".data"}};

} // unnamed namespace


/*
 * Get configuration for read only.
 */
const sGameConfig &GameConfig()
{
	return Config;
}

/*
 * Get configuration for read and write.
 */
sGameConfig &ChangeGameConfig()
{
	return Config;
}

/*
 * Configure virtual internal resolution by view size.
 */
void ConfigVirtualInternalResolution()
{
	if (StandardAspectRation({GameConfig().Width, GameConfig().Height})) {
		ChangeGameConfig().InternalWidth = config::VirtualWidth_Standard;
		ChangeGameConfig().InternalHeight = config::VirtualHeight_Standard;
	} else {
		ChangeGameConfig().InternalWidth = config::VirtualWidth_Wide;
		ChangeGameConfig().InternalHeight = config::VirtualHeight_Wide;
	}
	vw_SetInternalResolution(GameConfig().InternalWidth, GameConfig().InternalHeight, true);
}

/*
 * Pack data with XOR.
 * Since we store data in plain text, all data should be 'converted' into proper symbols.
 * In our case, we start from 97 (small 'a', see ASCII table) and store separately
 * tens (0-25, since unsigned char max is 255) and ones (0-9). So, as result we will
 * see string with small letters only.
 */
static void PackWithXOR(std::vector<unsigned char> &DataXOR, int DataSize, unsigned char *Data)
{
	// should be stored: XOR key, 'ten', 'one', and null-terminated symbol at the end of array
	DataXOR.resize(DataSize * 3 + 1);
	for (int i = 0; i < DataSize; i++) {
		int tmpOffset = DataSize + i * 2;
		// XOR key, randomize for each character
		DataXOR[i] = static_cast<unsigned char>(97 + vw_iRandNum(25));
		// XOR
		unsigned char tmpDataXOR = Data[i] ^ DataXOR[i];
		// store 'ten'
		DataXOR[tmpOffset] = static_cast<unsigned char>(97 + tmpDataXOR / 10.0f);
		// store 'one'
		DataXOR[tmpOffset + 1] = static_cast<unsigned char>(97 + (tmpDataXOR - (DataXOR[tmpOffset] - 97) * 10));
	}
	// we should return null-terminated array
	DataXOR[DataSize * 3] = '\0';
}

/*
 * Save Pilot Profiles.
 */
static void SavePilotProfiles()
{
	std::string tmpPilotProfiles{GetConfigPath() + ProfilesFileName};
	std::vector<unsigned char> ProfileXOR{};
	PackWithXOR(ProfileXOR, sizeof(sPilotProfile) * 5, reinterpret_cast<unsigned char *>(Config.Profile));

	SDL_RWops *File = SDL_RWFromFile(tmpPilotProfiles.c_str(), "wb");
	if (!File) {
		std::cerr << __func__ << "(): " << "Can't create file " << tmpPilotProfiles << "\n";
		return;
	}

	SDL_RWwrite(File, ProfileXOR.data(), ProfileXOR.size(), 1);
	SDL_RWclose(File);
}

/*
 * Save configuration file.
 */
void SaveXMLConfigFile()
{
	if (GetConfigPath().empty()) {
		std::cerr << __func__ << "(): config path not set.\n";
		return;
	}

	SavePilotProfiles();

	std::unique_ptr<cXMLDocument> XMLdoc{new cXMLDocument};

	sXMLEntry *RootXMLEntry = XMLdoc->CreateRootEntry("AstroMenaceSettings");

	if (!RootXMLEntry) {
		std::cerr << __func__ << "(): " << "Can't create XML root element.\n";
		return;
	}

	XMLdoc->AddComment(*RootXMLEntry, " AstroMenace game Settings ");

	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MenuLanguage"), "value",
				  vw_GetText("en", Config.MenuLanguage));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VoiceLanguage"), "value",
				  vw_GetText("en", Config.VoiceLanguage));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "FontNumber"), "value", Config.FontNumber);

	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "DisplayIndex"), "value", Config.DisplayIndex);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "Width"), "value", Config.Width);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "Height"), "value", Config.Height);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "Fullscreen"), "value", Config.Fullscreen);

	XMLdoc->AddComment(*RootXMLEntry, " Common settings ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MSAA"), "value", Config.MSAA);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "CSAA"), "value", Config.CSAA);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VisualEffectsQuality"), "value",
				  Config.VisualEffectsQuality);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "AnisotropyLevel"), "value", Config.AnisotropyLevel);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "UseGLSL120"), "value", Config.UseGLSL120);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "ShadowMap"), "value", Config.ShadowMap);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MaxPointLights"), "value", Config.MaxPointLights);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MusicVolume"), "value", Config.MusicVolume);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "SoundVolume"), "value", Config.SoundVolume);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VoiceVolume"), "value", Config.VoiceVolume);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VSync"), "value", Config.VSync);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "Brightness"), "value", Config.Brightness);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "ShowFPS"), "value", Config.ShowFPS);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "GameWeaponInfoType"), "value", Config.GameWeaponInfoType);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "GameSpeed"), "value", Config.GameSpeed);

	XMLdoc->AddComment(*RootXMLEntry, " Control settings ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardLeft"), "value",
				  SDL_GetKeyName(Config.KeyBoardLeft));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardRight"), "value",
				  SDL_GetKeyName(Config.KeyBoardRight));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardUp"), "value",
				  SDL_GetKeyName(Config.KeyBoardUp));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardDown"), "value",
				  SDL_GetKeyName(Config.KeyBoardDown));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardPrimary"), "value",
				  SDL_GetKeyName(Config.KeyBoardPrimary));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardSecondary"), "value",
				  SDL_GetKeyName(Config.KeyBoardSecondary));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MousePrimary"), "value", Config.MousePrimary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MouseSecondary"), "value", Config.MouseSecondary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickPrimary"), "value", Config.JoystickPrimary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickSecondary"), "value", Config.JoystickSecondary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickNum"), "value", Config.JoystickNum);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickDeadZone"), "value", Config.JoystickDeadZone);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "ControlSensivity"), "value", Config.ControlSensivity);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MouseControl"), "value", Config.MouseControl);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "LastProfile"), "value", Config.LastProfile);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MenuScript"), "value", Config.MenuScript);

	XMLdoc->AddComment(*RootXMLEntry, " Tips and Hints settings ");
	for(int i = 0; i < 10; i++) {
		std::string tmpString{"HintStatus" + std::to_string(i + 1)};
		XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, tmpString.c_str()), "value",
					  Config.NeedShowHint[i]);
	}

	XMLdoc->Save(GetConfigPath() + ConfigFileName);
}

/*
 * Unpack data with XOR.
 */
static void UnpackWithXOR(unsigned char *Data, unsigned int DataSize, const std::vector<unsigned char> &DataXOR)
{
	if ((DataXOR.size() / 3) < DataSize) {
		std::cerr << __func__ << "(): " << "DataXOR string less than expected.\n";
		return;
	}

	for (unsigned int i = 0; i < DataSize; i++) {
		int tmpOffset = DataSize + i * 2;
		// Data = XOR key ^ (tens + ones)
		Data[i] = static_cast<unsigned char>(DataXOR[i] ^
						     (((DataXOR[tmpOffset] - 97) * 10) + (DataXOR[tmpOffset + 1] - 97)));
	}
}

/*
 * Check configuration.
 */
static void CheckConfig()
{
	// SDL_GetKeyFromName() return SDLK_UNKNOWN if the name wasn't recognized
	if (Config.KeyBoardUp == SDLK_UNKNOWN)
		Config.KeyBoardUp = SDLK_UP;
	if (Config.KeyBoardDown == SDLK_UNKNOWN)
		Config.KeyBoardDown = SDLK_DOWN;
	if (Config.KeyBoardLeft == SDLK_UNKNOWN)
		Config.KeyBoardLeft = SDLK_LEFT;
	if (Config.KeyBoardRight == SDLK_UNKNOWN)
		Config.KeyBoardRight = SDLK_RIGHT;
	if (Config.KeyBoardPrimary == SDLK_UNKNOWN)
		Config.KeyBoardPrimary = SDLK_LCTRL;
	if (Config.KeyBoardSecondary == SDLK_UNKNOWN)
		Config.KeyBoardSecondary = SDLK_SPACE;

	// std::atoi() - if no conversion can be performed, ​0​ is returned, mouse buttons should starts from 1
	if (Config.MousePrimary == 0)
		Config.MousePrimary = SDL_BUTTON_LEFT;
	if (Config.MouseSecondary == 0)
		Config.MouseSecondary = SDL_BUTTON_RIGHT;

	// note, we don't check JoystickPrimary and JoystickSecondary here (same std::atoi() are used), since 0 is allowed

	if ((Config.FontNumber < 0) || (Config.FontNumber >= GetFontQuantity()))
		Config.FontNumber = 0;
	if (Config.ControlSensivity > 10) // NOTE use std::clamp here (since C++17)
		Config.ControlSensivity = 10;
	if (Config.Brightness > 10) // NOTE use std::clamp here (since C++17)
		Config.Brightness = 10;
	if (Config.JoystickDeadZone > 10) // NOTE use std::clamp here (since C++17)
		Config.JoystickDeadZone = 10;
}

/*
 * Setup current profile and mission from loaded configurations.
 */
static void SetupCurrentProfileAndMission()
{
	if ((Config.LastProfile < 0) ||
	    (Config.LastProfile >= config::MAX_PROFILES))
		return;

	if (Config.Profile[Config.LastProfile].Used)
		CurrentProfile = Config.LastProfile;
	if (CurrentProfile != -1)
		CurrentMission = Config.Profile[Config.LastProfile].LastMission;
}

/*
 * Load Pilot Profiles.
 */
static void LoadPilotProfiles()
{
	std::string tmpPilotProfiles{GetConfigPath() + ProfilesFileName};
	std::vector<unsigned char> ProfileXOR{};

	SDL_RWops *File = SDL_RWFromFile(tmpPilotProfiles.c_str(), "rb");
	if (!File) {
		std::cerr << __func__ << "(): " << "Can't open file " << tmpPilotProfiles << "\n";
		return;
	}

	SDL_RWseek(File, 0, SEEK_END);
	int tmpSize = SDL_RWtell(File);
	SDL_RWseek(File, 0, SEEK_SET);

	ProfileXOR.resize(tmpSize);
	// NOTE remove const_cast in future, (since C++17) "CharT* data();" also added.
	SDL_RWread(File, const_cast<unsigned char *>(ProfileXOR.data()), tmpSize, 1);
	SDL_RWclose(File);

	UnpackWithXOR(reinterpret_cast<unsigned char *>(Config.Profile), sizeof(sPilotProfile) * 5, ProfileXOR);
}

/*
 * Load configuration file.
 */
bool LoadXMLConfigFile(bool NeedResetConfig)
{
	if (GetConfigPath().empty()) {
		std::cerr << __func__ << "(): config path not set.\n";
		return false;
	}

	LoadPilotProfiles();

	// NeedResetConfig, only pilot profiles should be loaded
	if (NeedResetConfig)
		return false;

	std::unique_ptr<cXMLDocument> XMLdoc{new cXMLDocument(GetConfigPath() + ConfigFileName)};

	if (!XMLdoc->GetRootEntry()) {
		SaveXMLConfigFile();
		return true;
	}

	sXMLEntry *RootXMLEntry = XMLdoc->GetRootEntry();

	if (!RootXMLEntry) {
		std::cerr << __func__ << "(): " << "Game configuration file corrupted: " << ConfigFileName << "\n";
		SaveXMLConfigFile();
		return true;
	}
	if ("AstroMenaceSettings" != RootXMLEntry->Name) {
		std::cerr << __func__ << "(): " << "Game configuration file corrupted: " << ConfigFileName << "\n";
		SaveXMLConfigFile();
		return true;
	}

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MenuLanguage")) {
		std::string tmpMenuLanguage{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MenuLanguage"), "value",
					      tmpMenuLanguage)) {
			for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
				if (tmpMenuLanguage == vw_GetText("en", i)) {
					Config.MenuLanguage = i;
					break;
				}
			}
		}
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceLanguage")) {
		std::string tmpVoiceLanguage{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceLanguage"), "value",
					      tmpVoiceLanguage)) {
			for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
				if (tmpVoiceLanguage == vw_GetText("en", i)) {
					Config.VoiceLanguage = i;
					break;
				}
			}
		}
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "FontNumber"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "FontNumber"), "value",
					   Config.FontNumber);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "DisplayIndex"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "DisplayIndex"), "value", Config.DisplayIndex);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "Width"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "Width"), "value", Config.Width);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "Height"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "Height"), "value", Config.Height);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "Fullscreen"))
		XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "Fullscreen"), "value", Config.Fullscreen);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MSAA"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MSAA"), "value", Config.MSAA);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "CSAA"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "CSAA"), "value", Config.CSAA);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VisualEffectsQuality"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VisualEffectsQuality"), "value",
					   Config.VisualEffectsQuality);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "AnisotropyLevel"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "AnisotropyLevel"), "value",
					   Config.AnisotropyLevel);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "UseGLSL120"))
		XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "UseGLSL120"), "value",
					   Config.UseGLSL120);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "ShadowMap"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "ShadowMap"), "value",
					   Config.ShadowMap);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MaxPointLights"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MaxPointLights"), "value",
					   Config.MaxPointLights);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MusicVolume"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MusicVolume"), "value",
					   Config.MusicVolume);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "SoundVolume"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "SoundVolume"), "value",
					   Config.SoundVolume);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceVolume"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceVolume"), "value",
					   Config.VoiceVolume);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VSync"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VSync"), "value",
					   Config.VSync);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "Brightness"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "Brightness"), "value",
					   Config.Brightness);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "ShowFPS"))
		XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "ShowFPS"), "value",
					   Config.ShowFPS);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "GameWeaponInfoType"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "GameWeaponInfoType"), "value",
					   Config.GameWeaponInfoType);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "GameSpeed"))
		XMLdoc->fGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "GameSpeed"), "value",
					   Config.GameSpeed);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardLeft")) {
		std::string tmpKeyBoardLeft{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardLeft"), "value",
					      tmpKeyBoardLeft))
			Config.KeyBoardLeft = SDL_GetKeyFromName(tmpKeyBoardLeft.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardRight")) {
		std::string tmpKeyBoardRight{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardRight"), "value",
					      tmpKeyBoardRight))
			Config.KeyBoardRight = SDL_GetKeyFromName(tmpKeyBoardRight.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardUp")) {
		std::string tmpKeyBoardUp{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardUp"), "value",
					      tmpKeyBoardUp))
			Config.KeyBoardUp = SDL_GetKeyFromName(tmpKeyBoardUp.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardDown")) {
		std::string tmpKeyBoardDown{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardDown"), "value",
					      tmpKeyBoardDown))
			Config.KeyBoardDown = SDL_GetKeyFromName(tmpKeyBoardDown.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardPrimary")) {
		std::string tmpKeyBoardPrimary{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardPrimary"), "value",
					      tmpKeyBoardPrimary))
			Config.KeyBoardPrimary = SDL_GetKeyFromName(tmpKeyBoardPrimary.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardSecondary")) {
		std::string tmpKeyBoardSecondary{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardSecondary"), "value",
					      tmpKeyBoardSecondary))
			Config.KeyBoardSecondary = SDL_GetKeyFromName(tmpKeyBoardSecondary.c_str());
	}

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MousePrimary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MousePrimary"), "value",
					   Config.MousePrimary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MouseSecondary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MouseSecondary"), "value",
					   Config.MouseSecondary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickPrimary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickPrimary"), "value",
					   Config.JoystickPrimary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickSecondary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickSecondary"), "value",
					   Config.JoystickSecondary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickNum"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickNum"), "value",
					   Config.JoystickNum);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickDeadZone"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickDeadZone"), "value",
					   Config.JoystickDeadZone);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "ControlSensivity"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "ControlSensivity"), "value",
					   Config.ControlSensivity);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MouseControl"))
		XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MouseControl"), "value",
					   Config.MouseControl);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "LastProfile"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "LastProfile"), "value",
					   Config.LastProfile);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MenuScript"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MenuScript"), "value",
					   Config.MenuScript);

	for(int i = 0; i < 10; i++) {
		std::string tmpString{"HintStatus" + std::to_string(i + 1)};
		if (XMLdoc->FindEntryByName(*RootXMLEntry, tmpString.c_str()))
			 XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, tmpString.c_str()), "value",
						    Config.NeedShowHint[i]);
	}

	CheckConfig();
	SetupCurrentProfileAndMission();

	return false;
}

/*
 * Game's difficulty in %, calculated by profile settings (result is cached).
 * For more speed, we don't check ProfileNumber for [0, config::MAX_PROFILES) range.
 */
int ProfileDifficulty(int ProfileNumber, eDifficultyAction Action)
{
	auto CalculateDifficulty = [] (unsigned Num) {
		return 150 - ((Config.Profile[Num].EnemyWeaponPenalty - 1) * 12 +
			      (Config.Profile[Num].EnemyArmorPenalty - 1) * 7 +
			      (Config.Profile[Num].EnemyTargetingSpeedPenalty - 1) * 7 +
			      Config.Profile[Num].UnlimitedAmmo * 10 +
			      Config.Profile[Num].UndestroyableWeapon * 7 +
			      Config.Profile[Num].WeaponTargetingMode * 10 +
			      Config.Profile[Num].SpaceShipControlMode * 7);
	};

	auto CalculateDifficulties = [&CalculateDifficulty] () {
		std::array<int, config::MAX_PROFILES> tmpDifficultiesArray{};
		for (int i = 0; i < config::MAX_PROFILES; i++) {
			if (Config.Profile[i].Used)
				tmpDifficultiesArray[i] = CalculateDifficulty(i);
		}
		return tmpDifficultiesArray;
	};

	static std::array<int, config::MAX_PROFILES> DifficultiesArray{CalculateDifficulties()};

	switch (Action) {
	case eDifficultyAction::Get:
		return DifficultiesArray[ProfileNumber];

	case eDifficultyAction::Update:
		DifficultiesArray[ProfileNumber] = CalculateDifficulty(ProfileNumber);
		break;

	case eDifficultyAction::UpdateAll:
		DifficultiesArray = CalculateDifficulties();
		break;
	}

	return 0;
}
