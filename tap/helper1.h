#include <stdio.h>
#include <fstream>
#include <windows.h>
#include <iostream>
#define Uint8 unsigned char
inline int LoadBitmap1(const char* location, Uint8* pixels,
                       int &w, int &h)
{
    Uint8* datBuff[2] = {nullptr, nullptr}; // Header buffers

	pixels = nullptr; // Pixels

	BITMAPFILEHEADER* bmpHeader = nullptr; // Header
	BITMAPINFOHEADER* bmpInfo   = nullptr; // Info
	// The file... We open it with it's constructor
	std::ifstream file(location, std::ios::binary);
	if(!file)
	{
		std::cout << "Failure to open bitmap file.\n";

		return 1;
	}
	// Allocate byte memory that will hold the two headers
	datBuff[0] = new Uint8[sizeof(BITMAPFILEHEADER)];
	datBuff[1] = new Uint8[sizeof(BITMAPINFOHEADER)];

	file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
	file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));
	bmpHeader = (BITMAPFILEHEADER*) datBuff[0];
	bmpInfo   = (BITMAPINFOHEADER*) datBuff[1];
// Check if the file is an actual BMP file
	if(bmpHeader->bfType != 0x4D42)
	{
		std::cout << "File \"" << location << "\" isn't a bitmap file\n";
		return 2;
	}
	// First allocate pixel memory
	pixels = new Uint8[bmpInfo->biSizeImage];

	// Go to where image data starts, then read in image data
	file.seekg(bmpHeader->bfOffBits);
	file.read((char*)pixels, bmpInfo->biSizeImage);
	Uint8 tmpRGB = 0; // Swap buffer
	for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
	{
		tmpRGB        = pixels[i];
		pixels[i]     = pixels[i + 2];
		pixels[i + 2] = tmpRGB;
	}

	// Set width and height to the values loaded from the file
	w = bmpInfo->biWidth;
	h = bmpInfo->biHeight;
}
