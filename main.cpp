#include<bits/stdc++.h>
#include "Graph.h"
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
    MM1* mm1 = new MM1();
    Graph* G = new Graph(mm1); 

    // Form initial Graph/Network of INITIAL_PEERS peers

    for(int i=0;i<INITIAL_PEERS;i++)
    {
        // Initially assign random alpha and labda values to the peers
        double rAlpha = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
        double rLambda = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
        double rGamma = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
        Peer* p = new Peer(i, dist(rng), dist(rng), rAlpha, rLambda, rGamma);
        G->addPeer(p);
    }
    // G->printPeerInfo();
    auto start = chrono::steady_clock::now();
    G->clusterGraph(nClusters);
    auto end = chrono::steady_clock::now();
    cout<<"Time for clustering graph: ";
    cout<<chrono::duration <double, milli> (end - start).count()<<endl;
    
    // Code for assigning packet chunks to initial clusters
    G->assignPacketsToClusters(NUM_PACKETS);

//    G->printSubtrackerInfo();


    // Testing threads for peers
    start = chrono::steady_clock::now();
    G->startPeers();
    end = chrono::steady_clock::now();
    cout<<"Time for Simulation: ";
    cout<<chrono::duration <double, milli> (end - start).count()<<endl;


    G->printPeerQoEs();

    // for(auto it:G->peers)
    // {
    //     if(it->errors.size()>0)
    //     {
    //         cout<<it->ID<<" ";
    //     }
    // }


    

}