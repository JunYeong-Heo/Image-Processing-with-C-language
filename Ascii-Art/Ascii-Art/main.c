#include "pch.h"

#define PIXEL_SIZE 1	//픽셀 한 개의 크키 1byte
#define PIXEL_ALIGN 4	//픽셀 데이터 가로 한 줄은 4의 배수로 저장됨
#define BIT 8

int ispNull(void* pData)
{
	if (pData == NULL)
		return -1;
	return 0;
}
int main(void)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	FILE* fpBmp = fopen("Lenna.bmp", "rb");
	FILE* fpTxt = fopen("AsciiArt.txt", "wt");

	char ascii[] = { 'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd', 'e', 'e', ' ' };

	unsigned char* image;
	int size;
	int width, height;
	int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기

	if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpBmp) < 1)
	{
		fclose(fpBmp);
		return 0;
	}
	if (fileHeader.bfType != 'MB')
	{
		fclose(fpBmp);
		return 0;
	}
	if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpBmp) < 1)
	{
		fclose(fpBmp);
		return 0;
	}
	if (infoHeader.biBitCount != (PIXEL_SIZE * BIT))
	{
		printf("ss");
		fclose(fpBmp);
		return 0;
	}

	size = infoHeader.biSizeImage;    // 픽셀 데이터 크기
	width = infoHeader.biWidth;       // 비트맵 이미지의 가로 크기
	height = infoHeader.biHeight;     // 비트맵 이미지의 세로 크기

	padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

	if (size == 0)    // 픽셀 데이터 크기가 0이라면
	{
		// 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
		// 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
		size = (width * PIXEL_SIZE + padding) * height;
	}

	image = (unsigned char*)malloc(sizeof(char) * size);    // 픽셀 데이터의 크기만큼 동적 메모리 할당

   // 파일 포인터를 픽셀 데이터의 시작 위치로 이동
	fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

	// 파일에서 픽셀 데이터 크기만큼 읽음. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
	if (fread(image, size, 1, fpBmp) < 1)
	{
		fclose(fpBmp);
		return 0;
	}

	fclose(fpBmp);    // 비트맵 파일 닫기

	if (fpTxt == NULL)    // 파일 열기에 실패하면
	{
		free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제
		return 0;         // 프로그램 종료
	}

	// 픽셀 데이터는 아래 위가 뒤집혀서 저장되므로 아래쪽부터 반복
	// 세로 크기만큼 반복
	for (int row = height - 1; row >= 0; row--)
	{
		// 가로 크기만큼 반복
		for (int col = 0; col < width; col++)
		{
			int index = (col * PIXEL_SIZE) + (row * (width * PIXEL_SIZE)) + (padding * row);
			// 현재 픽셀의 주소를 RGBTRIPLE 포인터로 변환하여 RGBTRIPLE 포인터에 저장
			RGBTRIPLE* pixel = (RGBTRIPLE*)&image[index];

			// RGBTRIPLE 구조체로 파랑, 초록, 빨강값을 가져옴
			unsigned char blue = pixel->rgbtBlue;
			unsigned char green = pixel->rgbtGreen;
			unsigned char red = pixel->rgbtRed;

			// 파랑, 초록, 빨강값의 평균을 구하면 흑백 이미지를 얻을 수 있음
			unsigned char gray = (red + green + blue) / 3;

			// 흑백값에 ASCII 문자의 개수를 곱한 뒤 256으로 나누면 흑백값에 따라 
			// ASCII 문자의 인덱스를 얻을 수 있음
			char c = ascii[gray * sizeof(ascii) / 256];

			// 비트맵 이미지에서는 픽셀의 가로, 세로 크기가 똑같지만
			// 보통 ASCII 문자는 세로로 길쭉한 형태이므로 정사각형 모양과 비슷하게 보여주기 위해
			// 같은 문자를 두 번 저장해줌
			fprintf(fpTxt, "%c%c", c, c);    // 텍스트 파일에 문자 출력
		}

		fprintf(fpTxt, "\n");    // 가로 픽셀 저장이 끝났으면 줄바꿈을 해줌
	}

	fclose(fpTxt);    // 텍스트 파일 닫기

	free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제

	return 0;
}