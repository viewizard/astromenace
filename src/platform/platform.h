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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef PLATFORM_RELATED_H
#define PLATFORM_RELATED_H

#include "../core/base.h"

// Get configuration path for current platform.
const std::string &GetConfigPath();

// Get desktop path for current platform.
const std::string &GetDesktopPath();

// Get binary location path for current platform.
// For macOS bundle, return path to resources (see https://wiki.libsdl.org/SDL_GetBasePath).
const std::string &GetBasePath();

// Set data path for current platform by command line parameter.
void SetDataPathByParameter(char *argv, const std::string &ParameterName);
// Get data path for current platform.
const std::string &GetDataPath();

// Set raw data path for current platform by command line parameter.
void SetRawDataPathByParameter(char *argv, const std::string &ParameterName);
// Get raw data path for current platform.
const std::string &GetRawDataPath();

#endif // PLATFORM_RELATED_H
