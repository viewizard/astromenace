/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


#include "VFS.h"



//------------------------------------------------------------------------------------
// разарх данных методом Хаффмана с динамической таблицей
//------------------------------------------------------------------------------------
int vw_HAFFtoDATA(int size, BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS)
{
	// читаем счетчик бит...
	int BitCount = 0 ;
	BitCount = srcVFS[3];
    BitCount *= 0x100; // сдвиг на 8 бит...
	BitCount += srcVFS[2];
    BitCount *= 0x100; // сдвиг на 8 бит...
	BitCount += srcVFS[1];
    BitCount *= 0x100; // сдвиг на 8 бит...
	BitCount += srcVFS[0];

    // читаем кол-во элементов таблицы
    int  TabCount = srcVFS[4] + 1;// кол-во элементов...

	// в цикле, читаем все элементы таблицы
	BYTE Count[256]; // порядковый номер (код) элемента в таблице...
	for (int i = 5; i < TabCount+5; i++)
		Count[i-5] = srcVFS[i];// записываем только код...

	// первый проход - просчет памяти резулт. послед...
	(*dsizeVFS) = 0;
	BYTE tmpMask = 0;// маска, с которой будем работать...
	BYTE mMask = 0;// маска, которую составляем...
	int cBit = 0;// кол-во информационных бит в маске...
    int j = 0; // указатель на позицию в преобразованных данных.
	bool Dat = false;
	int tmpBitCount = BitCount;
	bool ex = false;

	if (size == 0)
	{
		for (int i = 0 + 4 + 1 + TabCount; i < ssizeVFS; i++)
		{
			tmpMask = srcVFS[i];
			if (tmpBitCount<=0) break;
			// работаем с каждым битом в байте...
			for (int k = 0; k < 8; k++)
			{
				ex=false;
				// если закрыли последовательность и след. бит - 0, символ 0...
				if ((cBit==0)&((tmpMask & 1) == 0)&(!Dat)&(!ex))
				{
					// пишем в рез. послед. 0-й элемент...
					(*dsizeVFS)++;
					tmpBitCount --;
					if (tmpBitCount<=0) break;
					ex=true;
				}
				// если закрыли последовательность и след. бит - 1, работаем...
				if ((cBit>=0)&((tmpMask & 1) == 1)&(!Dat)&(!ex))
				{
					// ув. счетчик бит...
					cBit++;
					tmpBitCount --;
					if (tmpBitCount<=0) break;
					ex=true;
				}
				if ((Dat)&(!ex))
				{
  					mMask = mMask*2;
					mMask = mMask & 0xFE;
					mMask = mMask | (tmpMask & 1);
					cBit --;
					if (cBit<=0)
					{
						Dat = false;
						(*dsizeVFS)++;
						cBit = 0;
					}
					tmpBitCount --;
					if (tmpBitCount<=0) break;
					ex=true;
				}
				if ((cBit>0)&((tmpMask & 1) == 0)&(!Dat)&(!ex))
				{
					mMask = 1;
					cBit --;
					if (cBit>0) {Dat= true;}
					else
					{ (*dsizeVFS)++; }
					tmpBitCount --;
					if (tmpBitCount<=0) break;
					ex=true;
				}
				// сдвиг. - установка на след бит...
				tmpMask = tmpMask/2;
			}
		}
	}
	else (*dsizeVFS) = size;

	// резервируем память...
    *dstVFS = new BYTE[*dsizeVFS]; if ((*dstVFS) == 0) return 0;

	// второй проход - получение результирующ. послед...
	cBit = 0;
	Dat = false;
	tmpBitCount = BitCount;
	ex = false;
	for (int i = 0 + 4 + 1 + TabCount; i < ssizeVFS; i++)
	{
		tmpMask = srcVFS[i];
		if (tmpBitCount<=0) break;
		// работаем с каждым битом в байте...
		for (int k = 0; k < 8; k++)
		{
			ex=false;
			// если закрыли последовательность и след. бит - 0, символ 0...
			if ((cBit==0)&((tmpMask & 1) == 0)&(!Dat)&(!ex))
			{
				// пишем в рез. послед. 0-й элемент...
				(*dstVFS)[j] = Count[0];
				j++;
				tmpBitCount --;
				if (tmpBitCount<=0) break;
				ex=true;
			}
			// если закрыли последовательность и след. бит - 1, работаем...
			if ((cBit>=0)&((tmpMask & 1) == 1)&(!Dat)&(!ex))
			{
				// ув. счетчик бит...
				cBit++;
				tmpBitCount --;
				if (tmpBitCount<=0) break;
				ex=true;
			}
			if ((Dat)&(!ex))
			{
				mMask = mMask*2;
				mMask = mMask & 0xFE;
				mMask = mMask | (tmpMask & 1);
				cBit --;
				if (cBit<=0)
				{
					Dat = false;
					(*dstVFS)[j] = Count[mMask];
					cBit = 0;
					j++;
				}
				tmpBitCount --;
				if (tmpBitCount<=0) break;
				ex=true;
			}

			if ((cBit>0)&((tmpMask & 1) == 0)&(!Dat)&(!ex))
			{
				mMask = 1;
				cBit --;
				if (cBit>0) {Dat= true;}
				else
				{
					(*dstVFS)[j] = Count[mMask];
					j++;
				}
				tmpBitCount --;
				if (tmpBitCount<=0) break;
				ex=true;
			}

			// сдвиг. - установка на след бит...
			tmpMask = tmpMask/2;
		}
	}

	return 0;
}




//------------------------------------------------------------------------------------
// архивир данных методом Хаффмана с динамической таблицей
//------------------------------------------------------------------------------------
int vw_DATAtoHAFF(BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS)
{
	// мат. анализ структуры входных данных...
    // собираем данные о вхождении каждого символа...
	BYTE Count[256]; // порядковый номер (код) элемента в таблице...
	int  mathTab[256]; // кол-во вхождений каждого эл-та...
	int  TabCount = 0;// кол-во элементов... (для составления динам. таблици...)
    short bitMask[256]; // битовая маска для каждого эл-та таблици...(2 бита - по макс.)
	int bitMaskS[256]; // количество бит в маске...
	*dsizeVFS = 0;

    // заносим номера (коды) в таблицу...иниц. табл. вхождений...
    for (int i = 0; i < 256; i++)
	{
		Count[i] = i;
		mathTab[i] = 0;
		bitMask[i] = 0;
		bitMaskS[i] = 0;
	}

	// исследуем данные...
    for (int i = 0; i < ssizeVFS; i++) mathTab[srcVFS[i]]++;

	// упорядочевание полученной таблицы...
    // сортируем данные по возростанию кол-ва вхождений...Пузырьковая Сортировка
	for (int i = 0; i < 255; i++)
		for (int j = 0; j < 255; j++)
			if (mathTab[j+1] > mathTab[j])
			{
				int tmp1 = mathTab[j+1];
				mathTab[j+1] = mathTab[j];
				mathTab[j] = tmp1;
				BYTE tmp2 = Count[j+1];
				Count[j+1] = Count[j];
				Count[j] = tmp2;
			}

	// находим элементы, которые есть в последовательности (т.е первые эл-ты упоряд. посл.)
	for (int i = 0; i < 256; i++)
		if (mathTab[i] >0) TabCount++;

    // составляем маски для существующих элементов....
    // для 0...
    bitMask[0] = 0;
    bitMaskS[0] = 1;
    // для 1...
    bitMask[1] = 2;
    bitMaskS[1] = 2;
    // для 2,3...
	for (int i = 2; i < 4; i++)
    {
		bitMask[i] = i-2;
		bitMask[i] = bitMask[i] | 12; // 1100
		bitMaskS[i] = 4;
	}
    // для 4,7...
	for (int i = 4; i < 8; i++)
    {
		bitMask[i] = i-4;
		bitMask[i] = bitMask[i] | 56; // 111000
		bitMaskS[i] = 6;
	}
    // для 8,15...
	for (int i = 8; i < 16; i++)
    {
		bitMask[i] = i-8;
		bitMask[i] = bitMask[i] | 240; // 11110000
		bitMaskS[i] = 8;
	}
    // для 16,31...
	for (int i = 16; i < 32; i++)
    {
		bitMask[i] = i-16;
		bitMask[i] = bitMask[i] | 992; // 1111100000
		bitMaskS[i] = 10;
	}
    // для 32,63...
	for (int i = 32; i < 64; i++)
    {
		bitMask[i] = i-32;
		bitMask[i] = bitMask[i] | 4032; // 11111110000000
		bitMaskS[i] = 12;
	}
    // для 64,127...
	for (int i = 64; i < 128; i++)
    {
		bitMask[i] = i-64;
		bitMask[i] = bitMask[i] | 16256; // 11111110000000
		bitMaskS[i] = 14;
	}
    // для 128,255...
	for (int i = 128; i < 256; i++)
    {
		bitMask[i] = i-128;
		bitMask[i] = bitMask[i] | 65280; // 1111111100000000
		bitMaskS[i] = 16;
	}

	// просчет кол-ва необходимого места...
    (*dsizeVFS) ++;// кол-во элементов в массиве...
    (*dsizeVFS) += TabCount;// место для таблици...
    // вычисляем место для данных...
	int tmp = 0;
    for (int i = 0; i < TabCount; i++)
	{
		tmp += (mathTab[i] * bitMaskS[i]);
	}
	(*dsizeVFS) += 4 ; //счетчик бит...
    (*dsizeVFS) += tmp / 8 +1;// т.к. считали в битах...

    // резервируем память...
	*dstVFS = 0;
    *dstVFS = new BYTE[*dsizeVFS];
	if (*dstVFS == 0) return 0;

    // создаем выходную последовательность...
    int j = 0; // указатель на позицию в преобразованных данных.

	// записываем счетчик бит...
	(*dstVFS)[j] =  (BYTE)tmp;
	j++;
    tmp /= 0x100;
	(*dstVFS)[j] =  (BYTE)tmp;
	j++;
    tmp /= 0x100;
	(*dstVFS)[j] =  (BYTE)tmp;
	j++;
    tmp /= 0x100;
	(*dstVFS)[j] =  (BYTE)tmp;
	j++;

	// записываем кол-во данных в таблице...
	(*dstVFS)[j] = (BYTE)(TabCount-1) ;// чтобы было от 0 до 255 ...
	j++;
	// пишем таблицу...
    for (int i = 0; i < TabCount; i++)
	{
		(*dstVFS)[j] = Count[i];// записываем только код...
		j++;
	}
    // "разупорядочиваем" массивы во временные последовательности...
    short tmbitMask[256];
	int tmbitMaskS[256];
    for (int i = 0; i < 256; i++)
	{
		tmbitMask[Count[i]] = bitMask[i];
		tmbitMaskS[Count[i]] = bitMaskS[i];
	}


    // перебираем данные и пакуем...
	short tmpMask = 0;// маска, с которой будем работать...
    int BitWr = 0; // кол-во бит, которые надо записать из маски...
	int DstBA = 0; // текущий указатель на активный бит в байте результ. массива...
    BYTE T = 0; // текущий бит на установку...
	(*dstVFS)[j] = 0; // предустановка на ИЛИ...

    for (int i = 0; i < ssizeVFS; i++)
	{
		// берем исходные данные...
		tmpMask = tmbitMask[srcVFS[i]];
		BitWr = tmbitMaskS[srcVFS[i]];

 		// уст. крайний бит...
		//tmpMask = (short)(tmpMask * exp( (16-BitWr) * log(2)));
		if (BitWr == 1) tmpMask = tmpMask * 32768;
		if (BitWr == 2) tmpMask = tmpMask * 16384;
		if (BitWr == 4) tmpMask = tmpMask * 4096;
		if (BitWr == 6) tmpMask = tmpMask * 1024;
		if (BitWr == 8) tmpMask = tmpMask * 256;
		if (BitWr == 10) tmpMask = tmpMask * 64;
		if (BitWr == 12) tmpMask = tmpMask * 16;
		if (BitWr == 14) tmpMask = tmpMask * 4;

		// в цикле по BitWr
		while (BitWr > 0)
		{
			// проверка крайнего бита в tmpMask
			// установка аналога в dst[j]
			if ((tmpMask & 0x8000) == 0x8000) T = 1;
			else T = 0;
			// убираем крайний (уже исследованный) бит
			tmpMask = tmpMask * 2;
			// сдвиг на DstBA в tmpMask
			if (DstBA == 1) T = T * 2;
			if (DstBA == 2) T = T * 4;
			if (DstBA == 3) T = T * 8;
			if (DstBA == 4) T = T * 16;
			if (DstBA == 5) T = T * 32;
			if (DstBA == 6) T = T * 64;
			if (DstBA == 7) T = T * 128;

			(*dstVFS)[j] = (*dstVFS)[j] + T;
			BitWr -- ;
			// увеличение DstBA
			DstBA++;
			// проверка на достижение DstBA==8
			if (DstBA > 7)
			{
 				// если да - j++, dst[j] = 0, DstBA=0
				j++;
				(*dstVFS)[j] = 0;
				DstBA=0;
			}
		}
	}

	return 0;
}
