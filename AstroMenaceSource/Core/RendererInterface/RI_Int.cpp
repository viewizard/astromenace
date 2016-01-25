/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "RendererInterface.h"






//------------------------------------------------------------------------------------
// делаем скриншот
//------------------------------------------------------------------------------------
int vw_Screenshot(int w, int h, char *filename)
{
        SDL_Surface *temp;
        unsigned char *pixels;

        temp = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
        0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
        );
        if (temp == NULL) return -1;

        pixels = (unsigned char *)malloc(3 * w * h);
        if (pixels == NULL) {
                SDL_FreeSurface(temp);
                return -1;
        }

        glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        for (int i=0; i<h; i++)
                memcpy(((char *) temp->pixels) + temp->pitch * i, pixels + 3*w * (h-i-1), w*3);

        free(pixels);

        SDL_SaveBMP(temp, filename);
        SDL_FreeSurface(temp);
        return 0;
}
