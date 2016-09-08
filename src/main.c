#define _XOPEN_SOURCE 700

#include <limits.h>
#include <string.h>
#include <time.h>

#include "bmp.c"
#include "kmeans.c"



/*
 * TODO:
 *	- DUNNO :(
 */



int valid_file(char *filename);
int * kmeans_bmp(struct BMP *bmp, int k);
void clusters_to_bmp(int *cluster_map, int k, int height, int width, char *filename);
unsigned char * merge_clusters(int *cluster_map, int k, int height, int width, char *filename);
void test_split(char *filename, int k);
void test_posterize(char *filename, int k);
void reorder_cluster(unsigned char *matrix, int *cluster_map, int height, int width, int k);
int find(int element, int *array, int size);

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
	int stretch = (width+height)*k;
	for (int i=0 ; i<height ; ++i) {
		for (int j=0 ; j<width ; ++j) {
			space[(i*width + j)*3 + 0] = (long) i;
			space[(i*width + j)*3 + 1] = (long) j;
			space[(i*width + j)*3 + 2] = ((long) matrix[i*width + j]) * stretch;
		}
	}

	int *cluster_map = kmeans(space, width*height, 3, k);
	reorder_cluster(matrix, cluster_map, height, width, k);

	free(space);

	return cluster_map;
}


/*
 * Reorder cluster_map (IN PLACE!) by comparing first seen levels from the original matrix
 */
void reorder_cluster(unsigned char *matrix, int *cluster_map, int height, int width, int k)
{
	int *new_cs = malloc(k*sizeof(int));
	unsigned char *gray_v = malloc(k*sizeof(unsigned char));
	int n = 0;
	int *seen = malloc(k*sizeof(int));

	for (int n=0 ; n<k ; ++n)
		seen[n] = -1;
	
	/* new_cs <- clusters' indexes
	 * gray_v <- clusters' first picked value
	 */
	for (int i=0 ; i<height*width && n<k ; ++i) {
		if (find(cluster_map[i], seen, k) < 0) {
			new_cs[n] = cluster_map[i];
			gray_v[n] = matrix[i];
			seen[n] = cluster_map[i];
			++n;
		}
	}

	/* insertion sort to order clusters' values and the respective indexes */
	for (int i=1 ; i<k ; ++i) {
		unsigned char value = gray_v[i];
		int cluster = new_cs[i];
		int j = i-1;
		while (j>=0 && gray_v[j] > value) {
			gray_v[j+1] = gray_v[j];
			new_cs[j+1] = new_cs[j];
			j = j-1;
		}
		gray_v[j+1] = value;
		new_cs[j+1] = cluster;
	}

	/* Convoluted things ahead (remember that clusters are now sorted!)
	 *
	 * pointers <- position in which each cluster's index appears
	 * cluster_map <- new cluster index
	 *
	 * e.g.
	 * 
	 * Sorted clusters' indexes (new_cs)
	 * {2, 0, 1}	(example values {0, 127, 255})
	 * Pointers:
	 * {1, 2, 0}
	 *
	 * New clusters' indexes: (assuming cluster_map = new_cs)
	 * {0, 1, 2}
	 * 
	 * Old cluster was 2, pointers[2]=0 which is the new cluster's index
	 * Old cluster was 0, pointers[0]=1 which is the new cluster's index
	 * Old cluster was 1, pointers[1]=2 which is the new cluster's index
	 */

	int *pointers = malloc(k*sizeof(int));
	for (int n=0 ; n<k ; ++n)
		pointers[new_cs[n]] = n;

	for (int i=0 ; i<height*width ; ++i)
		cluster_map[i] = pointers[cluster_map[i]];

	free(new_cs);
	free(gray_v);
	free(pointers);
}


int find(int element, int *array, int size)
{
	for (int i=0 ; i<size ; ++i) {
		if (element==array[i])
			return i;
	}
	return -1;
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
	
	float level = min;
	for (int i=0 ; i<k ; ++i) {
		gray_levels[i] = (unsigned char) round(level);
		level += step;
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
