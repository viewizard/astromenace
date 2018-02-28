/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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


#include "texture.h"
#include "../graphics/graphics.h"



//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
sTexture *StartTexMan = nullptr;// Указатель на первую текстуру в списке...
sTexture *EndTexMan = nullptr;	// Указатель на последнюю текстуру в списке...
int NumTexMan = 0;		// Последний использов. уникальный номер
// Ключ прорисовки текстуры (near, linear, ... )
int FilteringTexMan = RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT;
// Ключ прорисовки текстуры (wrap, clamp ... )
int Address_ModeTexMan = RI_WRAP_U | RI_WRAP_V;
// указывает, что канал есть...(или нужно создать по цвету...)
bool AlphaTexMan = false;
// цвет прозрачности для создания Alpha канала...
uint8_t ARedTexMan = 0;
uint8_t AGreenTexMan = 0;
uint8_t ABlueTexMan = 0;
int  AFlagTexMan = TX_ALPHA_EQUAL;

bool MipMap = true;









//------------------------------------------------------------------------------------
// освобождаем все текстуры подключенные к менеджеру текстур
//------------------------------------------------------------------------------------
void vw_ReleaseAllTextures()
{
	// Чистка памяти...
	sTexture *Tmp = StartTexMan;
	while (Tmp != nullptr) {
		sTexture *Tmp1 = Tmp->Next;
		vw_ReleaseTexture(Tmp);
		Tmp = Tmp1;
	}


	StartTexMan = nullptr;
	EndTexMan = nullptr;
	NumTexMan = 0;

	FilteringTexMan = RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT;
	Address_ModeTexMan = RI_WRAP_U | RI_WRAP_V;
	AlphaTexMan = false;
}







//------------------------------------------------------------------------------------
// подключение текстуры к менеджеру текстур
//------------------------------------------------------------------------------------
void vw_AttachTexture(sTexture *Texture)
{
	if (Texture == nullptr)
		return;

	// первый в списке...
	if (EndTexMan == nullptr) {
		Texture->Prev = nullptr;
		Texture->Next = nullptr;
		NumTexMan += 1;
		Texture->Num = NumTexMan;
		StartTexMan = Texture;
		EndTexMan = Texture;
	} else { // продолжаем заполнение...
		Texture->Prev = EndTexMan;
		Texture->Next = nullptr;
		EndTexMan->Next = Texture;
		NumTexMan += 1;
		Texture->Num = NumTexMan;
		EndTexMan = Texture;
	}
}





//------------------------------------------------------------------------------------
// отключение текстуры от менеджера текстур
//------------------------------------------------------------------------------------
void vw_DetachTexture(sTexture *Texture)
{
	if (Texture == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartTexMan == Texture)
		StartTexMan = Texture->Next;
	if (EndTexMan == Texture)
		EndTexMan = Texture->Prev;

	if (Texture->Next != nullptr)
		Texture->Next->Prev = Texture->Prev;
	else if (Texture->Prev != nullptr)
		Texture->Prev->Next = nullptr;

	if (Texture->Prev != nullptr)
		Texture->Prev->Next = Texture->Next;
	else if (Texture->Next != nullptr)
		Texture->Next->Prev = nullptr;
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
void vw_SetTextureAlpha(uint8_t nARed, uint8_t nAGreen, uint8_t nABlue)
{
	ARedTexMan = nARed;
	AGreenTexMan = nAGreen;
	ABlueTexMan = nABlue;
}









//------------------------------------------------------------------------------------
// Нахождение текстуры по уникальному номеру...
//------------------------------------------------------------------------------------
sTexture *vw_FindTextureByNum(int Num)
{
	sTexture *Tmp = StartTexMan;

	while (Tmp != nullptr) {
		sTexture *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num)
			return Tmp;
		Tmp = Tmp1;
	}

	return nullptr;
}






//------------------------------------------------------------------------------------
// Нахождение текстуры по имени...
//------------------------------------------------------------------------------------
sTexture *vw_FindTextureByName(const char *Name)
{
	sTexture *Tmp = StartTexMan;

	while (Tmp != nullptr) {
		sTexture *Tmp1 = Tmp->Next;
		if(strcmp(Tmp->Name, Name) == 0)
			return Tmp;
		Tmp = Tmp1;
	}

	return nullptr;
}



