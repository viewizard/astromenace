/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: TextureManager.cpp

	Copyright (c) 2001-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

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


#include "Texture.h"
#include "../RendererInterface/RendererInterface.h"



//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
eTexture *StartTexMan = 0;// Указатель на первую текстуру в списке...
eTexture *EndTexMan = 0;	// Указатель на последнюю текстуру в списке...
int NumTexMan = 0;		// Последний использов. уникальный номер
// Ключ прорисовки текстуры (near, linear, ... )
int FilteringTexMan = RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT;
// Ключ прорисовки текстуры (wrap, clamp ... )
int Address_ModeTexMan = RI_WRAP_U | RI_WRAP_V;
// указывает, что канал есть...(или нужно создать по цвету...)
bool AlphaTexMan = false;
// цвет прозрачности для создания Alpha канала...
BYTE ARedTexMan = 0;
BYTE AGreenTexMan = 0;
BYTE ABlueTexMan = 0;
int  AFlagTexMan = TX_ALPHA_EQUAL;

bool MipMap = true;



// чтобы при работе не было сбоев в последовательности текстур... блокируем этот участок
// при создании и удалении текстур
bool texturebusy = false;






//------------------------------------------------------------------------------------
// проверка приоритета текстур
//------------------------------------------------------------------------------------
void vw_CheckTexturesPrior()
{
	eTexture *Tmp = StartTexMan;

	// находим эквивалент еденице
	float MaxCount = 0.0f;
	while (Tmp != 0)
	{
		eTexture *Tmp1 = Tmp->Next;
		if (Tmp->TexturePrior > 0.0f)
		if (Tmp->Width*Tmp->Height > MaxCount) MaxCount = Tmp->Width*Tmp->Height*1.0f;
		Tmp = Tmp1;
	}


	// устанавливаем правильный приоритет
	Tmp = StartTexMan;
	while (Tmp != 0)
	{
		eTexture *Tmp1 = Tmp->Next;
		float CurrPr = 0.0f;
		vw_GetPrioritizeTextures(Tmp->TextureID, &CurrPr);

		// если не вызывали текстуру, у нее приоритет нулевой
		float NeedPr = 0.0f;
		if (Tmp->TexturePrior > 0.0f)
		{
			NeedPr = (Tmp->Width*Tmp->Height*1.0f)/MaxCount;
		}
		if (NeedPr != CurrPr)
			vw_SetPrioritizeTextures(Tmp->TextureID, NeedPr);

		Tmp->TexturePrior = 0.0f;

		Tmp = Tmp1;
	}

}







//------------------------------------------------------------------------------------
// освобождаем все текстуры подключенные к менеджеру текстур
//------------------------------------------------------------------------------------
void vw_ReleaseAllTextures()
{
	// Чистка памяти...
	eTexture *Tmp = StartTexMan;
	while (Tmp != 0)
	{
		eTexture *Tmp1 = Tmp->Next;
		vw_ReleaseTexture(Tmp);
		Tmp = Tmp1;
	}


	StartTexMan = 0;
	EndTexMan = 0;
	NumTexMan = 0;

	FilteringTexMan = RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT;
	Address_ModeTexMan = RI_WRAP_U | RI_WRAP_V;
	AlphaTexMan = false;
}







//------------------------------------------------------------------------------------
// подключение текстуры к менеджеру текстур
//------------------------------------------------------------------------------------
void AttachTexture(eTexture* Texture)
{
	if (Texture == 0) return;

	// возможно используется многопоточность, смотрим чтобы не портить данные
	while (texturebusy) SDL_Delay(1);
	// говорим, что работаем с это процедурой
	texturebusy = true;

	// первый в списке...
	if (EndTexMan == 0)
	{
		Texture->Prev = 0;
		Texture->Next = 0;
		NumTexMan += 1;
		Texture->Num = NumTexMan;
		StartTexMan = Texture;
		EndTexMan = Texture;
	}
	else // продолжаем заполнение...
	{
		Texture->Prev = EndTexMan;
		Texture->Next = 0;
		EndTexMan->Next = Texture;
		NumTexMan += 1;
		Texture->Num = NumTexMan;
		EndTexMan = Texture;
	}

	texturebusy = false;

}





//------------------------------------------------------------------------------------
// отключение текстуры от менеджера текстур
//------------------------------------------------------------------------------------
void DetachTexture(eTexture* Texture)
{
	if (Texture == 0) return;

	// возможно используется многопоточность, смотрим чтобы не портить данные
	while (texturebusy) SDL_Delay(1);
	// говорим, что работаем с это процедурой
	texturebusy = true;

	// переустанавливаем указатели...
	if (StartTexMan == Texture) StartTexMan = Texture->Next;
	if (EndTexMan == Texture) EndTexMan = Texture->Prev;


	if (Texture->Next != 0) Texture->Next->Prev = Texture->Prev;
		else if (Texture->Prev != 0) Texture->Prev->Next = 0;
	if (Texture->Prev != 0) Texture->Prev->Next = Texture->Next;
		else if (Texture->Next != 0) Texture->Next->Prev = 0;

	texturebusy = false;
}








//------------------------------------------------------------------------------------
// Установка свойств текстур...
//------------------------------------------------------------------------------------
void vw_SetTextureProp(int nFiltering, int nAddress_Mode, bool nAlpha, int nAFlag, bool nMipMap)
{
	FilteringTexMan = nFiltering;
	Address_ModeTexMan = nAddress_Mode;
	AlphaTexMan = nAlpha;
	AFlagTexMan = nAFlag;
	MipMap = nMipMap;
}








//------------------------------------------------------------------------------------
// Установка цвета альфа канала
//------------------------------------------------------------------------------------
void vw_SetTextureAlpha(BYTE nARed, BYTE nAGreen, BYTE nABlue)
{
	ARedTexMan = nARed;
	AGreenTexMan = nAGreen;
	ABlueTexMan = nABlue;
}









//------------------------------------------------------------------------------------
// Нахождение текстуры по уникальному номеру...
//------------------------------------------------------------------------------------
eTexture* vw_FindTextureByNum(int Num)
{
	eTexture *Tmp = StartTexMan;

	while (Tmp != 0)
	{
		eTexture *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) return Tmp;
		Tmp = Tmp1;
	}

	return 0;
}






//------------------------------------------------------------------------------------
// Нахождение текстуры по имени...
//------------------------------------------------------------------------------------
eTexture* vw_FindTextureByName(const char *Name)
{
	eTexture *Tmp = StartTexMan;

	while (Tmp != 0)
	{
		eTexture *Tmp1 = Tmp->Next;
		if(vw_strcmp(Tmp->Name, Name) == 0) return Tmp;
		Tmp = Tmp1;
	}

	return 0;
}



