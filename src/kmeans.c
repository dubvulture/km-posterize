#define _XOPEN_SOURCE 700

#include <math.h>
#include <stdlib.h>



/*
 * Lloyd's K-Means clustering algorithm
 * This version only works with \mathbb{Z}^n vectorial spaces for our purpose
 * of using it on BMP files' pixel matrices accordingly transformed.
 */



/* kmeans-core */
int * kmeans(long *space, int points, int dim, int k);
long * pick_centroids(long *space, int points, int dim, int k);
long * compute_centroids(long *space, int *cluster_map, int points, int dim, int k);
int * partition(long *space, long *centroids, int points, int dim, int k);
int assign_cluster(long *vector, long *centroids, int dim, int k);

/* kmeans-utilities */
int arr_equal(int *arr1, int *arr2, int k);
long * vsum(long *v1, long *v2, int dim);
long * vsub(long *v1, long *v2, int dim);
long distance(long *v1, long *v2, int dim);
int stolen_random(int size);



/*
 * I guess if you're reading this, you already know what k-mean clustering is.
 * Return a cluster map of space.
 */
int *kmeans(long *space, int points, int dim, int k)
{
	long *centroids = pick_centroids(space, points, dim, k);
	int *cluster_map = calloc(points, sizeof(int));
	int *new_cluster;
	int cycles = 0;

	while (1) {
		new_cluster = partition(space, centroids, points, dim, k);

		if (arr_equal(cluster_map, new_cluster, k)) {
			free(cluster_map);
			return new_cluster;
		} else {
			free(cluster_map);
			cluster_map = new_cluster;
			free(centroids);
			centroids = compute_centroids(space, cluster_map, points, dim, k);
			++cycles;
		}
	}
}


/*
 * The Forgy's way, pick k random centroids.
 */
long *pick_centroids(long *space, int points, int dim, int k)
{
	long *centroids = malloc(sizeof(long*) * k*dim);

	/* initialize random pool and retrieve 1st random number */
	int random = stolen_random(points);
	for (int i=0 ; i<dim ; ++i)
		centroids[0 + i] = space[random*dim + i];

	for (int n=1 ; n<k ; ++n) {
		random = stolen_random(-1);

		for (int i=0 ; i<dim ; ++i)
			centroids[n*dim + i] = space[random*dim + i];
	}

	return centroids;
}


/*
 * Compute centroids of given clusters
 */
long *compute_centroids(long *space, int *cluster_map, int points, int dim, int k)
{
	/* calloc -> need all 0s for first iteration */
	long *centroids = calloc(k*dim, sizeof(long));
	int *cluster_points = calloc(k, sizeof(int));

	for (int i=0 ; i<points ; ++i) {
		int n = cluster_map[i];
		long *sum = vsum(&centroids[n*dim +0], &space[i*dim + 0], dim);
		
		for (int j=0 ; j<dim ; ++j)
			centroids[n*dim + j] = sum[j];

		free(sum);
		++cluster_points[n];
	}

	for (int n=0 ; n<k ; ++n) {
		for (int i=0 ; i<dim ; ++i) 
			centroids[n*dim + i] = centroids[n*dim + i] / (long) cluster_points[n];
	}

	return centroids;
}

/*
 * Given k centroids, return a partition of space
 */
int *partition(long *space, long *centroids, int points, int dim, int k)
{
	int *cluster_map = malloc(sizeof(int) * points);

	for (int i=0 ; i<points ; ++i) {
		cluster_map[i] = assign_cluster(&space[i*dim + 0], centroids, dim, k);
	}

	return cluster_map;
}


/*
 * Decide which centroid the vector is the closest to.
 */
int assign_cluster(long *vector, long *centroids, int dim, int k)
{
	long min = distance(vector, &centroids[0], dim);
	int minpos = 0;

	for (int n=1 ; n<k ; ++n) {
		long dist = distance(vector, &centroids[n*dim + 0], dim);
		if (dist < min) {
			min = dist;
			minpos = n;
		}
	}

	return minpos;
}




/*
 * Just check whether arr1 and arr2 are equal
 */
int arr_equal(int *arr1, int *arr2, int k)
{
	int flag = 1;
	for (int i=0 ; i<k && flag ; ++i) {
		if (arr1[i] != arr2[i])
			flag = 0;
	}

	return flag;
}


/*
 * Vector Addition
 */
long *vsum(long *v1, long *v2, int dim)
{
	long *res = malloc(sizeof(long) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = v1[i] + v2[i];

	return res;
}


/*
 * Vector Substraction
 */
long *vsub(long *v1, long *v2, int dim)
{
	long *res = malloc(sizeof(long) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = v1[i] - v2[i];

	return res;
}


/*
 * Manhattan distance
 */
long distance(long *v1, long *v2, int dim)
{
	long *vector = vsub(v1, v2, dim);
	long distance = 0;

	for (int i=0 ; i<dim ; ++i)
		distance += abs(vector[i]);

	free(vector);
	return distance;
}


/*
 * Shame on me, I copied this from the internet
 * https://stackoverflow.com/questions/5064379/generating-unique-random-numbers-in-c
 */
int stolen_random(int size)
{
	int i, n;
	static int numNums = 0;
	static int *numArr = NULL;

	/* Initialize with a specific size */
	if (size >= 0) {
		if (numArr != NULL)
			free(numArr);
		if ((numArr = malloc(sizeof(int) * size)) == NULL)
			return -2; /* out of memory */
		for (i = 0; i  < size; i++)
			numArr[i] = i;
		numNums = size;
	}

	/* Error if no numbers left in pool */
	if (numNums == 0)
		return -1;

	/* Get random number from pool and remove it (rnd in this
	 * case returns a number between 0 and numNums-1 inclusive)
	 */
	n = rand() % numNums;
	i = numArr[n];
	numArr[n] = numArr[numNums-1];
	numNums--;
	if (numNums == 0) {
		free (numArr);
		numArr = 0;
	}

	return i;
}
