import sys
import csv
import numpy
import getopt
import math
import random

def usage():
    print '$> python data_generator.py <required args>\n' + \
        '\t-k <#>\t\tNumber of clusters to generate\n' + \
        '\t-p <#>\t\tNumber of DNA strands per cluster\n' + \
        '\t-l <#>\t\tLength of DNA strand\n' + \
        '\t-o <file>\tFilename for the output of the raw data\n'

def handleArgs(args):
    # set up return values
    numClusters = -1
    numPoints = -1
    len_strand = -1
    output = None


    try:
        optlist, args = getopt.getopt(args[1:], 'k:p:l:o:')
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

    for key, val in optlist:
        # first, the required arguments
        if   key == '-k':
            numClusters = int(val)
        elif key == '-p':
            numPoints = int(val)
        elif key == '-l':   
            len_strand = int(val)
        elif key == '-o':
            output = str(val)


    # check required arguments were inputted  
    if numClusters < 0 or numPoints < 0 or len_strand < 4 or output is None:
        print numClusters
        print numPoints
        print len_strand
        print output
        usage()
        sys.exit()
    return (numClusters, numPoints, output, len_strand)


def dna_similarity(p1, p2):
    '''
    Takes two DNA strands and computes the distance between them.
    '''
    #print p1
    #print p2
    diff_count = 0
    
    for i in range(len(p1)):

        if p1[i] != p2[i]:
            diff_count = diff_count + 1

    return len(p1) - diff_count


def dna_difference(p1, p2):

    diff_count = 0
    
    for i in range(len(p1)):

        if p1[i] != p2[i]:
            diff_count = diff_count + 1

    return diff_count


def generateCentroid(length):
    dna_centroid = ''
    
    for k in range(length):
        dna_centroid = dna_centroid + rand_base('ACGT')

    return dna_centroid

def tooClose(dna, strands, minDist):
    
    for strand in strands:
        if ((dna_similarity(strand,dna)) > minDist):
            return True

    return False;

def rand_base(string):
    return random.choice(string) 

# start by reading the command line
numClusters, numPoints, output, len_strand = handleArgs(sys.argv)

points_writer = csv.writer(open(output, "w"))
centroid_writer = csv.writer(open("centroids.csv", "w"))

# step 1: generate centroids
centroids = []
for i in range(numClusters):
    
    dna_centroid = generateCentroid(len_strand)
   
    while(tooClose(dna_centroid, centroids, math.ceil(0.2*len_strand))):
        dna_centroid = generateCentroid(len_strand);

    centroids.append(dna_centroid)
    centroid_writer.writerow(dna_centroid)

    print centroids[i]

# step 2: generate points 
points = []
for i in range(numClusters):
        
    for j in range(numPoints):
            
        dna_strand = generateCentroid(len_strand)

        while( dna_similarity(dna_strand,centroids[i]) < math.ceil(0.8*len_strand)):
            dna_strand = generateCentroid(len_strand);

        points_writer.writerow(dna_strand)



