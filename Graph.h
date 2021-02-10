#include<bits/stdc++.h>
#include "Peer.h"
#include "cluster.h"

using namespace std;

class Graph 
{
    public:
        int nClusters;
        int nPeers;                     // Total number of peers in network
        int nPeersSince;                // Number of peers added to graph since last clustering
        vector<int> subtrackers;        // IDs of subtrackers
        double ** distMatrix;
        vector<Peer*> peers;
        unordered_map<int, Peer*> IDPeerMapping; // Used to return Peer from its ID


        void addPeer(Peer*);            // Simply add peer in the graph 
        void assignSubtracker(Peer*);   // Assign a subtracker to this peer this is after initialisation and reclustering
        void clusterGraph(int);                 // Cluster the graph
        double computeDistance(Peer*, Peer*);   // Computes the distance between two peers 
        void computeDistMatrix();
        Graph();
};

Graph::Graph()
{
    this->nClusters = 0;
    this->nPeers = 0;
    this->nPeersSince = 0;
}

void Graph::addPeer(Peer* peer)
{
    this->peers.push_back(peer);
    this->nPeers++;
    this->nPeersSince++;
    this->IDPeerMapping[peer->ID] = peer;
}

// Find the suubtracker which has least distance from this peer and assign it
void Graph::assignSubtracker(Peer* peer)
{
    double min_distance = DBL_MAX;
    int SID = -1;
    for(int i=0;i<nClusters;i++)
    {
        double dist = computeDistance(IDPeerMapping[subtrackers[i]], peer);
        if(dist < min_distance)
        {
            SID = subtrackers[i];
            min_distance = dist;
        }
    }

    peer->subtrackerID = SID;
}

void Graph::clusterGraph(int nClusters)
{
    this->nClusters = nClusters;
    computeDistMatrix();
    this->nPeersSince = 0;


    int npass = 1000;
    int* clusterid = (int*)malloc(nPeers*sizeof(int));
    double error;
    int ifound = 0;
    kmedoids(nClusters, nPeers, distMatrix, npass, clusterid, &error, &ifound);
    printf("----- 1000 passes of the EM algorithm (result should not change)\n");
    printf ("Solution found %d times; within-cluster sum of distances is %f\n",ifound, error);
    printf ("Cluster assignments:\n");

    set<int> tempS;
    for (int i = 0; i < nPeers; i++)
    {
        printf ("Peer %d: cluster %d\n", i, clusterid[i]);
        IDPeerMapping[i]->subtrackerID = clusterid[i];
        tempS.insert(clusterid[i]);
    }

    if(this->subtrackers.size()!=0){this->subtrackers.empty();}
    for(auto it=tempS.begin();it!=tempS.end();it++)
    {
        this->subtrackers.push_back(*it);
    }


}

double Graph::computeDistance(Peer* peer1, Peer* peer2)
{
    return pow(peer1->x - peer2->x, 2) + pow(peer1->y - peer2->y, 2);
}

void Graph::computeDistMatrix()
{
    if(sizeof(distMatrix)!=0)
    {
        free(distMatrix);
    }

    distMatrix = (double**)malloc(sizeof(double*)*nPeers);
    for(int i=0;i<nPeers;i++)distMatrix[i] = (double*)malloc(sizeof(double)*nPeers);

    for(int i=0;i<nPeers;i++)
    {
        for(int j=0;j<nPeers;j++)
        {
            distMatrix[i][j] = computeDistance(IDPeerMapping[i], IDPeerMapping[j]);
        }
    }
}