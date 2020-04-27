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
int dist_dna(char* p1, char* p2, int length)
{
	//Error checks
	if (p1 == NULL || p2 == NULL)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
	}

	int diff_count = 0;

	for (int i = 0; i < length; i ++)
	{
		if (p1[i] == p2[i])
            diff_count++;
	} 

	return length - diff_count;
	
}

int closest_cluster_dna(double* point, double** centroid, int k, int length)
{
	// Error checks 
	if (point == NULL || centroid == NULL || k == 0 || length < 4)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
	}

	// Initially assuming first centroid is closest to point
	int min = 0;

	// Similarity between points and first centrid
	double dist = euclid_dist(point, centroid[0]);
	// printf("This is init dist for point [%lf, %lf] to centroid [%lf, %lf] - %lf\n", 
	// 	point[0], point[1], centroid[0][0], centroid[0][1], dist);
	

	for (int i = 1; i < k; i ++)
	{
		// Calculate similarity from point to centroid
		double temp = euclid_dist(point, centroid[i]);
		// printf("This is calc dist for point [%lf, %lf] to centroid [%lf, %lf] - %lf\n", 
		// point[0], point[1], centroid[i][0], centroid[i][1], temp);
		
		// Check if this similarity value is the smaller than min
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

double** k_means_dna(char* filename, double** data_pts, int k, int length, int num_pts, int num_itr)
{

	if(filename == NULL || data_pts == NULL || k == 0 || num_pts == 0 || length < 4 )
	{
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}

	// Allocating array to store centroid
	char** centroids = (char**)malloc(k * sizeof(char*));

	// Allocate 2d array for each index in points (this is to store x, y)
	for (int i = 0; i < k ; i++)
	{
        centroids[i] = (char*)calloc(length,sizeof(double)); 
	}

	// Allocating array to store which cluster each point belongs to
	int* which_cluster = (int*)malloc(num_pts * sizeof(int));
	for (int i = 0; i < k ; i++)
	{
        which_cluster[i] = -1; 
	}

	// Select random centroids from data_pts and save in centroids
	srand(time(NULL));
	
	for(int i = 0; i < k; i++)
	{
        int rand_index = rand() % num_pts;

        for (int j = 0; j < length; j ++)
        {
        	centroids[i][j] = data_pts[rand_index][j];
        }
        
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
			int closest = closest_cluster_dna(data_pts[i], centroids, k, length);

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
				
				int a_count = 0;
				int c_count = 0;
				int g_count = 0;
				int t_count = 0;


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

}

