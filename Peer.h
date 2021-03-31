#ifndef _PEER_GUARD
#define _PEER_GUARD

#include<bits/stdc++.h>
#include "constants.h"
#include "Subtracker.h"

using namespace std;

class Subtracker;   // This will only work if we have a pointer to Subtracker
class Peer 
{

    public:
        mutex p;
        condition_variable cv;
        bool ready;
        int ID;
        double x;
        double y;
        Subtracker* subtracker;  // will be same as medoid/centroid of the cluster in which this peer resides i.e. the subtracker.
        unordered_set<int> packets;
        vector<pair<int, int>> packetTime;  // PeerID from which this packet is taken and Time to download each packet. In nanoseconds
        queue<double> mm1Times;
        
        // Maximize this
        double QoE;                         // Given by 1/(mean+standard deviation) mean and standard deviation are of packet times
        double alpha;                       // RTT parameter
        double lambda;                      // Centdian calculation parameter
        double centdianScore;
        double bestQoE;                     // Stores the best QoE so far of this peer
        double bestAlpha;                   // Stores the alpha corresponding to the best QoE
        double bestLambda;                  // Stores the lambda corresponding to the best QoE
        vector<int> errors;

        // Storing only for Peers which send data to this peer ------DOUBT
        unordered_map<int, double> PeerRTT;   // (1 − α) ∗ received RTT + α ∗ old RTT 

        Peer(int, double, double, double, double);
        double computeDistance(Peer*, Peer*);
        void operate();
        double getQueueTime();
        void changePeerRTT(int, double);  // PeerRTT[PeerID] = (1 − α) ∗ double + α ∗ old RTT 
        void getPeerCentdian();
        bool ifReady() const;
};
#endif