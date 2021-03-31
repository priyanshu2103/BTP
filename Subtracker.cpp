#include "Subtracker.h"
using namespace std;

Subtracker::Subtracker()
{
}

Peer* Subtracker::getPeer(int packetNum, Peer* p, Peer* nearestP)
{

    lock_guard<mutex> guard(this->m);
    double d = DBL_MAX;
    for(auto it:packetToPeersMapping[packetNum])
    {
        double dTemp = sqrt(pow(p->x - it->x, 2) + pow(p->y - (it)->y, 2));
        if(dTemp <= d)
        {
            nearestP = it;
        }
    }
    packetToPeersMapping[packetNum].push_back(p);
    p->ready = true;

    return nearestP;    
}

void Subtracker::packetReceivedNotification(int packetNum, Peer* p)
{
    lock_guard<mutex> guard(m);
    packetToPeersMapping[packetNum].push_back(p);
}


void Subtracker::printInfo()
{
    cout<<"subtracker ID:"<<"Peer IDs(packets)"<<endl;
    cout<<(this)->ID<<": ";
    for(auto it1 = (this)->peers.begin();it1 != (this)->peers.end();it1++)
    {
        cout<<(*it1)->ID<<"(";
        for(auto it2=(*it1)->packets.begin();it2!=(*it1)->packets.end();it2++)
        {
            cout<<*it2<<",";
        }
        cout<<")";
    }
        cout<<endl;
}

void Subtracker::printPacketToPeerMapping()
{
    for(auto it:packetToPeersMapping)
    {
        cout<<it.first<<":";
        for(auto it1:it.second)
        {
            cout<<it1->ID<<" ";
        }
        cout<<endl;
    }
}

