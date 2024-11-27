// BmpTest.cpp : 定义控制台应用程序的入口点。
//

/* File name:   bmpTest.c
Author:      WanChuan XianSheng
Date:        Oct 01, 2011
Description: Show all Info a bmp file has. including
FileHeader Info, InfoHeader Info and Data Part.

Reference: BMP图像数据的C语言读取源码
*/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#define BITMAPFILEHEADERLENGTH 14   // The bmp FileHeader length is 14
#define BM 19778                    // The ASCII code for BM

/* Test the file is bmp file or not */
void bmpFileTest(FILE* fpbmp);
/* To get the OffSet of header to data part */
void bmpHeaderPartLength(FILE* fpbmp);
/* To get the width and height of the bmp file */
void BmpWidthHeight(FILE* fpbmp);
/* Show bmp file tagBITMAPFILEHEADER info */
void bmpFileHeader(FILE* fpbmp);
/* Show bmp file tagBITMAPINFOHEADER info */
void bmpInfoHeader(FILE* fpbmp);
/* Show the Data Part of bmp file */
void bmpDataPart(FILE* fpbmp);

unsigned int OffSet = 0;    // OffSet from Header part to Data Part
long BmpWidth = 0;          // The Width of the Data Part
long BmpHeight = 0;         // The Height of the Data Part


int main(int argc, char* argv[])
{
	/* Open bmp file */
	FILE *fpbmp = fopen("1.bmp", "r+");
	if (fpbmp == NULL)
	{
		fprintf(stderr, "Open lena.bmp failed!!!\n");
		return 1;
	}

	bmpFileTest(fpbmp);                //Test the file is bmp file or not
	bmpHeaderPartLength(fpbmp);        //Get the length of Header Part
	BmpWidthHeight(fpbmp);             //Get the width and width of the Data Part
	bmpFileHeader(fpbmp);            //Show the FileHeader Information
	bmpInfoHeader(fpbmp);            //Show the InfoHeader Information
	bmpDataPart(fpbmp);                //Reserve the data to file

	fclose(fpbmp);
	return 0;
}

/* Test the file is bmp file or not */
void bmpFileTest(FILE* fpbmp)
{
	unsigned short bfType = 0;
	fseek(fpbmp, 0L, SEEK_SET);
	fread(&bfType, sizeof(char), 2, fpbmp);
	if (BM != bfType)
	{
		fprintf(stderr, "This file is not bmp file.!!!\n");
		exit(1);
	}
}

/* To get the OffSet of header to data part */
void bmpHeaderPartLength(FILE* fpbmp)
{
	fseek(fpbmp, 10L, SEEK_SET);
	fread(&OffSet, sizeof(char), 4, fpbmp);
	printf("The Header Part is of length %d.\n", OffSet);
}

/* To get the width and height of the bmp file */
void BmpWidthHeight(FILE* fpbmp)
{
	fseek(fpbmp, 18L, SEEK_SET);
	fread(&BmpWidth, sizeof(char), 4, fpbmp);
	fread(&BmpHeight, sizeof(char), 4, fpbmp);
	printf("The Width of the bmp file is %ld.\n", BmpWidth);
	printf("The Height of the bmp file is %ld.\n", BmpHeight);
}

/* Show bmp file tagBITMAPFILEHEADER info */
void bmpFileHeader(FILE* fpbmp)
{
	unsigned short bfType;              //UNIT        bfType;
	unsigned int   bfSize;              //DWORD       bfSize;
	unsigned short bfReserved1;         //UINT        bfReserved1;
	unsigned short bfReserved2;         //UINT        bfReserved2;
	unsigned int   bfOffBits;           //DWORD       bfOffBits;

	fseek(fpbmp, 0L, SEEK_SET);

	fread(&bfType, sizeof(char), 2, fpbmp);
	fread(&bfSize, sizeof(char), 4, fpbmp);
	fread(&bfReserved1, sizeof(char), 2, fpbmp);
	fread(&bfReserved2, sizeof(char), 2, fpbmp);
	fread(&bfOffBits, sizeof(char), 4, fpbmp);

	printf("************************************************\n");
	printf("*************tagBITMAPFILEHEADER info***********\n");
	printf("************************************************\n");
	printf("bfType              is %d.\n", bfType);
	printf("bfSize              is %d.\n", bfSize);
	printf("bfReserved1         is %d.\n", bfReserved1);
	printf("bfReserved2         is %d.\n", bfReserved2);
	printf("bfOffBits           is %d.\n", bfOffBits);
}

/* Show bmp file tagBITMAPINFOHEADER info */
void bmpInfoHeader(FILE* fpbmp)
{
	unsigned int biSize;              // DWORD        biSize;
	long         biWidth;                // LONG         biWidth;
	long         biHeight;               // LONG         biHeight;
	unsigned int biPlanes;               // WORD         biPlanes;
	unsigned int biBitCount;             // WORD         biBitCount;
	unsigned int biCompression;          // DWORD        biCompression;
	unsigned int biSizeImage;            // DWORD        biSizeImage;
	long         biXPelsPerMerer;        // LONG         biXPelsPerMerer;
	long           biYPelsPerMerer;        // LONG         biYPelsPerMerer;
	unsigned int biClrUsed;              // DWORD        biClrUsed;
	unsigned int biClrImportant;         // DWORD        biClrImportant;

	fseek(fpbmp, 14L, SEEK_SET);

	fread(&biSize, sizeof(char), 4, fpbmp);
	fread(&biWidth, sizeof(char), 4, fpbmp);
	fread(&biHeight, sizeof(char), 4, fpbmp);
	fread(&biPlanes, sizeof(char), 4, fpbmp);
	fread(&biBitCount, sizeof(char), 4, fpbmp);
	fread(&biCompression, sizeof(char), 4, fpbmp);
	fread(&biSizeImage, sizeof(char), 4, fpbmp);
	fread(&biXPelsPerMerer, sizeof(char), 4, fpbmp);
	fread(&biYPelsPerMerer, sizeof(char), 4, fpbmp);
	fread(&biClrUsed, sizeof(char), 4, fpbmp);
	fread(&biClrImportant, sizeof(char), 4, fpbmp);

	printf("************************************************\n");
	printf("*************tagBITMAPINFOHEADER info***********\n");
	printf("************************************************\n");
	printf("biSize              is %d. \n", biSize);
	printf("biWidth             is %ld.\n", biWidth);
	printf("biHeight            is %ld.\n", biHeight);
	printf("biPlanes            is %d. \n", biPlanes);
	printf("biBitCount          is %d. \n", biBitCount);
	printf("biCompression       is %d. \n", biCompression);
	printf("biSizeImage         is %d. \n", biSizeImage);
	printf("biXPelsPerMerer     is %ld.\n", biXPelsPerMerer);
	printf("biYPelsPerMerer     is %ld.\n", biYPelsPerMerer);
	printf("biClrUsed           is %d. \n", biClrUsed);
	printf("biClrImportant      is %d. \n", biClrImportant);
}

/* Show the Data Part of bmp file */
void bmpDataPart(FILE* fpbmp)
{
	int i, j;
	unsigned char **bmpPixel = new unsigned char*[BmpHeight];
	for (i = 0; i < BmpHeight; i++){
		bmpPixel[i] = new unsigned char[BmpWidth];
	}

	unsigned char* bmpPixelTmp = NULL;
	FILE* fpDataBmp;

	/* New a file to save the data matrix */
	if ((fpDataBmp = fopen("bmpData.dat", "w+")) == NULL)
	{
		fprintf(stderr, "Failed to construct file bmpData.dat.!!!");
		exit(1);
	}

	fseek(fpbmp, OffSet, SEEK_SET);
	if ((bmpPixelTmp = (unsigned char*)malloc(sizeof(char)*BmpWidth*BmpHeight)) == NULL)
	{
		fprintf(stderr, "Data allocation failed.!!!\n");
		exit(1);
	}
	fread(bmpPixelTmp, sizeof(char), BmpWidth*BmpHeight, fpbmp);

	/* Read the data to Matrix and save it in file bmpData.dat */
	for (i = 0; i < BmpHeight; i++)
	{
		fprintf(fpDataBmp, "The data in line %-3d:\n", i + 1);
		for (j = 0; j < BmpWidth; j++)
		{
			bmpPixel[i][j] = bmpPixelTmp[BmpWidth*(BmpHeight - 1 - i) + j];
			//fwrite(&chartmp, sizeof(char), 1, fpDataBmp);
			fprintf(fpDataBmp, "%-3d ", bmpPixel[i][j]);
			if ((j + 1) % 32 == 0)
			{
				fprintf(fpDataBmp, "\n");
			}
		}
	}
	/* Used to test the data read is true or false
	You can open the file using Matlab to compare the data */
	//printf("bmpPixel[2][3]   is %d.\n", bmpPixel[2][3]);
	//printf("bmpPixel[20][30] is %d.\n", bmpPixel[20][30]);

	free(bmpPixelTmp);
	fclose(fpDataBmp);

	//释放空间
	for (int i = 0; i < BmpHeight; i++)
		delete[]bmpPixel[i];
	delete bmpPixel;
	bmpPixel = NULL;

}