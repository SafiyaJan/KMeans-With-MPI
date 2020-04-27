2d_kmeans_seq: points_seq.c 2d_io.c
			   clear
			   gcc -Wall -lm points_seq.c 2d_io.c -o 2d_kmeans_seq 

2d_kmeans_mpi: points_par.c 2d_io.c 2d_io_mpi.c
			   clear
			   mpicc -Wall -lm points_par.c 2d_io.c 2d_io_mpi.c 2d_mpi_io_temp.c 2d_io_temp.c -o 2d_kmeans_mpi
			   for i in 2 3 4; do scp 2d_kmeans_mpi hadoop@sjan-n0$$i.qatar.cmu.edu:/home/hadoop/P3;done 

dna_seq: dna_io.c dna_seq.c
		 clear
		 gcc -Wall -lm dna_io.c dna_seq.c -o dna_seq

dna_mpi: dna_io.c dna_io_mpi.c dna_par.c
		 clear
		 mpicc -Wall -lm dna_io.c dna_io_mpi.c dna_par.c -o dna_mpi
		 for i in 2 3 4; do scp dna_mpi hadoop@sjan-n0$$i.qatar.cmu.edu:/home/hadoop/P3;done 