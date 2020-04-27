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

- To make 2D sequential, run command:
	```make 2d_kmeans_seq```

- To make 2D mpi, run command:
	```make 2d_kmeans_mpi```

- To make DNA sequential, run command:
	```make dna_seq```

- To make DNA mpi, run command:
	```make dna_mpi```

Point Generation
----------------

 - To generate 2D points, run command:
	```sh kmeans.sh```

 - To generage DNA strands, run command:
	```sh dna_kmeans.sh```


Output file names 
-----------------

- 2D seq centroids - "2D_seq_finalcentroid.csv"
- 2D mpi centroids - "2D_mpi_finalcentroid.csv"
- DNA seq centroids - "DNA_seq_finalcentroid.csv"
- DNA mpi centroids - "DNA_mpi_finalcentroid.csv"

Running program
---------------
The following commands runs K-Means on a dataset of 100 points, 3 clusters, data from pointsGenerated.txt for 5 iterations
```mpiexec -f <Machinefile > -n <Number_of_processes > ./your_executable -t 100 -c 3 -i points_generated.txt -n 5`



