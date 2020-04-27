#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "io.h"
#include <math.h>
#include <getopt.h>

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
	// printf("This is init dist for point [%lf, %lf] to centroid [%lf, %lf] - %lf\n", 
	// 	point[0], point[1], centroid[0][0], centroid[0][1], dist);
	

	for (int i = 1; i < k; i ++)
	{
		// Calculate euclid dist from point to centroid
		
		double temp = euclid_dist(point, centroid[i]);
		// printf("This is calc dist for point [%lf, %lf] to centroid [%lf, %lf] - %lf\n", 
		// point[0], point[1], centroid[i][0], centroid[i][1], temp);
		
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





void printpoints(double** points, int k)
{
	for (int i = 0; i < k; i++)
	{
		printf("\n");
		
		for (int j = 0; j < 2; j ++)
		{
			printf("%lf\t", points[i][j]);

		}

	}
	printf("\n");

}

/* 
 * k_means(char* file_name, double** data_pts, int k, int num_pts)
 * calculates k cluster centroids given the number of clusters needed 
 * and total data points
 */
double** k_means(char* filename, double** data_pts, int k, int num_pts, int num_itr)
{
	
	if(filename == NULL || data_pts == NULL || k == 0 || num_pts == 0)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}

	// Allocating array to store centroid
	double** centroids = (double**)malloc(k * sizeof(double*));
	
	// Allocate 2d array for each index in points (this is to store x, y)
	for (int i = 0; i < k ; i++)
	{
        centroids[i] = (double*)malloc(sizeof(double)); 
	}

	
	// Allocating array to store which cluster each point belongs to
	int* which_cluster = (int*)malloc(num_pts * sizeof(int));
	for (int i = 0; i < num_pts ; i++)
	{
        which_cluster[i] = -1; 
	}

	int i;

	int centroid_index = 0;

	srand(time(NULL));
	
	for(i = 0; i < k; i++)
	{
		int rand_index = (rand() % (num_pts/k)) + i*(num_pts/k);
		printf("centroid_index - %d\n", rand_index);

		centroids[i][0] = data_pts[rand_index][0];
		centroids[i][1] = data_pts[rand_index][1];
        
    }

   
	int itr = 0;
	
	double error_threshold = 0.0;
	double error = 1 + error_threshold;

	while(error > error_threshold || itr < num_itr)
	{
		
		// To check how many points have changed to different clusters
		int pointschanged = 0;

		for (int i = 0; i < num_pts; i++)
		{
			// Finding the closest cluster that point belonngs to
			int closest = closest_cluster(data_pts[i], centroids, k);

			// Check if point is closer to a different cluster
			if(which_cluster[i] != closest)
			{
			
				which_cluster[i] = closest;
				pointschanged++;
			}
			

		}

		// Calculating new centroids
		for (int i = 0; i < k; i++)
		{
			double x = 0.0;
			double y = 0.0;
			int count = 0;

			//printf("This is index %d\n", i );

			for (int j = 0; j < num_pts; j++)
			{
				/* Check if we're summing correct x & y values
				 * Eg - if i = 0, sum x & y values of points 
				 * that belongs to cluster 0 */
				
				if(which_cluster[j] == i)
				{
					//printf("which_cluster[j] %d\n", which_cluster[j]);
					// sum all values of x & y for each cluster
					x += data_pts[j][0];
					y += data_pts[j][1];
					count ++;
				}

			}

			//Centroid stays the same
			if (count == 0)
			{
				continue;
			}
		
			// Average of a x and y coordinates are the new x,y coords 
			// of the new centroid
			else
			{
				centroids[i][0] = x/((double)count);
				centroids[i][1] = y/((double)count);
			}

		}

		//Calculate error by checking num of points changed vs total num_pts
		error = pointschanged/((double)num_pts);
		
		itr++;
		
	}

	printf("Number of iterations it took - %d & error - %f\n", itr, error);

	printf("Altered centroids");

	printpoints(centroids, k);
	
	FILE* fp = fopen(filename, "w");

	// If file could not be opened for writing
	if (fp == NULL)
	{
		fprintf(stderr,"Error reading the file, returning NULL\n");
	}


	for(int i = 0; i < k; i ++)
	{
		// Write to file the final cluster centroids
		fprintf(fp, "%lf,", centroids[i][0]);
		fprintf(fp, "%lf,", centroids[i][1]);
		
		int count = 0;
		
		// Count up number of points per cluster
		for (int j = 0; j < num_pts; j ++)
		{
			if (which_cluster[j] == i)
				count++;
		}

		//Write to the file the number of points per cluster
		fprintf(fp, "%d\n", count);

	} 

	// Free array 
	free(which_cluster);

	return centroids;

}




int main(int argc, char *argv[])
{
	//Ensure atleast 3 args are present
	if (argc < 3)
	{
		printf("Too few arguments, exiting");
		return 0;
	}

	int num_clusters = 0 ;
	int num_pts = 0 ;
	char* input_datafile = "";

	int num_itr = 0 ;
	char opt;

	// Parse the arguments and store them in local vars
	while (( opt = getopt (argc, argv, "c:t:i:n::")) != -1)
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
				num_pts = atoi(optarg);
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

	// Read data points from file
	printf("Reading points\n");
	double** points = readDataPoints(input_datafile, num_pts);

	printf("Doing k means\n");
	clock_t  start = clock();
	//Apply kmeans on the data & saving final centroids in file
	double** final_centroids =  k_means("2d_seq_finalcentroids.csv", points, num_clusters, num_pts, num_itr);
	clock_t end = clock();

	double time_taken = (double)(end-start)/CLOCKS_PER_SEC;
	printf("time taken - %lf\n",time_taken);
	
	//CLEAN UP - free all mememory allocations
	for (int i = 0; i < num_pts; i++)
	{
		free(points[i]);

	}

	for (int i = 0; i < num_clusters; i++)
	{
		free(final_centroids[i]);

	}

	free(final_centroids);
	free(points);



	return 0;

}


