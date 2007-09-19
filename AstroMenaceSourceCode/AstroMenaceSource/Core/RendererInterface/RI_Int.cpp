/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: RI_Int.cpp

	Copyright (c) 2003-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "RendererInterface.h"






//------------------------------------------------------------------------------------
// делаем скриншот
//------------------------------------------------------------------------------------
int vw_Screenshot(SDL_Surface *screen, char *filename)
{
        SDL_Surface *temp;
        unsigned char *pixels;
        int i;

        if (!(screen->flags & SDL_OPENGL))
        {
                SDL_SaveBMP(screen, filename);
                return 0;
        }

        temp = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                    screen->w, screen->h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
        0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
        );
        if (temp == NULL) return -1;

        pixels = (unsigned char *)malloc(3 * screen->w * screen->h);
        if (pixels == NULL) {
                SDL_FreeSurface(temp);
                return -1;
        }

        glReadPixels(0, 0, screen->w, screen->h,
                     GL_RGB, GL_UNSIGNED_BYTE, pixels);

        for (i=0; i<screen->h; i++)
                memcpy(((char *) temp->pixels) + temp->pitch * i,
                       pixels + 3*screen->w * (screen->h-i-1),
                       screen->w*3);
        free(pixels);

        SDL_SaveBMP(temp, filename);
        SDL_FreeSurface(temp);
        return 0;
}
