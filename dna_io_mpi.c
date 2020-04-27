#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "io.h"
#include <mpi.h>

#define TAG 1

//Reads dna strands from file and distributes the points across all processes
char** read_DNAstrands_mpi(char* filename, int total_num_pts, int rank, 
	char** centroids, int num_clusters, int num_procs, int* num_pts, int length)
{
	if(filename == NULL || total_num_pts == 0 || centroids == NULL || num_pts == NULL ||
		num_clusters == 0 || num_pts == NULL)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}

	MPI_Status status;
	
	int i,j,k;

	int myRank = rank;


	if (myRank == 0)
	{
		char** points = read_DNAstrands(filename, total_num_pts, length);
		char** master_points;

		int rand_index = 0;

		for(i = 0; i < num_clusters; i++)
		{
			
			//int rand_index = (rand() % (total_num_pts/num_clusters)) + i*(total_num_pts/num_clusters);

			for(j = 0; j < length; j++)
			{
				centroids[i][j] = points[rand_index][j];
			}

			// prev = rand_index;
			// int j = 0;
			// while(euclid_dist_read(points[prev],points[rand_index]) < 2)
			// {
			// 	rand_index++;
			// }

			rand_index += total_num_pts/num_clusters;
	        
	    }

	    printf("Distributing strands now...\n");

	    for(j = 1; j < num_procs; j++)
	    {
	    	//Send the number of centroids 
	    
	    	MPI_Send(&num_clusters, 1, MPI_INT, j, TAG, MPI_COMM_WORLD);
	    	
	    	// Send the centroids
	    	for(k = 0; k < num_clusters; k++)
			{
		
				MPI_Send(centroids[k],length,MPI_CHAR, j, j, MPI_COMM_WORLD);
			}

	    }

	    for(j = 0; j < num_procs; j++)
	    {
	    	// Len is number of points to send
			int len =  (total_num_pts * (j + 1) / num_procs) - (total_num_pts*j/num_procs);
			
			// Start is the index from where to send from 
			int start = total_num_pts*j/num_procs;

			if (j == 0)
			{
				// Allocating array to store data points
				master_points = (char**)malloc(len * sizeof(char*));
				
				// Allocate 2d array for each index in points (this is to store x, y)
				for (i = 0; i < len ; i++)
				{
			        master_points[i] = (char*)calloc(length,sizeof(char)); 
				}

				// Store masters points in master_points array
				for (i = 0; i < len ; i++)
				{
			        for(k = 0; k < length; k++)
			        {
			        	master_points[i][k] = points[i][k];
			        }

				}

				*num_pts = len;
			}

			else
			{
				// Send to other processes the number of points to be recieved
				
				MPI_Send(&len, 1, MPI_INT, j, TAG, MPI_COMM_WORLD);
				MPI_Send(points[start],len*length,MPI_CHAR, j, j, MPI_COMM_WORLD);
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
	         MPI_Recv(centroids[i], length , MPI_CHAR, 0, myRank, MPI_COMM_WORLD, &status);
		}

		// Recv number of data points from master
		MPI_Recv(&num_elems, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);

		*num_pts = num_elems;

		// Allocating array to store data points
		char** points = (char**)malloc(num_elems*sizeof(char*));
		char* darray = (char*)malloc(length*num_elems*sizeof(char));

		// Allocate 2 d array for each index in points (this is to store x, y)
		for (i = 0; i < num_elems; i++)
		{
	         points[i] = &darray[i*length];
		}
			
	    MPI_Recv(points[0], length*(*num_pts) , MPI_CHAR, 0, myRank, MPI_COMM_WORLD, &status);

		return points;
	}

}

