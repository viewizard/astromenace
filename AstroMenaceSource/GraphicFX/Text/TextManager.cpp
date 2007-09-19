/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: TextManager.cpp

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


#include "Text.h"




CText * StartText = 0;
CText * EndText = 0;




//-----------------------------------------------------------------------------
//	Присоеденяем Text к списку
//-----------------------------------------------------------------------------
void AttachText(CText * NewText)
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
void DetachText(CText * OldText)
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
void ReleaseAllText()
{
	// для всех Text
	CText *tmp = StartText;
	while (tmp!=0)
	{
		CText *tmp2 = tmp->Next;
		// удаляем и очищаем всю память, в релизе стоит DetachShip
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartText = 0;
	EndText = 0;
}











//-----------------------------------------------------------------------------
//	Прорисовываем все Text
//-----------------------------------------------------------------------------
void DrawAllText()
{
	// для всех
	CText *tmp = StartText;
	while (tmp!=0)
	{
		CText *tmp2 = tmp->Next;
		tmp->Draw();
		tmp = tmp2;
	}
}





//-----------------------------------------------------------------------------
//	Апдейтим все Text
//-----------------------------------------------------------------------------
void UpdateAllText(float Time)
{
	// для всех
	CText *tmp = StartText;
	while (tmp!=0)
	{
		CText *tmp2 = tmp->Next;
		if (!tmp->Update(Time))
			delete tmp; tmp = 0;
		tmp = tmp2;
	}
}



