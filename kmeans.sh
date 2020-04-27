#!/bin/sh
#Number of Points
b=8000000
#Number of Cluster
k=5
h=10000
echo GENERATING $b INPUT POINTS EACH IN $k CLUSTERS
python ./DataGeneratorScripts/randomclustergen/generaterawdata.py -c $k -p $b -o DataGeneratorScripts/input/cluster_40million.csv -v $h

