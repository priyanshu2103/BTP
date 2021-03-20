#ifndef _SUBTRACKER_GUARD
#define _SUBTRACKER_GUARD

#include<bits/stdc++.h>
#include "Peer.h"

using namespace std;
class Peer;
class Subtracker 
{
    public:
        int ID;
        int x;
        int y;
        vector<Peer*> peers;
        map<int, vector<Peer*>> packetToPeersMapping; 

        mutex m;    // Used for locking individual objects

        Subtracker();
        Peer* getPeer(int, Peer*);
        void packetReceivedNotification(int, Peer*);
    
};
#endif
