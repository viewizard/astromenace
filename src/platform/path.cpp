/****************************************************************************

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

*****************************************************************************/

// NOTE in future, use std::filesystem::create_directory() instead of mkdir() (since C++17)

// TODO (GetDesktopPath(), macOS) probably, we should use NSHomeDirectory() here (not tested)

// TODO (GetDesktopPath(), Windows) for new Windows versions (since Vista), SHGetKnownFolderPath() should be used
//      SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_DEFAULT, nullptr, &wszPath);

#include "../core/base.h"
#include "../build_config.h"
#include "SDL2/SDL.h"
#include <string.h>

#if defined(WIN32)
#include <shlobj.h>
#include <codecvt>
#endif // WIN32

#if defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/stat.h> // mkdir
#endif // unix

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

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

std::string RawDataPath{};

} // unnamed namespace


/*
 * Get configuration path for current platform.
 */
const std::string &GetConfigPath()
{
    static std::string ConfigPath{};

    if (!ConfigPath.empty()) {
        return ConfigPath;
    }

    // by some reason, SDL use XDG_CONFIG_DATA for preferences/configs, so,
    // we are forced to use own code instead of SDL_GetPrefPath() for unix
#ifdef __unix
    // act accordingly to XDG Base Directory Specification
    // "$XDG_CONFIG_HOME" > "$HOME/.config"
    const char *tmpEnvCH = std::getenv("XDG_CONFIG_HOME");
    if (tmpEnvCH) {
        ConfigPath = tmpEnvCH;
    } else {
        const char *tmpEnvH = std::getenv("HOME");
        if (tmpEnvH) {
            ConfigPath = tmpEnvH;
            ConfigPath += "/.config";

            struct stat st;
            if (stat(ConfigPath.c_str(), &st) == -1) {
                mkdir(ConfigPath.c_str(), 0755);
            }
        } else {
            return ConfigPath;
        }
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
 * Get desktop path for current platform.
 */
const std::string &GetDesktopPath()
{
    static std::string DesktopPath{};

    if (!DesktopPath.empty()) {
        return DesktopPath;
    }

#ifdef __unix
    // act accordingly to XDG Base Directory Specification
    // "$XDG_DESKTOP_DIR" > "$HOME/Desktop"
    const char *tmpEnvDD = std::getenv("XDG_DESKTOP_DIR");
    if (tmpEnvDD) {
        DesktopPath = tmpEnvDD;
    } else {
        const char *tmpEnvH = std::getenv("HOME");
        if (tmpEnvH) {
            DesktopPath = tmpEnvH;
            DesktopPath += "/Desktop";
        }
    }

    // only check, that DesktopPath is available, don't create it
    struct stat st;
    if (stat(DesktopPath.c_str(), &st) == -1) {
        DesktopPath.clear();
    } else {
        DesktopPath += "/";
    }
#elif defined(__APPLE__) && defined(__MACH__)
    const char *tmpEnvH = std::getenv("HOME");
    if (tmpEnvH) {
        DesktopPath = tmpEnvH;
        DesktopPath += "/Desktop";
    }

    // only check, that DesktopPath is available, don't create it
    struct stat st;
    if (stat(DesktopPath.c_str(), &st) == -1) {
        DesktopPath.clear();
    } else {
        DesktopPath += "/";
    }
#elif defined(WIN32)
    WCHAR tmpPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, tmpPath))) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter_wchar;
        DesktopPath = converter_wchar.to_bytes(tmpPath);
        if (DesktopPath.back() != '\\') {
            DesktopPath += "\\";
        }
    }
#endif

    return DesktopPath;
}

/*
 * Get binary location path for current platform.
 */
const std::string &GetBasePath()
{
    static std::string BasePath{};

    if (!BasePath.empty()) {
        return BasePath;
    }

    char *base_path = SDL_GetBasePath();
    if (base_path) {
        BasePath = base_path;
        SDL_free(base_path);
    } else {
        BasePath = "./";
    }

    return BasePath;
}

/*
 * Set path by command line parameter.
 */
static void SetPathByParameter(std::string &Path, char *argv, const std::string &ParameterName)
{
    if (!argv || strlen(argv) <= ParameterName.size()) {
        return;
    }

    Path = argv + ParameterName.size();

#ifdef WIN32
    if (Path.back() != '\\') {
        Path += "\\";
    }
#else
    if (Path.back() != '/') {
        Path += "/";
    }
#endif // WIN32
}

/*
 * Set data path for current platform by command line parameter.
 */
void SetDataPathByParameter(char *argv, const std::string &ParameterName)
{
    SetPathByParameter(DataPath, argv, ParameterName);
}

/*
 * Get data path for current platform.
 */
const std::string &GetDataPath()
{
    if (!DataPath.empty()) {
        return DataPath;
    }

    DataPath = GetBasePath();

    return DataPath;
}

/*
 * Set raw data path for current platform by command line parameter.
 */
void SetRawDataPathByParameter(char *argv, const std::string &ParameterName)
{
    SetPathByParameter(RawDataPath, argv, ParameterName);
}

/*
 * Get raw data path for current platform.
 */
const std::string &GetRawDataPath()
{
    if (!RawDataPath.empty()) {
        return RawDataPath;
    }

    RawDataPath = GetBasePath();
    if (!RawDataPath.empty()) {
        RawDataPath += "gamedata/";
    }

    return RawDataPath;
}

} // astromenace namespace
} // viewizard namespace
