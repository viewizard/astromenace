/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Font3D.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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


#include "../Game.h"



//------------------------------------------------------------------------------------
// прорисовка фонта в 3д пространстве
//------------------------------------------------------------------------------------
void DrawFont3D(float X, float Y, float Z, const char *Text, ...)
{
	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	// собираем текстовую часть
	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);

	if (strlen(text) == 0) return;



	RECT DestRest, SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	float Ystart = 0.0f;
	float Xstart = 0.0f;
	int Ysm = 0;
	int XlenDecr = 0;

	// коэф. ум. размера
	float Resize = 10.0f;
	// размер пробела в пикселях
	float Space = 13.0f;


	eTexture *FontTexture = vw_FindTextureByName("DATA/FONT/font_white.tga");
	vw_SetTextureT(0, FontTexture, 1);

	float *tmp = 0;
	// делаем массив для всех элементов
	// войдет RI_3f_XYZ | RI_2f_TEX
	tmp = new float[4*(3+2)*strlen(text)]; if (tmp == 0) return;




	vw_PushMatrix();

	vw_Translate(VECTOR3D(X, Y, Z));
	VECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);

	// поворачиваем к камере
	vw_Rotate(0, CurrentCameraRotation.y, 0);
	vw_Rotate(CurrentCameraRotation.x, 0, 0);


	// текущая позиция в массиве
	int k=0;
	// реальное кол-во прорисовываемых символов (без пробелов)
	int SymbolsCount = 0;

	// прорисовка текста
	for (unsigned int i=0; i<strlen(text); i++)
	{
		int CurrentChar = text[i];
		// если 2-х байтовое, юникод
		if (text[i]<0)
		{
			Uint8 Temp8[2];
			Temp8[0] = text[i+1];
			Temp8[1] = text[i];
			Uint16 *Temp16 = (Uint16 *)(Temp8);
			CurrentChar = (int)(Temp16[0]);
			i++;
		}

		GetFontData(CurrentChar, &SrcRest, &XlenDecr, &Ysm);

		if (CurrentChar != ' ')
		{
			SymbolsCount++;

			SetRect(&DestRest, 0, 0,
				SrcRest.right-SrcRest.left, SrcRest.bottom-SrcRest.top);

			tmp[k++] = DestRest.left/Resize + Xstart/Resize;	// X
			tmp[k++] = DestRest.bottom/Resize + Ystart/Resize;		// Y
			tmp[k++] = 0.0f;		// Z
			tmp[k++] = SrcRest.left*1.0f/FontTexture->Width;
			tmp[k++] = 1.0f-SrcRest.top*1.0f/FontTexture->Height;

			tmp[k++] = DestRest.left/Resize + Xstart/Resize;	// X
			tmp[k++] = DestRest.top/Resize + Ystart/Resize;		// Y
			tmp[k++] = 0.0f;		// Z
			tmp[k++] = SrcRest.left*1.0f/FontTexture->Width;
			tmp[k++] = 1.0f-SrcRest.bottom*1.0f/FontTexture->Height;

			tmp[k++] = DestRest.right/Resize+ Xstart/Resize;	// X
			tmp[k++] = DestRest.top/Resize + Ystart/Resize;		// Y
			tmp[k++] = 0.0f;		// Z
			tmp[k++] = SrcRest.right*1.0f/FontTexture->Width;
			tmp[k++] = 1.0f-SrcRest.bottom*1.0f/FontTexture->Height;

			tmp[k++] = DestRest.right/Resize + Xstart/Resize;	// X
			tmp[k++] = DestRest.bottom/Resize + Ystart/Resize;		// Y
			tmp[k++] = 0.0f;		// Z
			tmp[k++] = SrcRest.right*1.0f/FontTexture->Width;
			tmp[k++] = 1.0f-SrcRest.top*1.0f/FontTexture->Height;


			Xstart += (SrcRest.right+XlenDecr) - SrcRest.left-1;
		}
		else
			Xstart += Space;
	}



	vw_SendVertices(RI_QUADS, 4*SymbolsCount, RI_3f_XYZ | RI_1_TEX, tmp, 5*sizeof(float));


	vw_PopMatrix();



	if (tmp != 0){delete [] tmp; tmp = 0;}
	vw_SetTextureDef(0);

}





