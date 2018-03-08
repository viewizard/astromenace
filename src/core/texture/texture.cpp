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

// TODO translate comments
// TODO move to std::vector and std::array usage

#include "../graphics/graphics.h"
#include "../vfs/vfs.h"
#include "texture.h"

namespace {

// Ключ прорисовки текстуры (near, linear, ... )
int FilteringTexMan{RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT};
// Ключ прорисовки текстуры (wrap, clamp ... )
int Address_ModeTexMan{RI_WRAP_U | RI_WRAP_V};
// указывает, что канал есть...(или нужно создать по цвету...)
bool AlphaTexMan{false};
// цвет прозрачности для создания Alpha канала...
uint8_t ARedTexMan{0};
uint8_t AGreenTexMan{0};
uint8_t ABlueTexMan{0};
int AFlagTexMan{TX_ALPHA_EQUAL};
// mip mapping
bool MipMap{true};
// Map with all loaded textures.
std::unordered_map<std::string, sTexture> TexturesMap;

} // unnamed namespace

namespace texture {
int ReadTGA(std::vector<uint8_t> &DIB, sFILE *pFile, int &DWidth, int &DHeight, int &DChanels);
} // namespace texture


/*
 * Set textures properties.
 */
void vw_SetTextureProp(int nFiltering, int nAddress_Mode, bool nAlpha, int nAFlag, bool nMipMap)
{
	FilteringTexMan = nFiltering;
	Address_ModeTexMan = nAddress_Mode;
	AlphaTexMan = nAlpha;
	AFlagTexMan = nAFlag;
	MipMap = nMipMap;
}

/*
 * Set textures alpha color.
 */
void vw_SetTextureAlpha(uint8_t nARed, uint8_t nAGreen, uint8_t nABlue)
{
	ARedTexMan = nARed;
	AGreenTexMan = nAGreen;
	ABlueTexMan = nABlue;
}

/*
 * Find texture by name.
 */
sTexture *vw_FindTextureByName(const std::string &Name)
{
	auto tmpTexture = TexturesMap.find(Name);

	if (tmpTexture != TexturesMap.end())
		return &tmpTexture->second;

	return nullptr;
}

/*
 * Release texture.
 */
void vw_ReleaseTexture(sTexture *Texture)
{
	if (!Texture)
		return;

	vw_DeleteTexture(Texture->TextureID);

	// we are forced to check them all in cycle, since our key is texture's name
	for (const auto &tmpTexture : TexturesMap) {
		// TextureID should be unique, as texture name
		if (tmpTexture.second.TextureID == Texture->TextureID) {
			TexturesMap.erase(tmpTexture.first);
			// forced to leave - current iterator invalidated by erase()
			break;
		}
	}
}

/*
 * Release all textures.
 */
void vw_ReleaseAllTextures()
{
	for (auto &tmpTexture : TexturesMap) {
		vw_DeleteTexture(tmpTexture.second.TextureID);
	}
	TexturesMap.clear();

	FilteringTexMan = RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT;
	Address_ModeTexMan = RI_WRAP_U | RI_WRAP_V;
	AlphaTexMan = false;
}

/*
 * Calculate power of two.
 */
static int power_of_two(int Num)
{
	int value = 1;

	while (value < Num) {
		value <<= 1;
	}
	return value;
}

/*
 * Resize image to closest power of two size.
 */
static void Resize(uint8_t **DIB, sTexture *Texture)
{
	if (!DIB || !Texture)
		return;

	// берем размеры к которым нужно "подгонять"
	int powWidth = power_of_two(Texture->Width);
	int powHeight = power_of_two(Texture->Height);

	// нужно ли обрабатывать вообще?
	if ((powWidth == Texture->Width) &&
	    (powHeight == Texture->Height))
		return;

	uint8_t *DIBtemp = *DIB;
	*DIB = new uint8_t[powWidth * powHeight * Texture->Bytes];

	// делаем все по цвету-прозначности + ставим все прозрачным
	uint8_t ColorF[4]{Texture->ARed,
			  Texture->AGreen,
			  Texture->ABlue,
			  0}; //если Texture->Bytes == 4, его возьмем
	for (int i = 0; i < powWidth * powHeight * Texture->Bytes; i += Texture->Bytes) {
		memcpy(*DIB+i, ColorF, Texture->Bytes);
	}

	// находим отступ между строчками
	int stride = Texture->Width * Texture->Bytes;
	// должен быть приведен к кратности 4 байта построчно (чтобы не было проблем с нечетными данными)
	while((stride % 4) != 0) {
		stride++;
	}

	// вставляем исходный рисунок
	for (int y = 0; y < Texture->Height; y++) {
		int st1 = ((y + powHeight - Texture->Height) * powWidth) * Texture->Bytes;
		int st2 = (y * stride);
		memcpy(*DIB + st1, DIBtemp + st2, stride);
	}

	// меняем значения текстуры
	Texture->Width = powWidth;
	Texture->Height = powHeight;
	// освобождаем память
	if (DIBtemp)
		delete [] DIBtemp;
}

/*
 * Resize image to custom size.
 */
static void ResizeImage(int width, int height, uint8_t **DIB, sTexture *Texture)
{
	if (!DIB || !Texture ||
	    ((width == Texture->Width) && (height == Texture->Height)))
		return;

	// переносим во временный массив данные...
	uint8_t *src = *DIB;
	uint8_t *dst = new uint8_t[width * height * Texture->Bytes];

	// растягиваем исходный массив (или сжимаем)
	for (int j = 0; j < height; j++) {
		int y = (j * Texture->Height) / height;
		int offset_y = y * Texture->Width;

		for (int i = 0; i < width; i++) {
			int x = (i * Texture->Width) / width;
			int offset_x = (offset_y + x) * Texture->Bytes;

			dst[(i + j * width) * Texture->Bytes] = src[offset_x];
			dst[(i + j * width) * Texture->Bytes + 1] = src[offset_x + 1];
			dst[(i + j * width) * Texture->Bytes + 2] = src[offset_x + 2];
			if (Texture->Bytes == 4)
				dst[(i + j * width) * Texture->Bytes + 3] = src[offset_x + 3];
		}
	}

	// меняем значения текстуры
	Texture->Width = width;
	Texture->Height = height;
	// освобождаем память
	if (src)
		delete [] src;

	// устанавливаем указатель на новый блок памяти
	*DIB = dst;
}

/*
 * Create alpha channel.
 */
static void CreateAlpha(uint8_t **DIBRESULT, sTexture *Texture, int AlphaFlag)
{
	if (!DIBRESULT || !Texture)
		return;

	// находим отступ между строчками
	int stride = Texture->Width * 3;
	if (Texture->Width != 2 && Texture->Width != 1) {
		while((stride % 4) != 0) {
			stride++;
		}
	}
	int stride2 = Texture->Width * 4;
	while((stride2 % 4) != 0) {
		stride2++;
	}

	// сохраняем во временном указателе
	uint8_t *DIBtemp = *DIBRESULT;
	uint8_t *DIB = new uint8_t[stride2 * Texture->Height];

	// Формируем данные по цветам...
	uint8_t GreyRedC = (uint8_t)(((float)Texture->ARed / 255) * 76);
	uint8_t GreyGreenC = (uint8_t)(((float)Texture->AGreen / 255) * 150);
	uint8_t GreyBlueC = (uint8_t)(((float)Texture->ABlue / 255) * 28);
	uint8_t GreyC = GreyBlueC + GreyGreenC + GreyRedC;

	for(int j1 = 0; j1 < Texture->Height; j1++) {
		int k1 = stride * j1;// делаем правильное смещение при переходе
		int k2 = stride2 * j1;

		for(int j2 = 0; j2 < Texture->Width; j2++) {
			DIB[k2] = DIBtemp[k1];
			DIB[k2 + 1] = DIBtemp[k1 + 1];
			DIB[k2 + 2] = DIBtemp[k1 + 2];

			uint8_t GreyRed, GreyGreen, GreyBlue, Grey;

			switch(AlphaFlag) {
			case TX_ALPHA_GREYSC:
				// Формируем данные по цветам...
				GreyRed = (uint8_t)(((float)DIB[k2 + 2] / 255) * 76);
				GreyGreen = (uint8_t)(((float)DIB[k2 + 1] / 255) * 150);
				GreyBlue = (uint8_t)(((float)DIB[k2] / 255) * 28);
				DIB[k2 + 3] = GreyBlue + GreyGreen + GreyRed;
				break;

			case TX_ALPHA_EQUAL:
				if ((Texture->ABlue == DIB[k2]) &&
				    (Texture->AGreen == DIB[k2 + 1]) &&
				    (Texture->ARed == DIB[k2 + 2]))
					DIB[k2 + 3] = 0;//Alpha
				else
					DIB[k2 + 3] = 255;
				break;

			case TX_ALPHA_GEQUAL:
				// Формируем данные по цветам...
				GreyRed = (uint8_t)(((float)DIB[k2 + 2] / 255) * 76);
				GreyGreen = (uint8_t)(((float)DIB[k2 + 1] / 255) * 150);
				GreyBlue = (uint8_t)(((float)DIB[k2] / 255) * 28);
				Grey = GreyBlue + GreyGreen + GreyRed;

				if (GreyC >= Grey)
					DIB[k2+3] = 0;//Alpha
				else
					DIB[k2 + 3] = 255;
				break;

			case TX_ALPHA_LEQUAL:
				// Формируем данные по цветам...
				GreyRed = (uint8_t)(((float)DIB[k2 + 2] / 255) * 76);
				GreyGreen = (uint8_t)(((float)DIB[k2 + 1] / 255) * 150);
				GreyBlue = (uint8_t)(((float)DIB[k2] / 255) * 28);
				Grey = GreyBlue + GreyGreen + GreyRed;

				if (GreyC <= Grey)
					DIB[k2+3] = 0;//Alpha
				else
					DIB[k2 + 3] = 255;
				break;

			case TX_ALPHA_GREAT:
				// Формируем данные по цветам...
				GreyRed = (uint8_t)(((float)DIB[k2 + 2] / 255) * 76);
				GreyGreen = (uint8_t)(((float)DIB[k2 + 1] / 255) * 150);
				GreyBlue = (uint8_t)(((float)DIB[k2] / 255) * 28);
				Grey = GreyBlue + GreyGreen + GreyRed;

				if (GreyC > Grey)
					DIB[k2 + 3] = 0;//Alpha
				else
					DIB[k2 + 3] = 255;
				break;

			case TX_ALPHA_LESS:
				// Формируем данные по цветам...
				GreyRed = (uint8_t)(((float)DIB[k2 + 2] / 255) * 76);
				GreyGreen = (uint8_t)(((float)DIB[k2 + 1] / 255) * 150);
				GreyBlue = (uint8_t)(((float)DIB[k2] / 255) * 28);
				Grey = GreyBlue + GreyGreen + GreyRed;

				if (GreyC < Grey)
					DIB[k2+3] = 0;//Alpha
				else
					DIB[k2 + 3] = 255;
				break;

			default:
				DIB[k2 + 3] = 255;
				break;
			}

			k2 += 4;
			k1 += 3;
		}
	}

	if (DIBtemp)
		delete [] DIBtemp;
	*DIBRESULT = DIB;
	Texture->Bytes = 4;
}

/*
 * Remove alpha channel.
 */
static void RemoveAlpha(uint8_t **DIBRESULT, sTexture *Texture)
{
	// находим отступ между строчками
	int stride = Texture->Width * 3;
	while ((stride % 4) != 0) {
		stride++;
	}
	int stride2 = Texture->Width * 4;
	while ((stride2 % 4) != 0) {
		stride2++;
	}

	// сохраняем во временном указателе
	uint8_t *DIBtemp  = *DIBRESULT;
	uint8_t *DIB = new uint8_t[stride * Texture->Height];

	for(int j1 = 0; j1 < Texture->Height; j1++) {
		int k1 = stride * j1;
		int k2 = stride2 * j1;

		for(int j2 = 0; j2 < Texture->Width; j2++) {
			DIB[k1] = DIBtemp[k2];
			DIB[k1 + 1] = DIBtemp[k2 + 1];
			DIB[k1 + 2] = DIBtemp[k2 + 2];

			k2 += 4;
			k1 += 3;
		}
	}

	if (DIBtemp)
		delete [] DIBtemp;
	*DIBRESULT = DIB;
	Texture->Bytes = 3;
}

/*
 * Convert supported image file format to VW2D format.
 */
void vw_ConvertImageToVW2D(const std::string &SrcName, const std::string &DestName)
{
	if (SrcName.empty() || DestName.empty())
		return;

	int DWidth{0};
	int DHeight{0};
	int DChanels{0};
	std::vector<uint8_t> tmpImage{};
	int LoadAs{TGA_FILE};

	// Открываем файл
	std::unique_ptr<sFILE> pFile = vw_fopen(SrcName);
	if (pFile == nullptr) {
		std::cerr << "Unable to found " << SrcName << "\n";
		return;
	}

	// Ищем как грузить по расширению
	// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
	const char *file_ext = strrchr(SrcName.c_str(), '.');
	if (file_ext) {
		if (!strcasecmp(".tga", file_ext))
			LoadAs = TGA_FILE;
		else
			std::cerr << "Format not supported " << SrcName << "\n";
	}

	// Загружаем
	switch (LoadAs) {
	case TGA_FILE:
		texture::ReadTGA(tmpImage, pFile.get(), DWidth, DHeight, DChanels);
		break;

	default:
		std::cerr << "Unable to load " << SrcName << "\n";
		return;
	}

	if (tmpImage.empty()) {
		std::cerr << "Unable to load " << SrcName << "\n";
		return;
	}

	// все, файл нам больше не нужен
	vw_fclose(pFile);

	// записываем данные на диск
	SDL_RWops *FileVW2D;
	FileVW2D = SDL_RWFromFile(DestName.c_str(), "wb");
	// если не можем создать файл на запись - уходим
	if (!FileVW2D) {
		std::cerr << "Can't create " << DestName << " file on disk.\n";
		return;
	}

	// маркер файла 4 байта
	char tmpSign[4]{'V','W','2','D'};
	SDL_RWwrite(FileVW2D, tmpSign, 4, 1);

	SDL_RWwrite(FileVW2D, &DWidth, sizeof(DWidth), 1);
	SDL_RWwrite(FileVW2D, &DHeight, sizeof(DHeight), 1);
	SDL_RWwrite(FileVW2D, &DChanels, sizeof(DChanels), 1);
	SDL_RWwrite(FileVW2D, tmpImage.data(), DWidth * DHeight * DChanels, 1);

	SDL_RWclose(FileVW2D);
}

/*
 * Load texture from file.
 */
sTexture *vw_LoadTexture(const std::string &nName, int CompressionType, int LoadAs, int NeedResizeW, int NeedResizeH)
{
	if (nName.empty())
		return nullptr;

	int DWidth{0};
	int DHeight{0};
	int DChanels{0};
	std::vector<uint8_t> tmpImage{};

	// Открываем файл
	std::unique_ptr<sFILE> pFile = vw_fopen(nName);
	if (!pFile) {
		std::cerr << "Unable to found " << nName << "\n";
		return nullptr;
	}

	// Ищем как грузить текстуру по расширению
	if (LoadAs == AUTO_FILE) {
		// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
		const char *file_ext = strrchr(nName.c_str(), '.');
		if (file_ext) {
			if (!strcasecmp(".tga", file_ext))
				LoadAs = TGA_FILE;
			else if (!strcasecmp(".vw2d", file_ext))
				LoadAs = VW2D_FILE;
			else
				std::cerr << "Format not supported " << nName << "\n";
		}
	}

	// Загружаем текстуру
	switch(LoadAs) {
	case TGA_FILE:
		texture::ReadTGA(tmpImage, pFile.get(), DWidth, DHeight, DChanels);
		break;

	case VW2D_FILE:
		// пропускаем заголовок "VW2D"
		pFile->fseek(4, SEEK_SET);
		// считываем ширину
		pFile->fread(&DWidth, sizeof(DWidth), 1);
		// считываем высоту
		pFile->fread(&DHeight, sizeof(DHeight), 1);
		// считываем кол-во каналов
		pFile->fread(&DChanels, sizeof(DChanels), 1);
		// резервируем память
		tmpImage.resize(DWidth * DHeight * DChanels);
		// считываем уже готовый к созданию текстуры массив
		pFile->fread(tmpImage.data(), DWidth * DHeight * DChanels, 1);
		break;

	default:
		return nullptr;
	}

	if (tmpImage.empty()) {
		std::cerr << "Unable to load " << nName << "\n";
		return nullptr;
	}

	// все, файл нам больше не нужен
	vw_fclose(pFile);

	return vw_CreateTextureFromMemory(nName, tmpImage.data(), DWidth, DHeight, DChanels,
					  CompressionType, NeedResizeW, NeedResizeH);
}

/*
 * Create texture from memory.
 */
sTexture *vw_CreateTextureFromMemory(const std::string &TextureName, uint8_t *DIB, int DWidth,
				     int DHeight, int DChanels, int CompressionType, int NeedResizeW,
				     int NeedResizeH, bool NeedDuplicateCheck)
{
	if (TextureName.empty() || !DIB || (DWidth <= 0) || (DHeight <= 0))
		return nullptr;

	// проверяем в списке, если уже создавали ее - просто возвращаем указатель
	if (NeedDuplicateCheck) {
		sTexture *tmpTexture = vw_FindTextureByName(TextureName);
		if (tmpTexture) {
			std::cout << "Texture already loaded: " << TextureName << "\n";
			return tmpTexture;
		}
	}

	// Начальные установки текстуры
	TexturesMap[TextureName].ARed = ARedTexMan;
	TexturesMap[TextureName].AGreen = AGreenTexMan;
	TexturesMap[TextureName].ABlue = ABlueTexMan;
	TexturesMap[TextureName].TextureID = 0;
	TexturesMap[TextureName].Width = DWidth;
	TexturesMap[TextureName].Height = DHeight;
	TexturesMap[TextureName].Bytes = DChanels;

	// временный массив данных
	uint8_t *tmp_image = new uint8_t[DWidth * DHeight * DChanels];
	memcpy(tmp_image, DIB, DWidth * DHeight * DChanels);

	// Делаем альфа канал
	if (TexturesMap[TextureName].Bytes == 4) {
		if (!AlphaTexMan)
			RemoveAlpha(&tmp_image, &TexturesMap[TextureName]);
	} else if ((TexturesMap[TextureName].Bytes == 3) && (AlphaTexMan))
		CreateAlpha(&tmp_image, &TexturesMap[TextureName], AFlagTexMan);

	// Растягиваем, если есть запрос
	if ((NeedResizeW != 0) && (NeedResizeH != 0))
		ResizeImage(NeedResizeW, NeedResizeH, &tmp_image, &TexturesMap[TextureName]);

	// Сохраняем размеры картинки
	TexturesMap[TextureName].SrcWidth = TexturesMap[TextureName].Width;
	TexturesMap[TextureName].SrcHeight = TexturesMap[TextureName].Height;

	// Делаем подгонку по размерам, с учетом необходимости железа
	if (!vw_GetDevCaps()->TextureNPOTSupported) // если не поддерживаем - берем и сами растягиваем до степени двойки
		Resize(&tmp_image, &TexturesMap[TextureName]);

	// Создаем текстуру
	TexturesMap[TextureName].TextureID = vw_BuildTexture(tmp_image, TexturesMap[TextureName].Width,
							     TexturesMap[TextureName].Height, MipMap,
							     TexturesMap[TextureName].Bytes, CompressionType);
	// устанавливаем параметры
	vw_SetTextureFiltering(FilteringTexMan);
	vw_SetTextureAddressMode(Address_ModeTexMan);
	// анбиндим
	vw_BindTexture(0, 0);

	// освобождаем память
	if (tmp_image)
		delete [] tmp_image;

	// присоединяем текстуру к менеджеру текстур
	std::cout << "Texture created from memory: " << TextureName << "\n";
	return &TexturesMap[TextureName];
}
