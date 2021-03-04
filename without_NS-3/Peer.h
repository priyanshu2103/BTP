#include<bits/stdc++.h>

using namespace std;

class Peer 
{

    public:
        int ID;
        double x;
        double y;
        int subtrackerID;  // will be same as medoid/centroid of the cluster in which this peer resides i.e. the subtracker.
        vector<int> packets;
        Peer(int, double, double);
};

Peer::Peer(int ID, double x, double y)
{
    this->ID = ID;
    this->x = x;
    this->y = y;
}