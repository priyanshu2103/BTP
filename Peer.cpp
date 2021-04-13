#include "Peer.h"

using namespace std;

Peer::Peer(int ID, double x, double y, double rAlpha, double rLambda, double rGamma)
{
    this->ID = ID;
    this->x = x;
    this->y = y;
    this->alpha = rAlpha;
    this->lambda = rLambda;
    this->gamma = rGamma;
    this->bestQoE = 0.0;
    packetTime.assign(NUM_PACKETS, {0, 0});
}

// Main function for each thread i.e peer
void Peer::operate()
{
    // Clearing packetTime and PeerRTT for every iteration
    packetTime.clear();
    packetTime.assign(NUM_PACKETS, {0,0});
    PeerRTT.clear();

    // cout<<"Inside thread for peer:"<<this->ID<<endl;

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

    for(int i=0;i<NUM_PACKETS;i++)
    {
        if(packets.find(i)!=packets.end())
        {
            // Packet already with peer do nothing
            packetTime[i] = {this->ID, 0};
        }
        else
        {
            // Wait for RTT/2
            // TODO: may have to change speed of transmission from 3*10^8 to something else
            this_thread::sleep_for(chrono::nanoseconds((int)(distanceFromSubtracker*pow(10, 9)/(2*TRASMISSION_SPEED))));    // Add this time.
            ready = false;
            Peer *peer = nullptr;
            while(!ready){peer = subtracker->getPeer(i, this, peer);}

            // Again wait for RTT/2 as subtracker takes time to return the peer
            this_thread::sleep_for(chrono::nanoseconds((int)(distanceFromSubtracker*pow(10, 9)/(2*TRASMISSION_SPEED))));
            changePeerRTT(subtracker->ID, (distanceFromSubtracker*pow(10, 9)/TRASMISSION_SPEED));

            // Wait for the peer to send this packet
            double distance = computeDistance(this, peer);
            // RTT to receive the packet from peer
            this_thread::sleep_for(chrono::nanoseconds((int)(distance*pow(10, 9)/(TRASMISSION_SPEED))));
            changePeerRTT(peer->ID, distance*pow(10, 9)/TRASMISSION_SPEED);

            // MM1 Queue time for other peer
            double queueTime = peer->getQueueTime();
            this_thread::sleep_for(chrono::nanoseconds((int)(queueTime)));
            changePeerRTT(peer->ID, queueTime);

            // Again wait for RTT/2 to notify the susbtracker that I have this packet
            this_thread::sleep_for(chrono::nanoseconds((int)(distanceFromSubtracker*pow(10, 9)/(2*TRASMISSION_SPEED))));
            changePeerRTT(subtracker->ID, (distanceFromSubtracker*pow(10, 9)/2*TRASMISSION_SPEED));

            packets.insert(i);
            packetTime[i] = {peer->ID, queueTime + 3*(distanceFromSubtracker*pow(10, 9)/(2*TRASMISSION_SPEED)) + distance*pow(10, 9)/(TRASMISSION_SPEED)};
            if(peer->ID == this->ID)
            {
                errors.push_back(i);
            }

        }
    }

    getPeerCentdian();

    // QoE computation after receiving all packets
    double mean = 0, sd = 0;
    int count = 0;
    for(auto it=packetTime.begin();it!=packetTime.end();it++)
    {
        if(it->second!=0)
        {
            mean+=it->second;
            count++;
        }
    }
    mean /= count;

    for(auto it=packetTime.begin();it!=packetTime.end();it++)
    {
        if(it->second!=0)
        {
            sd+=pow(mean - it->second, 2);
        }
    }
    sd /= count;
    sd = sqrt(sd);

    this->QoE = (double)1/(mean + sd) * 1000;
    
    // Add queue length also to the QoE
    // The more number of packets this peer gives to others -> less will be its length and more should be its contribution to QoE

    // Storing the best QoE, alpha dn lambda values
    if(QoE >= bestQoE)
    {
        bestQoE = QoE;
        bestAlpha = alpha;
        bestLambda = lambda;
        bestGamma = gamma;
    }

}
double Peer::computeDistance(Peer* peer1, Peer* peer2)
{
    return sqrt(pow(peer1->x - peer2->x, 2) + pow(peer1->y - peer2->y, 2));
}

double Peer::getQueueTime()
{
    double time = mm1Times.front();
    mm1Times.pop();
    return time;
}

void Peer::changePeerRTT(int ID, double receivedRTT)
{
    // If First receivd RTT
    if(PeerRTT.find(ID)==PeerRTT.end())
    {
        PeerRTT[ID] = (1-alpha) * receivedRTT;
    }
    // If already RTT presents
    else
    {
        PeerRTT[ID] = (1-alpha) * receivedRTT + alpha * PeerRTT[ID];
    }
}

void Peer::getPeerCentdian()
{
    // DOUBTS:
        // Should I consider only RTT or distance also
        // If RTT then only considering RTTs of peers from which this peer took packets and subtracker
    double center = DBL_MIN;
    double median;
    vector<double> list;
    // Calculate Peer center and median
    for(auto it=PeerRTT.begin(); it!=PeerRTT.end();it++)
    {
        if(it->second >= center){center = it->second;}
        if(it->second!=0){list.push_back(it->second);}
    }

    sort(list.begin(), list.end());
    
    //TODO: take care of list.size() == 0 --> will only happen when this peer already has all the packets
    if(list.size()%2==0)
    {
        median = (list[list.size()/2] + list[list.size()/2 - 1])/(double)2;
    }    
    else
    {
        median = list[list.size()/2];
    }
    centdianScore = lambda * center + (1-lambda) * median + gamma * mm1Times.size();


}

bool Peer::ifReady() const {
    return ready;
}
