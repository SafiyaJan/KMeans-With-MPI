double** readDataPoints(char* filename, int num_pts);

double** readDataPoints_temp(char* filename, int num_pts);

void writeDataPoints(char* filename, double** centroids, int num_pts);

double** readDataPoints_mpi(char* filename, int total_num_pts, int rank, 
	double** centroids, int num_clusters, int num_procs, int* num_pts);

double** readDataPoints_mpi_temp(char* filename, int total_num_pts, int rank, 
	double** centroids, int num_clusters, int num_procs, int* num_pts, double* starttime);

char** read_DNAstrands(char* filename, int num_pts, int length);

char** read_DNAstrands_mpi(char* filename, int total_num_pts, int rank, 
	char** centroids, int num_clusters, int num_procs, int* num_pts, int length);

