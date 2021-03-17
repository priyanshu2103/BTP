// #ifndef _PEER_GUARD
// #define _PEER_GUARD

#include<bits/stdc++.h>
// #include "Subtracker.h"
using namespace std;

class Subtracker;   // This will only work if we have a pointer to Subtracker
class Peer 
{

    public:
        int ID;
        double x;
        double y;
        Subtracker* subtracker;  // will be same as medoid/centroid of the cluster in which this peer resides i.e. the subtracker.
        unordered_set<int> packets;
        vector<int> packetTime;  // Time to download each packet. In nanoseconds

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

    /* 
        Determine which packet I want
        Go to subtracker and get peer which is closest to me and has this packet --> RTT 
        Wait for the packet to arrive                                            --> M/M/1 + RTT
        notify the subracker that I have this packet                             --> 1/2 * RTT
    */  
   /*
        Check for the above time calculations 
        Factors of RTT and How to incorporate M/M/1
   */

    // Distance between this peer and the subtracker
    double distanceFromSubtracker = sqrt(pow((x - subtracker->x), 2) + pow((y - subtracker->y), 2));


    // change this 10 to a variable by adding a constants file
    for(int i=0;i<10;i++)
    {
        if(packets.find(i)!=packets.end())
        {
            // Packet already with peer do nothing
            packetTime[i] = 0;
        }
        else
        {
            // Wait for RTT/2
            // TODO: may have to change speed of transmission from 3*10^8 to something else
            this_thread::sleep_for(chrono::nanoseconds((int)(distanceFromSubtracker*10/6)));    // Add this time
            Peer* p = subtracker->getPeer(i, this);
            // Again wait for RTT/2 as subtracker takes time to return the peer
            this_thread::sleep_for(chrono::nanoseconds((int)(distanceFromSubtracker*10/6))); 
            // Wait for the peer to send this packet
            // TODO:How to incorporate M/M/1 queue here
            this_thread::sleep_for(chrono::nanoseconds(2));
            // Again wait for RTT/2 to notify the susbtracker that I have this packet
            this_thread::sleep_for(chrono::nanoseconds((int)(distanceFromSubtracker*10/6)));
            subtracker->packetReceivedNotification(i, this);

            packets.insert(i);
            packetTime[i] = 3*(distanceFromSubtracker*10/6) + 2;
        }
    }

}

// #endif