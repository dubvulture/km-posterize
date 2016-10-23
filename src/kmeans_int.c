#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "generics.h"
#include "shared.h"


typedef __int128 int128_t;



int32_t * kmeans_int32(int32_t *space, int points, int dim, int k)
{
	int32_t *centroids = PICK_CENTROIDS(space, points, dim, k);
	int32_t *cluster_map = calloc(points, sizeof(int32_t));
	int32_t *new_cluster;

	while (1) {
		new_cluster = PARTITION(space, centroids, points, dim, k);

		if (arr_equal(cluster_map, new_cluster, k)) {
			free(cluster_map);
			free(new_cluster);
			return centroids;
		} else {
			free(cluster_map);
			cluster_map = new_cluster;
			free(centroids);
			centroids = COMPUTE_CENTROIDS(space, cluster_map, points, dim, k);
		}
	}
}


int32_t * pick_centroids_int32(int32_t *space, int points, int dim, int k)
{
	int32_t *centroids = malloc(sizeof(int32_t) * k*dim);

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


/**
 *  VSUM has been manually inlined and operates on int128_t types to prevent overflows.
 *  (which forces us to set INT32_MAX_POINTS)
 *  We could count cluster_points and then divide every vector casted to float by it
 *  before doing the summation, but that slows things down.
 *  (and INT32_MAX_POINTS should be enough for the use case)
 */
int32_t * compute_centroids_int32(int32_t *space, int32_t *cluster_map, int points, int dim, int k)
{
	int32_t *centroids = calloc(k*dim, sizeof(int32_t));
	int128_t *sums = calloc(k*dim, sizeof(int128_t));
	int *cluster_points = calloc(k, sizeof(int));

	for (int i=0 ; i<points ; ++i) {
		int n = cluster_map[i];
	
		for (int j=0 ; j<dim ; ++j)
			sums[n*dim + j] = sums[n*dim + j] + (int128_t) space[i*dim + j];

		++cluster_points[n];
	}

	for (int n=0 ; n<k ; ++n) {
		if (cluster_points[n]) {
			for (int i=0 ; i<dim ; ++i)
				centroids[n*dim + i] = (int32_t) (sums[n*dim + i] / cluster_points[n]);
		}
	}
	free(cluster_points);

	return centroids;
}


int * partition_int32(int32_t *space, int32_t *centroids, int points, int dim, int k)
{
	int *cluster_map = malloc(sizeof(int) * points);

	for (int i=0 ; i<points ; ++i) {
		cluster_map[i] = ASSIGN_CLUSTER(&space[i*dim + 0], centroids, dim, k);
	}

	return cluster_map;
}


int assign_cluster_int32(int32_t *vector, int32_t *centroids, int dim, int k)
{
	uint64_t min = DISTANCE(vector, &centroids[0], dim);
	int minpos = 0;

	for (int n=1 ; n<k ; ++n) {
		uint64_t dist = DISTANCE(vector, &centroids[n*dim + 0], dim);
		if (dist < min) {
			min = dist;
			minpos = n;
		}
	}

	return minpos;
}


/**
 * Not used anywhere, kept because of reasons.
 * Cast to int64_t type to avoid overflows.
 */
int32_t * vsum_int32(int32_t *v1, int32_t *v2, int dim)
{
	int32_t *res = malloc(sizeof(int32_t) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = (int32_t) ((int64_t) v1[i] + (int64_t) v2[i]);

	return res;
}


/**
 * Used just for DISTANCE which needs absolute values.
 * Cast to int63_t type to avoid overflows.
 */
int32_t * vsub_int32(int32_t *v1, int32_t *v2, int dim)
{
	int32_t *res = malloc(sizeof(int32_t) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = (int32_t) abs((int64_t) v1[i] - (int64_t) v2[i]);

	return res;
}


/**
 * Manhattan Distance. Sum of absolute values.
 */
uint64_t distance_int32(int32_t *v1, int32_t *v2, int dim)
{
	int32_t *vector = VSUB(v1, v2, dim);
	uint64_t distance = 0;

	for (int i=0 ; i<dim ; ++i)
		distance += abs(vector[i]);

	free(vector);
	return distance;
}
