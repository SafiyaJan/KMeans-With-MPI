#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "io.h"
#include <math.h>
#include <getopt.h>


typedef struct 
{
	int A,C,T,G;
}	base_counter;

int dist_dna(char* p1, char* p2, int length)
{
	//Error checks
	if (p1 == NULL || p2 == NULL)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
	}

	int i = 0;
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
	double dist = dist_dna(point, centroid[0], length);

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
 * k_means(char* file_name, double** data_pts, int k, int num_pts)
 * calculates k cluster centroids given the number of clusters needed 
 * and total data points
 */
char** dna_k_means(char** data_pts, int k, int num_pts, int num_itr, int length)
{
	if(data_pts == NULL || k == 0 || num_pts == 0 )
	{
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}

	int i,j;

	//Allocating array to store centroids
	char** centroids =  (char**)malloc(k*sizeof(char*));
	for (i = 0; i < k ; i++)
	{
        centroids[i] = (char*)malloc(length*sizeof(char)); 
	}

	// Allocating array to store which cluster each strand belongs to
	int* which_cluster = (int*)malloc(num_pts*sizeof(int));
	for (i = 0; i < num_pts; i++)
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

	int rand_index = 0;
	for(i = 0; i < k; i++)
	{
		
        for (j = 0; j < length; j ++)
        {
        	centroids[i][j] = data_pts[rand_index][j];
        }   
        
        rand_index += (num_pts/k);

    }

    int itr = 0;
    
	double error = 1.0;

	// Clustering begins
	while(error > 0.0 && itr < num_itr)
	{
		
		double pointschanged = 0.0;
		
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
				pointschanged++;
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

			//pointsPerCluster[closest]++;

		}

		//Find new centroids
		for(i = 0; i < k; i++)
		{
			// For each index of strand, find the max occuring base and 
			// set index to that base
			for (j = 0; j < length; j++)
			{
				int max_count = 0;

				if ((base_count[i][j].A) > max_count)
				{
					max_count = base_count[i][j].A;
					centroids[i][j] = 'A';
				}

				if ((base_count[i][j].C) > max_count)
				{
					max_count = base_count[i][j].C;
					centroids[i][j] = 'C';
				}

				if ((base_count[i][j].T) > max_count)
				{
					max_count = base_count[i][j].T;
					centroids[i][j] = 'T';
				}

				if ((base_count[i][j].G) > max_count)
				{
					max_count = base_count[i][j].G;
					centroids[i][j] = 'G';
				}

			}

		}

		error = pointschanged/(double)num_pts;
		itr++;
		
	}

	printf("Number of iterations it took - %d & error - %f\n", itr, error);
		
	printf("Final Centroids - \n");
	for(i = 0; i < k; i++)
	{
		for(j=0; j < length-1; j++)
			printf("%c,", centroids[i][j]);
		 
		printf("%c\n", centroids[i][j++]);
	}
	
	//Write centroids to the file
	FILE* fp = fopen("DNA_seq_finalcentroids.csv", "w");

	// If file could not be opened for writing
	if (fp == NULL)
	{
		fprintf(stderr,"Error reading the file, returning NULL\n");
	}

	//Write data to file
	for(i = 0; i < k; i ++)
	{
		for(j = 0; j < length-1; j++)
		{
			
			fprintf(fp, "%c,", centroids[i][j]);
		}
		fprintf(fp,"%c,", centroids[i][j++]);
		//fprintf(fp,"%d\n", pointsPerCluster[i]);

	}

    return centroids;
}



int main(int argc, char *argv[])
{
	
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

	clock_t start_time = clock(); 

	//Read points from file
	printf("Reading points now...\n");
	char** points = read_DNAstrands(input_datafile, total_num_pts, length);

	printf("Doing K means now...\n");
	char** centroids = dna_k_means(points, num_clusters, total_num_pts, num_itr, length);
	clock_t end_time = clock();
    double time_taken = (double)(end_time-start_time)/CLOCKS_PER_SEC;

    printf("\nTime taken to complete - %lf\n",time_taken);

    int i;
    for (i = 0; i < num_clusters; i++)
	{
		free(centroids[i]);
	}

	free(centroids);
	free(points);


	return 0;

}
