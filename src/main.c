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



int valid_file(char *filename);
int * kmeans_bmp(struct BMP *bmp, int k);
void clusters_to_bmp(int *cluster_map, int k, int height, int width, char *filename);
unsigned char * merge_clusters(int *cluster_map, int k, int height, int width, char *filename);
void test_split(char *filename, int k);
void test_posterize(char *filename, int k);


int valid_file(char *filename)
{
	FILE *fp = fopen (filename, "rb");
	if (fp != NULL) fclose(fp);
	
	return (fp != NULL);
}


/*
 * Apply K-Means clustering to an 8-bit BMP non-compressed grayscale image
 */
int * kmeans_bmp(struct BMP *bmp, int k)
{
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

	return cluster_map;
}


/*
 * Save clusters as B/N BMP images (still 8-bit grayscale though)
 */
void clusters_to_bmp(int *cluster_map, int k, int height, int width, char *filename)
{
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


/* Merge k clusters into a single matrix where each cluster's point is assigned
 * an increasing level of gray
 */
unsigned char * merge_clusters(int *cluster_map, int k, int height, int width, char *filename)
{
	float max = 255.0f;
	float min = 0.0f;
	float step = (max - min) / (float) k;

	unsigned char *gray_levels = malloc(k*sizeof(unsigned char));
	
	float level = max;
	for (int i=0 ; i<k ; ++i) {
		gray_levels[i] = (unsigned char) round(level);
		level -= step;
	}

	unsigned char *matrix = malloc(height*width*(sizeof(unsigned char)));
	/* matrix filled with 255s */
	for (int i=0 ; i<height ; ++i) {
		for (int j=0 ; j<width ; ++j) {
			int n = cluster_map[i*width + j];
			matrix[i*width + j] = gray_levels[n];
		}
	}

	free(gray_levels);

	return matrix;

}


/* Get a cluster map of given BMP and save clusters as new BMP images.
 */
void test_split(char *filename, int k)
{
	struct BMP *bmp = readBMP(filename);
	int *cluster_map = kmeans_bmp(bmp, k);
	clusters_to_bmp(cluster_map, k, bmp->height, bmp->width, filename);

	free(bmp->matrix);
	free(bmp);
	free(cluster_map);
}


/* Get a cluster map of given BMP and save clusters as new BMP images.
 */
void test_posterize(char *filename, int k)
{
	struct BMP *bmp = readBMP(filename);
	int *cluster_map = kmeans_bmp(bmp, k);
	unsigned char *matrix = merge_clusters(cluster_map, k, bmp->height, bmp->width, filename);
	/* truncate ".bmp" */
	filename[strlen(filename) - 4] = 0;
	char *output = malloc((strlen(filename)+11)*sizeof(char));
	sprintf(output, "%s_posterized.bmp", filename);
	writeBMP(matrix, bmp->height, bmp->width, output);

	free(output);
	free(matrix);
	free(bmp->matrix);
	free(bmp);
	free(cluster_map);
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
		char *path;
		path = realpath(argv[i], actualpath);

		test_posterize(path, 4);
	}
}
