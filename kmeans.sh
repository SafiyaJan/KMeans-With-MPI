#!/bin/sh
#Number of Points
b=200000
#Number of Cluster
k=5
h=1000
echo GENERATING $b INPUT POINTS EACH IN $k CLUSTERS
python ./DataGeneratorScripts/randomclustergen/generaterawdata.py -c $k -p $b -o DataGeneratorScripts/input/cluster_1million.csv -v $h

