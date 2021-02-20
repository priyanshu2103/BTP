#include<bits/stdc++.h>
#include "ns3/Graph.h"

using namespace std;

#define INITIAL_PEERS 5
#define MIN_CORD 0
#define MAX_CORD 100.0


int main()
{

    // Random double number generators for x and y values for peers

    // Type of random number distribution
    std::uniform_real_distribution<double> dist(MIN_CORD, MAX_CORD);  //(min, max)
    //Mersenne Twister: Good quality random number generator
    std::mt19937 rng; 
    //Initialize with non-deterministic seeds
    rng.seed(std::random_device{}()); 

    // nClusters, will increase after each nPeersSince threshold
    int nClusters = 1;

    Graph* G = new Graph();

    // Form initial Graph/Network of INITIAL_PEERS peers

    for(int i=0;i<INITIAL_PEERS;i++)
    {
        Peer* p = new Peer(i, dist(rng), dist(rng));
        G->addPeer(p);
    }
    G->printPeerInfo();
    G->clusterGraph(nClusters);

}