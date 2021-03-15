#ifndef _SUBTRACKER_GUARD
#define _SUBTRACKER_GUARD

#include<bits/stdc++.h>
#include "Peer.h"

using namespace std;

class Subtracker 
{
    public:
        int ID;
        vector<Peer*> peers;
        map<int, vector<Peer*>> packetToPeersMapping; 

        Subtracker();
    
};

Subtracker::Subtracker()
{
    
}

#endif
