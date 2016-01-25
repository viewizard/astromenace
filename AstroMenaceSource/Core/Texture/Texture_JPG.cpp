/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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


#include "Texture.h"


#ifndef jpeg



//------------------------------------------------------------------------------------
// загрузка данных из JPG файла в DIB массив
//------------------------------------------------------------------------------------
int ReadJPG(BYTE **UNUSED(DIB), eFILE *UNUSED(pFile), int *UNUSED(DWidth), int *UNUSED(DHeight), int *UNUSED(DChanels))
{
	// просто сообщаем что не поддерживаем, так скомпилирован код
	fprintf(stderr, "Game compiled without JPG format support.\n");
	// возвращаем код ошибки
	return 0;
}



#else // jpeg






extern "C" {
 #include "jpeglib.h"
}
#include "setjmp.h"
#include "jmorecfg.h"


// структура содержащая необходимую для этого информацию
struct cDataManagerJPEGError
{
    jpeg_error_mgr  pub;      // ссылочка на стандартный менеджер
    jmp_buf    setjmp_buffer;  // jump-буфера, идентификатор прыжка
};


// функция обработки ошибок
void JPEGErrorExit(j_common_ptr cinfo)
{
    // получаем ссылочку на нашу структуру
    cDataManagerJPEGError* myerr = (cDataManagerJPEGError*)cinfo->err;
    // выводим сообщение об ошибке (наверное можно убрать)
    (*cinfo->err->output_message)(cinfo);
    // делаем прыжок на очистку данных и ретурн ошибки
    longjmp(myerr->setjmp_buffer, 1);
}


struct cDataManagerJPEGSource
{
    jpeg_source_mgr  pub;    // ссылочка на стандартный менеджер
    eFILE      *file;  // открытый файл
    JOCTET    *buffer;  // буфер данных
    bool      sof;    // признак того, что файл только что открыли
};
// размер буфера в памяти, куда будем читать информацию
const int c_JPEGInputBufferSize = 4096;



// инициализация источника. вызывается до того, как какая-нибудь информация
// будет из него прочтена.
void JPEGInitSource(j_decompress_ptr cinfo)
{
	cDataManagerJPEGSource* src = (cDataManagerJPEGSource*)cinfo->src;
	// говорим, шо файл тока шо открыт, выдыруг он пустой? :)
	src->sof = true;
}



// вызывается, когда переменная bytes_in_buffer достигает 0 и возникает
// необходимость в новой порции информации. возвращает TRUE, если буфер
// перезаполнен успешно, и FALSE если произошла ошибка ввода/вывода.
boolean JPEGFillInputBuffer(j_decompress_ptr cinfo)
{
	cDataManagerJPEGSource* src = (cDataManagerJPEGSource*)cinfo->src;
	// читаем кусками по c_JPEGInputBufferSize байт
	size_t nbytes = src->file->fread(src->buffer, sizeof(JOCTET), c_JPEGInputBufferSize);
	// если мы ничего не считали :(
	if (nbytes == 0)
	{
		if ( src->sof )  return(FALSE); // блин, нам дали пустой файл - заорем "нехорошо" :)
		// если уже читали до этого, то вставляем в буфер инфу о конце файла
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	// загоняем инфу в буфер, и размер скока прочли
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->sof = false;  // файл не пустой, пронесло :)))
	// возвращаем успешное выполнение операции
	return(TRUE);
}


// происходит, когда необходимо пропустить num_bytes. в случае опустошения
// буфера, его необходимо перезагрузить.
void JPEGSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
	cDataManagerJPEGSource* src = (cDataManagerJPEGSource*)cinfo->src;
	// если нужно снести 0 байт :) обижено уходим
	if (num_bytes <= 0) return;
	// выкидываем инфу из буфера и перегружаем его, пока num_bytes не станет
	// меньше размера буфера
	while (num_bytes > (long) src->pub.bytes_in_buffer) {
		num_bytes -= (long) src->pub.bytes_in_buffer;
		JPEGFillInputBuffer(cinfo);
	}
	// а теперь просто правильно настраиваем указатели на оставшуюся часть
	src->pub.next_input_byte += (size_t) num_bytes;
	src->pub.bytes_in_buffer -= (size_t) num_bytes;
}

// убить ресурс. вызывается функцией jpeg_finish_decompress когда все данные будут
// прочтены. у нас ничего сносить не надо.
void JPEGTermSource(j_decompress_ptr UNUSED(cinfo)) { }







void JPEGStdioSrc(j_decompress_ptr cinfo, eFILE* file)
{
	cDataManagerJPEGSource* src = 0;
	// смотрим, выделена ли память под JPEG-декомпрессор менеджер?
	// возможна ситуация, когда происходит одновременное обращение к источнику
	// от нескольких библиотек
	if (cinfo->src == 0)
	{
      // выделим память под наш менеджер, и установим на него указатель глобальной структуры
      // библиотеки. так как я использую менеджер памяти библиотеки JPEG то позаботится об
      // освобождении она сама. JPOOL_PERMANENT - означает что эта память выделяется на все
      // время работы с библиотекой
      cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
        ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(cDataManagerJPEGSource));
      src = (cDataManagerJPEGSource*) cinfo->src;
      // выделяю память для буффера данных, прочитанных из файла
      src->buffer = (JOCTET*) (*cinfo->mem->alloc_small)
        ((j_common_ptr) cinfo, JPOOL_PERMANENT, c_JPEGInputBufferSize * sizeof(JOCTET));
      memset(src->buffer, 0, c_JPEGInputBufferSize * sizeof(JOCTET));
	}
	// для краткости - сестры таланта
	src = (cDataManagerJPEGSource*)cinfo->src;
	// настраиваем обработчики событий на наши функции
    src->pub.init_source = JPEGInitSource;
    src->pub.fill_input_buffer = JPEGFillInputBuffer;
    src->pub.skip_input_data = JPEGSkipInputData;
    src->pub.resync_to_restart = jpeg_resync_to_restart; // use default method
    src->pub.term_source = JPEGTermSource;
    // теперь заполняем поля нашей структуры
    src->file = file;
    // настраиваем указатели на буфера
    src->pub.bytes_in_buffer = 0;  // forces fill_input_buffer on first read
    src->pub.next_input_byte = 0;  // until buffer loaded
    // !!! ВНИМАНИЕ !!! в библиотеке глючок, почему-то если infile стандартного менеджера
    // установлен в 0 то jpeg_start_decompress отказывается работать... поэтому установим его в 1 :)
//    src->pub.infile = (FILE*)1;    // указатель на файл в стандартном менеджере, см. выше
//    src->pub.buffer = 0;      // это буфер стандартного менеджера загрузки, мы им не пользуемся
}






//------------------------------------------------------------------------------------
// загрузка данных из JPG файла в DIB массив
//------------------------------------------------------------------------------------
int ReadJPG(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels)
{

	jpeg_decompress_struct	cinfo;
	cDataManagerJPEGError	jerr;

	// Настраиваем обработку ошибок JPEG декомпрессором.
	cinfo.err = jpeg_std_error(&jerr.pub);    // устанавливаем дефолтный менеджер обработки ошибок
	jerr.pub.error_exit = JPEGErrorExit;    // присваиваем дефолтную функцию для обработки ошибки
	// Устанавливаем контекст возвращения setjmp на JPEGErrorExit
	if( setjmp(jerr.setjmp_buffer) )
	{
		// когда-то произошла ошибка. Очистим структуру и закроем файл
		jpeg_destroy_decompress(&cinfo);
		return 0;
	}

	// инициализируем декомпрессионный объект
	jpeg_create_decompress(&cinfo);
	// Шаг 2: указываем источник данных
	JPEGStdioSrc(&cinfo, pFile);
	// Шаг 3: читаем параметры JPEG файла
	jpeg_read_header(&cinfo, TRUE);
	// Шаг 5: начинаем декомпрессию
	jpeg_start_decompress(&cinfo);
	// вычисляем размер строки в байтах
	int row_stride = cinfo.output_width * cinfo.output_components;
	// выделяем память для картинки
	*DWidth = cinfo.output_width;
	*DHeight = cinfo.output_height;
	*DIB = new BYTE[cinfo.output_height * row_stride];
	// Выделяем память для одной строки. Но нужно помнить, что это массив строк, просто он состоит
	// из одной строки :) JPOOL_IMAGE, означает, что память выделена пока читаем эту картинку
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)( (j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1 );
	memset(*buffer, 0, sizeof(JSAMPLE) * row_stride);
	// Шаг 6: считываем данные
	// Т.к. юзаем внутреннюю переменную cinfo.output_scanline, то не будем отслеживать сами себя
	int y = 0;
	while ( cinfo.output_scanline < cinfo.output_height ) {
		// читаем строку в буфер
		jpeg_read_scanlines(&cinfo, buffer, 1);
		// и вставляем эту строку на нужное место в массиве картинки
		// блин, они опять перевернули картинку! :(
		BYTE* temp = *DIB + (cinfo.output_height - y - 1) * row_stride;
		memcpy(temp, buffer[0], row_stride);
		y++;
	}
	// Шаг 7: Завершаем декомпрессию файла
	jpeg_finish_decompress(&cinfo);
	// Шаг 8: Очищаем объект декомпрессии
	jpeg_destroy_decompress(&cinfo);


	*DChanels = 3;

	return 1;
}



#endif // jpeg
