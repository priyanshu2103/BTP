#include<bits/stdc++.h>
#include "cluster.h"
#include "Subtracker.h"

using namespace std;

#define THRESHOLD 10

class Graph 
{
    public:
        int nClusters;
        int nPeers;                     // Total number of peers in network
        int nPeersSince;                // Number of peers added to graph since last clustering
        vector<Subtracker*> subtrackers;        // IDs of subtrackers
        double ** distMatrix;           // Distance matrix of size nPeers*nPeers
        vector<Peer*> peers;            // List of peers
        unordered_map<int, Peer*> IDPeerMapping; // Used to return Peer from its ID
        unordered_map<int, Subtracker*> IDSubtrackerMapping; // Used to return subtracker from its ID
        // unordered_map<int, vector<Peer*>> subtrackerToPeer;  // Stores peers for each subtracker, mapping from ID of subtracker to peer IDs

        void addPeer(Peer*);            // Simply add peer in the graph 
        void assignSubtracker(Peer*);   // Assign a subtracker to this peer this is after initialisation and reclustering
        void clusterGraph(int);                 // Cluster the graph
        double computeDistance(Peer*, Peer*);   // Computes the distance between two peers 
        void computeDistMatrix();               // Compute distance matrix
        void assignPacketsToClusters(int);
        void printPeerInfo();
        void printSubtrackerInfo();      
        void startPeers();       
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
    
    // If nPeersSince reaches a THRESHOLD then call clusterGraph
    if(this->nPeersSince == THRESHOLD)
    {
        this->nClusters++;  
        clusterGraph(nClusters);  // Call with existing number of cluters + 1
    }
}

// Find the subtracker which has least distance from this peer and assign it
void Graph::assignSubtracker(Peer* peer)
{
    double min_distance = DBL_MAX;
    Subtracker* SID = NULL;
    for(int i=0;i<subtrackers.size();i++)
    {
        double dist = computeDistance(IDPeerMapping[subtrackers[i]->ID], peer);
        if(dist < min_distance)
        {
            SID = subtrackers[i];
            min_distance = dist;
        }
    }

    peer->subtracker = SID;
    (SID->peers).push_back(peer);
    // subtrackerToPeer[SID].push_back(peer);
}

void Graph::clusterGraph(int nClusters)
{
    this->nClusters = nClusters;
    computeDistMatrix();
    this->nPeersSince = 0;

    // Empty subtracekrToPeer mapping before clustering
    // subtrackerToPeer.clear();

    // Empty subtrackers vector as subtrackers will be created again and each peer will be a subtracker again
    subtrackers.clear();

    // Also empty the IDSubtrackermapping
    IDSubtrackerMapping.clear();

    int npass = 1000;
    // int* clusterid = (int*)malloc(nPeers*sizeof(int));
    // cout<<nPeers<<endl;
    int clusterid[nPeers] = {0};
    double error;
    int ifound = 0;
    kmedoids(nClusters, nPeers, distMatrix, npass, clusterid, &error, &ifound);
    printf("----- 1000 passes of the EM algorithm (result should not change)\n");
    printf ("Solution found %d times; within-cluster sum of distances is %f\n",ifound, error);
    printf ("Cluster assignments:\n");

    // set<int> tempS;
    for (int i = 0; i < nPeers; i++)
    {
        printf ("Peer %d: cluster %d\n", i, clusterid[i]);

        // If subtracker not formed yet with this ID  
        if(IDSubtrackerMapping.find(clusterid[i])==IDSubtrackerMapping.end())
        {
            Subtracker *s = new Subtracker();
            s->ID = clusterid[i];
            s->x = IDPeerMapping[clusterid[i]]->x;
            s->y = IDPeerMapping[clusterid[i]]->y;
            (s->peers).push_back(IDPeerMapping[i]);
            subtrackers.push_back(s);
            IDSubtrackerMapping[clusterid[i]] = s;
            IDPeerMapping[i]->subtracker = s;
        }
        else
        {
            (IDSubtrackerMapping[clusterid[i]]->peers).push_back(IDPeerMapping[i]);
            IDPeerMapping[i]->subtracker = IDSubtrackerMapping[clusterid[i]];
        }

        

        // IDPeerMapping[i]->subtrackerID = clusterid[i];
        // subtrackerToPeer[clusterid[i]].push_back(IDPeerMapping[i]);    // Add peer to subtracker mapping list
        // tempS.insert(clusterid[i]);
    }

    // // Stores subtracker IDs 
    // subtrackers.clear();
    // for(auto it=tempS.begin();it!=tempS.end();it++)
    // {
    //     this->subtrackers.push_back(*it);
    // }
    // // cout<<"A:"<<subtrackers.size()<<endl;


}

// TODO: change this to sqrt later on
double Graph::computeDistance(Peer* peer1, Peer* peer2)
{
    return pow(peer1->x - peer2->x, 2) + pow(peer1->y - peer2->y, 2);
}

void Graph::computeDistMatrix()
{
    if(distMatrix != NULL)
    {
        distMatrix = NULL;
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

void Graph::printPeerInfo()
{
    cout<<"----------------------PEER INFO----------------------"<<endl;
    for(int i=0;i<nPeers;i++)
    {
        cout<<peers[i]->ID<<" : ("<<peers[i]->x<<", "<<peers[i]->y<<")"<<endl;
    }
}

void Graph::printSubtrackerInfo()
{

    set<int> packetsInSubtrackcer;
    cout<<"subtracker ID:"<<"Peer IDs(packets)"<<endl;
    for(auto it=subtrackers.begin(); it!=subtrackers.end();it++)
    {
        packetsInSubtrackcer.clear();
        cout<<(*it)->ID<<": ";
        for(auto it1 = (*it)->peers.begin();it1 != (*it)->peers.end();it1++)
        {
            cout<<(*it1)->ID<<"(";
            for(auto it2=(*it1)->packets.begin();it2!=(*it1)->packets.end();it2++)
            {
                cout<<*it2<<",";
                packetsInSubtrackcer.insert(*it2);
            }
            cout<<")";
        }
        cout<<endl;
        cout<<"Packets in this cluster:"<<packetsInSubtrackcer.size()<<endl;
    }
}

void Graph::assignPacketsToClusters(int numPackets)
{
    // Create a temp set with all packets
    set<int> packets;
    for(int i=0;i<numPackets;i++)
    {
        packets.insert(i);
    }

    // Each clusters should have atleast one instance of each packet
    for(auto it=subtrackers.begin();it!=subtrackers.end();it++)
    {
        
        vector<Peer*> peers = (*it)->peers;
        
        // To store which all packets have been added to this cluster so that any remaining can be randomly added to peers later
        set<int> packetsRemaining = packets;

        // Iterate over all peers of this cluster
        for(auto it1=peers.begin();it1!=peers.end();it1++)
        {
            // For each peer assign packets by getting a random probablity for each packet and assigning it
            for(int i=0;i<numPackets;i++)
            {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                if(r>0.9)
                {
                    // *it1 peer gets packet i 
                    (*it1)->packets.insert(i);

                    // Add this packet to peer maping in Subtracker(*it) also
                    (*it)->packetToPeersMapping[i].push_back(*it1);
                    packetsRemaining.erase(i);
                }
            }
        }

        // Assign remaining packets
        if(packetsRemaining.size()!=0)
        {
            for(auto it2=packetsRemaining.begin();it2!=packetsRemaining.end();it2++)
            {
                int randNum = rand()%((*it)->peers.size());  // Add packet *it2 to this peer
                (*it)->peers[randNum]->packets.insert(*it2);

                    // Add this packet to peer maping in Subtracker(*it) also
                    (*it)->packetToPeersMapping[*it2].push_back((*it)->peers[randNum]);
            }
        }
    }
}

// TODO: Don't start threads for subtrackers there might be some problems
void Graph::startPeers()
{
    vector<thread> threads;
    for(auto it=peers.begin();it!=peers.end();it++)
    {
        threads.push_back(thread(&Peer::operate, *it));
    }
    for(auto& thread: threads){
        thread.join();
    }

    cout<<"Threading complete"<<endl;
}