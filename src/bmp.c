#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>



/*
 * A simple utility to read and write BMP files.
 * Now it only supports 8-bit non-compressed grayscales images.
 */



/*
 * Barebones struct representing a BMP
 */
struct BMP
{
	int width;
	int height;
	unsigned char *matrix;
};



struct BMP * readBMP(char *filename);
void writeBMP(unsigned char *matrix, int height, int width, char *filename);



/*
 * Read a basic 8-bit non-compressed BMP image
 */
struct BMP * readBMP(char* filename)
{
	FILE *in = fopen(filename, "rb");
	fseek(in, 18, 0);
	int width;
	fread(&width, 4, 1, in);
	int height;
	fread(&height, 4, 1, in);

	fseek(in, 1078, 0);

	int padding = (4-(width%4))%4;

	unsigned char *matrix = malloc(sizeof(unsigned char*) * height*width);

	if (!padding) {
		fread(matrix, 1, height*width, in);
	} else {
		for (int i=0 ; i<height ; ++i) {
				fread(&matrix[i*width + 0], 1, width, in);
				fseek(in, padding, SEEK_CUR); /* skip padding pixels */
		}
	}

	struct BMP *bmp = malloc(sizeof(struct BMP));
	bmp->width = width;
	bmp->height = height;
	bmp->matrix = matrix;

	return bmp;
}


/*
 * Write a matrix of values [0, 255] as 8-bit non-compressed BMP image
 */
void writeBMP(unsigned char *matrix, int height, int width, char *filename)
{
	FILE *out = fopen(filename, "wb");

	// Headers
	unsigned char signature[2] = {'B','M'};
	uint32_t filesize = 0;
	uint32_t reserved = 0;
	uint32_t offset = 54;
	uint32_t ihsize = 40;
	/* better safe than sorry with int and uint32 */
	uint32_t p_width = (uint32_t) width;
	uint32_t p_height = (uint32_t) height;
	uint16_t planes = 1;
	uint16_t bpp = 8;
	uint32_t compression = 0;
	uint32_t imagesize = 0;
	uint32_t xppm = 0;
	uint32_t yppm = 0;
	uint32_t colours = 256;
	uint32_t impcolours = 0;

	fwrite(signature, sizeof(unsigned char), 2, out);
	fwrite(&filesize, 4, 1, out);
	fwrite(&reserved, 4, 1, out);
	fwrite(&offset, 4, 1, out);
	fwrite(&ihsize, 4, 1, out);
	fwrite(&p_width, 4, 1, out);
	fwrite(&p_height, 4, 1, out);
	fwrite(&planes, 2, 1, out);
	fwrite(&bpp, 2, 1, out);
	fwrite(&compression, 4, 1, out);
	fwrite(&imagesize, 4, 1, out);
	fwrite(&xppm, 4, 1, out);
	fwrite(&yppm, 4, 1, out);
	fwrite(&colours, 4, 1, out);
	fwrite(&impcolours, 4, 1, out);

	// Colour table
	unsigned char bmpcolourtable[1024];

	for (int i=0 ; i<1024 ; ++i)
		bmpcolourtable[i] = 0;

	int k=0;

	for(int i=0 ; i<256; ++i){
		bmpcolourtable[k] = i;
		bmpcolourtable[k+1] = i;
		bmpcolourtable[k+2] = i;
		k = k+4;
	}

	fwrite(&bmpcolourtable, 1, 1024, out);

	// Pixel array
	int remaining = (4-(width%4))%4;
	unsigned char *zero = calloc(remaining, sizeof(unsigned char));

	for(int i=0 ; i<height ; ++i) {
		fwrite(&matrix[i*width + 0], 1, width, out);
		fwrite(zero, 1, remaining, out);
	}

	fclose(out);
}
