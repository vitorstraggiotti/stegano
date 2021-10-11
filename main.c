/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Program to hide information in .bmp image files using steganography
 *
 * Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva
 * Start date: 11/10/2021  (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "bitmap.h"


int main(int argc, char *argv[])
{
	char		*Signature = "stegVHAHSS";
	uint8_t		PayloadInfoFlag = 0;
	uint8_t		ExtractPayloadFlag = 0;
	uint8_t		AttachPayloadFlag = 0;
	
	FILE		*Payload = NULL;
	img24_t		*Image = NULL;
	
	uint32_t	MaxPayloadSize = 0;
	
	//Verify program input
	if(argc != 4)
	{
		printf("\nUsage: %s <option> <image_input> <file_to_attach_or_extract_to>\n\n", argv[0]);
		printf("<option>:\n");
		printf(" i  --> Show information on payload size limit that can be attached to the image.\n\n");
		printf(" x  --> Extract payload from image.\n\n");
		printf(" c  --> Attach payload to image.\n\n");
		printf(" Options can be combined.Ex.:\n");
		printf(" Show info and attach payload: %s ic img.bmp file_input\n", argv[0]);
		printf(" Show info and extract payload: %s ix img.bmp file_output\n\n", argv[0]);		
		exit(EXIT_FAILURE);
	}
	else
	{
		for(uint8_t i = 0;; i++)
		{
			if(argv[1][i] == '\0')
				break;
			
			switch (argv[1][i])
			{
				case 'i':					//show info
					PayloadInfoFlag = 1;
					break;
				case 'x':					//Extract payload
					ExtractPayloadFlag = 1;
					break;
				case 'c':					//Attach payload to image
					AttachPayloadFlag = 1;
					break;
				default:
					printf("Invalid option(s)!\n");
					exit(EXIT_FAILURE);
			}
		}
		//Check option compatibility
		if(ExtractPayloadFlag && AttachPayloadFlag)
		{
			printf("Incompatible options used! Can't Extract and Attach at same time.\n");
			exit(EXIT_FAILURE);
		}
	}
	
	Image = read_BMP(argv[2]);
	
	if(ExtractPayloadFlag == 1)
	{
		Payload = fopen(argv[3], "wb");
		if(Payload == NULL)
		{
			printf("Could not open payload file!\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(AttachPayloadFlag == 1)
	{
		Payload = fopen(argv[3], "rb");
		if(Payload == NULL)
		{
			printf("Could not open payload file!\n");
			exit(EXIT_FAILURE);
		}
	}
	
	//Finding and showing max payload that can be attached to the image
	MaxPayloadSize = (Image->Height * Image->Width * 3) / 8;
	
	if(PayloadInfoFlag == 1)
		printf("Max file size to be Attached (bytes): %u\t%.3fK\t%.3fM\n",
				MaxPayloadSize, MaxPayloadSize/1000.0, MaxPayloadSize/1000000.0);
	
	//Attaching or extracting payload from image
	for(int32_t row = 0; row < Image->Height; row++)
	{
		for(int32_t column = 0; column < Image->Width; column++)
		{
			
		}
	}

	if((ExtractPayloadFlag == 1) || (AttachPayloadFlag == 1))
		fclose(Payload);
	
	free_img(Image);
	return 0;
}



