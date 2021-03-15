// #ifndef _PEER_GUARD
// #define _PEER_GUARD

#include<bits/stdc++.h>
// #include "Subtracker.h"
using namespace std;

class Subtracker;   // This will only work if we are pointer to Subtracker
class Peer 
{

    public:
        int ID;
        double x;
        double y;
        Subtracker* subtracker;  // will be same as medoid/centroid of the cluster in which this peer resides i.e. the subtracker.
        vector<int> packets;
        Peer(int, double, double);

        void operate();
};

Peer::Peer(int ID, double x, double y)
{
    this->ID = ID;
    this->x = x;
    this->y = y;
}

// Main function for each thread i.e peer
void Peer::operate()
{
    cout<<"Inside thread for peer:"<<this->ID<<endl;
}

// #endif