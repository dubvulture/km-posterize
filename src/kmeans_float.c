#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "generics.h"
#include "shared.h"



float * kmeans_float(float *space, int points, int dim, int k)
{
	float *centroids = PICK_CENTROIDS(space, points, dim, k);
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


float * pick_centroids_float(float *space, int points, int dim, int k)
{
	float *centroids = malloc(sizeof(float) * k*dim);

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


float * compute_centroids_float(float *space, int *cluster_map, int points, int dim, int k)
{
	float *centroids = calloc(k*dim, sizeof(float));
	int *cluster_points = calloc(k, sizeof(int));

	for (int i=0 ; i<points ; ++i) {
		int n = cluster_map[i];
		float *sum = VSUM(&centroids[n*dim + 0], &space[i*dim + 0], dim);
		for (int j=0 ; j<dim ; ++j)
			centroids[n*dim + j] = sum[j];

		free(sum);
		++cluster_points[n];
	}

	for (int n=0 ; n<k ; ++n) {
		if (cluster_points[n]) {
			for (int i=0 ; i<dim ; ++i)
				centroids[n*dim + i] = centroids[n*dim + i] / (float) cluster_points[n];
		}
	}

	free(cluster_points);

	return centroids;
}


int * partition_float(float *space, float *centroids, int points, int dim, int k)
{
	int *cluster_map = malloc(sizeof(int) * points);

	#pragma omp parallel for
	for (int i=0 ; i<points ; ++i) {
		cluster_map[i] = ASSIGN_CLUSTER(&space[i*dim + 0], centroids, dim, k);
	}

	return cluster_map;
}


int assign_cluster_float(float *vector, float *centroids, int dim, int k)
{
	float min = DISTANCE(vector, &centroids[0], dim);
	int minpos = 0;

	for (int n=1 ; n<k ; ++n) {
		float dist = DISTANCE(vector, &centroids[n*dim + 0], dim);
		if (dist < min) {
			min = dist;
			minpos = n;
		}
	}

	return minpos;
}


float * vsum_float(float *v1, float *v2, int dim)
{
	float *res = malloc(sizeof(float) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = v1[i] + v2[i];

	return res;
}


float * vsub_float(float *v1, float *v2, int dim)
{
	float *res = malloc(sizeof(float) * dim);
	
	for (int i=0 ; i<dim ; ++i)
		res[i] = v1[i] - v2[i];

	return res;
}


float distance_float(float *v1, float *v2, int dimension)
{
	float *vector = VSUB(v1, v2, dimension);
	float distance = sqrt(INNERPROD(vector, vector, dimension));
	free(vector);
	return distance;
}


float innerprod_float(float *v1, float *v2, int dimension)
{
	float res = 0;
	
	for (int i=0 ; i<dimension ; ++i) {
		res += v1[i] * v2[i];
	}

	return res;
}
