#include "Subtracker.h"
using namespace std;

Subtracker::Subtracker()
{}

Peer* Subtracker::getPeer(int packetNum, Peer* p)
{
    lock_guard<mutex> guard(m);
    double d = DBL_MAX;
    Peer* nearestP = NULL;
    for(auto it=packetToPeersMapping[packetNum].begin(); it!=packetToPeersMapping[packetNum].end();it++)
    {
        double dTemp = sqrt(pow(p->x - (*it)->x, 2) + pow(p->y - (*it)->y, 2));
        if(dTemp <= d)
        {
            nearestP = *it;
        }
    }
    return nearestP;
    
}

void Subtracker::packetReceivedNotification(int packcetNum, Peer* p)
{
    lock_guard<mutex> guard(m);
    packetToPeersMapping[packcetNum].push_back(p);
}
