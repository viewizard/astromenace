/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2014 Michael Kurinnoy, Viewizard


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


#include "GameLvlText.h"




CGameLvlText * StartText = 0;
CGameLvlText * EndText = 0;




//-----------------------------------------------------------------------------
//	Присоеденяем Text к списку
//-----------------------------------------------------------------------------
void AttachGameLvlText(CGameLvlText * NewText)
{
	if (NewText == 0) return;

	// первый в списке...
	if (EndText == 0)
	{
		NewText->Prev = 0;
		NewText->Next = 0;
		StartText = NewText;
		EndText = NewText;
	}
	else // продолжаем заполнение...
	{
		NewText->Prev = EndText;
		NewText->Next = 0;
		EndText->Next = NewText;
		EndText = NewText;
	}

}




//-----------------------------------------------------------------------------
//	Удаляем Text из списка
//-----------------------------------------------------------------------------
void DetachGameLvlText(CGameLvlText * OldText)
{
	if (OldText == 0) return;

	// переустанавливаем указатели...
	if (StartText == OldText) StartText = OldText->Next;
	if (EndText == OldText) EndText = OldText->Prev;


	if (OldText->Next != 0) OldText->Next->Prev = OldText->Prev;
		else if (OldText->Prev != 0) OldText->Prev->Next = 0;
	if (OldText->Prev != 0) OldText->Prev->Next = OldText->Next;
		else if (OldText->Next != 0) OldText->Next->Prev = 0;
}




//-----------------------------------------------------------------------------
//	Удаляем все Text в списке
//-----------------------------------------------------------------------------
void ReleaseAllGameLvlText()
{
	// для всех Text
	CGameLvlText *tmp = StartText;
	while (tmp!=0)
	{
		CGameLvlText *tmp2 = tmp->Next;
		// удаляем и очищаем всю память, в релизе стоит DetachShip
		delete tmp;
		tmp = tmp2;
	}

	StartText = 0;
	EndText = 0;
}











//-----------------------------------------------------------------------------
//	Прорисовываем все Text
//-----------------------------------------------------------------------------
void DrawAllGameLvlText()
{
	// для всех
	CGameLvlText *tmp = StartText;
	while (tmp!=0)
	{
		CGameLvlText *tmp2 = tmp->Next;
		tmp->Draw();
		tmp = tmp2;
	}
}





//-----------------------------------------------------------------------------
//	Апдейтим все Text
//-----------------------------------------------------------------------------
void UpdateAllGameLvlText(float Time)
{
	// для всех
	CGameLvlText *tmp = StartText;
	while (tmp!=0)
	{
		CGameLvlText *tmp2 = tmp->Next;
		if (!tmp->Update(Time)) delete tmp;
		tmp = tmp2;
	}
}



