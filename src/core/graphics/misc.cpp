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

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "graphics.h"

namespace viewizard {

/*
 * Create screenshot from current OpenGL surface.
 */
int vw_Screenshot(int Width, int Height, const std::string &FileName)
{
    // fixed for BMP format, supported by libSDL
    constexpr uint8_t BitsPerPixels = 24;
    constexpr uint8_t ChannelsNumber = 3;

    SDL_Surface *tmpSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, BitsPerPixels,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                                                   0x000000FF, 0x0000FF00, 0x00FF0000,
#else
                                                   0x00FF0000, 0x0000FF00, 0x000000FF,
#endif
                                                   0);
    if (!tmpSurface) {
        return ERR_MEM;
    }

    // std::unique_ptr, we need only memory allocation without container's features
    // don't use std::vector here, since it allocates AND value-initializes
    std::unique_ptr<uint8_t[]> Pixels{new uint8_t[ChannelsNumber * Width * Height]};

    glReadPixels(0, 0, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, Pixels.get());

    // flip pixels in proper order
    for (int i = 0; i < Height; i++) {
        memcpy((uint8_t *)tmpSurface->pixels + tmpSurface->pitch * i,
               Pixels.get() + ChannelsNumber * Width * (Height - i - 1),
               Width * ChannelsNumber);
    }

    SDL_SaveBMP(tmpSurface, FileName.c_str());
    SDL_FreeSurface(tmpSurface);
    return 0;
}

} // viewizard namespace
