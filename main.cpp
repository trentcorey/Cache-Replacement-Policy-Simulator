#include <stdio.h>
#include <iostream>
#include <time.h>
#include <sys/types.h>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <map>
#include "provided_code/splay.h"
#include <chrono>
#include "freelist.h"
#include <vector>
#include <queue>
#include "event.h"
using namespace std;
//using clock = chrono::system_clock;
using ms = std::chrono::duration<double, std::milli>;

int numFiles; // Number of files in internet
int cacheCap; // Cache capacity
long numRequests; // Current number of requests
long totRequests; // Total number of requests to be completed
long numLoops;
long long requestsInService;
double totTime; // Total amount of time to be simulated
int completedReqs; // Total number of completed requests
const gsl_rng *RandGen; // Rand num generator
unsigned int RandomSeed;   // random seed number
double Pareto_A; // Pareto Alpha value used for rand num gen
double Pareto_K; // Pareto K val used for rand num gen
double currTime;
Event *pqRoot; // Root of event priority queue / splay tree
vector<Packet> packets; // All files (represented as packets)
int instBandwidth; // Instiutional bandwidth (Rc) = 1,000 Mbps
int accBandwidth; // Access link bandwidth (Ra) = 15 Mbps
double delayMean, delayStdDev; // Mean and standard deviation for the internet delay
double expLambda; //Lambda parameter for exponential sample
queue<Event*> fifoQueue; // FIFO queue files go to before cache
vector<float> responseTimes; // Total round trip times of filees from internet/cache to user
priority_queue<Packet, vector<Packet>, ComparePacketPopularity> pqCachePop; // Cache with popularity replacement policy
priority_queue<Packet, vector<Packet>, ComparePacketSize> pqCacheSize; // Cache with size replacement policy
priority_queue<Packet, vector<Packet>, CompareRecentlyRequested> pqCacheRecent; // Cache with least recently requested replacement policy
int cacheType;
map<int, bool> filesInCache; // filesInCache[packet.id] = true means packet is in cache
int numFilesInCache;

static void startup(string input_file);
static void loop();
static void newRequestEvent(Event* event);
static void fileReceivedEvent(Event *event);
static void arriveAtQueueEvent(Event *event);
static void departQueueEvent(Event* event);
static Packet* assignPacket();
Event* checkCache(Event* ev, Event* root);
Event* rotateRight(Event* ev);
Event* rotateLeft(Event* ev);
static void makePackets();

int main(int argc, char *argv[])
{

    // Startup with first command line arg being name of input file.
    startup(argv[1]);
    
    // Get start time
    cout << "Simulation starting..." << endl;
    time_t start = time(0);

    // Loop
    loop();

    // Get end time
    double duration = difftime(time(0), start);
    cout << "Simulation completed in " << duration << " second(s)." << endl;

    // End & display sim statistics
    double averageRequestsInService = requestsInService / numLoops;
    cout << "Average requests in service: "  << averageRequestsInService  << endl;
    float totalResponseTimes = 0;
    for(int i = 0; i < responseTimes.size(); i++)
    {
        totalResponseTimes += responseTimes[i];
    }
    cout << "Average response time: " << totalResponseTimes / responseTimes.size() << endl;
    cout << "Total simulated time: " << currTime << endl;

    return 0;
}

static void startup(string input_file)
{
    // Init cache, priority queue, random nums
    ifstream file(input_file);
    string str;
    numFilesInCache = 0;
    numLoops = 0;
    requestsInService = 0;
    gsl_rng_default_seed = RandomSeed;
    RandGen = gsl_rng_alloc (gsl_rng_default);

    // Read in from input file. First line should be numFiles, second is cache capacity, and so on.
    getline(file,str);
    numFiles = stoi(str);
    getline(file, str);
    cacheCap = stoi(str);
    getline(file, str);
    Pareto_A = stod(str);
    getline(file, str);
    Pareto_K = stod(str);
    getline(file, str);
    delayMean = stod(str);
    getline(file, str);
    delayStdDev = stod(str);
    getline(file, str);
    expLambda = stod(str);
    getline(file, str);
    if(str == "pop\r"){
        cacheType = 1;
    }
    else if (str == "size\r"){
        cacheType = 2;
    }
    else if (str == "recent\r"){
        cacheType = 3;
    }
    else {
        cout << "Improper input file. Replacement policy must be 'size', 'pop', or 'recent'" << endl;
        exit(1);
    }
    getline(file, str);
    RandomSeed = stoi(str);
    getline(file, str);
    totRequests = stoi(str);
    getline(file, str);
    totTime = stoi(str);
    getline(file, str);
    instBandwidth = stoi(str);
    getline(file, str);
    accBandwidth = stoi(str);

    cout << "Number of files: " <<  numFiles << endl;
    cout << "Cache capacity: " << cacheCap << endl;
    cout << "Alpha Pareto value: " << Pareto_A << endl;
    cout << "K Pareto value: " << Pareto_K << endl;
    cout << "Average delay: " << delayMean << endl;
    cout << "Delay standard deviation: " << delayStdDev << endl;
    cout << "Exponential sample lambda value: " << expLambda << endl;
    cout << "Cache replacement policy: " << cacheType << endl;
    cout << "Random Seed: " << RandomSeed << endl;
    cout << "Request limit: " << totRequests << endl;
    cout << "Simulated time limit: " << totTime << endl;
    cout << "Institutional bandwidth: " << instBandwidth << endl;
    cout << "Access link bandwidth: " << accBandwidth << endl;

    // Initialize splay tree
    ((SplayTree*) pqRoot);

    // Make file packets
    makePackets();
}

static void loop()
{
    Event *ev = new Event;
    Packet *pack;
    currTime = 0;
    numRequests = 0;

    pack = assignPacket();
    pack->request_time = currTime;
    float tmp = 0;
    for (unsigned int i = 0; i < packets.size(); i++)
    {
        tmp +=packets[i].popularity;
    }
    ev->key = currTime;
    ev->packet = pack;
    ev->func = newRequestEvent;
    ev->pack_request_time = currTime;
    ev_enqueue(ev, &pqRoot);
    completedReqs = 0;

    while(completedReqs < totRequests && currTime < totTime)
    {
        ev = ev_dequeue(&pqRoot);
        currTime = ev->key;
        (*(ev->func))(ev);
        requestsInService += (numRequests - completedReqs);
        numLoops++;
    }
}

static void newRequestEvent(Event *event)
{
    numRequests++;
    Event *nextEvent = new Event;
    nextEvent->packet = event->packet;

    // Check if file i is in cache
    if(filesInCache[event->packet->id])
    {
        // If it is, make new file received event w/ time = curr + S/R
        nextEvent->key = currTime + (event->packet->size / instBandwidth);
        nextEvent->func = fileReceivedEvent;
        nextEvent->pack_request_time = event->pack_request_time;
    }
    else
    {
        // If not, make new arrive at queue event w time = curr + D
        nextEvent->key = currTime + gsl_ran_lognormal(RandGen, delayMean, delayStdDev);
        nextEvent->pack_request_time = currTime + gsl_ran_exponential(RandGen, 1.0/expLambda);
        nextEvent->func = arriveAtQueueEvent;
    }
    ev_enqueue(nextEvent, &pqRoot);
    Packet *newPacket = assignPacket();
    Event *newEvent = new Event;
    newEvent->pack_request_time = currTime + gsl_ran_exponential(RandGen, 1.0/expLambda);
    newEvent->packet = newPacket;
    newEvent->func = newRequestEvent;
    newEvent->key = newEvent->pack_request_time;
    ev_enqueue(newEvent, &pqRoot);
}

static void fileReceivedEvent(Event *event)
{
    // Find total round trip time
    float responseTime = currTime - event->pack_request_time;
    responseTimes.push_back(responseTime);
    completedReqs++;
}

static void arriveAtQueueEvent(Event *event)
{
    if(fifoQueue.size() == 0)
    {
        // If FIFO queue is empty, immediately depart it
        Event* nextEvent = new Event;
        nextEvent->func = departQueueEvent;
        nextEvent->packet = event->packet;
        nextEvent->pack_request_time = event->pack_request_time;
        nextEvent->key = currTime + (event->packet->size / accBandwidth);
        ev_enqueue(nextEvent, &pqRoot);
    }

    fifoQueue.push(event);
}

static void departQueueEvent(Event* event)
{
    float cacheSize = 0;
    map<int, bool>::iterator it;
    for(it=filesInCache.begin(); it != filesInCache.end(); it++)
    {
        if(it->second == true)
        {
            cacheSize += packets[it->first].size;
        }
    }

    if(cacheSize < cacheCap)
    {
        // Cache is not full so add file to cache
        if(cacheType == 1)
            pqCachePop.push(*event->packet);
        else if (cacheType == 2)
            pqCacheSize.push(*event->packet);
        else
            pqCacheRecent.push(*event->packet);
        numFilesInCache++;
        filesInCache[event->packet->id] = true;
    }
    else
    {
        // Remove node based on replacement policy and add file to cache
        if(cacheType == 1){
            filesInCache[pqCachePop.top().id] = false;
            pqCachePop.pop();
            pqCachePop.push(*event->packet);
        }
        else if (cacheType == 2)
        {
            filesInCache[pqCacheSize.top().id] = false;
            pqCacheSize.pop();
            pqCacheSize.push(*event->packet);
        }
        else
        {
            filesInCache[pqCacheRecent.top().id] = false;
            pqCacheRecent.pop();
            pqCacheRecent.push(*event->packet);
        }
        filesInCache[event->packet->id] = true;
    }
    
    Event* nextEvent = new Event;
    nextEvent->func = fileReceivedEvent;
    nextEvent->packet = event->packet;
    nextEvent->pack_request_time = event->pack_request_time;
    nextEvent->key = currTime + (event->packet->size / instBandwidth);
    ev_enqueue(nextEvent, &pqRoot);

    // If FIFO queue not empty, make new event to pop next file
    if(fifoQueue.size() > 0)
    {
        fifoQueue.pop();
        Event* newEvent = new Event;
        newEvent->func = departQueueEvent;
        fifoQueue.front();
        newEvent->packet = &packets[fifoQueue.front()->packet->id]; //SEGMENTATION FAULT
        newEvent->pack_request_time = fifoQueue.front()->pack_request_time;
        newEvent->key = currTime + (newEvent->packet->size / accBandwidth);
        ev_enqueue(newEvent, &pqRoot);
    }
}

static void makePackets()
{
    double totPop = 0;
    double q;
    float totSize = 0;
    float avgSize;

    for(unsigned int i = 0; i < numFiles; i++)
    {
        Packet p;
        q = gsl_ran_pareto(RandGen, Pareto_A, Pareto_K);
        totPop += q;
        p.id = i;
        p.size =  gsl_ran_pareto(RandGen, Pareto_A, Pareto_K);
        totSize += p.size;
        p.popularity = q;
        packets.push_back(p);
        filesInCache[i] = false;
    }

    for(unsigned int i = 0; i < numFiles; i++)
    {
        packets[i].popularity = packets[i].popularity / totPop;
    }
}

static Packet* assignPacket()
{
    // Generate random number between 0 and 1
    double magicNum = gsl_rng_uniform(RandGen);
    for(unsigned int i = 0; i < numFiles; i++)
    {
        // Subtract the probability of the next packet from that number
        magicNum -= packets[i].popularity;
        // If that number is now 0 or less, then return that packet
        if(magicNum <= 0)
            return &packets[i];
    }
    // This shouldn't ever get called hypothetically unless some weird rounding occurs
    // This also crashes if packets is empty
    return &packets.back();
}