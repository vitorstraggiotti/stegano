/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 * Source code to manage bitmap image											*
 * Image creation characteristics:												*
 * 24 bit color depth, no color table, no compression							*
 *																				*
 * Autor: Vitor Henrique Andrade Helfensteller Satraggiotti Silva				*
 * Start date: 28/05/2021														*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>		//To sue precise data types (uint8_t, uint16_t ...)

#include "bitmap.h"


/*******************************************************************************
 *                              FUNCTION DEFINITIONS                           *
 *******************************************************************************/

//Create BMP image file (header used: BITMAPINFOHEADER (V1)) [OK]
void save_BMP(img24_t *Img, const char *Filename)
{
	file_header_t	FileHeader;
	bmp_headerV1_t	BMPHeaderV1;
	uint8_t			ByteZero = 0;
	int32_t 		SizeWidthByte;
	int32_t			TotalWidthMod4;

	//evaluate image dimensions
	if((Img->Width > 20000)||(Img->Height > 20000))
	{
		printf("Error: Image dimensions exceed creation limits of 20000 by 20000\n\n");
		exit(EXIT_FAILURE);
	}
	
	if((Img->Width < 2)||(Img->Height < 2))
	{
		printf("Error: Dimensions for image creation should be equal or greater than 2 by 2\n\n");
		exit(EXIT_FAILURE);
	}

	FileHeader.CharID_1 = 0x42;
	FileHeader.CharID_2 = 0x4D;
	FileHeader.Reserved_1 = 0;
	FileHeader.Reserved_2 = 0;
	FileHeader.OffsetPixelMatrix = 54;
	
	BMPHeaderV1.SizeHeader = 40;
	BMPHeaderV1.Width = Img->Width;
	BMPHeaderV1.Height = Img->Height;
	BMPHeaderV1.Planes = 1;
	BMPHeaderV1.ColorDepth = 24;
	BMPHeaderV1.Compression = 0;
	BMPHeaderV1.ResolutionX = RESOLUTION_X;
	BMPHeaderV1.ResolutionY = RESOLUTION_Y;
	BMPHeaderV1.NumColorsInTable = 0;
	BMPHeaderV1.NumImportantColors = 0;

	//Finding pixel matrix size and adding padding
	SizeWidthByte = Img->Width * 3;			//size of one line in bytes
	TotalWidthMod4 = SizeWidthByte % 4;
	
	if(TotalWidthMod4 != 0)
	{
		SizeWidthByte = SizeWidthByte + 4 - TotalWidthMod4;
	}
	
	BMPHeaderV1.SizePixelMatrix = SizeWidthByte * Img->Height;

	//Finding total image file size
	FileHeader.FileSize = 54 + BMPHeaderV1.SizePixelMatrix;

	//Opening image file
	FILE *ImageFile;
	
	ImageFile = fopen(Filename, "wb");
	if(ImageFile == NULL)
	{
		printf("Error: problem ocurred while creating image file\n\n");
		exit(EXIT_FAILURE);
	}
	
	//Writing headers
	fwrite(&FileHeader, sizeof(file_header_t), 1, ImageFile);
	fwrite(&BMPHeaderV1, sizeof(bmp_headerV1_t), 1, ImageFile);
	
	//Writing image
	for(int32_t row = 0; row < Img->Height; row++)
	{
		for(int32_t column = 0; column < Img->Width; column++)
		{
			fwrite(&Img->Pixel[row][column], sizeof(pixel24_t), 1, ImageFile);
			
			if(column == (Img->Width - 1))
			{
				if(TotalWidthMod4 == 1)
				{
					fwrite(&ByteZero, sizeof(uint8_t), 3, ImageFile);
					
				}else if(TotalWidthMod4 == 2)
				{
					fwrite(&ByteZero, sizeof(uint8_t), 2, ImageFile);
					
				}else if(TotalWidthMod4 == 3)
				{
					fwrite(&ByteZero, sizeof(uint8_t), 1, ImageFile);
				}
			}
		}
	}
	
	fclose(ImageFile);
}

/******************************************************************************/
//Read BMP image to a pixel matrix [OK]
img24_t *read_BMP(const char *Filename)
{
	file_header_t	FileHeader;
	bmp_headerV1_t	BMPHeaderV1;
	bmp_headerV2_t	BMPHeaderV2;
	bmp_headerV3_t	BMPHeaderV3;
	bmp_headerV4_t	BMPHeaderV4;
	bmp_headerV5_t	BMPHeaderV5;
	
	img24_t			*Img;

	uint8_t 		Trash;
	
	FILE 			*Image;
	
	//open image
	Image = fopen(Filename, "rb");
	if(Image == NULL)
	{
		printf("Error: problem occured while readiing image file\n\n");
		exit(EXIT_FAILURE);
	}
	
	//Acquire file header and verify if valid
	fread(&FileHeader, sizeof(file_header_t), 1, Image);
	
	if((FileHeader.CharID_1 != 0x42) || (FileHeader.CharID_2 != 0x4D))
	{
		printf("Error: input file is not a BMP image or have incompatible BMP file identifier. (should be: \"BM\")\n");
		exit(EXIT_FAILURE);
	}
	
	//Allocate space for image struct
	Img = malloc(sizeof(img24_t));
	
	//Finding out BMP header version and reading it
	switch(FileHeader.OffsetPixelMatrix - sizeof(file_header_t))
	{
		case BITMAP_V1_INFOHEADER :
			fread(&BMPHeaderV1, sizeof(bmp_headerV1_t), 1, Image);
			Img->Width = BMPHeaderV1.Width;
			Img->Height = BMPHeaderV1.Height;
			break;
			
		case BITMAP_V2_INFOHEADER :
			fread(&BMPHeaderV2, sizeof(bmp_headerV2_t), 1, Image);
			Img->Width = BMPHeaderV2.Width;
			Img->Height = BMPHeaderV2.Height;
			break;
		
		case BITMAP_V3_INFOHEADER :
			fread(&BMPHeaderV3, sizeof(bmp_headerV3_t), 1, Image);
			Img->Width = BMPHeaderV3.Width;
			Img->Height = BMPHeaderV3.Height;
			break;
		
		case BITMAP_V4_INFOHEADER :
			fread(&BMPHeaderV4, sizeof(bmp_headerV4_t), 1, Image);
			Img->Width = BMPHeaderV4.Width;
			Img->Height = BMPHeaderV4.Height;
			break;
			
		case BITMAP_V5_INFOHEADER :
			fread(&BMPHeaderV5, sizeof(bmp_headerV5_t), 1, Image);
			Img->Width = BMPHeaderV5.Width;
			Img->Height = BMPHeaderV5.Height;
			break;
			
		default :
			printf("Error: bitmap header is not supported. Suported bitmap headers for reading:\n");
			printf("       - BITMAPIFOHEADER     (V1)\n");
			printf("       - BITMAPV2INFOHEADER  (V2)\n");
			printf("       - BITMAPV3INFOHEADER  (V3)\n");
			printf("       - BITMAPV4HEADER      (V4)\n");
			printf("       - BITMAPV5HEADER      (V5)\n");
			exit(EXIT_FAILURE);
	}
		
	//allocate space for pixel matrix
	Img->Pixel = malloc(Img->Height * sizeof(pixel24_t*));
	
	for(int32_t row = 0; row < Img->Height; row++)
	{
		Img->Pixel[row] = malloc(Img->Width * sizeof(pixel24_t));
	}

	//Reading image and copying to pixel matrix
	for(int32_t row = 0; row < Img->Height; row++)
	{
		for(int32_t column = 0; column < Img->Width; column++)
		{
			fread(&Img->Pixel[row][column], sizeof(pixel24_t), 1, Image);
			
			if(column == (Img->Width - 1))
			{
				if(((Img->Width * 3) % 4) == 1)
				{
					fread(&Trash, sizeof(uint8_t), 3, Image);
					
				}else if(((Img->Width * 3) % 4) == 2)
				{
					fread(&Trash, sizeof(uint8_t), 2, Image);
					
				}else if(((Img->Width * 3) % 4) == 3)
				{
					fread(&Trash, sizeof(uint8_t), 1, Image);
				}
			}
		}
	}
	
	fclose(Image);
	
	return Img;
}

/******************************************************************************/
//Find dimensions of the BMP image [OK]
#if 0
dimensions_t dimensions_BMP(const char *Filename)
{
	file_header_t	FileHeader;
	bmp_headerV1_t	BMPHeaderV1;
	bmp_headerV2_t	BMPHeaderV2;
	bmp_headerV3_t	BMPHeaderV3;
	bmp_headerV4_t	BMPHeaderV4;
	bmp_headerV5_t	BMPHeaderV5;
	dimensions_t Dimension;
	FILE *Image;
	
	//Open image
	Image = fopen(Filename, "rb");
	if(Image == NULL)
	{
		printf("Error: problem ocurred while reading file for width information\n\n");
		exit(EXIT_FAILURE);
	}
	
	//Acquire file header and verify if valid
	fread(&FileHeader, sizeof(file_header_t), 1, Image);
	
	if((FileHeader.CharID_1 != 0x42) || (FileHeader.CharID_2 != 0x4D))
	{
		printf("Error: input file is not a BMP image or have incompatible BMP file identifier. (should be: \"BM\")\n");
		exit(EXIT_FAILURE);
	}
	
	//Finding out BMP header version and reading it
	switch(FileHeader.OffsetPixelMatrix - sizeof(file_header_t))
	{
		case BITMAP_V1_INFOHEADER :
			fread(&BMPHeaderV1, sizeof(bmp_headerV1_t), 1, Image);
			Dimension.Width = BMPHeaderV1.Width;
			Dimension.Height = BMPHeaderV1.Height;
			break;
			
		case BITMAP_V2_INFOHEADER :
			fread(&BMPHeaderV2, sizeof(bmp_headerV2_t), 1, Image);
			Dimension.Width = BMPHeaderV2.Width;
			Dimension.Height = BMPHeaderV2.Height;
			break;
		
		case BITMAP_V3_INFOHEADER :
			fread(&BMPHeaderV3, sizeof(bmp_headerV3_t), 1, Image);
			Dimension.Width = BMPHeaderV3.Width;
			Dimension.Height = BMPHeaderV3.Height;
			break;
		
		case BITMAP_V4_INFOHEADER :
			fread(&BMPHeaderV4, sizeof(bmp_headerV4_t), 1, Image);
			Dimension.Width = BMPHeaderV4.Width;
			Dimension.Height = BMPHeaderV4.Height;
			break;
			
		case BITMAP_V5_INFOHEADER :
			fread(&BMPHeaderV5, sizeof(bmp_headerV5_t), 1, Image);
			Dimension.Width = BMPHeaderV5.Width;
			Dimension.Height = BMPHeaderV5.Height;
			break;
			
		default :
			printf("Error: bitmap header is not supported. Suported bitmap headers for reading:\n");
			printf("       - BITMAPIFOHEADER     (V1)\n");
			printf("       - BITMAPV2INFOHEADER  (V2)\n");
			printf("       - BITMAPV3INFOHEADER  (V3)\n");
			printf("       - BITMAPV4HEADER      (V4)\n");
			printf("       - BITMAPV5HEADER      (V5)\n");
			exit(EXIT_FAILURE);
	}

	fclose(Image);

	return Dimension;

}
#endif
/******************************************************************************/
//Display header information [OK]
void display_header(const char *Filename)
{		
	file_header_t FileHeader;
	bmp_headerV1_t BMPHeaderV1;
	bmp_headerV2_t BMPHeaderV2;
	bmp_headerV3_t BMPHeaderV3;
	bmp_headerV4_t BMPHeaderV4;
	bmp_headerV5_t BMPHeaderV5;
	
	FILE *File;
	
	//Opening image
	File = fopen(Filename, "rb");
	if(File == NULL)
	{
		printf("Error: problem occurred while reading file for displaying header information\n\n");
	}
	
	//Print file header information
	fread(&FileHeader, sizeof(file_header_t), 1, File);
	printf("\n");
	printf("Character ID_1: '%c'\n", FileHeader.CharID_1);
	printf("Character ID_2: '%c'\n", FileHeader.CharID_2);
	printf("File size: %u bytes\n", FileHeader.FileSize);
	printf("Reserved_1: %u\n", FileHeader.Reserved_1);
	printf("Reserved_2: %u\n", FileHeader.Reserved_2);
	printf("Offset until pixel matrix: %u\n\n", FileHeader.OffsetPixelMatrix);
	
	//Print Windows BMP header information
	switch(FileHeader.OffsetPixelMatrix - sizeof(file_header_t))
	{
		//----------------------------------------------------------------------
		case BITMAP_V1_INFOHEADER :
			
			fread(&BMPHeaderV1, sizeof(bmp_headerV1_t), 1, File);
			printf("BMP header type: BITMAPINFOHEADER (V1)\n");
			printf("BMP header size ............... %u bytes\n", BMPHeaderV1.SizeHeader);
			printf("Image width ................... %d pixels\n", BMPHeaderV1.Width);
			printf("Image Height .................. %d pixels\n", BMPHeaderV1.Height);
			printf("Number of color plnaes ........ %u\n", BMPHeaderV1.Planes);
			printf("Bits per pixel ................ %u\n", BMPHeaderV1.ColorDepth);
			printf("Image size .................... %u\n", BMPHeaderV1.SizePixelMatrix);
			printf("X axis printing resolution .... %d\n", BMPHeaderV1.ResolutionX);
			printf("Y axis printing resolution .... %d\n", BMPHeaderV1.ResolutionY);
			printf("Number of palette colors ...... %u\n", BMPHeaderV1.NumColorsInTable);
			printf("Number of important colors .... %u\n", BMPHeaderV1.NumImportantColors);
			
			//Print compression method
			printf("Compression method ............ ");
			switch(BMPHeaderV1.Compression)
			{
				case 0 :
					printf("none (ID code: BI_RGB)\n\n");
					break;
					
				case 1 :
					printf("RLE 8-bit/pixel (ID code: BI_RLE8)\n\n");
					break;
					
				case 2 :
					printf("RLE 4-bit/pixel (ID code: BI_RLE4)\n\n");
					break;
					
				case 3 :
					printf("OS22XBITMAPHEADER: Huffman 1D (ID code: BI_BITFIELDS)\n\n");
					break;
					
				case 4 :
					printf("OS22XBITMAPHEADER: RLE-24 (ID code: BI_JPEG)\n\n");
					break;
					
				case 5 :
					printf("Undefined (ID code: BI_PNG)\n\n");
					break;
					
				case 6 :
					printf("RGBA bit field masks (ID code: BI_ALPHABITFIELDS)\n\n");
					break;
					
				case 11 :
					printf("none (ID code: BI_CMYK)\n\n");
					break;
					
				case 12 :
					printf("RLE-8 (ID code: BI_CMYKRLE8)\n\n");
					break;
					
				case 13 :
					printf("RLE-4 (ID code: BI_CMYKRLE4)\n\n");
					break;
					
				default :
					printf("Error! invalid value\n\n");
			}
			
			break;

		//----------------------------------------------------------------------
		case BITMAP_V2_INFOHEADER :
			
			fread(&BMPHeaderV2, sizeof(bmp_headerV2_t), 1, File);
			printf("BMP header type: BITMAPV2INFOHEADER (V2)\n");
			printf("BMP header size ............... %u bytes\n", BMPHeaderV2.SizeHeader);
			printf("Image width ................... %d pixels\n", BMPHeaderV2.Width);
			printf("Image Height .................. %d pixels\n", BMPHeaderV2.Height);
			printf("Number of color plnaes ........ %u\n", BMPHeaderV2.Planes);
			printf("Bits per pixel ................ %u\n", BMPHeaderV2.ColorDepth);
			printf("Image size .................... %u\n", BMPHeaderV2.SizePixelMatrix);
			printf("X axis printing resolution .... %d\n", BMPHeaderV2.ResolutionX);
			printf("Y axis printing resolution .... %d\n", BMPHeaderV2.ResolutionY);
			printf("Number of palette colors ...... %u\n", BMPHeaderV2.NumColorsInTable);
			printf("Number of important colors .... %u\n", BMPHeaderV2.NumImportantColors);
			
			//Print compression method
			printf("Compression method ............ ");
			switch(BMPHeaderV2.Compression)
			{
				case 0 :
					printf("none (ID code: BI_RGB)\n");
					break;
					
				case 1 :
					printf("RLE 8-bit/pixel (ID code: BI_RLE8)\n");
					break;
					
				case 2 :
					printf("RLE 4-bit/pixel (ID code: BI_RLE4)\n");
					break;
					
				case 3 :
					printf("OS22XBITMAPHEADER: Huffman 1D (ID code: BI_BITFIELDS)\n");
					break;
					
				case 4 :
					printf("OS22XBITMAPHEADER: RLE-24 (ID code: BI_JPEG)\n");
					break;
					
				case 5 :
					printf("Undefined (ID code: BI_PNG)\n");
					break;
					
				case 6 :
					printf("RGBA bit field masks (ID code: BI_ALPHABITFIELDS)\n");
					break;
					
				case 11 :
					printf("none (ID code: BI_CMYK)\n");
					break;
					
				case 12 :
					printf("RLE-8 (ID code: BI_CMYKRLE8)\n");
					break;
					
				case 13 :
					printf("RLE-4 (ID code: BI_CMYKRLE4)\n");
					break;
					
				default :
					printf("Error! invalid value\n");
			}
			
			printf("Red mask ...................... %u\n", BMPHeaderV2.RedMask);
			printf("Green mask .................... %u\n", BMPHeaderV2.GreenMask);
			printf("Blue mask ..................... %u\n\n", BMPHeaderV2.BlueMask);
			
			break;
	
		//----------------------------------------------------------------------
		case BITMAP_V3_INFOHEADER :
			
			fread(&BMPHeaderV3, sizeof(bmp_headerV3_t), 1, File);
			printf("BMP header type: BITMAPV3INFOHEADER (V3)\n");
			printf("BMP header size ............... %u bytes\n", BMPHeaderV3.SizeHeader);
			printf("Image width ................... %d pixels\n", BMPHeaderV3.Width);
			printf("Image Height .................. %d pixels\n", BMPHeaderV3.Height);
			printf("Number of color plnaes ........ %u\n", BMPHeaderV3.Planes);
			printf("Bits per pixel ................ %u\n", BMPHeaderV3.ColorDepth);
			printf("Image size .................... %u\n", BMPHeaderV3.SizePixelMatrix);
			printf("X axis printing resolution .... %d\n", BMPHeaderV3.ResolutionX);
			printf("Y axis printing resolution .... %d\n", BMPHeaderV3.ResolutionY);
			printf("Number of palette colors ...... %u\n", BMPHeaderV3.NumColorsInTable);
			printf("Number of important colors .... %u\n", BMPHeaderV3.NumImportantColors);
			
			//Print compression method
			printf("Compression method ............ ");
			switch(BMPHeaderV3.Compression)
			{
				case 0 :
					printf("none (ID code: BI_RGB)\n");
					break;
					
				case 1 :
					printf("RLE 8-bit/pixel (ID code: BI_RLE8)\n");
					break;
					
				case 2 :
					printf("RLE 4-bit/pixel (ID code: BI_RLE4)\n");
					break;
					
				case 3 :
					printf("OS22XBITMAPHEADER: Huffman 1D (ID code: BI_BITFIELDS)\n");
					break;
					
				case 4 :
					printf("OS22XBITMAPHEADER: RLE-24 (ID code: BI_JPEG)\n");
					break;
					
				case 5 :
					printf("Undefined (ID code: BI_PNG)\n");
					break;
					
				case 6 :
					printf("RGBA bit field masks (ID code: BI_ALPHABITFIELDS)\n");
					break;
					
				case 11 :
					printf("none (ID code: BI_CMYK)\n");
					break;
					
				case 12 :
					printf("RLE-8 (ID code: BI_CMYKRLE8)\n");
					break;
					
				case 13 :
					printf("RLE-4 (ID code: BI_CMYKRLE4)\n");
					break;
					
				default :
					printf("Error! invalid value\n");
			}
			
			printf("Red mask ...................... %u\n", BMPHeaderV3.RedMask);
			printf("Green mask .................... %u\n", BMPHeaderV3.GreenMask);
			printf("Blue mask ..................... %u\n", BMPHeaderV3.BlueMask);
			
			printf("Alpha mask .................... %u\n\n", BMPHeaderV3.AlphaMask);

			break;
	
		//----------------------------------------------------------------------
		case BITMAP_V4_INFOHEADER :

			fread(&BMPHeaderV4, sizeof(bmp_headerV4_t), 1, File);
			printf("BMP header type: BITMAPV4HEADER (V4)\n");
			printf("BMP header size ............... %u bytes\n", BMPHeaderV4.SizeHeader);
			printf("Image width ................... %d pixels\n", BMPHeaderV4.Width);
			printf("Image Height .................. %d pixels\n", BMPHeaderV4.Height);
			printf("Number of color plnaes ........ %u\n", BMPHeaderV4.Planes);
			printf("Bits per pixel ................ %u\n", BMPHeaderV4.ColorDepth);
			printf("Image size .................... %u\n", BMPHeaderV4.SizePixelMatrix);
			printf("X axis printing resolution .... %d\n", BMPHeaderV4.ResolutionX);
			printf("Y axis printing resolution .... %d\n", BMPHeaderV4.ResolutionY);
			printf("Number of palette colors ...... %u\n", BMPHeaderV4.NumColorsInTable);
			printf("Number of important colors .... %u\n", BMPHeaderV4.NumImportantColors);
			
			//Print compression method
			printf("Compression method ............ ");
			switch(BMPHeaderV4.Compression)
			{
				case 0 :
					printf("none (ID code: BI_RGB)\n");
					break;
					
				case 1 :
					printf("RLE 8-bit/pixel (ID code: BI_RLE8)\n");
					break;
					
				case 2 :
					printf("RLE 4-bit/pixel (ID code: BI_RLE4)\n");
					break;
					
				case 3 :
					printf("OS22XBITMAPHEADER: Huffman 1D (ID code: BI_BITFIELDS)\n");
					break;
					
				case 4 :
					printf("OS22XBITMAPHEADER: RLE-24 (ID code: BI_JPEG)\n");
					break;
					
				case 5 :
					printf("Undefined (ID code: BI_PNG)\n");
					break;
					
				case 6 :
					printf("RGBA bit field masks (ID code: BI_ALPHABITFIELDS)\n");
					break;
					
				case 11 :
					printf("none (ID code: BI_CMYK)\n");
					break;
					
				case 12 :
					printf("RLE-8 (ID code: BI_CMYKRLE8)\n");
					break;
					
				case 13 :
					printf("RLE-4 (ID code: BI_CMYKRLE4)\n");
					break;
					
				default :
					printf("Error! invalid value\n");
			}
			
			printf("Red mask ...................... %u\n", BMPHeaderV4.RedMask);
			printf("Green mask .................... %u\n", BMPHeaderV4.GreenMask);
			printf("Blue mask ..................... %u\n", BMPHeaderV4.BlueMask);
			
			printf("Alpha mask .................... %u\n", BMPHeaderV4.AlphaMask);

			printf("Color space ................... ");
			switch(BMPHeaderV4.CSType)
			{
				case 0x0:
					printf("LCS_CALIBRATED_RGB\n");
					break;
				
				case 0x73524742 : //"sRGB"
					printf("LCS_sRGB\n");
					break;
				
				case 0x57696E20 :
					printf("LCS_WINDOWS_COLOR_SPACE\n");
					break;
				
				default :
					printf("PROFILE_LINKED or PROFILE_EMBEDDED\n");
			}
			printf("X red endpoint ................ %d\n", BMPHeaderV4.RedX);
			printf("Y red endpoint ................ %d\n", BMPHeaderV4.RedY);
			printf("Z red endpoint ................ %d\n", BMPHeaderV4.RedZ);
			printf("X green endpoint .............. %d\n", BMPHeaderV4.GreenX);
			printf("Y green endpoint .............. %d\n", BMPHeaderV4.GreenY);
			printf("Z green endpoint .............. %d\n", BMPHeaderV4.GreenZ);
			printf("X Blue endpoint ............... %d\n", BMPHeaderV4.BlueX);
			printf("Y Blue endpoint ............... %d\n", BMPHeaderV4.BlueY);
			printf("Z Blue endpoint ............... %d\n", BMPHeaderV4.BlueZ);
			printf("Toned response for red ........ %X\n", BMPHeaderV4.GammaRed);
			printf("Toned response for green ...... %X\n", BMPHeaderV4.GammaGreen);
			printf("Toned response for blue ....... %X\n\n", BMPHeaderV4.GammaBlue);

			break;

		//----------------------------------------------------------------------
		case BITMAP_V5_INFOHEADER :

			fread(&BMPHeaderV5, sizeof(bmp_headerV4_t), 1, File);
			printf("BMP header type: BITMAPV5HEADER (V5)\n");
			printf("BMP header size ............... %u bytes\n", BMPHeaderV5.SizeHeader);
			printf("Image width ................... %d pixels\n", BMPHeaderV5.Width);
			printf("Image Height .................. %d pixels\n", BMPHeaderV5.Height);
			printf("Number of color plnaes ........ %u\n", BMPHeaderV5.Planes);
			printf("Bits per pixel ................ %u\n", BMPHeaderV5.ColorDepth);
			printf("Image size .................... %u\n", BMPHeaderV5.SizePixelMatrix);
			printf("X axis printing resolution .... %d\n", BMPHeaderV5.ResolutionX);
			printf("Y axis printing resolution .... %d\n", BMPHeaderV5.ResolutionY);
			printf("Number of palette colors ...... %u\n", BMPHeaderV5.NumColorsInTable);
			printf("Number of important colors .... %u\n", BMPHeaderV5.NumImportantColors);
			
			//Print compression method
			printf("Compression method ............ ");
			switch(BMPHeaderV5.Compression)
			{
				case 0 :
					printf("none (ID code: BI_RGB)\n");
					break;
					
				case 1 :
					printf("RLE 8-bit/pixel (ID code: BI_RLE8)\n");
					break;
					
				case 2 :
					printf("RLE 4-bit/pixel (ID code: BI_RLE4)\n");
					break;
					
				case 3 :
					printf("OS22XBITMAPHEADER: Huffman 1D (ID code: BI_BITFIELDS)\n");
					break;
					
				case 4 :
					printf("OS22XBITMAPHEADER: RLE-24 (ID code: BI_JPEG)\n");
					break;
					
				case 5 :
					printf("Undefined (ID code: BI_PNG)\n");
					break;
					
				case 6 :
					printf("RGBA bit field masks (ID code: BI_ALPHABITFIELDS)\n");
					break;
					
				case 11 :
					printf("none (ID code: BI_CMYK)\n");
					break;
					
				case 12 :
					printf("RLE-8 (ID code: BI_CMYKRLE8)\n");
					break;
					
				case 13 :
					printf("RLE-4 (ID code: BI_CMYKRLE4)\n");
					break;
					
				default :
					printf("Error! invalid value\n");
			}
			
			printf("Red mask ...................... %u\n", BMPHeaderV5.RedMask);
			printf("Green mask .................... %u\n", BMPHeaderV5.GreenMask);
			printf("Blue mask ..................... %u\n", BMPHeaderV5.BlueMask);
			
			printf("Alpha mask .................... %u\n", BMPHeaderV5.AlphaMask);

			printf("Color space ................... ");
			switch(BMPHeaderV5.CSType)
			{
				case 0x0:
					printf("LCS_CALIBRATED_RGB\n");
					break;
				
				case 0x73524742 : //"sRGB"
					printf("LCS_sRGB\n");
					break;
				
				case 0x57696E20 :
					printf("LCS_WINDOWS_COLOR_SPACE\n");
					break;
				
				default :
					printf("PROFILE_LINKED or PROFILE_EMBEDDED\n");
			}
			printf("X red endpoint ................ %d\n", BMPHeaderV5.RedX);
			printf("Y red endpoint ................ %d\n", BMPHeaderV5.RedY);
			printf("Z red endpoint ................ %d\n", BMPHeaderV5.RedZ);
			printf("X green endpoint .............. %d\n", BMPHeaderV5.GreenX);
			printf("Y green endpoint .............. %d\n", BMPHeaderV5.GreenY);
			printf("Z green endpoint .............. %d\n", BMPHeaderV5.GreenZ);
			printf("X Blue endpoint ............... %d\n", BMPHeaderV5.BlueX);
			printf("Y Blue endpoint ............... %d\n", BMPHeaderV5.BlueY);
			printf("Z Blue endpoint ............... %d\n", BMPHeaderV5.BlueZ);
			printf("Toned response for red ........ %X\n", BMPHeaderV5.GammaRed);
			printf("Toned response for green ...... %X\n", BMPHeaderV5.GammaGreen);
			printf("Toned response for blue ....... %X\n", BMPHeaderV5.GammaBlue);

			printf("Rendering intent .............. ");
			switch(BMPHeaderV5.Intent)
			{
				case 0x8 :
					printf("LCS_GM_ABS_COLORIMETRIC\n");
					break;
				
				case 0x1 :
					printf("LCS_GM_BUSINESS\n");
					break;
					
				case 0x2 :
					printf("LCS_GM_GRAPHICS\n");
					break;
					
				case 0x4 :
					printf("LCS_GM_IMAGES\n");
					break;
					
				default :
					printf("Error: invalid value\n");
			}			
			printf("Profile data .................. %u\n", BMPHeaderV5.ProfileData);
			printf("Profile size .................. %u\n", BMPHeaderV5.ProfileSize);
			printf("Reserved ...................... %u\n\n", BMPHeaderV5.Reserved);

			break;

		//----------------------------------------------------------------------
		default :
		
		printf("Error: wrong size of BMP header");
		exit(EXIT_FAILURE);

	}

	fclose(File);

}

/******************************************************************************/
//Frees space occupied by Image
void free_img(img24_t *Img)
{
	for (int32_t row = 0; row < Img->Height; row++)
	{
		free(Img->Pixel[row]);
	}
	free(Img->Pixel);
	free(Img);
}




















