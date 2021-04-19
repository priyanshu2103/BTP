#ifndef _GRAPH_GUARD
#define _GRAPH_GUARD

#include<bits/stdc++.h>

#include <random>
#include "cluster.h"
#include "Subtracker.h"
#include "MM1.h"

using namespace std;


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
        MM1* mm1Queue;
        unordered_map<Peer*, vector<int>> initialPacketAssignment;  // Stores peer to packet assignment for use in iterations
        double globalBestAlpha;
        double globalBestLambda;
        double globalBestGamma;
        unordered_map<Peer*, pair<double, double>> velocitiesParams;    // Peer->{v1, v2} v1->velocity for alpha dna v2->veloity for lambda
        unordered_map<Peer*, double> velocityGamma;
        double c1;
        double c2;
        double w;
        unordered_map<Peer*, vector<double>> peerQoEs;
        vector<double> Qas; // Stores the Qa for each iteration
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
        void peerPacketTimes();   
        void getBestParams();
        void changeDPSOParams(int);
        void getPeerQoE();
        void printPeerQoEs();
        bool checkPeerSub(Peer*);     // checks if Peer is a subtracker or not if yes then don't make a thread for it
        Graph(MM1*);
};

Graph::Graph(MM1* q)
{
    this->nClusters = 0;
    this->nPeers = 0;
    this->nPeersSince = 0;
    this->mm1Queue = q;
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
}

void Graph::clusterGraph(int nClusters)
{
    this->nClusters = nClusters;
    computeDistMatrix();
    this->nPeersSince = 0;

    // Empty subtrackers vector as subtrackers will be created again and each peer will be a subtracker again
    subtrackers.clear();

    // Also empty the IDSubtrackermapping
    IDSubtrackerMapping.clear();

    int npass = KMEDOIDS_NUM_PASS;
    int clusterid[nPeers] = {0};
    double error;
    int ifound = 0;
    //kmedoids(nClusters, nPeers, distMatrix, npass, clusterid, &error, &ifound);
    //printf("----- 1000 passes of the EM algorithm (result should not change)\n");
    //printf ("Solution found %d times; within-cluster sum of distances is %f\n",ifound, error);

    //---------------------------------------------------------
    vector<int> tempS;
    for(int i=0;i<nPeers;i++){tempS.push_back(i);}
    shuffle(begin(tempS), end(tempS), std::mt19937(std::random_device()()));
    vector<int> s;
    for(int i=0;i<nClusters;i++){s.push_back(tempS[i]);}
    for(int i=0;i<nPeers;i++)
    {
        clusterid[i] = s[i%nClusters];
    }
    //---------------------------------------------------------

    for (int i = 0; i < nPeers; i++)
    {
//        printf ("Peer %d: cluster %d\n", i, clusterid[i]);

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
    }

}

double Graph::computeDistance(Peer* peer1, Peer* peer2)
{
    return sqrt(pow(peer1->x - peer2->x, 2) + pow(peer1->y - peer2->y, 2));
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
    cout<<"subtracker, QoE, alpha, lambda,gamma, bestQoE, bestalpha, bestgamma, bestlambda"<<endl;
    for(int i=0;i<nPeers;i++)
    {
        cout<<peers[i]->ID<<" : ("<<peers[i]->subtracker->ID<<", "<<peers[i]->QoE<<", "<<peers[i]->alpha<<", "<<peers[i]->lambda<<", "<<peers[i]->gamma<<", "<<peers[i]->bestQoE<<", "<<peers[i]->bestAlpha<<", "<<peers[i]->bestLambda<<peers[i]->bestGamma<<")"<<endl;
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

    // Store initial packet assignment
    for(auto it=peers.begin();it!=peers.end();it++)
    {
        initialPacketAssignment[*it].assign((*it)->packets.begin(),(*it)->packets.end());
    }
}

void Graph::startPeers()
{
    // int iter = 0;
    // First get queue times from MM1
    queue<double> times = mm1Queue->getTime();

    for(int iter=0;iter<MAX_ITER;iter++)
    {
        
        // Chaning the initial variables for all peers
        for(auto it:peers)
        {
            queue<double>().swap(it->mm1Times);
            it->mm1Times = times;
            it->packets.clear();
            copy(initialPacketAssignment[it].begin(), initialPacketAssignment[it].end(), inserter(it->packets, it->packets.end()));
        }
        vector<thread> threads;
        for(auto it=peers.begin();it!=peers.end();it++)
        {
            threads.push_back(thread(&Peer::operate, *it));
        }
        for(auto& thread: threads){
            thread.join();
        }
        cout<<"iteration " << iter <<" complete"<<endl;
        getPeerQoE();
        
        /*
            We have QoE, alphas, lambdas of all the peers at this moment
            Basically two things to be done after each iteration
                Optimise alpha and lambda for each peer
                Change the subtracker for each cluster
        */
        // First get global alpha and lambda
        getBestParams();

        // Change DPSO params best computing velocity params
        changeDPSOParams(iter);
        // Change the vis' for each peer
        for(auto it:peers)
        {
                double randNum = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
                double randNum2 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);   
                double va = w * velocitiesParams[it].first + c1 * randNum * (it->bestAlpha - it->alpha) + c2 * randNum2 * (globalBestAlpha - it->alpha);
                double vl = w * velocitiesParams[it].second + c2 * randNum * (it->bestLambda - it->lambda) + c2 * randNum2 * (globalBestLambda - it->lambda);
                double vg = w * velocityGamma[it] + c2 * randNum * (it->bestGamma - it->gamma) + c2 * randNum2 * (globalBestGamma - it->gamma);
                velocitiesParams[it] = {va, vl};
                velocityGamma[it] = vg;
        }

        // Change alpha and lambda for each peer
        for(auto it:peers)
        {
            it->alpha += velocitiesParams[it].first;
            it->lambda += velocitiesParams[it].second;
            it->gamma = velocityGamma[it];
        }

        // Subtracker management algorithm
        vector<Subtracker*> newSubtrackers;
        for(auto it:subtrackers)
        {
            // For each subtracker get best peer with least centdian
            Peer* bestPeer = NULL;
            double bestScore = DBL_MAX;
            for(auto it1:it->peers)
            {
                if(bestScore > it1->centdianScore)
                {
                    bestScore = it1->centdianScore;
                    bestPeer = it1;
                }
            }
            // Best Peer will be the new subtracker for this cluster
            Subtracker* s = new Subtracker();
            s->ID = bestPeer->ID;
            s->x = bestPeer->x;
            s->y = bestPeer->y;
            s->peers = it->peers;   
            s->packetToPeersMapping = it->packetToPeersMapping; 
            newSubtrackers.push_back(s);

            // Now change the subtracker for all the peers in this cluster i.e. with it as their subtracker
            for(auto it1:it->peers)
            {
                it1->subtracker = s;
            }
        }

        subtrackers.clear();
        subtrackers = newSubtrackers;
    }
    


}

void Graph::getBestParams()
{
    double q = DBL_MIN;
    for(auto it:peers)
    {
        if(it->bestQoE >= q)
        {
            globalBestAlpha = it->bestAlpha;
            globalBestLambda = it->bestLambda;
            globalBestGamma = it->bestGamma;
            
        }
    }
}

void Graph::changeDPSOParams(int iter)
{
    w = 1.3 * exp(-1 * (iter * M_PI)/(2 * MAX_ITER)) + 0.1;
    c1 = 2.5 * exp(-1 * (iter * M_PI)/(2 * MAX_ITER));
    c2 = -2.5 * exp(-1 * (iter * M_PI)/(2 * MAX_ITER)) + 3;
}

bool Graph::checkPeerSub(Peer *peer)
{
    for(auto it:subtrackers)
    {
        if(it->ID == peer->ID)
        {
            return true;
        }
    }
    return false;
}
void Graph::peerPacketTimes()
{
    for(auto it:peers)
    {
        cout<<it->ID<<":"<<endl;
        for(auto it1:it->packetTime)
        {
            cout<<"{"<<it1.first<<" "<<it1.second<<"} ";
        }
        cout<<endl;
    }
}

void Graph::getPeerQoE()
{
    double Qa;
    double d = 0.0;
    for(auto it:peers)
    {
         d += (double )1-it->QoE;
        peerQoEs[it].push_back(it->QoE);
    }
     for(auto it:peers)
     {
         Qa += ((1-it->QoE) * it->QoE ) / d;

     }
     Qas.push_back(Qa);
}

void Graph::printPeerQoEs()
{
    vector<double> averageQoe(MAX_ITER, 0.0);
    for(auto it:peerQoEs)
    {
        cout<<it.first->ID<<": ";
        int i=0;
        for(auto it1:it.second)
        {
            averageQoe[i] += it1;
            i++;
            cout<<it1<<" ";
        }
        cout<<endl;
    }
    cout<<"Average QoEs:"<<endl;
    for(auto it:averageQoe)
    {
        cout<<it<<" ";
    }
    cout<<endl;

    cout<<"Qas"<<endl;
    for(auto it:Qas)
    {
        cout<<it<<" ";
    }
    cout<<endl;


}
#endif
