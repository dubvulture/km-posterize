#ifndef GENERICS_H
#define GENERICS_H


#include <assert.h>
#include <stdint.h>


#define KMEANS(X, points, dim, k) \
			_Generic((X), \
				float * :		kmeans_float, \
				int32_t * :		kmeans_int32, \
				uint8_t * :		kmeans_uint8, \
				default:		g_error \
			) (X, points, dim, k)

#define PICK_CENTROIDS(X, points, dim, k) \
			_Generic((X), \
				float * :		pick_centroids_float, \
				int32_t * :		pick_centroids_int32, \
				uint8_t * :		pick_centroids_uint8, \
				default:		g_error \
			) (X, points, dim, k)

#define COMPUTE_CENTROIDS(X, cluster_map, points, dim, k) \
			_Generic((X), \
				float * :		compute_centroids_float, \
				int32_t * :		compute_centroids_int32, \
				uint8_t * :		compute_centroids_uint8, \
				default:		g_error \
			) (X, cluster_map, points, dim, k)

#define PARTITION(X, Y, points, dim, k) \
			_Generic((X), \
				float * :		_Generic((Y), \
									float * :		partition_float, \
									default:		g_error \
								), \
				int32_t * :		_Generic((Y), \
									int32_t * :		partition_int32, \
									default:		g_error \
								), \
				uint8_t * :		_Generic((Y), \
									uint8_t * :		partition_uint8, \
									default:		g_error \
								), \
				default:		g_error \
			) (X, Y, points, dim, k)

#define ASSIGN_CLUSTER(X, Y, dim, k) \
			_Generic((X), \
				float * :		_Generic((Y), \
									float * :		assign_cluster_float, \
									default:		g_error \
								), \
				int32_t * :		_Generic((Y), \
									int32_t * :		assign_cluster_int32, \
									default:		g_error \
								), \
				uint8_t * :		_Generic((Y), \
									uint8_t * :		assign_cluster_uint8, \
									default:		g_error \
								), \
				default:		g_error \
			) (X, Y, dim, k)

#define VSUM(X, Y, dim) \
			_Generic((X), \
				float * :		_Generic((Y), \
									float * :		vsum_float, \
									default:		g_error \
								), \
				int32_t * :		_Generic((Y), \
									int32_t * :		vsum_int32, \
									default:		g_error \
								), \
				uint8_t * :		_Generic((Y), \
									uint8_t * :		vsum_uint8, \
									default:		g_error \
								), \
				default:		g_error \
			) (X, Y, dim)

#define VSUB(X, Y, dim) \
			_Generic((X), \
				float * :		_Generic((Y), \
									float * :		vsub_float, \
									default:		g_error \
								), \
				int32_t * :		_Generic((Y), \
									int32_t * :		vsub_int32, \
									default:		g_error \
								), \
				uint8_t * :		_Generic((Y), \
									uint8_t * :		vsub_uint8, \
									default:		g_error \
								), \
				default:		g_error \
			) (X, Y, dim)

#define INNERPROD(X, Y, dim) \
			_Generic((X), \
				float * :		_Generic((Y), \
									float * :		innerprod_float, \
									default:		g_error \
								), \
				default:		g_error \
			) (X, Y, dim)

#define DISTANCE(X, Y, dim) \
			_Generic((X), \
				float * :		_Generic((Y), \
									float * :		distance_float, \
									default:		g_error \
								), \
				int32_t * :		_Generic((Y), \
									int32_t * :		distance_int32, \
									default:		g_error \
								), \
				uint8_t * :		_Generic((Y), \
									uint8_t * :		distance_uint8, \
									default:		g_error \
								), \
				default:		g_error \
			) (X, Y, dim)

#define ASSIGN_COLOR(X, Y, pixels, dim, k) \
			_Generic((X), \
				float * :		_Generic((Y), \
									float * :		assign_color_float, \
									default:		g_error \
								), \
				int32_t * :		_Generic((Y), \
									int32_t * :		assign_color_int32, \
									default:		g_error \
								), \
				uint8_t * :		_Generic((Y), \
									uint8_t * :		assign_color_uint8, \
									default:		g_error \
								), \
				default:		g_error \
			) (X, Y, pixels, dim, k)


float * kmeans_float(float *space, int points, int dim, int k);
int32_t * kmeans_int32(int32_t *space, int points, int dim, int k);
uint8_t * kmeans_uint8(uint8_t *space, int points, int dim, int k);

float * pick_centroids_float(float *space, int points, int dim, int k);
int32_t * pick_centroids_int32(int32_t *space, int points, int dim, int k);
uint8_t * pick_centroids_uint8(uint8_t *space, int points, int dim, int k);

float * compute_centroids_float(float *space, int32_t *cluster_map, int points, int dim, int k);
int32_t * compute_centroids_int32(int32_t *space, int32_t *cluster_map, int points, int dim, int k);
uint8_t * compute_centroids_uint8(uint8_t *space, int32_t *cluster_map, int points, int dim, int k);

int32_t * partition_float(float *space, float *centroids, int points, int dim, int k);
int32_t * partition_int32(int32_t *space, int32_t *centroids, int points, int dim, int k);
int32_t * partition_uint8(uint8_t *space, uint8_t *centroids, int points, int dim, int k);

int assign_cluster_float(float *vector, float *centroids, int dim, int k);
int assign_cluster_int32(int32_t *vector, int32_t *centroids, int dim, int k);
int assign_cluster_uint8(uint8_t *vector, uint8_t *centroids, int dim, int k);

float * vsum_float(float *v1, float *v2, int dim);
int32_t * vsum_int32(int32_t *v1, int32_t *v2, int dim);
uint8_t * vsum_uint8(uint8_t *v1, uint8_t *v2, int dim);

float * vsub_float(float *v1, float *v2, int dim);
int32_t * vsub_int32(int32_t *v1, int32_t *v2, int dim);
uint8_t * vsub_uint8(uint8_t *v1, uint8_t *v2, int dim);

float innerprod_float(float *v1, float *v2, int dim);

float distance_float(float *v1, float *v2, int dim);
uint64_t distance_int32(int32_t *v1, int32_t *v2, int dim);
uint16_t distance_uint8(uint8_t *v1, uint8_t *v2, int dim);

float * assign_color_float(float *matrix, float *centroids, int pixels, int dim, int k);
int32_t * assign_color_int32(int32_t *matrix, int32_t *centroids, int pixels, int dim, int k);
uint8_t * assign_color_uint8(uint8_t *matrix, uint8_t *centroids, int pixels, int dim, int k);


static void g_error(void)
{
	assert(0);
}


#endif
