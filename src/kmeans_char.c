#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "generics.h"
#include "shared.h"


/**
 * Max distance in a N dimensional space is N*255, hence here we allow a max of N=257
 * (which I think should be enough)
 * ((2**16 - 1) / 255 = 257
 */
#define UINT8_MAX_DIM 257
/*
 *  With uint32_t as we allow a maximum number of (2**32 -1) / 255 points in our space.
 */
#define UINT8_MAX_POINTS 16843009



uint8_t * kmeans_uint8(uint8_t *space, int points, int dim, int k)
{
	assert(points <= UINT8_MAX_POINTS);
	assert(dim <= UINT8_MAX_DIM);
	
	uint8_t *centroids = PICK_CENTROIDS(space, points, dim, k);
	int *cluster_map = calloc(points, sizeof(int));
	int *new_cluster;

	int threshold = points / 500; // 0.5%
	while (1) {
		new_cluster = PARTITION(space, centroids, points, dim, k);

		if (difference(cluster_map, new_cluster, points) < threshold) {
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


uint8_t * pick_centroids_uint8(uint8_t *space, int points, int dim, int k)
{
	uint8_t *centroids = malloc(sizeof(uint8_t) * k*dim);

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
 *  VSUM has been manually inlined and operates on uint32_t types to prevent overflows.
 *  (which forces us to set UINT8_MAX_POINTS)
 *  We could count cluster_points and then divide every vector casted to float by it
 *  before doing the summation, but that slows things down.
 *  (and UINT8_MAX_POINTS should be enough for the use case)
 */
uint8_t * compute_centroids_uint8(uint8_t *space, int *cluster_map, int points, int dim, int k)
{
	uint8_t *centroids = malloc(k*dim * sizeof(uint8_t));
	uint32_t *sums = calloc(k*dim, sizeof(uint32_t));
	int *cluster_points = calloc(k, sizeof(int));

	for (int i=0 ; i<points ; ++i) {
		int n = cluster_map[i];
	
		for (int j=0 ; j<dim ; ++j)
			sums[n*dim + j] = sums[n*dim + j] + (uint32_t) space[i*dim + j];

		++cluster_points[n];
	}

	for (int n=0 ; n<k ; ++n) {
		if (cluster_points[n]) {
			for (int i=0 ; i<dim ; ++i)
				centroids[n*dim + i] = (uint8_t) (sums[n*dim + i] / cluster_points[n]);
		}
	}

	free(sums);
	free(cluster_points);

	return centroids;
}


int * partition_uint8(uint8_t *space, uint8_t *centroids, int points, int dim, int k)
{
	int *cluster_map = malloc(sizeof(int) * points);

	#pragma omp parallel for
	for (int i=0 ; i<points ; ++i) {
		cluster_map[i] = ASSIGN_CLUSTER(&space[i*dim + 0], centroids, dim, k);
	}

	return cluster_map;
}


int assign_cluster_uint8(uint8_t *vector, uint8_t *centroids, int dim, int k)
{
	uint16_t min = DISTANCE(vector, &centroids[0], dim);
	int minpos = 0;

	for (int n=1 ; n<k ; ++n) {
		uint16_t dist = DISTANCE(vector, &centroids[n*dim + 0], dim);
		if (dist < min) {
			min = dist;
			minpos = n;
		}
	}

	return minpos;
}


/**
 * Not used anywhere, kept because of reasons.
 * Cast to uint16_t type to avoid overflows.
 */
uint8_t * vsum_uint8(uint8_t *v1, uint8_t *v2, int dim)
{
	uint8_t *res = malloc(sizeof(uint8_t) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = (uint8_t) ((uint16_t) v1[i] + (uint16_t) v2[i]);

	return res;
}


/**
 * Used just for DISTANCE which needs absolute values.
 * Cast to int16_t type to avoid overflows.
 */
uint8_t * vsub_uint8(uint8_t *v1, uint8_t *v2, int dim)
{
	uint8_t *res = malloc(sizeof(uint8_t) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = (uint8_t) abs((int16_t) v1[i] - (int16_t) v2[i]);

	return res;
}


/**
 * Manhattan Distance. Sum of absolute values.
 */
uint16_t distance_uint8(uint8_t *v1, uint8_t *v2, int dim)
{
	uint8_t *vector = VSUB(v1, v2, dim);
	uint16_t distance = 0;

	for (int i=0 ; i<dim ; ++i)
		distance += vector[i];

	free(vector);
	return distance;
}
