#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** read_DNAstrands(char* filename, int num_pts, int length)
{
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

	// Allocating array to store data points
	char** points = (char**)malloc(num_pts*sizeof(char*));
	char* array = (char*)malloc( ( num_pts*(length) ) *sizeof(char));

	//Reading points into array
	char temp[250];
	int i = 0;

	//Reading dna strands into array
	while(fgets(temp,250,fp)!=NULL && i < num_pts*(length))
	{
		char* tok = strtok(temp, ",\n");
		while(tok!=NULL)
		{
			if(!sscanf(tok, "%c", &array[i]))
			{
				fclose(fp);
				printf("Invalid DNA strand, found non-character, exiting\n");
				return NULL;
				
			}

			tok = strtok(NULL, ",\n");
			i++;
		}

		memset(temp,0,250);
	}

	for(i = 0; i < num_pts; i++)
	{
		 points[i] = &array[i*(length)];
	}



	return points;
}

