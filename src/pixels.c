#include <stdint.h>
#include <stdlib.h>

#include "generics.h"



float * assign_color_float(float *matrix, float *centroids, int pixels, int dim, int k)
{
	float *ret = malloc(sizeof(float) * pixels*dim);

	for (int i=0 ; i<pixels ; ++i) {
		int cluster = ASSIGN_CLUSTER(&matrix[i*dim + 0], centroids, dim, k);
		for (int j=0 ; j<dim ; ++j)
			ret[i*dim + j] = centroids[cluster*dim + j];
	}

	return ret;
}


int32_t * assign_color_int32(int32_t *matrix, int32_t *centroids, int pixels, int dim, int k)
{
	int32_t *ret = malloc(sizeof(int32_t) * pixels*dim);

	for (int i=0 ; i<pixels ; ++i) {
		int cluster = ASSIGN_CLUSTER(&matrix[i*dim + 0], centroids, dim, k);
		for (int j=0 ; j<dim ; ++j)
			ret[i*dim + j] = centroids[cluster*dim + j];
	}

	return ret;
}


uint8_t * assign_color_uint8(uint8_t *matrix, uint8_t *centroids, int pixels, int dim, int k)
{
	uint8_t *ret = malloc(sizeof(uint8_t) * pixels*dim);

	for (int i=0 ; i<pixels ; ++i) {
		int cluster = ASSIGN_CLUSTER(&matrix[i*dim + 0], centroids, dim, k);
		for (int j=0 ; j<dim ; ++j)
			ret[i*dim + j] = centroids[cluster*dim + j];
	}

	return ret;
}