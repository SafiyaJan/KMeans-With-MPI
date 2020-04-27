#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * readDataPoints(char* filename, int num_pts)
 * reads data from filename and saves the data points in
 * 2D array of size num_pts
 */

double** readDataPoints(char* filename, int num_pts)
{

	if(filename == NULL || num_pts == 0)
	{
		printf("ERROR HERE\n");
		fprintf(stderr, "Invalid arguments, returning\n");
		return NULL;
	}
	

	// Creating a file pointer
	FILE* fp = fopen(filename, "r");
	printf("FILENAME - %s\n",filename );
	
	// Checking if file exists
	if (fp == NULL)
	{
		fprintf(stderr,"Error reading the file, returning NULL\n");
		return NULL;
	}

	// Allocating array to store data points
	double** points = (double**)malloc(num_pts*sizeof(double*));
	
	// Allocate 2 day array for each index in points (this is to store x, y)
	int i = 0;
	for (i = 0; i < num_pts ; i++)
	{
         points[i] = (double*)calloc(2,sizeof(double)); 
	}

	
	// Read from input file and store data in allocated array
	for (i = 0; i<num_pts; i++)
	{
		
		//Check two values are present
		if (fscanf(fp, "%lf, %lf", &points[i][0], &points[i][1]) != 2)
		{
			// if not, probably EOF
			break;
		}
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
void writeDataPoints(char* filename, double** centroids, int num_pts)
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
