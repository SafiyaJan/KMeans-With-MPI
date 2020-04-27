#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "io.h"
#include <math.h>
#include <getopt.h>
#include <mpi.h>

#define TAG 1


int main(int argc, char **argv)
{
	int myRank;   
	int num_procs; 
	int points_per_proc;
	
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if (myRank == 0)
	{
	
		if (points < num_procs)
		{
			fprintf(stderr, "Cannot have points less that number of procs, exiting\n");
			return 0;
		}

		double** points = readDataPoints("/DataGeneratorScripts/input/cluster.csv", 10);


		// If each proc can get equal number of elems
		if(num_pts % num_procs == 0)
		{
			points_per_proc = num_pts/num_procs;

			for(int j = 1; j < num_procs)
			{
				int start_index = j * points_per_proc;

				//Send num_elems to process
				MPI_Send(&points_per_proc, 1, MPI_Double, j, TAG, MPI_COMM_WORLD);
				
				//Send num_elems to process
				MPI_Send(points[start_index],points_per_proc*2, MPI_Double, j, j, MPI_COMM_WORLD);
			}


			double** master_points = (double**)malloc(points_per_proc * sizeof(double*));
			// Allocate 2d array for each index in points (this is to store x, y)
			for (int i = 0; i < points_per_proc ; i++)
			{
		        master_points[i] = (double*)calloc(2,sizeof(double)); 
			}

			for (int i = 0; i < points_per_proc ; i++)
			{
		        master_points[i][0] = points[i][0];
		        master_points[i][1] = points[i][1];

			}


			for(int i = 0; i < points_per_proc; i++)
			{
				printf("x - %lf, y - %lf\n", master_points[i][0], master_points[i][1]);
			}


		}

		else
		{
			// int quotient = (num_pts % num_procs);
			// points_per_proc = (num_pts-quotient)/num_procs;
			
			// for (int j = 1; j < num_procs; j++)
			// {				
			// 	int start_index = (j*points_per_proc)-quotient;
				
			// 	//Send num_elems to process
			// 	MPI_Send(&points_per_proc, 1, MPI_Double, j, TAG, MPI_COMM_WORLD);
				
			// 	//Send data points to process 
			// 	MPI_Send(points[startIndex], NUM_ELEM_PER_PROC*2, MPI_Double, j, j, MPI_COMM_WORLD); 				
			// }


			// double** master_points = (double**)malloc(points_per_proc * sizeof(double*));
			// // Allocate 2d array for each index in points (this is to store x, y)
			// for (int i = 0; i < points_per_proc ; i++)
			// {
		 //        master_points[i] = (double*)calloc(2,sizeof(double)); 
			// }

			// for (int i = 0; i < points_per_proc ; i++)
			// {
		 //        master_points[i][0] = points[i][0];
		 //        master_points[i][1] = points[i][1];

			// }


			// for(int i = 0; i < points_per_proc; i++)
			// {
			// 	printf("x - %lf, y - %lf\n", master_points[i][0], master_points[i][1]);
			// }

		}

	}

	else
	{
		int num_elems;
		
		MPI_Recv(&num_elems, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
		
		// Allocating array to store data points
		double** points = (double**)malloc(num_elems*sizeof(double*));
	
		// Allocate 2 day array for each index in points (this is to store x, y)
		for (int i = 0; i < num_elem ; i++)
		{
         	points[i] = (double*)calloc(2,sizeof(double)); 
		}


		MPI_Recv(points[0], num_elems*2 , MPI_DOUBLE, 0, rank, MPI_COMM_WORLD, &status);

		for(int i = 0; i< num_elems; i++)
		{
			printf("x - %lf, y - %lf\n", points[i][0], points[i][1]);
		}

	}

	MPI_Finalize();

}
