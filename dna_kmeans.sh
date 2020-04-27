#!/bin/sh
#Number of Points
t=10
#Number of Cluster
c=2
#Length of Strand
s=10
echo GENERATING $t INPUT POINTS EACH IN $c CLUSTERS
python ./DataGeneratorScripts/randomclustergen/datagenerator.py -k $c -p $t -l $s -o DataGeneratorScripts/input/dna_cluster.csv

