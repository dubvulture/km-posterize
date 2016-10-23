/**
 * No ifndef/define because this needs to be read by CFFI which doesn't support such things
 */


enum { KM_FLOAT32, KM_INT32, KM_UINT8 };


void * kmeans(void *space, int points, int dim, int k, int type);
void * compute_centroids(void *space, int *cluster_map, int points, int dim, int k, int type);
void * partition(void *space, void *centroids, int points, int dim, int k, int type);
int assign_cluster(void *vector, void *centroids, int dim, int k, int type);
void * assign_color(void *matrix, void *centroids, int pixels, int dim, int k, int type);