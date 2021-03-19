#include<bits/stdc++.h>
#include "Graph.h"
// #include "constants.h"
using namespace std;




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
    int nClusters = INITIAL_CLUSTERS;

    Graph* G = new Graph();

    // Form initial Graph/Network of INITIAL_PEERS peers

    for(int i=0;i<INITIAL_PEERS;i++)
    {
        Peer* p = new Peer(i, dist(rng), dist(rng));
        G->addPeer(p);
    }
    G->printPeerInfo();
    G->clusterGraph(nClusters);
    
    // Code for assigning packet chunks to initial clusters
    G->assignPacketsToClusters(NUM_PACKETS);

    G->printSubtrackerInfo();


    // Testing threads for peers
    G->startPeers();


    

}
// TODO: might have to add new class for subtrackers for threading
// TODO:Code for adding peers to the graph at random time interval
