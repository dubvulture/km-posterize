#define _XOPEN_SOURCE 700

#include <limits.h>
#include <string.h>
#include <time.h>

#include "bmp.c"
#include "kmeans.c"



/*
 * TODO:
 *	- merge matrices and save "posterized" image
 */



void kmeans_8bit_grayscale_bmp(char *, int);
int valid_file(char *filename);


/*
 * Apply K-Means clustering to an 8-bit BMP non-compressed grayscale image and
 * save the clusters as B/N BMP images. (still 8-bit grayscale though)
 */
void kmeans_8bit_grayscale_bmp(char *filename, int k)
{
	struct BMP *bmp = readBMP(filename);
	int width = bmp->width;
	int height = bmp->height;
	unsigned char *matrix = bmp->matrix;

	long *space = malloc(3*height*width*sizeof(long));

	/* transform 2D matrix into array of vectors (x, y, matrix[x][y])
	 * arbitrary stretch factor, need further investigation on optimal value
	 * (probably correlated to height and width)
	 */
	int stretch = 100;
	for (int i=0 ; i<height ; ++i) {
		for (int j=0 ; j<width ; ++j) {
			space[(i*width + j)*3 + 0] = (long) i;
			space[(i*width + j)*3 + 1] = (long) j;
			space[(i*width + j)*3 + 2] = ((long) matrix[i*width + j]) * stretch;
		}
	}

	int *cluster_map = kmeans(space, width*height, 3, k);

	free(space);
	free(matrix);
	free(bmp);

	/* matrix_cluster filled with 255s */
	unsigned char **matrix_cluster = malloc(k*sizeof(unsigned char*));
	for (int n=0 ; n<k ; ++n) {
		matrix_cluster[n] = malloc(height*width*sizeof(unsigned char*));
		for (int i=0 ; i<height ; ++i) {
			for (int j=0 ; j<width ; ++j)
				matrix_cluster[n][i*width + j] = 255;
		}
	}

	int *pos = calloc(k, sizeof(int));
	/* set to 0 (i*width + j)th point belonging to nth cluster*/
	for (int i=0 ; i<height ; ++i) {
		for (int j=0 ; j<width ; ++j) {
			int n = cluster_map[i*width + j];
			matrix_cluster[n][i*width + j] = 0;
			pos[n]++;
		}
	}

	/* truncate ".bmp" */
	filename[strlen(filename) - 4] = 0;

	char *output;
	for (int n=0 ; n<k ; ++n) {
		output = malloc(128*sizeof(char));
		sprintf(output, "%s_%d.bmp", filename, n);
		writeBMP(matrix_cluster[n], height, width, output);
		free(output);
		free(matrix_cluster[n]);
	}

	free(matrix_cluster);

}


int valid_file(char *filename)
{
	FILE *fp = fopen (filename, "rb");
	if (fp != NULL) fclose(fp);
	
	return (fp != NULL);
}


int main(int argc, char **argv)
{
	if (argc==1)
		printf("Filename of BMP picture is needed\n");
	
	for (int i=1 ; i<argc ; ++i) {
		if (!valid_file(argv[i])) {
			printf("\"%s\"", argv[i]);
			printf(" does not exists or not enough rights.\n");
			continue;
		}

		/* get real path (otherwise bad things happen) */
		char actualpath[PATH_MAX+1];
		char *ptr;
		ptr = realpath(argv[i], actualpath);

		kmeans_8bit_grayscale_bmp(ptr, 4);
	}
}
