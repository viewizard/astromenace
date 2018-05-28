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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE in future, use std::filesystem::create_directory() instead of mkdir() (since C++17)

#include "../core/base.h"
#include "../build_config.h"

#ifdef __unix
#include <sys/stat.h> // mkdir
#endif // unix

namespace {

std::string DataPath{
#ifdef DATADIR
	std::string(DATADIR) +
#ifdef WIN32
	std::string("\\")
#else
	std::string("/")
#endif // WIN32
#endif // DATADIR
	};

} // unnamed namespace


/*
 * Get configuration path for current platform.
 */
const std::string &GetConfigPath()
{
	static std::string ConfigPath{};

	if (!ConfigPath.empty())
		return ConfigPath;

	// by some reason, SDL use XDG_CONFIG_DATA for preferences/configs, so,
	// we are forced to use own code instead of SDL_GetPrefPath() for unix
#ifdef __unix
	// act accordingly to XDG Base Directory Specification
	// "$XDG_CONFIG_HOME" > "$HOME/.config"
	const char *tmpEnvCH = std::getenv("XDG_CONFIG_HOME");
	if (tmpEnvCH)
		ConfigPath = tmpEnvCH;
	else {
		const char *tmpEnvH = std::getenv("HOME");
		if (tmpEnvH) {
			ConfigPath = tmpEnvH;
			ConfigPath += "/.config";

			struct stat st;
			if (stat(ConfigPath.c_str(), &st) == -1) {
				mkdir(ConfigPath.c_str(), 0755);
			}
		} else
			return ConfigPath;
	}

	ConfigPath += "/astromenace";

	struct stat st;
	if (stat(ConfigPath.c_str(), &st) == -1) {
		mkdir(ConfigPath.c_str(), 0755);
	}

	ConfigPath += "/";
#else
	char *pref_path = SDL_GetPrefPath("Viewizard", "AstroMenace");
	if (pref_path) {
		ConfigPath = pref_path;
		SDL_free(pref_path);
	}
#endif // unix

	return ConfigPath;
}

/*
 * Get binary location path for current platform.
 */
const std::string &GetBasePath()
{
	static std::string BasePath{};

	if (!BasePath.empty())
		return BasePath;

	char *base_path = SDL_GetBasePath();
	if (base_path) {
		BasePath = base_path;
		SDL_free(base_path);
	} else
		BasePath = "./";

	return BasePath;
}

/*
 * Set data path for current platform by command line parameter.
 */
void SetDataPathByParameter(char *argv, const std::string &ParameterName)
{
	if (!argv ||
	    (strlen(argv) <= ParameterName.size()))
		return;

	DataPath = argv + ParameterName.size();

#ifdef WIN32
	if (DataPath.back() != '\\')
		DataPath += "\\";
#else
	if (DataPath.back() != '/')
		DataPath += "/";
#endif // WIN32
}

/*
 * Get data path for current platform.
 */
const std::string &GetDataPath()
{
	if (!DataPath.empty())
		return DataPath;

	DataPath = GetBasePath();

	return DataPath;
}
