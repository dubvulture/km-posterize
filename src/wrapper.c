#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "generics.h"
#include "wrapper.h"



void * kmeans(void *space, int points, int dim, int k, int type)
{
	switch (type) {
		case 0:
			return (void *) KMEANS((float *) space, points, dim, k);
			break;
		case 1:
			return (void *) KMEANS((int32_t *) space, points, dim, k);
			break;
		case 2:
			return (void *) KMEANS((uint8_t *) space, points, dim, k);
			break;
		default:
			assert(0 && "Unknown type");
	}
}


void * compute_centroids(void *space, int32_t *cluster_map, int points, int dim, int k, int type)
{
	switch (type) {
		case 0:
			return (void *) COMPUTE_CENTROIDS((float *) space, cluster_map, points, dim, k);
			break;
		case 1:
			return (void *) COMPUTE_CENTROIDS((int32_t *) space, cluster_map, points, dim, k);
			break;
		case 2:
			return (void *) COMPUTE_CENTROIDS((uint8_t *) space, cluster_map, points, dim, k);
			break;
		default:
			assert(0 && "Unknown type");
	}
}


void * partition(void *space, void *centroids, int points, int dim, int k, int type)
{
	switch (type) {
		case 0:
			return (void *) PARTITION((float *) space, (float *) centroids, points, dim, k);
			break;
		case 1:
			return (void *) PARTITION((int32_t *) space, (int32_t *) centroids, points, dim, k);
			break;
		case 2:
			return (void *) PARTITION((uint8_t *) space, (uint8_t *) centroids, points, dim, k);
			break;
		default:
			assert(0 && "Unknown type");
	}
}


int assign_cluster(void *vector, void *centroids, int dim, int k, int type)
{
	switch (type) {
		case 0:
			return ASSIGN_CLUSTER((float *) vector, (float *) centroids, dim, k);
			break;
		case 1:
			return ASSIGN_CLUSTER((int32_t *) vector, (int32_t *) centroids, dim, k);
			break;
		case 2:
			return ASSIGN_CLUSTER((uint8_t *) vector, (uint8_t *) centroids, dim, k);
			break;
		default:
			assert(0 && "Unknown type");
	}
}


void * assign_color(void *matrix, void *centroids, int pixels, int dim, int k, int type)
{
	switch (type) {
		case 0:
			return (void *) ASSIGN_COLOR((float *) matrix, (float *) centroids, pixels, dim, k);
			break;
		case 1:
			return (void *) ASSIGN_COLOR((int32_t *) matrix, (int32_t *) centroids, pixels, dim, k);
			break;
		case 2:
			return (void *) ASSIGN_COLOR((uint8_t *) matrix, (uint8_t *) centroids, pixels, dim, k);
		default:
			assert(0 && "Unknown type");
	}
}
