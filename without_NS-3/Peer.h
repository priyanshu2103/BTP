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
        int ID;
        double x;
        double y;
        Subtracker* subtracker;  // will be same as medoid/centroid of the cluster in which this peer resides i.e. the subtracker.
        unordered_set<int> packets;
        vector<pair<int, int>> packetTime;  // PeerID from which this packet is taken and Time to download each packet. In nanoseconds
        queue<double> mm1Times;

        Peer(int, double, double);
        double computeDistance(Peer*, Peer*);
        void operate();
        double getQueueTime();
};
#endif