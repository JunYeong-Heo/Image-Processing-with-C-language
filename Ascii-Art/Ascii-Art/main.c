#include "pch.h"

#define PIXEL_SIZE 1	//�ȼ� �� ���� ũŰ 1byte
#define PIXEL_ALIGN 4	//�ȼ� ������ ���� �� ���� 4�� ����� �����
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
	int padding;             // �ȼ� �������� ���� ũ�Ⱑ 4�� ����� �ƴ� �� ���� ������ ũ��

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

	size = infoHeader.biSizeImage;    // �ȼ� ������ ũ��
	width = infoHeader.biWidth;       // ��Ʈ�� �̹����� ���� ũ��
	height = infoHeader.biHeight;     // ��Ʈ�� �̹����� ���� ũ��

	padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

	if (size == 0)    // �ȼ� ������ ũ�Ⱑ 0�̶��
	{
		// �̹����� ���� ũ�� * �ȼ� ũ�⿡ ���� ������ �����ָ� ������ ���� �� �� ũ�Ⱑ ����
		// ���⿡ �̹����� ���� ũ�⸦ �����ָ� �ȼ� �������� ũ�⸦ ���� �� ����
		size = (width * PIXEL_SIZE + padding) * height;
	}

	image = (unsigned char*)malloc(sizeof(char) * size);    // �ȼ� �������� ũ�⸸ŭ ���� �޸� �Ҵ�

   // ���� �����͸� �ȼ� �������� ���� ��ġ�� �̵�
	fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

	// ���Ͽ��� �ȼ� ������ ũ�⸸ŭ ����. �б⿡ �����ϸ� ���� �����͸� �ݰ� ���α׷� ����
	if (fread(image, size, 1, fpBmp) < 1)
	{
		fclose(fpBmp);
		return 0;
	}

	fclose(fpBmp);    // ��Ʈ�� ���� �ݱ�

	if (fpTxt == NULL)    // ���� ���⿡ �����ϸ�
	{
		free(image);      // �ȼ� �����͸� ������ ���� �޸� ����
		return 0;         // ���α׷� ����
	}

	// �ȼ� �����ʹ� �Ʒ� ���� �������� ����ǹǷ� �Ʒ��ʺ��� �ݺ�
	// ���� ũ�⸸ŭ �ݺ�
	for (int row = height - 1; row >= 0; row--)
	{
		// ���� ũ�⸸ŭ �ݺ�
		for (int col = 0; col < width; col++)
		{
			int index = (col * PIXEL_SIZE) + (row * (width * PIXEL_SIZE)) + (padding * row);
			// ���� �ȼ��� �ּҸ� RGBTRIPLE �����ͷ� ��ȯ�Ͽ� RGBTRIPLE �����Ϳ� ����
			RGBTRIPLE* pixel = (RGBTRIPLE*)&image[index];

			// RGBTRIPLE ����ü�� �Ķ�, �ʷ�, �������� ������
			unsigned char blue = pixel->rgbtBlue;
			unsigned char green = pixel->rgbtGreen;
			unsigned char red = pixel->rgbtRed;

			// �Ķ�, �ʷ�, �������� ����� ���ϸ� ��� �̹����� ���� �� ����
			unsigned char gray = (red + green + blue) / 3;

			// ��鰪�� ASCII ������ ������ ���� �� 256���� ������ ��鰪�� ���� 
			// ASCII ������ �ε����� ���� �� ����
			char c = ascii[gray * sizeof(ascii) / 256];

			// ��Ʈ�� �̹��������� �ȼ��� ����, ���� ũ�Ⱑ �Ȱ�����
			// ���� ASCII ���ڴ� ���η� ������ �����̹Ƿ� ���簢�� ���� ����ϰ� �����ֱ� ����
			// ���� ���ڸ� �� �� ��������
			fprintf(fpTxt, "%c%c", c, c);    // �ؽ�Ʈ ���Ͽ� ���� ���
		}

		fprintf(fpTxt, "\n");    // ���� �ȼ� ������ �������� �ٹٲ��� ����
	}

	fclose(fpTxt);    // �ؽ�Ʈ ���� �ݱ�

	free(image);      // �ȼ� �����͸� ������ ���� �޸� ����

	return 0;
}