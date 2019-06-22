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

// FIXME ostringstream is not so fast, move all string initialization into setup,
//       all ostringstream-related code should be called only one time in init

// TODO applying changes should be moved out from the vw_BeginRendering()/vw_EndRendering() block

// TODO translate comments

#include "../core/core.h"
#include "../enum.h"
#include "../config/config.h"
#include "../gfx/shadow_map.h"
#include "../assets/texture.h"
#include "../main.h"
#include "../command.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// временные данные для изменения и восстановления
int Options_TexturesAnisotropyLevel;
int Options_UseGLSL120;
int Options_MSAA;
int Options_CSAA;
int Options_ShadowMap;

const char *ButtonQuality[3] = {
    "High",
    "Medium",
    "Low"
};

const char *ShadowButtonQuality[10] = {
    "Off",
    "8x8 PCF" // don't need translation
};

const char *ButtonTextCompression[3] = {
    "Off",
    "S3TC", // don't need translation
    "BPTC" // don't need translation
};


const char *ButtonPointLights[7] = {
    "Disabled",
    "1 Light",
    "2 Lights",
    "3 Lights",
    "4 Lights",
    "5 Lights",
    "6 Lights"
};


const char *ButtonTexturesQuality[3] = {
    "Low",
    "Medium",
    "High"
};






void OptionsAdvMenu(float ContentTransp, float &ButtonTransp1, float &LastButtonUpdateTime1, float &ButtonTransp2, float &LastButtonUpdateTime2)
{
    sRECT SrcRect{0, 0, 2, 2};
    sRECT DstRect{0, 0, static_cast<int>(GameConfig().InternalWidth), 768};
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f * ContentTransp);







    int X1 = GameConfig().InternalWidth / 2 - 375;
    int Y1 = 65;
    int Prir1 = 55;




    // качество визуальных эффектов
    // VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Visual Effects Quality"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, GameConfig().VisualEffectsQuality == 2)) {
        ChangeGameConfig().VisualEffectsQuality++;
        if (GameConfig().VisualEffectsQuality > 2) {
            ChangeGameConfig().VisualEffectsQuality = 0;
        }

        vw_InitParticleSystems(GameConfig().UseGLSL120, GameConfig().VisualEffectsQuality+1.0f);
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, GameConfig().VisualEffectsQuality == 0)) {
        ChangeGameConfig().VisualEffectsQuality--;
        if (GameConfig().VisualEffectsQuality < 0) {
            ChangeGameConfig().VisualEffectsQuality = 2;
        }

        vw_InitParticleSystems(GameConfig().UseGLSL120, GameConfig().VisualEffectsQuality + 1.0f);
    }
    int Size = vw_TextWidthUTF32(vw_GetTextUTF32(ButtonQuality[GameConfig().VisualEffectsQuality]));
    int SizeI = (170 - Size) / 2;//High, Medium, Low
    vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32(ButtonQuality[GameConfig().VisualEffectsQuality]));







    // Максимальное кол-во источников света на 1 объекта
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Point Lights per Object"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, GameConfig().MaxPointLights == 0)) {
        ChangeGameConfig().MaxPointLights--;
        if (GameConfig().MaxPointLights < 0) {
            ChangeGameConfig().MaxPointLights = 0;
        }
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, GameConfig().MaxPointLights == 6)) {
        ChangeGameConfig().MaxPointLights++;
        if (GameConfig().MaxPointLights > 6) {
            ChangeGameConfig().MaxPointLights = 6;
        }
    }
    Size = vw_TextWidthUTF32(vw_GetTextUTF32(ButtonPointLights[GameConfig().MaxPointLights]));
    SizeI = (170-Size)/2;
    vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32(ButtonPointLights[GameConfig().MaxPointLights]));





    // анизотропия
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Anisotropy Level"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, (Options_TexturesAnisotropyLevel == 1) || !vw_DevCaps().MaxAnisotropyLevel)) {
        Options_TexturesAnisotropyLevel >>= 1;
        if (Options_TexturesAnisotropyLevel < 1) {
            Options_TexturesAnisotropyLevel = 1;
        }
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, (Options_TexturesAnisotropyLevel == vw_DevCaps().MaxAnisotropyLevel) || !vw_DevCaps().MaxAnisotropyLevel)) {
        Options_TexturesAnisotropyLevel <<= 1;
        if (Options_TexturesAnisotropyLevel > vw_DevCaps().MaxAnisotropyLevel) {
            Options_TexturesAnisotropyLevel = vw_DevCaps().MaxAnisotropyLevel;
        }
    }
    if (Options_TexturesAnisotropyLevel > 1) {
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << "x" << Options_TexturesAnisotropyLevel;
        Size = vw_TextWidth(tmpStream.str());
        SizeI = (170 - Size) / 2;
        vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, tmpStream.str());
    } else {
        if (vw_DevCaps().MaxAnisotropyLevel > 1) {
            Size = vw_TextWidthUTF32(vw_GetTextUTF32("Off"));
            SizeI = (170-Size)/2;
            vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Off"));
        } else {
            Size = vw_TextWidthUTF32(vw_GetTextUTF32("Not available"));
            SizeI = (170-Size)/2;
            vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, ContentTransp, vw_GetTextUTF32("Not available"));
        }
    }




    //Options_MultiSampleType
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Multisample Antialiasing"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, Options_MSAA == 0 || vw_DevCaps().MultisampleCoverageModes.empty())) {
        // находим текущий режим
        int CurrentMode = 0;
        if (Options_MSAA == 0) {
            CurrentMode = -1;
        } else {
            for (unsigned int i = 0; i < vw_DevCaps().MultisampleCoverageModes.size(); i++) {
                if (vw_DevCaps().MultisampleCoverageModes[i].ColorSamples == Options_MSAA
                    && vw_DevCaps().MultisampleCoverageModes[i].CoverageSamples == Options_CSAA) {
                    CurrentMode = i;
                    break;
                }
            }
        }

        CurrentMode--;
        if (CurrentMode < -1) {
            CurrentMode = vw_DevCaps().MultisampleCoverageModes.size() - 1;
        }

        // -1 - делаем "выключение" антиалиасинга
        if (CurrentMode == -1) {
            Options_MSAA = 0;
            Options_CSAA = 0;
        } else {
            Options_MSAA = vw_DevCaps().MultisampleCoverageModes[CurrentMode].ColorSamples;
            Options_CSAA = vw_DevCaps().MultisampleCoverageModes[CurrentMode].CoverageSamples;
        }
    }
    bool TestStateButton = false;
    if (!vw_DevCaps().MultisampleCoverageModes.empty()) {
        TestStateButton = vw_DevCaps().MultisampleCoverageModes.back().ColorSamples == Options_MSAA
                          && vw_DevCaps().MultisampleCoverageModes.back().CoverageSamples == Options_CSAA;
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, TestStateButton || vw_DevCaps().MultisampleCoverageModes.empty())) {
        // находим текущий режим
        int CurrentMode = 0;
        if (Options_MSAA == 0) {
            CurrentMode = -1;
        } else {
            for (unsigned int i = 0; i < vw_DevCaps().MultisampleCoverageModes.size(); i++) {
                if (vw_DevCaps().MultisampleCoverageModes[i].ColorSamples == Options_MSAA
                    && vw_DevCaps().MultisampleCoverageModes[i].CoverageSamples == Options_CSAA) {
                    CurrentMode = i;
                    break;
                }
            }
        }

        CurrentMode++;
        if (CurrentMode > static_cast<int>(vw_DevCaps().MultisampleCoverageModes.size() - 1)) {
            CurrentMode = -1;
        }

        // -1 - делаем "выключение" антиалиасинга
        if (CurrentMode == -1) {
            Options_MSAA = 0;
            Options_CSAA = 0;
        } else {
            Options_MSAA = vw_DevCaps().MultisampleCoverageModes[CurrentMode].ColorSamples;
            Options_CSAA = vw_DevCaps().MultisampleCoverageModes[CurrentMode].CoverageSamples;
        }
    }
    if (Options_MSAA == 0) {
        if (vw_DevCaps().MultisampleCoverageModes.empty()) {
            Size = vw_TextWidthUTF32(vw_GetTextUTF32("Not available"));
            SizeI = (170-Size)/2;
            vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, ContentTransp, vw_GetTextUTF32("Not available"));
        } else {
            Size = vw_TextWidthUTF32(vw_GetTextUTF32("Off"));
            SizeI = (170-Size)/2;
            vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Off"));
        }
    } else {
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0);
        if (Options_MSAA == Options_CSAA || Options_CSAA == 0) {
            tmpStream << Options_MSAA << "x MS";
        } else {
            tmpStream << Options_CSAA << "x CS/" << Options_MSAA << "x MS";
        }
        Size = vw_TextWidth(tmpStream.str());
        SizeI = (170 - Size) / 2; // Off, 2x, 4x ...
        vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, tmpStream.str());
    }




    // вкл-выкл шейдеров, если они поддерживаются
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("OpenGL Shading Language"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Off"), ContentTransp, !vw_DevCaps().OpenGL_2_0_supported || !vw_DevCaps().OpenGL_2_1_supported || !Options_UseGLSL120)
        || DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("On"), ContentTransp, !vw_DevCaps().OpenGL_2_0_supported || !vw_DevCaps().OpenGL_2_1_supported || Options_UseGLSL120)) {
        Options_UseGLSL120 = !Options_UseGLSL120;
        // если выключены шейдеры - выключаем и тени
        if (!Options_UseGLSL120) {
            Options_ShadowMap = 0;
        } else {
            if (GameConfig().UseGLSL120) {
                Options_ShadowMap = GameConfig().ShadowMap;
            } else {
                // same behaviour as for CheckOpenGLCapabilities()
                if (vw_DevCaps().OpenGL_3_0_supported
                    && vw_DevCaps().MaxTextureWidth >= 2048) {
                    Options_ShadowMap = 1;
                } else {
                    Options_ShadowMap = 0;
                }
            }
        }
    }
    if (vw_DevCaps().OpenGL_2_0_supported && vw_DevCaps().OpenGL_2_1_supported) {
        Size = vw_TextWidthUTF32(Options_UseGLSL120 ? vw_GetTextUTF32("On") : vw_GetTextUTF32("Off"));
        SizeI = (170-Size)/2;
        vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, Options_UseGLSL120 ? vw_GetTextUTF32("On") : vw_GetTextUTF32("Off"));
    } else {
        Size = vw_TextWidthUTF32(vw_GetTextUTF32("Not available"));
        SizeI = (170-Size)/2;
        vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, ContentTransp, vw_GetTextUTF32("Not available"));
    }





    // качество теней
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Shadow Quality"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, Options_ShadowMap == 0 || !vw_DevCaps().OpenGL_2_0_supported || !vw_DevCaps().OpenGL_2_1_supported ||
                        !vw_DevCaps().OpenGL_3_0_supported || !Options_UseGLSL120 || (vw_DevCaps().FramebufferObjectDepthSize < 24))) {
        Options_ShadowMap--;
        if (Options_ShadowMap < 0) {
            Options_ShadowMap = 0;
        }
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, Options_ShadowMap == 1 || !vw_DevCaps().OpenGL_2_0_supported || !vw_DevCaps().OpenGL_2_1_supported ||
                        !vw_DevCaps().OpenGL_3_0_supported || !Options_UseGLSL120 || (vw_DevCaps().FramebufferObjectDepthSize < 24))) {
        Options_ShadowMap++;
        if (Options_ShadowMap > 1) {
            Options_ShadowMap = 1;
        }
    }
    if (vw_DevCaps().OpenGL_2_0_supported && vw_DevCaps().OpenGL_2_1_supported && vw_DevCaps().OpenGL_3_0_supported && Options_UseGLSL120 && (vw_DevCaps().FramebufferObjectDepthSize >= 24)) {
        Size = vw_TextWidthUTF32(vw_GetTextUTF32(ShadowButtonQuality[Options_ShadowMap]));
        float WScale = 0;
        if (Size > 170) {
            Size = 170;
            WScale = -170;
        }
        SizeI = (170-Size)/2;//High, Medium, Low
        vw_DrawTextUTF32(X1+438+SizeI, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32(ShadowButtonQuality[Options_ShadowMap]));
    } else {
        Size = vw_TextWidthUTF32(vw_GetTextUTF32("Not available"));
        SizeI = (170-Size)/2;
        vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, ContentTransp, vw_GetTextUTF32("Not available"));
    }









    int Prir = 100;
    int X;
    int Y = 165+Prir*4;

    X = GameConfig().InternalWidth / 2 - 366;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Interface"), ContentTransp, false)) {
        if (MenuStatus == eMenuStatus::GAME) {
            SetOptionsMenu(eMenuStatus::INTERFACE);
            GameMenuStatus = eGameMenuStatus::INTERFACE;
        } else {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_INTERFACE);
        }
    }

    X = GameConfig().InternalWidth / 2 - 100;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Video & Audio"), ContentTransp, false)) {
        if (MenuStatus == eMenuStatus::GAME) {
            SetOptionsMenu(eMenuStatus::OPTIONS);
            GameMenuStatus = eGameMenuStatus::OPTIONS;
        } else {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_OPTIONS);
        }
    }

    X = GameConfig().InternalWidth / 2 + 166;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Config Controls"), ContentTransp, false)) {
        if (MenuStatus == eMenuStatus::GAME) {
            SetOptionsMenu(eMenuStatus::CONFCONTROL);
            GameMenuStatus = eGameMenuStatus::CONFCONTROL;
        } else {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_CONFCONTROL);
        }
    }








    if (Options_TexturesAnisotropyLevel == GameConfig().AnisotropyLevel
        && Options_MSAA == GameConfig().MSAA
        && Options_CSAA == GameConfig().CSAA
        && Options_UseGLSL120 == GameConfig().UseGLSL120
        && Options_ShadowMap == GameConfig().ShadowMap) {
        X = (GameConfig().InternalWidth - 384) / 2;
        Y = Y+Prir;
        if (MenuStatus == eMenuStatus::GAME) {
            if (DrawButton384(X,Y, vw_GetTextUTF32("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1)) {
                GameMenuStatus = eGameMenuStatus::GAME_MENU;
            }
        } else {
            if (DrawButton384(X,Y, vw_GetTextUTF32("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1)) {
                cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);
            }
        }
    } else {
        X = GameConfig().InternalWidth / 2 - 256 - 38;
        Y = Y+Prir;
        if (MenuStatus == eMenuStatus::GAME) {
            if (DrawButton256(X,Y, vw_GetTextUTF32("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1)) {
                GameMenuStatus = eGameMenuStatus::GAME_MENU;
            }
        } else {
            if (DrawButton256(X,Y, vw_GetTextUTF32("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1)) {
                cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);
            }
        }
        X = GameConfig().InternalWidth / 2 + 38;
        if (DrawButton256(X,Y, vw_GetTextUTF32("APPLY"), ContentTransp, ButtonTransp2, LastButtonUpdateTime2)) {
            if (GameConfig().MSAA != Options_MSAA || GameConfig().CSAA != Options_CSAA) {
                ChangeGameConfig().MSAA = Options_MSAA;
                ChangeGameConfig().CSAA = Options_CSAA;
                vw_InitOpenGLStuff(GameConfig().Width, GameConfig().Height,
                                   &ChangeGameConfig().MSAA, &ChangeGameConfig().CSAA);
            }
            if (GameConfig().AnisotropyLevel != Options_TexturesAnisotropyLevel) {
                ChangeGameConfig().AnisotropyLevel = Options_TexturesAnisotropyLevel;
                ChangeTexturesAnisotropyLevel();
            }
            if (GameConfig().ShadowMap != Options_ShadowMap) {
                ChangeGameConfig().ShadowMap = Options_ShadowMap;
                ShadowMap_SizeSetup(eShadowMapSetup::Menu);
            }

            // проверяем, нужно перегружать или нет
            if (Options_UseGLSL120 != GameConfig().UseGLSL120) {
                if (MenuStatus == eMenuStatus::GAME) {
                    SetCurrentDialogBox(eDialogBox::RestartOnAdvOptChanged);
                } else {
                    RecreateGameWindow();
                    SaveOptionsAdvMenuTmpData();
                }
            }
        }
    }
}

void SaveOptionsAdvMenuTmpData()
{
    ChangeGameConfig().UseGLSL120 = Options_UseGLSL120;
}

} // astromenace namespace
} // viewizard namespace
