#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "io.h"
#include <math.h>
#include <getopt.h>
#include <mpi.h>

#define ROOT 0
#define TAG 1

//Struct for keeping count of data 
typedef struct 
{
	int A,C,T,G;
}base_counter;

/* dist_dna - measures the difference between two dna strands */
int dist_dna(char* p1, char* p2, int length)
{
	//Error checks
	if (p1 == NULL || p2 == NULL)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
	}

	int i;
	int diff_count = 0;

	// Incrrement counter whenever indices don't match
	for (i = 0; i < length; i ++)
	{
		if (p1[i] != p2[i])
            diff_count++;
	} 

	return diff_count;
	
}

int closest_cluster(char* point, char** centroid, int k, int length)
{
	// Error checks 
	if (point == NULL || centroid == NULL)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
	}

	// Initially assuming first centroid is closest to point
	int min = 0;
	
	// Euclid distance between first strand and first centrid
	int dist = dist_dna(point, centroid[0], length);

	int i = 0;
	for (i = 1; i < k; i ++)
	{
		// Calculate difference between point to centroid
		int temp = dist_dna(point, centroid[i], length);
		
		// Check if this difference is the smaller than min
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
 * needed and total DNA strands. Computation is distributed across different
 * processes to quicken clustering time
 */
char** kmeans_mpi_dna(char* filename, char** data_pts, char** centroids, 
	int k, int num_pts, int num_itr, int rank, int num_procs, int total_pts, int length)
{

	int i,j;
	MPI_Status status;

	//Error checks
	if(filename == NULL || data_pts == NULL || centroids == NULL || num_pts == 0 
		|| num_itr <= 0 || k <=1 )
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

	//Allocating array of base_counters to store the count of bases in each centroids
	base_counter** base_count = (base_counter**)malloc(k*sizeof(base_counter*));
	for(i = 0; i < k; i++)
	{
		base_count[i] = (base_counter*)malloc(length*sizeof(base_counter));
	}

	double error =  1.0;
	int itr = 0;
	//Clustering begins
	while(error > 0.0 && itr < num_itr)
	{
		double partial_pointschanged = 0;
		double pointschanged = 0;

		for(i = 0; i < k; i++)
		{
			pointsPerCluster[i] = 0;
			
			for (j = 0; j < length; j++)
			{
				base_count[i][j].A = 0;
				base_count[i][j].C = 0;
				base_count[i][j].G = 0;
				base_count[i][j].T = 0;
			}
		}
		

		//Find the closest cluster to the DNA strand and increment appropriate counter 
		for(i = 0; i < num_pts; i++)
		{
			
			int closest = closest_cluster(data_pts[i], centroids, k, length);
			
		
			if (which_cluster[i] != closest)
			{
				which_cluster[i] = closest;
				partial_pointschanged++;
			}

			// Go through each strand for the the closest cluster and increment the count
			// for what ever base is in the data strand at that index
			for(j =  0; j < length; j++)
			{
				if (data_pts[i][j]=='A')
					base_count[closest][j].A++;

				if (data_pts[i][j]=='C')
					base_count[closest][j].C++;

				if (data_pts[i][j]=='T')
					base_count[closest][j].T++;

				 if (data_pts[i][j]=='G')
					base_count[closest][j].G++;
			}

			pointsPerCluster[closest]++;


		}


		for (i = 0; i < k; i++)
		{
			int temp = 0;
			MPI_Reduce(&(pointsPerCluster[i]), &temp, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
			if (rank == 0)
				pointsPerCluster[i] = temp;
		}



		//Find the new centroids
		for(i = 0; i < k; i++)
		{
			int Acount = 0;
			int Ccount = 0;
			int Tcount = 0;
			int Gcount = 0;
			
			// for each index of the dna strand, find gather the counts for each base
			// and set index j of the the stand to the highest occuring base
			for (j = 0; j < length; j++)
			{
				//Get all counts of each of the bases for index j
				MPI_Reduce(&(base_count[i][j].A), &Acount, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
				MPI_Reduce(&(base_count[i][j].C), &Ccount, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
				MPI_Reduce(&(base_count[i][j].T), &Tcount, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
				MPI_Reduce(&(base_count[i][j].G), &Gcount, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
	
				// If master
				if (rank == 0)
				{
					// Find the most occuring base and set the index of that 
					// centroid to the most occuring base
					int max_count = 0;

					if (Acount > max_count)
					{
						max_count = Acount;
						centroids[i][j] = 'A';
					}

					if (Ccount > max_count)
					{
						max_count = Ccount;
						centroids[i][j] = 'C';
					}

					if (Tcount > max_count)
					{
						max_count = Tcount;
						centroids[i][j] = 'T';
					}

					if (Gcount > max_count)
					{
						max_count = Gcount;
						centroids[i][j] = 'G';
					}

				}

			}


			//If master, send the newly calculated centroids
			if (rank == 0)
			{
				for (j = 1; j < num_procs; j++)
					MPI_Send(centroids[i], length, MPI_CHAR, j, TAG, MPI_COMM_WORLD);
			}
			// If slaves receive the new centroids
			else
			{
				MPI_Recv(centroids[i], length, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);

			}

		}

		// Master sums the number of oints that have changed to different clusters
		// and sends this number to all the slaves
		MPI_Allreduce(&partial_pointschanged, &pointschanged, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		
		//Each process calculate the the error and iterates once more
		error = pointschanged/(double)total_pts;
		itr++;

	}

	


	// If master 
	if (rank==0)
	{
		printf("Number of iterations it took - %d & error - %f\n", itr, error);
		
		printf("Final Centroids - \n");
		for(i = 0; i < k; i++)
		{
			for(j=0; j < length-1; j++)
				printf("%c,", centroids[i][j]);
			 
			printf("%c\n", centroids[i][j++]);
		}
		
		//Write centroids to the file
		FILE* fp = fopen(filename, "w");

		// If file could not be opened for writing
		if (fp == NULL)
		{
			fprintf(stderr,"Error reading the file, returning NULL\n");
		}


		for(i = 0; i < k; i ++)
		{
			for(j = 0; j < length-1; j++)
			{
				
				fprintf(fp, "%c,", centroids[i][j]);
			}
			fprintf(fp,"%c,", centroids[i][j++]);
			fprintf(fp,"%d\n", pointsPerCluster[i]);

		} 

	}


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
	int length;

	int num_itr = 1;
	char opt;

	// Parse the arguments and store them in local vars
	while (( opt = getopt (argc, argv, "c:t:i:l:n:")) != -1)
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

			case'l':
			{
				length = atoi(optarg);
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
	char** centroids = (char**)malloc(num_clusters * sizeof(char*));
	
	// Allocate 2d array for each index in points (this is to store x, y)
	for (i = 0; i < num_clusters ; i++)
	{
        centroids[i] = (char*)calloc(length,sizeof(char)); 
	}

	int* num_pts = (int*)calloc(1,sizeof(int));

	double start_time = MPI_Wtime();

	if (myRank == 0)printf("Distributing points now...\n");
	
	char** points = read_DNAstrands_mpi(input_datafile, total_num_pts, myRank, centroids, 
		num_clusters, num_procs, num_pts, length);

	MPI_Barrier(MPI_COMM_WORLD);
	
	if (myRank == 0)printf("Performing Kmeans now...\n");	
	
	kmeans_mpi_dna("DNA_mpi_finalcentroids.csv", points, centroids, num_clusters, *num_pts,
	 num_itr, myRank, num_procs, total_num_pts, length);

	MPI_Barrier(MPI_COMM_WORLD);

	if (myRank == ROOT) {
		double end_time = MPI_Wtime();
		printf("\nTime taken to complete - %lf\n",end_time-(start_time));

	}

	//Clean up - freeing all allocated memory
	for (i = 0; i < num_clusters; i++)
	{
		free(centroids[i]);
	}

	free(centroids);
	free(points);
	free(num_pts);


	return 0;


}
