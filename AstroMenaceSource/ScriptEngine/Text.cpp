/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Text.cpp

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


// основной документ
TiXmlDocument	*xmlTextDoc = 0;
// основной элемент
TiXmlElement	*xmlAstroMenaceText = 0;


//-----------------------------------------------------------------------------
// освобождаем данные
//-----------------------------------------------------------------------------
void ReleaseGameText()
{
	if (xmlTextDoc != 0)
	{
		xmlTextDoc->Clear();
		delete xmlTextDoc; xmlTextDoc = 0;
	}
}


//-----------------------------------------------------------------------------
// иним текстовых xml
//-----------------------------------------------------------------------------
void InitGameText(const char *FileName)
{
	// иним скрипт
	xmlTextDoc = new TiXmlDocument(FileName);

	// обязательно правильно задавать кодировку в самом файле!
	if (!xmlTextDoc->LoadFile())
	{
		ReleaseGameText();
		return;
	}

	// берем первый элемент в скрипте
	xmlAstroMenaceText = xmlTextDoc->FirstChildElement("AstroMenaceText");
}




// проверяем только цифры
int strcmpNum(const char *a, const char *b)
{
	while(*a == *b)
	{
		if(*a == '\0')return 0;
		a++;
		b++;
	}
	// если дошли до этого - все нормально
	if(*b == '_')return 0;

	return 1;
}




//-----------------------------------------------------------------------------
// получаем текст из файлы
//-----------------------------------------------------------------------------
const char NoText[] = " ";
const char *GetText(const char *ItemID)
{
	if (xmlTextDoc == 0) return NoText;
	if (xmlAstroMenaceText == 0) return NoText;


	TiXmlElement *xmlTextElem = xmlAstroMenaceText->FirstChildElement("itemlist");




	while (xmlTextElem)
	{
		// проверяем номер, будет проверять только цифры
		if (!strcmpNum(xmlTextElem->Attribute("id"), ItemID))
		{
			TiXmlElement *xmlTextElem2 = xmlTextElem->FirstChildElement("item");

			while (xmlTextElem2)
			{
				if (!vw_strcmp(ItemID, xmlTextElem2->Attribute("id")))
				{
					return xmlTextElem2->GetText();
				}


				xmlTextElem2 = xmlTextElem2->NextSiblingElement("item");
			}

		}


		xmlTextElem = xmlTextElem->NextSiblingElement("itemlist");
	}

	return NoText;
}




//-----------------------------------------------------------------------------
// проверяем, есть ли символ в фонте
//-----------------------------------------------------------------------------
int CheckFontCharsInText()
{
	if (xmlTextDoc == 0) return -1;
	if (xmlAstroMenaceText == 0) return -1;


	TiXmlElement *xmlTextElem = xmlAstroMenaceText->FirstChildElement("itemlist");


	printf("Font characters detection start.\n");

	while (xmlTextElem)
	{

		TiXmlElement *xmlTextElem2 = xmlTextElem->FirstChildElement("item");
		while (xmlTextElem2)
		{
				const char *CharsList = xmlTextElem2->GetText();
				if (CharsList!=0)
				{
					// перебираем всю строку
					while (strlen(CharsList) > 0)
					{
						unsigned CurrentChar;
						// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
						CharsList = utf8_to_utf32(CharsList, &CurrentChar);
						// загружаем символ и все необходимые данные для него
						if (!vw_FindFontCharByUTF32(CurrentChar))
						{
							printf("!!! FontChar was not created, Unicode: " );
							if ( CurrentChar < 0x80 && CurrentChar > 0 )
							{
								printf( "%c (0x%04X)\n", (char)CurrentChar,
										CurrentChar );
							}
							else
							{
								printf( "? (0x%04X)\n", CurrentChar );
							}
						}
					}
				}


			xmlTextElem2 = xmlTextElem2->NextSiblingElement("item");
		}


		xmlTextElem = xmlTextElem->NextSiblingElement("itemlist");
	}

	printf("Font characters detection end.\n\n");

	return 0;
}
