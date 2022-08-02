README:

To compile using a Linux PC wit g++ compiler: g++ -o main main.cpp provided_code/splay.c -lgsl -g
To start after compiling: ./main in.txt

INPUT
To modify the input paramters, edit the in.txt file within the "Probability Project" file
The in.txt has the following parameters:
numFiles
cacheCap
Pareto_A
Pareto_K
delayMean
delayStdDev
expLambda
cacheType (either pop, size, or recent)
RandomSeed
totRequests
totTime
instBandwidth
accBandwidth

OUTPUT
The program will print the dureation of the simulation, the average requests in service, the average response time, and the total simulated time.