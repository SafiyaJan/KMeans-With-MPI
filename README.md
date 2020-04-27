# KMeans-With-MPI

Hello!

- Clustering allows a set of objects to be 'clustered' into certain groups. The members of these groupd have a high degreee of similarity with one another, whereas the groups themselves have a low degree of similarity between each other. K-Means is a clustering algorithm that generates k-similar groups in a given data set via minimizing a mean squared distance function.

- This repository contains code that performs the K-Means algorithm on 2D Data Points and DNA strands. The clustering is done in a both sequential and parallel manner in order to examine the efficiency between the two implementations. 

## Parallel Implementation - Using MPI
- For the parallel implementation, a Message Passing Interface (MPICH2) was used is it a high performance and widely
portable implementation of the MPI standard.
- The clustering of the data was done over 4 Virtual Machines with different number of processes running on the machines


Compilation
-----------
- To make 2D sequential, run command: ```make 2d_kmeans_seq```
- To make 2D mpi, run command: ```make 2d_kmeans_mpi```
- To make DNA sequential, run command: ```make dna_seq```
- To make DNA mpi, run command: ```make dna_mpi```

Point Generation
----------------
- 2D Points Generator
	- To generate 2D points, run command: ```sh kmeans.sh```
	- You can alter the number of points & number of clusters you would like to generate, by changing the ```kmeans.sh``` file
- DNA Strands Generator
	- To generage DNA strands, run command: ```sh dna_kmeans.sh```
	- You can alter the number of points, number of clusters and the length of the DNA strands you would like to generate, by changing the ```dna_kmeans.sh``` file


Output file names 
-----------------
- 2D seq centroids - "2D_seq_finalcentroid.csv"
- 2D mpi centroids - "2D_mpi_finalcentroid.csv"
- DNA seq centroids - "DNA_seq_finalcentroid.csv"
- DNA mpi centroids - "DNA_mpi_finalcentroid.csv"

Running program
---------------
- You will need to have access to 4 Virtual Machines in order to run the parallel implementation
- The following commands runs K-Means on a dataset of 100 points, 3 clusters, data from pointsGenerated.txt for 5 iterations
```mpiexec -f <Machinefile > -n <Number_of_processes > ./your_executable -t 100 -c 3 -i points_generated.txt -n 5```

Analysis
---------------
- Various scalability studies were conducted on the sequential and parallel implementation of K-Means and the analysis for the studies is detailed in the ```Analysis.pdf``` document.

