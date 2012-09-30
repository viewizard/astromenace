/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Texture_PNG.cpp

	Copyright (c) 2011-2012 Michael Kurinnoy, Viewizard
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

#ifndef png

//------------------------------------------------------------------------------------
// загрузка данных из PNG файла в DIB массив
//------------------------------------------------------------------------------------
int ReadPNG(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels)
{
	// просто сообщаем что не поддерживаем, так скомпилирован код
	fprintf(stderr, "Game compiled without PNG format support.\n");
	// возвращаем код ошибки
	return 0;
}

#else // png

#include <png.h>

//------------------------------------------------------------------------------------
// наша функция для работы с виртуальной файловой системой
//------------------------------------------------------------------------------------
void PNGReadFunction(png_structp png_ptr, png_bytep data, png_size_t length)
{
	eFILE* pFile = (eFILE*)png_get_io_ptr(png_ptr);
	pFile->fread(data, 1, length);
}
//------------------------------------------------------------------------------------
// загрузка данных из PNG файла в DIB массив
//------------------------------------------------------------------------------------
int ReadPNG(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels)
{


	// проверяем сигнатуру файла (первые 8 байт)
	png_byte sig[8] = {0};
	pFile->fread(sig, sizeof(png_byte), 8);
	if ( !png_check_sig(sig, 8) )
	{
		// это не png-файл
		fprintf(stderr, "Wrong PNG file SIG data.\n");
		return 0;
	}


	// создаем внутреннюю структуру png для работы с файлом
	// последние параметры - структура, для функции обработки ошибок и варнинга (последн. 2 параметра)
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if ( !png_ptr )
	{
		fprintf(stderr, "PNG: out of memory.\n");
		return 0;
	}
	// создаем структуру с информацией о файле
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if ( !info_ptr )
	{
		png_destroy_read_struct(&png_ptr, 0, 0);	// убиваем внутреннюю структуру
		fprintf(stderr, "PNG: out of memory.\n");
		return 0;
	}


	// настраиваем библиотеку на наш способ чтения файла
	// указатель на pFile можно будет получить потом в PNGReadFunction
	png_set_read_fn(png_ptr, (void*)pFile, PNGReadFunction);

	// говорим библиотеке, что мы уже прочли 8 байт, когда проверяли сигнатуру
	png_set_sig_bytes(png_ptr, 8);


	// Тут можно настроить прогрессбар, таким макаром:
	// описываем функцию где-то выше
	// void read_row_callback(png_ptr, png_uint_32 row, int pass) { }
	// информируем библиотеку об этой функции
//	png_set_read_status_fn(png_ptr, read_row_callback);

	// В png файле инфа в альфа-канале обозначает уровень непрозрачности (level of opacity)
	// если мы хотим использовать альфа-канал как уровень прозрачности (level of transparency)
	// то нужно инвертировать альфа-канал функцией
//	png_set_invert_alpha(png_ptr);


	// читаем всю информацию о файле
	png_read_info(png_ptr, info_ptr); // Эта функция возвращает инфу из info_ptr


	png_uint_32 width = 0, height = 0;	// размер картинки в пикселях
	int bit_depth = 0;	// глубина цвета (одного из каналов, может быть 1, 2, 4, 8, 16)
	int color_type = 0;	// описывает какие каналы присутствуют:
						// PNG_COLOR_TYPE_GRAY, PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE,
						// PNG_COLOR_TYPE_RGB, PNG_COLOR_TYPE_RGB_ALPHA...
	// последние 3 параметра могут быть нулями и обозначают: тип фильтра, тип компрессии и тип смещения
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);


	// если, вдруг png окажется не того формата что нам нужен, постараемся его сконвертить в нужный нам

	// png формат может содержать 16 бит на канал, но нам нужно только 8, поэтому сужаем канал
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	// преобразуем файл если он содержит палитру в нормальный RGB
	if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8) png_set_palette_to_rgb(png_ptr);
	// если в грэйскейле меньше бит на канал чем 8, то конвертим к нормальному 8-битному
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
	// и добавляем полный альфа-канал
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	// все это можно сделать одной функцией png_set_expand(png_ptr);

	// для преобразования грэйскейл картинки в RGB формат, нужно сделать так:
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);



	// для правильного отображения картинки, нужно подобрать гамму в зависимости от настроек
	// монитора, но мы будем проще:
	// 2.2 - хорошо для PC мониторов в освещенной комнате
	// 2.0 - для PC в темной комнате
	// от 1.7 до 1.0 - для систем MacOS
	double gamma = 0.0f;
	// если есть информация о гамме в файле, то устанавливаем на 2.2
	if ( png_get_gAMA(png_ptr, info_ptr, &gamma) ) png_set_gamma(png_ptr, 2.2, gamma);
	// иначе ставим дефолтную гамму для файла в 0.45455 (good guess for GIF images on PCs)
	else png_set_gamma(png_ptr, 2.2, 0.45455);


	// после всех трансформаций, апдейтим информацию в библиотеке
	png_read_update_info(png_ptr, info_ptr);

	// опять получаем все размеры и параметры обновленной картинки
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);


	// получаем кол-во каналов на пиксель
	// может быть: 1 (GRAY, PALETTE), 2 (GRAY_ALPHA), 3 (RGB), 4 (RGB_ALPHA или RGB + filler byte)
	png_byte channels = png_get_channels(png_ptr, info_ptr);
	// определяем кол-во байт нужных для того чтобы вместить строку
	png_uint_32 row_bytes = png_get_rowbytes(png_ptr, info_ptr);


	// теперь, мы можем выделить память чтобы вместить картинку
	png_byte* data = new png_byte[row_bytes * height];
	// выделяем память, для указателей на каждую строку
	png_byte **row_pointers = new png_byte * [height];
	// сопоставляем массив указателей на строчки, с выделенными в памяти (res)
	// т.к. изображение перевернутое, то указатели идут снизу вверх
	for (unsigned int i = 0; i < height; i++)
		row_pointers[height - i - 1] = data + i * row_bytes;


	// все, читаем картинку
	png_read_image(png_ptr, row_pointers);

	// читаем дополнительную информацию о файле (на самом деле игнорируем ее)
	png_read_end(png_ptr, 0);


	// освобождаем память от указателей на строки
	delete [] row_pointers;
	// освобождаем память выделенную для библиотеки libpng
	png_destroy_read_struct(&png_ptr, 0, 0);


	// записываем информацию о картинке
	*DWidth = width;
	*DHeight = height;
	*DChanels = channels;
	*DIB = data;


	return 1;
}

#endif // png
