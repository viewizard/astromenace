/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Font.cpp

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


#include "../Core/Core.h"









//------------------------------------------------------------------------------------
// данные фонта
//------------------------------------------------------------------------------------
void GetFontData(int Char, RECT *SrcRest, int *X, int *Y)
{
	int XlenDecr = 0;
	int Ysm = 0;


	switch (Char)
	{
		case '!':	SetRect(SrcRest,36,19,44,40); XlenDecr=-2; Ysm = 0;	break;
		case '"':	SetRect(SrcRest,52,19,64,40); XlenDecr=-2; Ysm = 0;	break;
		case '#':	SetRect(SrcRest,71,19,88,40); XlenDecr=-2; Ysm = 0;	break;
		case '$':	SetRect(SrcRest,96,19,110,40); XlenDecr=-3; Ysm = 0;	break;
		case '%':	SetRect(SrcRest,117,19,141,40); XlenDecr=-2; Ysm = 0;	break;
		case '&':	SetRect(SrcRest,148,19,169,40); XlenDecr=-2; Ysm = 0;	break;
		case '\'':	SetRect(SrcRest,173,19,184,40); XlenDecr=-2; Ysm = 0;	break;
		case '(':	SetRect(SrcRest,191,19,203,43); XlenDecr=-2; Ysm = -2;	break;
		case ')':	SetRect(SrcRest,211,19,222,43); XlenDecr=-2; Ysm = -2;	break;
		case '*':	SetRect(SrcRest,231,19,244,40); XlenDecr=-2; Ysm = 0;	break;
		case '+':	SetRect(SrcRest,252,19,269,40); XlenDecr=-3; Ysm = 0;	break;
		case ',':	SetRect(SrcRest,276,19,284,44); XlenDecr=-2; Ysm = 0;	break;
		case '-':	SetRect(SrcRest,291,19,303,40); XlenDecr=-3; Ysm = 0;	break;
		case '.':	SetRect(SrcRest,310,19,319,40); XlenDecr=-3; Ysm = 0;	break;
		case '/':	SetRect(SrcRest,325,19,339,44); XlenDecr=-2; Ysm = 0;	break;
		case ':':	SetRect(SrcRest,247,47,257,69); XlenDecr=-2; Ysm = -1;	break;
		case ';':	SetRect(SrcRest,265,47,273,73); XlenDecr=-2; Ysm = -1;	break;
		case '<':	SetRect(SrcRest,280,47,297,69); XlenDecr=-3; Ysm = 0;	break;
		case '=':	SetRect(SrcRest,304,47,321,69); XlenDecr=-2; Ysm = 0;	break;
		case '>':	SetRect(SrcRest,327,47,344,69); XlenDecr=-2; Ysm = 0;	break;
		case '?':	SetRect(SrcRest,351,47,367,69); XlenDecr=-2; Ysm = 0;	break;
		case '@':	SetRect(SrcRest,20,76,41,97); XlenDecr=-2; Ysm = 1;	break;
		case '[':	SetRect(SrcRest,295,107,305,131); XlenDecr=-3; Ysm = 1;	break;
		case '\\':	SetRect(SrcRest,311,107,325,126); XlenDecr=-2; Ysm = 3;	break;
		case ']':	SetRect(SrcRest,332,107,342,131); XlenDecr=-3; Ysm = 1;	break;
		case '^':	SetRect(SrcRest,350,106,365,120); XlenDecr=-2; Ysm = 0;	break;
		case '_':	SetRect(SrcRest,374,106,391,131); XlenDecr=-2; Ysm = 3;	break;
		case '`':	SetRect(SrcRest,19,135,28,155); XlenDecr=-3; Ysm = 2;	break;
		case '{':	SetRect(SrcRest,253,164,265,188); XlenDecr=-3; Ysm = 2;	break;
		case '|':	SetRect(SrcRest,271,164,278,188); XlenDecr=-2; Ysm = 0;	break;
		case '}':	SetRect(SrcRest,285,164,297,188); XlenDecr=-3; Ysm = 2;	break;
		case '~':	SetRect(SrcRest,307,164,322,188); XlenDecr=-2; Ysm = 2;	break;

		case '0':	SetRect(SrcRest,20,47,36,69); XlenDecr=-2; Ysm = -1;	break;
		case '1':	SetRect(SrcRest,45,47,56,69); XlenDecr=-2; Ysm = -1;	break;
		case '2':	SetRect(SrcRest,66,47,82,69); XlenDecr=-3; Ysm = -1;	break;
		case '3':	SetRect(SrcRest,89,47,105,69); XlenDecr=-3; Ysm = -1;	break;
		case '4':	SetRect(SrcRest,111,47,128,69); XlenDecr=-3; Ysm = -1;	break;
		case '5':	SetRect(SrcRest,135,47,150,69); XlenDecr=-2; Ysm = -1;	break;
		case '6':	SetRect(SrcRest,157,47,173,69); XlenDecr=-2; Ysm = -1;	break;
		case '7':	SetRect(SrcRest,180,47,195,69); XlenDecr=-2; Ysm = -1;	break;
		case '8':	SetRect(SrcRest,201,47,218,69); XlenDecr=-3; Ysm = -1;	break;
		case '9':	SetRect(SrcRest,224,47,241,69); XlenDecr=-2; Ysm = -1;	break;

		case 'A':	SetRect(SrcRest,48,78,66,97); XlenDecr=-2; Ysm = 1;	break;
		case 'B':	SetRect(SrcRest,72,78,90,97); XlenDecr=-2; Ysm = 1;	break;
		case 'C':	SetRect(SrcRest,96,78,114,97); XlenDecr=-2; Ysm = 1;	break;
		case 'D':	SetRect(SrcRest,121,78,139,97); XlenDecr=-2; Ysm = 1;	break;
		case 'E':	SetRect(SrcRest,145,78,163,97); XlenDecr=-3; Ysm = 1;	break;
		case 'F':	SetRect(SrcRest,169,78,187,97); XlenDecr=-2; Ysm = 1;	break;
		case 'G':	SetRect(SrcRest,192,78,210,97); XlenDecr=-2; Ysm = 1;	break;
		case 'H':	SetRect(SrcRest,217,78,235,97); XlenDecr=-3; Ysm = 1;	break;
		case 'I':	SetRect(SrcRest,241,78,259,97); XlenDecr=-3; Ysm = 1;	break;
		case 'J':	SetRect(SrcRest,264,78,282,97); XlenDecr=-2; Ysm = 1;	break;
		case 'K':	SetRect(SrcRest,288,78,306,97); XlenDecr=-2; Ysm = 1;	break;
		case 'L':	SetRect(SrcRest,313,78,331,97); XlenDecr=-3; Ysm = 1;	break;
		case 'M':	SetRect(SrcRest,337,78,364,97); XlenDecr=-2; Ysm = 1;	break;
		case 'N':	SetRect(SrcRest,370,78,388,97); XlenDecr=-2; Ysm = 1;	break;
		case 'O':	SetRect(SrcRest,394,78,412,97); XlenDecr=-2; Ysm = 1;	break;
		case 'P':	SetRect(SrcRest,20,107,38,126); XlenDecr=-3; Ysm = 2;	break;
		case 'Q':	SetRect(SrcRest,44,107,62,129); XlenDecr=-2; Ysm = 2;	break;
		case 'R':	SetRect(SrcRest,68,107,86,126); XlenDecr=-2; Ysm = 2;	break;
		case 'S':	SetRect(SrcRest,93,107,110,126); XlenDecr=-2; Ysm = 2;	break;
		case 'T':	SetRect(SrcRest,116,107,134,126); XlenDecr=-3; Ysm = 2;	break;
		case 'U':	SetRect(SrcRest,140,107,158,126); XlenDecr=-2; Ysm = 2;	break;
		case 'V':	SetRect(SrcRest,164,107,182,126); XlenDecr=-2; Ysm = 2;	break;
		case 'W':	SetRect(SrcRest,188,107,215,126); XlenDecr=-2; Ysm = 2;	break;
		case 'X':	SetRect(SrcRest,222,107,240,126); XlenDecr=-2; Ysm = 2;	break;
		case 'Y':	SetRect(SrcRest,246,107,264,126); XlenDecr=-2; Ysm = 2;	break;
		case 'Z':	SetRect(SrcRest,270,107,288,126); XlenDecr=-2; Ysm = 2;	break;

		case 'a':	SetRect(SrcRest,35,135,52,155); XlenDecr=-2; Ysm = 1;	break;
		case 'b':	SetRect(SrcRest,59,135,75,155); XlenDecr=-2; Ysm = 1;	break;
		case 'c':	SetRect(SrcRest,82,135,95,155); XlenDecr=-3; Ysm = 1;	break;
		case 'd':	SetRect(SrcRest,101,135,118,155); XlenDecr=-3; Ysm = 1;	break;
		case 'e':	SetRect(SrcRest,125,135,142,155); XlenDecr=-3; Ysm = 1;	break;
		case 'f':	SetRect(SrcRest,148,135,158,155); XlenDecr=-2; Ysm = 1;	break;
		case 'g':	SetRect(SrcRest,164,135,181,160); XlenDecr=-2; Ysm = 0;	break;
		case 'h':	SetRect(SrcRest,188,135,203,155); XlenDecr=-3; Ysm = 1;	break;
		case 'i':	SetRect(SrcRest,210,135,218,155); XlenDecr=-2; Ysm = 1;	break;
		case 'j':	SetRect(SrcRest,223,135,234,160); XlenDecr=-3; Ysm = 0;	break;
		case 'k':	SetRect(SrcRest,241,135,255,155); XlenDecr=-2; Ysm = 1;	break;
		case 'l':	SetRect(SrcRest,262,135,270,155); XlenDecr=-2; Ysm = 1;	break;
		case 'm':	SetRect(SrcRest,277,135,298,155); XlenDecr=-2; Ysm = 1;	break;
		case 'n':	SetRect(SrcRest,305,135,319,155); XlenDecr=-2; Ysm = 1;	break;
		case 'o':	SetRect(SrcRest,326,135,343,155); XlenDecr=-2; Ysm = 1;	break;
		case 'p':	SetRect(SrcRest,20,164,37,188); XlenDecr=-2; Ysm = 0;	break;
		case 'q':	SetRect(SrcRest,44,164,61,188); XlenDecr=-3; Ysm = 0;	break;
		case 'r':	SetRect(SrcRest,67,164,78,188); XlenDecr=-2; Ysm = 1;	break;
		case 's':	SetRect(SrcRest,84,164,97,188); XlenDecr=-2; Ysm = 1;	break;
		case 't':	SetRect(SrcRest,104,164,114,188); XlenDecr=-2; Ysm = 1;	break;
		case 'u':	SetRect(SrcRest,120,164,135,188); XlenDecr=-3; Ysm = 1;	break;
		case 'v':	SetRect(SrcRest,141,164,156,188); XlenDecr=-3; Ysm = 1;	break;
		case 'w':	SetRect(SrcRest,162,164,183,188); XlenDecr=-3; Ysm = 1;	break;
		case 'x':	SetRect(SrcRest,189,164,203,188); XlenDecr=-3; Ysm = 1;	break;
		case 'y':	SetRect(SrcRest,209,164,223,188); XlenDecr=-3; Ysm = 0;	break;
		case 'z':	SetRect(SrcRest,230,164,244,188); XlenDecr=-3; Ysm = 1;	break;
		case ' ':	SetRect(SrcRest,0,0,13,0); break;



		case 0xC396:	SetRect(SrcRest,20,206,38,227); XlenDecr=-2; Ysm = -1;	break; // Ö
		case 0xC3B6:	SetRect(SrcRest,47,206,63,227); XlenDecr=-2; Ysm = -1;	break; // ö
		case 0xC384:	SetRect(SrcRest,73,206,90,227); XlenDecr=-2; Ysm = -1;	break; // Ä
		case 0xC3A4:	SetRect(SrcRest,99,206,115,227); XlenDecr=-2; Ysm = -1;	break; // ä
		case 0xC39C:	SetRect(SrcRest,125,206,142,227); XlenDecr=-2; Ysm = -1;	break; // Ü
		case 0xC3BC:	SetRect(SrcRest,153,206,167,227); XlenDecr=-2; Ysm = -1;	break; // ü
		case 0xC39F:	SetRect(SrcRest,178,206,191,228); XlenDecr=-2; Ysm = -1;	break; // ß
		case 0xC2B0:	SetRect(SrcRest,202,206,212,228); XlenDecr=-2; Ysm = -1;	break; // °
		case 0xC2A7:	SetRect(SrcRest,223,209,237,232); XlenDecr=-2; Ysm = -1;	break; // §



		case 0xC380:	SetRect(SrcRest,20,387,38,411); XlenDecr=-2; Ysm = -3;	break; // À
		case 0xC3A0:	SetRect(SrcRest,41,387,58,411); XlenDecr=-2; Ysm = -3;	break; // à
		case 0xC382:	SetRect(SrcRest,62,387,80,411); XlenDecr=-2; Ysm = -3;	break; // Â
		case 0xC3A2:	SetRect(SrcRest,83,387,100,411); XlenDecr=-2; Ysm = -3;	break; // â
		case 0xC386:	SetRect(SrcRest,105,387,126,411); XlenDecr=-2; Ysm = -3;	break; // Æ
		case 0xC3A6:	SetRect(SrcRest,132,387,154,411); XlenDecr=-2; Ysm = -3;	break; // æ
		case 0xC387:	SetRect(SrcRest,160,387,177,415); XlenDecr=-2; Ysm = -2;	break; // Ç
		case 0xC3A7:	SetRect(SrcRest,184,387,196,415); XlenDecr=-2; Ysm = -2;	break; // ç
		case 0xC388:	SetRect(SrcRest,204,387,222,411); XlenDecr=-2; Ysm = -3;	break; // È
		case 0xC3A8:	SetRect(SrcRest,226,387,243,411); XlenDecr=-2; Ysm = -3;	break; // è
		case 0xC389:	SetRect(SrcRest,249,387,267,411); XlenDecr=-2; Ysm = -3;	break; // É
		case 0xC3A9:	SetRect(SrcRest,272,387,288,411); XlenDecr=-2; Ysm = -3;	break; // é
		case 0xC38A:	SetRect(SrcRest,294,387,312,411); XlenDecr=-2; Ysm = -3;	break; // Ê
		case 0xC3AA:	SetRect(SrcRest,316,387,333,411); XlenDecr=-2; Ysm = -3;	break; // ê
		case 0xC38B:	SetRect(SrcRest,339,387,357,411); XlenDecr=-2; Ysm = -3;	break; // Ë
		case 0xC3AB:	SetRect(SrcRest,361,387,378,411); XlenDecr=-2; Ysm = -3;	break; // ë
		case 0xC38E:	SetRect(SrcRest,20,421,37,445); XlenDecr=-2; Ysm = -3;	break; // Î
		case 0xC3AE:	SetRect(SrcRest,43,421,54,445); XlenDecr=-2; Ysm = -3;	break; // î
		case 0xC38F:	SetRect(SrcRest,61,421,79,445); XlenDecr=-2; Ysm = -3;	break; // Ï
		case 0xC3AF:	SetRect(SrcRest,83,421,97,445); XlenDecr=-2; Ysm = -3;	break; // ï
		case 0xC394:	SetRect(SrcRest,103,421,121,445); XlenDecr=-2; Ysm = -3;	break; // Ô
		case 0xC3B4:	SetRect(SrcRest,128,421,145,445); XlenDecr=-2; Ysm = -3;	break; // ô
		case 0xC592:	SetRect(SrcRest,154,421,175,445); XlenDecr=-2; Ysm = -3;	break; // Œ
		case 0xC593:	SetRect(SrcRest,181,421,204,445); XlenDecr=-2; Ysm = -3;	break; // œ
		case 0xC399:	SetRect(SrcRest,210,421,228,445); XlenDecr=-2; Ysm = -3;	break; // Ù
		case 0xC3B9:	SetRect(SrcRest,233,421,248,445); XlenDecr=-2; Ysm = -3;	break; // ù
		case 0xC39B:	SetRect(SrcRest,255,421,273,445); XlenDecr=-2; Ysm = -3;	break; // Û
		case 0xC3BB:	SetRect(SrcRest,278,421,293,445); XlenDecr=-2; Ysm = -3;	break; // û
	//	case 0xC39C:	SetRect(SrcRest,301,421,319,445); XlenDecr=-2; Ysm = -3;	break; // Ü
	//	case 0xC3BC:	SetRect(SrcRest,324,421,339,445); XlenDecr=-2; Ysm = -3;	break; // ü
		case 0xC5B8:	SetRect(SrcRest,349,421,367,445); XlenDecr=-2; Ysm = -3;	break; // Ÿ
		case 0xC3BF:	SetRect(SrcRest,375,421,389,448); XlenDecr=-2; Ysm = -2;	break; // ÿ




		case 0xD090:	SetRect(SrcRest,21,262,38,280); XlenDecr=-2; Ysm = 1;	break; // A
		case 0xD091:	SetRect(SrcRest,46,262,63,280); XlenDecr=-2; Ysm = 1;	break; // Б
		case 0xD092:	SetRect(SrcRest,72,262,88,280); XlenDecr=-2; Ysm = 1;	break; // B
		case 0xD093:	SetRect(SrcRest,97,262,112,280); XlenDecr=-2; Ysm = 1;	break; // Г
		case 0xD094:	SetRect(SrcRest,116,262,138,283); XlenDecr=-2; Ysm = 1;	break; // Д
		case 0xD095:	SetRect(SrcRest,144,262,160,280); XlenDecr=-2; Ysm = 1;	break; // Е
		case 0xD081:	SetRect(SrcRest,167,259,183,280); XlenDecr=-2; Ysm =-2;	break; // Ё
		case 0xD096:	SetRect(SrcRest,191,262,216,280); XlenDecr=-2; Ysm = 1;	break; // Ж
		case 0xD097:	SetRect(SrcRest,223,262,239,280); XlenDecr=-2; Ysm = 1;	break; // З
		case 0xD098:	SetRect(SrcRest,247,262,265,280); XlenDecr=-2; Ysm = 1;	break; // И
		case 0xD099:	SetRect(SrcRest,274,259,291,280); XlenDecr=-2; Ysm =-2;	break; // Й
		case 0xD09A:	SetRect(SrcRest,300,262,317,280); XlenDecr=-2; Ysm = 1;	break; // К
		case 0xD09B:	SetRect(SrcRest,324,262,343,280); XlenDecr=-2; Ysm = 1;	break; // Л
		case 0xD09C:	SetRect(SrcRest,349,262,373,280); XlenDecr=-2; Ysm = 1;	break; // М
		case 0xD09D:	SetRect(SrcRest,382,262,399,280); XlenDecr=-2; Ysm = 1;	break; // Н
		case 0xD09E:	SetRect(SrcRest,407,262,430,280); XlenDecr=-2; Ysm = 1;	break; // О
		case 0xD09F:	SetRect(SrcRest,438,262,456,280); XlenDecr=-2; Ysm = 1;	break; // П
		case 0xD0A0:	SetRect(SrcRest,465,262,481,280); XlenDecr=-2; Ysm = 1;	break; // Р
		case 0xD0A1:	SetRect(SrcRest,20,291,36,309); XlenDecr=-2; Ysm = 1;	break; // С
		case 0xD0A2:	SetRect(SrcRest,43,291,58,309); XlenDecr=-2; Ysm = 1;	break; // Т
		case 0xD0A3:	SetRect(SrcRest,66,291,84,309); XlenDecr=-2; Ysm = 1;	break; // У
		case 0xD0A4:	SetRect(SrcRest,92,289,116,310); XlenDecr=-2; Ysm =-1;	break; // Ф
		case 0xD0A5:	SetRect(SrcRest,123,291,141,309); XlenDecr=-2; Ysm = 1;	break; // Х
		case 0xD0A6:	SetRect(SrcRest,149,291,168,312); XlenDecr=-2; Ysm = 1;	break; // Ц
		case 0xD0A7:	SetRect(SrcRest,175,291,192,309); XlenDecr=-2; Ysm = 1;	break; // Ч
		case 0xD0A8:	SetRect(SrcRest,201,291,226,309); XlenDecr=-2; Ysm = 1;	break; // Ш
		case 0xD0A9:	SetRect(SrcRest,235,291,261,312); XlenDecr=-2; Ysm = 1;	break; // Щ
		case 0xD0AA:	SetRect(SrcRest,267,291,288,309); XlenDecr=-2; Ysm = 1;	break; // Ъ
		case 0xD0AB:	SetRect(SrcRest,296,291,317,309); XlenDecr=-2; Ysm = 1;	break; // Ы
		case 0xD0AC:	SetRect(SrcRest,326,291,343,309); XlenDecr=-2; Ysm = 1;	break; // Ь
		case 0xD0AD:	SetRect(SrcRest,351,291,367,309); XlenDecr=-2; Ysm = 1;	break; // Э
		case 0xD0AE:	SetRect(SrcRest,375,291,402,309); XlenDecr=-2; Ysm = 1;	break; // Ю
		case 0xD0AF:	SetRect(SrcRest,410,291,427,309); XlenDecr=-2; Ysm = 1;	break; // Я

		case 0xD0B0:	SetRect(SrcRest,20,324,38,338); XlenDecr=-2; Ysm = 5;	break; // а
		case 0xD0B1:	SetRect(SrcRest,45,320,62,338); XlenDecr=-2; Ysm = 1;	break; // б
		case 0xD0B2:	SetRect(SrcRest,71,324,86,338); XlenDecr=-2; Ysm = 5;	break; // в
		case 0xD0B3:	SetRect(SrcRest,94,324,107,338); XlenDecr=-2; Ysm = 5;	break; // г
		case 0xD0B4:	SetRect(SrcRest,113,324,131,343); XlenDecr=-2; Ysm = 5;	break; // д
		case 0xD0B5:	SetRect(SrcRest,138,324,156,338); XlenDecr=-2; Ysm = 5;	break; // е
		case 0xD191:	SetRect(SrcRest,163,320,180,338); XlenDecr=-2; Ysm = 1;	break; // ё
		case 0xD0B6:	SetRect(SrcRest,188,324,208,338); XlenDecr=-2; Ysm = 5;	break; // ж
		case 0xD0B7:	SetRect(SrcRest,215,324,229,338); XlenDecr=-2; Ysm = 5;	break; // з
		case 0xD0B8:	SetRect(SrcRest,237,324,252,338); XlenDecr=-2; Ysm = 5;	break; // и
		case 0xD0B9:	SetRect(SrcRest,260,320,276,338); XlenDecr=-2; Ysm = 1;	break; // й
		case 0xD0BA:	SetRect(SrcRest,283,324,298,338); XlenDecr=-2; Ysm = 5;	break; // к
		case 0xD0BB:	SetRect(SrcRest,304,324,321,338); XlenDecr=-2; Ysm = 5;	break; // л
		case 0xD0BC:	SetRect(SrcRest,327,324,348,338); XlenDecr=-2; Ysm = 5;	break; // м
		case 0xD0BD:	SetRect(SrcRest,355,324,371,338); XlenDecr=-2; Ysm = 5;	break; // н
		case 0xD0BE:	SetRect(SrcRest,378,324,395,338); XlenDecr=-2; Ysm = 5;	break; // о
		case 0xD0BF:	SetRect(SrcRest,403,324,418,338); XlenDecr=-2; Ysm = 5;	break; // п
		case 0xD180:	SetRect(SrcRest,426,324,444,343); XlenDecr=-2; Ysm = 5;	break; // р
		case 0xD181:	SetRect(SrcRest,20,352,33,366); XlenDecr=-2; Ysm = 5;	break; // с
		case 0xD182:	SetRect(SrcRest,40,352,54,366); XlenDecr=-2; Ysm = 5;	break; // т
		case 0xD183:	SetRect(SrcRest,61,352,77,372); XlenDecr=-2; Ysm = 5;	break; // у
		case 0xD184:	SetRect(SrcRest,84,348,105,372); XlenDecr=-2; Ysm = 1;	break; // ф
		case 0xD185:	SetRect(SrcRest,112,352,126,366); XlenDecr=-2; Ysm = 5;	break; // х
		case 0xD186:	SetRect(SrcRest,133,352,151,369); XlenDecr=-2; Ysm = 5;	break; // ц
		case 0xD187:	SetRect(SrcRest,157,352,172,366); XlenDecr=-2; Ysm = 5;	break; // ч
		case 0xD188:	SetRect(SrcRest,180,352,201,366); XlenDecr=-2; Ysm = 5;	break; // ш
		case 0xD189:	SetRect(SrcRest,209,352,231,369); XlenDecr=-2; Ysm = 5;	break; // щ
		case 0xD18A:	SetRect(SrcRest,237,352,255,366); XlenDecr=-2; Ysm = 5;	break; // ъ
		case 0xD18B:	SetRect(SrcRest,263,352,281,366); XlenDecr=-2; Ysm = 5;	break; // ы
		case 0xD18C:	SetRect(SrcRest,289,352,304,366); XlenDecr=-2; Ysm = 5;	break; // ь
		case 0xD18D:	SetRect(SrcRest,311,352,325,366); XlenDecr=-2; Ysm = 5;	break; // э
		case 0xD18E:	SetRect(SrcRest,333,352,354,366); XlenDecr=-2; Ysm = 5;	break; // ю
		case 0xD18F:	SetRect(SrcRest,360,352,376,366); XlenDecr=-2; Ysm = 5;	break; // я




	default: SetRect(SrcRest,0,0,0,0); XlenDecr=0; Ysm = 0;
//fprintf(stderr, "%i\n", Char);
}





	*Y = Ysm;
	*X = XlenDecr;

}







//------------------------------------------------------------------------------------
// прорисовка фонта
//------------------------------------------------------------------------------------
void DrawFont(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text, ...)
{
	if (Text == 0) return;


// FlattenWidth - выравнивать по ширине
// MaxWidth - рисовать до ширины
// CharSet - 0-белый 1-желтый


	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	// в text уже полная строка
	if (strlen(text) == 0) return;



	RECT DestRest, SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ystart = Y;
	float Xstart = X*1.0f;
	int Ysm = 0;
	int XlenDecr = 0;


	// размер пробела в пикселях
	float Space = 13.0f;


	// находим абсолютное значения ограничения по MaxWidth
	float MaxWidthLimit = (Xstart+MaxWidth);






	// если нужно выравнивать, считаем данные пробелов
	if (FlattenWidth !=0)
	{
		float XsmDop = 0.0f;
		int XsmDopCount = 0;

		for (unsigned int i=0; i<strlen(text); i++)
		{
			int CurrentChar = text[i];
			// если 2-х байтовое, юникод
			if (text[i] & 0x80)
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
				XsmDop+=(SrcRest.right+XlenDecr) - SrcRest.left-1.0f;
			else
			{
				SetRect(&SrcRest,0,0,0,0); Ysm = 0; XsmDopCount++;
			}
		}

		if (XsmDopCount!=0) Space = (FlattenWidth - XsmDop)/XsmDopCount;
	}




	float R=1.0f;
	float G=1.0f;
	float B=1.0f;
	if (Transp >= 1.0f) Transp = 1.0f;

	switch (CharSet)
	{
		case 0: // белый
			R=1.0f;G=1.0f;B=1.0f;
			break;
		case 1: // желтый
			R=1.0f;G=1.0f;B=0.0f;
			break;
		case 2: // красный
			R=1.0f;G=0.0f;B=0.0f;
			break;
		case 3: // зеленый
			R=0.0f;G=1.0f;B=0.0f;
			break;
		case 4: // оранжевый
			R=1.0f;G=0.5f;B=0.0f;
			break;
		case 5: // серый
			R=0.5f;G=0.5f;B=0.5f;
			break;
	}

	eTexture *Tex = vw_FindTextureByName("DATA/FONT/font_white.tga");
	if (Tex == 0) return;

	float AW;
	float AH;
	bool ASpresent=false;
	ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_SetTextureV(0, Tex);
	vw_SetTexAlpha(true, 0.01f);
	vw_SetTexAddressMode(0, RI_CLAMP);
	vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

    // не можем ставить другое! если нет мипмапа
    vw_SetTexFiltering(0, RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, 1);

	float ImageHeight = Tex->Height*1.0f;
	float ImageWidth = Tex->Width*1.0f;

	float tmpPosY = 0;

	// выделяем память
	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = 0;
	tmp = new float[(2+2+4)*4*strlen(text)]; if (tmp == 0) return;
	int k=0;
	int SymbolCount = 0;





	// прорисовка текста
	for (unsigned int i=0; i<strlen(text); i++)
	{
		int CurrentChar = text[i];
		// если 2-х байтовое, юникод
		if (text[i] & 0x80)
		{
			Uint8 Temp8[2];
			Temp8[0] = text[i+1];
			Temp8[1] = text[i];
			Uint16 *Temp16 = (Uint16 *)(Temp8);
			CurrentChar = (int)(Temp16[0]);
			i++;
		}


		GetFontData(CurrentChar, &SrcRest, &XlenDecr, &Ysm);

		// достигли предельной ширины
		if (MaxWidth != 0)
			if (Xstart >= MaxWidthLimit) goto DrawLine;


		if (CurrentChar != ' ')
		{
			SymbolCount++;

			SetRect(&DestRest, (int)Xstart,	Ystart+Ysm,
				(int)Xstart+SrcRest.right-SrcRest.left, Ystart+Ysm+SrcRest.bottom-SrcRest.top);



			if (ASpresent) tmpPosY = (AH - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));
			else tmpPosY = (AHw - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));

			float FrameHeight = (SrcRest.bottom*1.0f )/ImageHeight;
			float FrameWidth = (SrcRest.right*1.0f )/ImageWidth;

			float Yst = (SrcRest.top*1.0f)/ImageHeight;
			float Xst = (SrcRest.left*1.0f)/ImageWidth;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-Yst;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-Yst;



			Xstart += (DestRest.right+XlenDecr) - DestRest.left-1;
		}
		else
			Xstart += Space;
	}

DrawLine:

	vw_SendVertices(RI_QUADS, 4*SymbolCount, RI_2f_XYZ | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(float));

	if (tmp != 0){delete [] tmp; tmp = 0;}
	vw_SetTexAlpha(false, 0.5f);
    vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_SetTextureDef(0);


}









//------------------------------------------------------------------------------------
// прорисовка фонта x2
//------------------------------------------------------------------------------------
void DrawFontX2(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text, ...)
{
	if (Text == 0) return;

// FlattenWidth - выравнивать по ширине
// MaxWidth - рисовать до ширины
// CharSet - 0-белый 1-желтый


	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	// в text уже полная строка
	if (strlen(text) == 0) return;



	RECT DestRest, SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ystart = Y;
	float Xstart = X*1.0f;
	int Ysm = 0;
	int XlenDecr = 0;


	// размер пробела в пикселях
	float Space = 13.0f;


	// находим абсолютное значения ограничения по MaxWidth
	float MaxWidthLimit = (Xstart+MaxWidth);






	// если нужно выравнивать, считаем данные пробелов
	if (FlattenWidth !=0)
	{
		float XsmDop = 0.0f;
		int XsmDopCount = 0;

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
			// делаем искуственно ув. на 2 ширины фонта... для просчета
			SrcRest.right += (int)((SrcRest.right - SrcRest.left)*0.5);

			if (CurrentChar != ' ')
				XsmDop+=(SrcRest.right+XlenDecr) - SrcRest.left-3.0f;
			else
			{
				SetRect(&SrcRest,0,0,0,0); Ysm = 0; XsmDopCount++;
			}
		}

		if (XsmDopCount!=0) Space = (FlattenWidth - XsmDop)/XsmDopCount;
	}





	// прорисовка текста

	float R=1.0f;
	float G=1.0f;
	float B=1.0f;
	if (Transp >= 1.0f) Transp = 1.0f;

	switch (CharSet)
	{
		case 0: // белый
			R=1.0f;G=1.0f;B=1.0f;
			break;
		case 1: // желтый
			R=1.0f;G=1.0f;B=0.0f;
			break;
		case 2: // красный
			R=1.0f;G=0.0f;B=0.0f;
			break;
		case 3: // зеленый
			R=0.0f;G=1.0f;B=0.0f;
			break;
		case 4: // оранжевый
			R=1.0f;G=0.5f;B=0.0f;
			break;
		case 5: // серый
			R=0.5f;G=0.5f;B=0.5f;
			break;
	}

	eTexture *Tex = vw_FindTextureByName("DATA/FONT/font_white.tga");
	if (Tex == 0) return;

	float AW;
	float AH;
	bool ASpresent=false;
	ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_SetTextureV(0, Tex);
	vw_SetTexAlpha(true, 0.01f);
	vw_SetTexAddressMode(0, RI_CLAMP);
	vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

    // не можем ставить другое! если нет мипмапа
    vw_SetTexFiltering(0, RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, 1);

	float ImageHeight = Tex->Height*1.0f;
	float ImageWidth = Tex->Width*1.0f;

	float tmpPosY = 0;

	// выделяем память
	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = 0;
	tmp = new float[(2+2+4)*4*strlen(text)]; if (tmp == 0) return;
	int k=0;
	int SymbolCount = 0;





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

		// достигли предельной ширины
		if (MaxWidth != 0)
			if (Xstart >= MaxWidthLimit) goto DrawLine;


		if (CurrentChar != ' ')
		{
			SymbolCount++;

			SetRect(&DestRest, (int)Xstart,	(int)(Ystart+Ysm*1.5),
				(int)(Xstart+(SrcRest.right-SrcRest.left)*1.5), (int)(Ystart+Ysm*1.5+(SrcRest.bottom-SrcRest.top)*1.5));



			if (ASpresent) tmpPosY = (AH - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));
			else tmpPosY = (AHw - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));

			float FrameHeight = (SrcRest.bottom*1.0f )/ImageHeight;
			float FrameWidth = (SrcRest.right*1.0f )/ImageWidth;

			float Yst = (SrcRest.top*1.0f)/ImageHeight;
			float Xst = (SrcRest.left*1.0f)/ImageWidth;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-Yst;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-Yst;



			Xstart += (DestRest.right+XlenDecr) - DestRest.left-3;
		}
		else
			Xstart += Space;
	}

DrawLine:

	vw_SendVertices(RI_QUADS, 4*SymbolCount, RI_2f_XYZ | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(float));

	if (tmp != 0){delete [] tmp; tmp = 0;}
	vw_SetTexAlpha(false, 0.5f);
    vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_SetTextureDef(0);


}











//------------------------------------------------------------------------------------
// прорисовка фонта x1.5
//------------------------------------------------------------------------------------
void DrawFontX15(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text, ...)
{
	if (Text == 0) return;

// FlattenWidth - выравнивать по ширине
// MaxWidth - рисовать до ширины
// CharSet - 0-белый 1-желтый


	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	// в text уже полная строка
	if (strlen(text) == 0) return;



	RECT DestRest, SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ystart = Y;
	float Xstart = X*1.0f;
	int Ysm = 0;
	int XlenDecr = 0;


	// размер пробела в пикселях
	float Space = 13.0f;


	// находим абсолютное значения ограничения по MaxWidth
	float MaxWidthLimit = (Xstart+MaxWidth);






	// если нужно выравнивать, считаем данные пробелов
	if (FlattenWidth !=0)
	{
		float XsmDop = 0.0f;
		int XsmDopCount = 0;

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
			// делаем искуственно ув. на 2 ширины фонта... для просчета
			SrcRest.right += (int)((SrcRest.right - SrcRest.left)*0.25);

			if (CurrentChar != ' ')
				XsmDop+=(SrcRest.right+XlenDecr) - SrcRest.left-2.0f;
			else
			{
				SetRect(&SrcRest,0,0,0,0); Ysm = 0; XsmDopCount++;
			}
		}

		if (XsmDopCount!=0) Space = (FlattenWidth - XsmDop)/XsmDopCount;
	}






	// прорисовка текста

	float R=1.0f;
	float G=1.0f;
	float B=1.0f;
	if (Transp >= 1.0f) Transp = 1.0f;

	switch (CharSet)
	{
		case 0: // белый
			R=1.0f;G=1.0f;B=1.0f;
			break;
		case 1: // желтый
			R=1.0f;G=1.0f;B=0.0f;
			break;
		case 2: // красный
			R=1.0f;G=0.0f;B=0.0f;
			break;
		case 3: // зеленый
			R=0.0f;G=1.0f;B=0.0f;
			break;
		case 4: // оранжевый
			R=1.0f;G=0.5f;B=0.0f;
			break;
		case 5: // серый
			R=0.5f;G=0.5f;B=0.5f;
			break;
	}

	eTexture *Tex = vw_FindTextureByName("DATA/FONT/font_white.tga");
	if (Tex == 0) return;

	float AW;
	float AH;
	bool ASpresent=false;
	ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_SetTextureV(0, Tex);
	vw_SetTexAlpha(true, 0.01f);
	vw_SetTexAddressMode(0, RI_CLAMP);
	vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

    // не можем ставить другое! если нет мипмапа
    vw_SetTexFiltering(0, RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, 1);

	float ImageHeight = Tex->Height*1.0f;
	float ImageWidth = Tex->Width*1.0f;

	float tmpPosY = 0;

	// выделяем память
	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = 0;
	tmp = new float[(2+2+4)*4*strlen(text)]; if (tmp == 0) return;
	int k=0;
	int SymbolCount = 0;





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

		// достигли предельной ширины
		if (MaxWidth != 0)
			if (Xstart >= MaxWidthLimit) goto DrawLine;


		if (CurrentChar != ' ')
		{
			SymbolCount++;

			SetRect(&DestRest, (int)Xstart,	(int)(Ystart+Ysm*1.25),
				(int)(Xstart+(SrcRest.right-SrcRest.left)*1.25), (int)(Ystart+Ysm*1.25+(SrcRest.bottom-SrcRest.top)*1.25));



			if (ASpresent) tmpPosY = (AH - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));
			else tmpPosY = (AHw - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));

			float FrameHeight = (SrcRest.bottom*1.0f )/ImageHeight;
			float FrameWidth = (SrcRest.right*1.0f )/ImageWidth;

			float Yst = (SrcRest.top*1.0f)/ImageHeight;
			float Xst = (SrcRest.left*1.0f)/ImageWidth;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-Yst;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-Yst;



			Xstart += (DestRest.right+XlenDecr) - DestRest.left-2;
		}
		else
			Xstart += Space;
	}

DrawLine:

	vw_SendVertices(RI_QUADS, 4*SymbolCount, RI_2f_XYZ | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(float));

	if (tmp != 0){delete [] tmp; tmp = 0;}
	vw_SetTexAlpha(false, 0.5f);
    vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_SetTextureDef(0);


}








//------------------------------------------------------------------------------------
// получение ширины фонта
//------------------------------------------------------------------------------------
int FontSize(const char *Text, ...)
{
	if (Text == 0) return 0;

	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	// в text уже полная строка
	if (strlen(text) == 0) return 0;


	RECT SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ysm = 0;
	int XlenDecr = 0;

	int Size = 0;



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
			Size += (SrcRest.right+XlenDecr) - SrcRest.left-1;
		else
			Size += 13;
	}


	// для последней буквы не делаем...
	Size -= XlenDecr;


	return Size;
}








//------------------------------------------------------------------------------------
// получение ширины фонта
//------------------------------------------------------------------------------------
int FontSizeX2(const char *Text, ...)
{
	if (Text == 0) return 0;

	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	// в text уже полная строка
	if (strlen(text) == 0) return 0;


	RECT SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ysm = 0;
	int XlenDecr = 0;

	int Size = 0;


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
		// делаем искуственно ув. на 2 ширины фонта... для просчета
		SrcRest.right += (int)((SrcRest.right - SrcRest.left)*0.5);

		if (CurrentChar != ' ')
			Size += (SrcRest.right+XlenDecr) - SrcRest.left-3;
		else
			Size += 13;
	}


	// для последней буквы не делаем...
	Size -= XlenDecr;


	return Size;
}






//------------------------------------------------------------------------------------
// получение ширины фонта
//------------------------------------------------------------------------------------
int FontSizeX15(const char *Text, ...)
{
	if (Text == 0) return 0;

	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	// в text уже полная строка
	if (strlen(text) == 0) return 0;


	RECT SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ysm = 0;
	int XlenDecr = 0;

	int Size = 0;


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
		// делаем искуственно ув. на 2 ширины фонта... для просчета
		SrcRest.right += (int)((SrcRest.right - SrcRest.left)*0.25);

		if (CurrentChar != ' ')
			Size += (SrcRest.right+XlenDecr) - SrcRest.left-2;
		else
			Size += 13;
	}


	// для последней буквы не делаем...
	Size -= XlenDecr;


	return Size;
}




















//------------------------------------------------------------------------------------
// прорисовка фонта
//------------------------------------------------------------------------------------
void DrawFont2(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text)
{
	if (Text == 0) return;

// FlattenWidth - выравнивать по ширине
// MaxWidth - рисовать до ширины
// CharSet - 0-белый 1-желтый


	// смотрим значения параметров в строке
	char	text[1024];
	strcpy(text, Text);
	if (strlen(text) == 0) return;


	RECT DestRest, SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ystart = Y;
	float Xstart = 1.0f*X;
	int Ysm = 0;
	int XlenDecr = 0;


	// размер пробела в пикселях
	float Space = 13;


	// находим абсолютное значения ограничения по MaxWidth
	float MaxWidthLimit = (Xstart+MaxWidth);



	// если нужно выравнивать, считаем данные пробелов
	if (FlattenWidth !=0)
	{
		float XsmDop = 0.0f;
		int XsmDopCount = 0;

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
				XsmDop+=(SrcRest.right+XlenDecr) - SrcRest.left-1.0f;
			else
			{
				SetRect(&SrcRest,0,0,0,0); Ysm = 0; XsmDopCount++;
			}
		}

		if (XsmDopCount!=0) Space = (FlattenWidth - XsmDop)/XsmDopCount;
	}




	// прорисовка текста
	float R=1.0f;
	float G=1.0f;
	float B=1.0f;
	if (Transp >= 1.0f) Transp = 1.0f;

	switch (CharSet)
	{
		case 0: // белый
			R=1.0f;G=1.0f;B=1.0f;
			break;
		case 1: // желтый
			R=1.0f;G=1.0f;B=0.0f;
			break;
		case 2: // красный
			R=1.0f;G=0.0f;B=0.0f;
			break;
		case 3: // зеленый
			R=0.0f;G=1.0f;B=0.0f;
			break;
		case 4: // оранжевый
			R=1.0f;G=0.5f;B=0.0f;
			break;
		case 5: // серый
			R=0.5f;G=0.5f;B=0.5f;
			break;
	}

	eTexture *Tex = vw_FindTextureByName("DATA/FONT/font_white.tga");
	if (Tex == 0) return;

	float AW;
	float AH;
	bool ASpresent=false;
	ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_SetTextureV(0, Tex);
	vw_SetTexAlpha(true, 0.01f);
	vw_SetTexAddressMode(0, RI_CLAMP);
	vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

    // не можем ставить другое! если нет мипмапа
    vw_SetTexFiltering(0, RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, 1);

	float ImageHeight = Tex->Height*1.0f;
	float ImageWidth = Tex->Width*1.0f;

	float tmpPosY = 0;

	// выделяем память
	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = 0;
	tmp = new float[(2+2+4)*4*strlen(text)]; if (tmp == 0) return;
	int k=0;
	int SymbolCount = 0;





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

		// достигли предельной ширины
		if (MaxWidth != 0)
			if (Xstart >= MaxWidthLimit) goto DrawLine;


		if (CurrentChar != ' ')
		{
			SymbolCount++;

			SetRect(&DestRest, (int)Xstart,	Ystart+Ysm,
				(int)Xstart+SrcRest.right-SrcRest.left, Ystart+Ysm+SrcRest.bottom-SrcRest.top);



			if (ASpresent) tmpPosY = (AH - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));
			else tmpPosY = (AHw - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));

			float FrameHeight = (SrcRest.bottom*1.0f )/ImageHeight;
			float FrameWidth = (SrcRest.right*1.0f )/ImageWidth;

			float Yst = (SrcRest.top*1.0f)/ImageHeight;
			float Xst = (SrcRest.left*1.0f)/ImageWidth;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-Yst;

				tmp[k++] = DestRest.left;	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);	// X
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-Yst;



			Xstart += (DestRest.right+XlenDecr) - DestRest.left-1;
		}
		else
			Xstart += Space;
	}

DrawLine:

	vw_SendVertices(RI_QUADS, 4*SymbolCount, RI_2f_XYZ | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(float));

	if (tmp != 0){delete [] tmp; tmp = 0;}
	vw_SetTexAlpha(false, 0.5f);
    vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_SetTextureDef(0);



}









//------------------------------------------------------------------------------------
// получение ширины фонта
//------------------------------------------------------------------------------------
int FontSize2(const char *Text)
{
	if (Text == 0) return 0;

	// смотрим значения параметров в строке
	char text[1024];
	strcpy(text, Text);
	if (strlen(text) == 0) return 0;


	RECT SrcRest;
	SetRect(&SrcRest, 0, 0, 0, 0);
	int Ysm = 0;
	int XlenDecr = 0;

	int Size = 0;



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
			Size += (SrcRest.right+XlenDecr) - SrcRest.left-1;
		else
			Size += 13;
	}


	// для последней буквы не делаем...
	Size -= XlenDecr;


	return Size;
}




