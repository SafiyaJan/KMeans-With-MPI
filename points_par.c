#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "io.h"
#include <math.h>
#include <getopt.h>
#include <mpi.h>

#define TAG 1
#define ROOT 0

/* 
 * euclid_dist(double* p1, double* p2)
 * calculates the euclid distance between 2, 2D points
 */
double euclid_dist(double* p1, double* p2)
{
	//Error checks
	if (p1 == NULL || p2 == NULL)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
	}

	// Calcuate difference squares
	double x_square_diff = (p1[0] - p2[0]) * (p1[0] - p2[0]);
	double y_square_diff = (p1[1] - p2[1]) * (p1[1] - p2[1]);

	// Return euclid distance
	return sqrt(x_square_diff + y_square_diff);
}


/* 
 * closest_cluster(double* point, double** centroid, int k)
 * calculates closest centroid to the given point
 */
int closest_cluster(double* point, double** centroid, int k)
{
	// Error checks 
	if (point == NULL || centroid == NULL)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
	}

	// Initially assuming first centroid is closest to point
	int min = 0;
	
	// Euclid distance between points and first centrid
	double dist = euclid_dist(point, centroid[0]);

	int i;
	for (i = 1; i < k; i ++)
	{
		// Calculate euclid dist from point to centroid
		
		double temp = euclid_dist(point, centroid[i]);
	
		// Check if this dist is the smaller than min
		if (temp < dist)
		{
			// If yes, make dist new min and update min index
			dist = temp;
			min = i;
		}
	}

	// Return index of closest cluster
	return min;


}

/* 
 * k_means_mpi - calculates k cluster centroids given the number of clusters 
 * needed and total data points. Computation is distributed across different
 * processes to quicken clustering time
 */
double** kmeans_mpi(char* filename, double** data_pts, double** centroids, 
	int k, int num_pts, int num_itr, int rank, int num_procs, int total_pts)
{


	int i,j,closest;
	MPI_Status status;

	//Null checks
	if(filename == NULL || data_pts == NULL || centroids == NULL || num_pts == 0 
		|| num_itr <= 0)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}

	// Initialinf array to store which cluster each point belongs to
	int* which_cluster = (int*)malloc(num_pts * sizeof(int));
	
	for (i = 0; i < num_pts ; i++)
	{
        which_cluster[i] = -1; 
	}

	// Array to keep track of how many points there are in each cluster
	int* pointsPerCluster = (int*)malloc(k * sizeof(int));
	for (i = 0; i < k ; i++)
	{
        pointsPerCluster[i] = 0; 
	}

	int itr = 0;
	
	double error = 1.0;

	// Clustering begins
	while(error > 0.0 || itr < num_itr)
	{
		double partial_pointschanged = 0;
		double pointschanged = 0;

		// Find the closest cluster to each point
		for(i = 0; i < num_pts; i++)
		{
			closest = closest_cluster(data_pts[i], centroids, k);

			if (which_cluster[i]!=closest)
			{
				which_cluster[i] = closest;
				partial_pointschanged++;
			}
		}
		
		//Calculate new centroids 
		for(i = 0; i < k; i++)
		{
			double partial_sum_x = 0.0;
			double partial_sum_y = 0.0;
			int partial_count = 0;
			
			// Go through each point sum the x and y coordinates of the points 
			// that belong to cluster 0, cluster 1 and so on
			for (j = 0; j < num_pts; j++)
			{
				/* Check if we're summing correct x & y values
				 * Eg - if i = 0, sum x & y values of points 
				 * that belongs to cluster 0 */
				
				if(which_cluster[j] == i)
				{
					partial_sum_x += data_pts[j][0];
					partial_sum_y += data_pts[j][1];
					partial_count ++;
				}

			}

			int count = 0;
			double x = 0.0;
			double y = 0.0;

			//Getting all the sums and counts from the slaves
			MPI_Reduce(&partial_count, &count, 1, MPI_INT,  MPI_SUM, ROOT, MPI_COMM_WORLD);
			MPI_Reduce(&partial_sum_x, &x, 1, MPI_DOUBLE,  MPI_SUM, ROOT, MPI_COMM_WORLD);
			MPI_Reduce(&partial_sum_y, &y, 1, MPI_DOUBLE,  MPI_SUM, ROOT, MPI_COMM_WORLD);
			MPI_Reduce(&partial_pointschanged, &pointschanged, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);

			//If process is master, compute the centroids and send to slaves
			if (rank == 0)
			{
				// Calculate new centroids by averaging x and y sums 
				double x_coord = x / (double)count;
				double y_coord = y / (double)count;

				pointsPerCluster[i] = count;

				centroids[i][0] = x_coord;
				centroids[i][1] = y_coord;

				// Send to slave
				for (j = 1; j < num_procs; j++)
					MPI_Send(centroids[i], 2, MPI_DOUBLE, j, TAG, MPI_COMM_WORLD);			
			}

			//If process is slave, recieve the new centroids
			else
			{
				MPI_Recv(centroids[i], 2, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);
			}

		}
		
		// Master sums the number of oints that have changed to different clusters
		// and sends this number to all the slaves
		MPI_Allreduce(&partial_pointschanged, &pointschanged, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		
		//Each process calculate the the error and iterates once more
		error = pointschanged/((double)total_pts);
		itr++;
	}

	// If master 
	if (rank==0)
	{
		// Store the final centroids in a file
		printf("Number of iterations it took - %d & error - %f\n", itr, error);
		
		FILE* fp = fopen(filename, "w");

		// If file could not be opened for writing
		if (fp == NULL)
		{
			fprintf(stderr,"Error reading the file, returning NULL\n");
		}

		for(i = 0; i < k; i ++)
		{
			// Write to file the final cluster centroids 
			fprintf(fp, "%lf,", centroids[i][0]);
			fprintf(fp, "%lf,", centroids[i][1]);
			fprintf(fp, "%d\n", pointsPerCluster[i]);
		} 

	}


	free(which_cluster);
	free(pointsPerCluster);

	return centroids;

}

int main(int argc, char **argv)
{
	int myRank;   
	int num_procs; 
	int i;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	//Ensure atleast 3 args are present
	if (argc < 3)
	{
		printf("Too few arguments, exiting");
		return 0;
	}

	int num_clusters = 0 ;
	int total_num_pts = 0 ;
	char* input_datafile = "";

	int num_itr = 1;
	char opt;

	// Parse the arguments and store them in local vars
	while (( opt = getopt (argc, argv, "c:t:i:n:")) != -1)
	{
		switch(opt)
		{
			case 'c':
			{
				num_clusters = atoi(optarg);
				break;
			}
			case 't':
			{	
				total_num_pts = atoi(optarg);
				break;
			}
			case'i':
			{
				input_datafile = optarg;
				break;
			}
			
			case'n':
			{
				num_itr = atoi(optarg);
				break;
			}

			case'?':
			{
				fprintf(stderr, "Incorrect option given - %c\n", opt);
			}

			default:
				break;
			
		}

	}


	// Allocating array to store centroid
	double** centroids = (double**)malloc(num_clusters * sizeof(double*));
	
	// Allocate 2d array for each index in points (this is to store x, y)
	for (i = 0; i < num_clusters ; i++)
	{
        centroids[i] = (double*)calloc(2,sizeof(double)); 
	}

	int* num_pts = (int*)calloc(1,sizeof(int));

	double* start_time = (double*)calloc(1,sizeof(double));

	//Read points and distribute to slaves
	double** points = readDataPoints_mpi_temp(input_datafile, total_num_pts, myRank, 
	centroids, num_clusters, num_procs, num_pts, start_time);
	
	MPI_Barrier(MPI_COMM_WORLD);


	if (myRank == 0)printf("Performing Kmeans now...\n");
	
	//Perform clustering on points 
	kmeans_mpi("2D_mpi_finalcentroids.csv", points, centroids, 
	num_clusters, *num_pts, num_itr, myRank, num_procs, total_num_pts);
	
	//Make sure all process reach hear at the same point
	MPI_Barrier(MPI_COMM_WORLD);

	if (myRank == ROOT) {
		double end_time = MPI_Wtime();
		printf("\nTime taken to complete - %lf\n",end_time-(*start_time));

	}

	//Print Calculated Centroids
	if (myRank == 0)
	{
		printf("Calculated centroids\n");
		for(i = 0; i < num_clusters; i++)
				printf("x - %lf, y - %lf\n", centroids[i][0], centroids[i][1]);	
	}

	MPI_Barrier(MPI_COMM_WORLD);

	//Clean up - free all allocated memory
	for (i = 0; i < num_clusters; i++)
	{
		free(centroids[i]);
	}

	free(centroids);
	free(points);
	free(start_time);

	MPI_Finalize();

	return 0;

}