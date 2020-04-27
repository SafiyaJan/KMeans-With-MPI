#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "io.h"
#include <math.h>
#include <mpi.h>

#define TAG 1


double euclid_dist_read(double* p1, double* p2)
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

double** readDataPoints_mpi_temp(char* filename, int total_num_pts, int rank, 
	double** centroids, int num_clusters, int num_procs, int* num_pts, double* starttime)
{
	

	if(filename == NULL || total_num_pts == 0 || centroids == NULL || num_pts == NULL
		|| starttime == NULL)
	{	
		printf("CAME HERE\n");
		
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}

	MPI_Status status;
	int i,j,k;

	int myRank = rank;
	
	//Master
	if (rank == 0)
	{
		//master reads points from file
		double start_time = MPI_Wtime();
		double** points = readDataPoints_temp(filename, total_num_pts);
		
		*starttime = start_time;
		double** master_points;

		//Choose random centroids

		int rand_index = 0;
		for(i = 0; i < num_clusters; i++)
		{
			
			//int rand_index = (rand() % (total_num_pts/num_clusters)) + i*(total_num_pts/num_clusters);

			centroids[i][0] = points[rand_index][0];
			centroids[i][1] = points[rand_index][1];

			rand_index += total_num_pts/num_clusters;
	        
	    }
	    printf("Distributing points now...\n");
	    //Send the centroids to other processes
	    for(j = 1; j < num_procs; j++)
	    {
	    	//Send the number of centroids 
	    
	    	MPI_Send(&num_clusters, 1, MPI_INT, j, TAG, MPI_COMM_WORLD);
	    	
	    	// Send the centroids
	    	for(k = 0; k < num_clusters; k++)
			{
		
				MPI_Send(centroids[k],2,MPI_DOUBLE, j, j, MPI_COMM_WORLD);
			}

	    }

		// Split data points and distribute among all processes
		for(j = 0; j < num_procs; j++)
		{
			// Len is number of points to send
			int len =  (total_num_pts * (j + 1) / num_procs) - (total_num_pts*j/num_procs);
			
			// Start is the index from where to send from 
			int start = total_num_pts*j/num_procs;
			
			// If master, master stores some points as well
			if(j == 0)
			{
				// Allocating array to store data points
				master_points = (double**)malloc(len * sizeof(double*));
				
				// Allocate 2d array for each index in points (this is to store x, y)
				for (i = 0; i < len ; i++)
				{
			        master_points[i] = (double*)calloc(2,sizeof(double)); 
				}

				// Store masters points in master_points array
				for (i = 0; i < len ; i++)
				{
			        master_points[i][0] = points[i][0];
			        master_points[i][1] = points[i][1];

				}

				*num_pts = len;
			}
			
			else
			{

				// Send to other processes the number of points to be recieved
				MPI_Send(&len, 1, MPI_INT, j, TAG, MPI_COMM_WORLD);

				MPI_Send(points[start],len*2,MPI_DOUBLE, j, j, MPI_COMM_WORLD);

			}
			
		}

		return master_points;	

	}

	else
	{

		int clusters;
		int num_elems;

		//Receive number of clusters + init_centroids
		MPI_Recv(&clusters, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);

		for (i = 0; i < clusters; i++)
		{
         	// Recieve initial centroids
         	MPI_Recv(centroids[i], 2 , MPI_DOUBLE, 0, myRank, MPI_COMM_WORLD, &status);
		}

		// Recv number of data points from master
		MPI_Recv(&num_elems, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);

		*num_pts = num_elems;
		
		// Allocating array to store data points
		double** points = (double**)malloc(num_elems*sizeof(double*));
		double* darray = (double*)malloc(2*num_elems*sizeof(double));
	
		// Allocate 2 d array for each index in points (this is to store x, y)
		for (i = 0; i < num_elems; i++)
		{
         	points[i] = &darray[i*2];
		}
		
        MPI_Recv(points[0], 2*(*num_pts) , MPI_DOUBLE, 0, myRank, MPI_COMM_WORLD, &status);

		return points;

	}

}