#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * readDataPoints(char* filename, int num_pts)
 * reads data from filename and saves the data points in
 * 2D array of size num_pts
 */

double** readDataPoints_temp(char* filename, int num_pts)
{

	printf("Reading points now...\n");

	if(filename == NULL || num_pts == 0)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}
	

	// Creating a file pointer
	FILE* fp = fopen(filename, "r");
	
	// Checking if file exists
	if (fp == NULL)
	{
		fprintf(stderr,"Error reading the file, returning NULL\n");
		return NULL;
	}

	// Array that contains pointers to the location of points in 'array'
	double** points = (double**)malloc(num_pts*sizeof(double*));
	// ID Array that stores all points 
	double* array = (double*)malloc(num_pts*2*sizeof(double));
	
	
	int i = 0;
	
	// Read from input file and store data in allocated array
	for (i = 0; i<2*num_pts; i++)
	{
		
		//Check two values are present
		if (fscanf(fp, "%lf, %lf", &array[i], &array[i+1]) != 2)
		{
			// if not, probably EOF
			break;
		}

		i++;
	}

	// At each point array index, store starting address of point
	for(i = 0; i < num_pts; i++)
	{
	   points[i] = &array[i*2];
	}

	// Close file
	fclose(fp);
	return points;
}


/* 
 * writeDataPoints(char* filename, int num_pts)
 * writes data from centroids and saves the coordinates of the 
 * centroids in a csv file named 'filename'
 */
void writeDataPoints_temp(char* filename, double** centroids, int num_pts)
{
	
	if(filename == NULL || centroids == NULL || num_pts == 0)
	{
		fprintf(stderr, "Invalid arguments, returning\n");
		return;
	}

	// Open file for writing
	FILE* fp = fopen(filename, "w");

	// If file could not be opened for writing
	if (fp == NULL)
	{
		fprintf(stderr,"Error reading the file, returning NULL\n");
	}

	// Write data to file
	int i = 0;
	for (i = 0; i < num_pts; i++)
	{
		fprintf(fp, "%lf,", centroids[i][0]);
		fprintf(fp, "%lf", centroids[i][1]);
		fprintf(fp, "\n");
	}

	// Close file
	fclose(fp);

}
