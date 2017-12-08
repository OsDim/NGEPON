/*
 * PON_Splitter.cc
 *
 *  Created on: 2014/2/18
 *      Author: chienson
 */

#include <string.h>
#include <omnetpp.h>
#include "common/MPCP_codes.h"
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "traffic_gen/Messages/MyPacket_m.h"
#include <map>

using namespace std;
class PON_Splitter : public cSimpleModule
{

private:
    int onuSize;
    double data_rate;

    map<uint32_t, simtime_t> start;
    map<uint32_t, simtime_t> end;

protected:
   // The following redefined virtual function holds the algorithm.
   virtual void initialize();
   virtual void handleMessage(cMessage *msg);
};

Define_Module(PON_Splitter);

void PON_Splitter::initialize()
{
    data_rate = par("up_data_rate");
    onuSize = gateSize("onuDown$o");
    WATCH_MAP(start);
    WATCH_MAP(end);
}

void PON_Splitter::handleMessage(cMessage *msg)
{
    // Arrival Gate
    cGate *fromGate = msg->getArrivalGate();

    const char *gateName = fromGate->getFullName();

    if (strcmp(gateName, "oltDown$i") == 0 ) // Downstream broad-casting
    {
        int outGateBaseId = gateBaseId("onuDown$o");
        for (int i=0; i<onuSize; i++)
            send(i==onuSize-1 ? msg : msg->dup(), outGateBaseId+i);
        //EV << "Splitter: " << msg << " pktid=" << msg->getId() << " T=" << simTime() << " Length=" << check_and_cast<cPacket*>(msg)->getByteLength() << endl ;
    }
    else // gate = portONU$i[ onu# ]          // Upstream TDMA
    {
        if ( fromGate->getType() == cGate::INPUT  )
        {
            //EV << "Splitter: " << msg << " pktid=" << msg->getId() << " T=" << simTime() ;
            if (msg->getKind() == MPCP_TYPE)
            {
                //EV << " onu" << check_and_cast<MPCP*>(msg)->getSrcAddr() << endl;
                start[check_and_cast<MPCP*>(msg)->getSrcAddr()]=simTime();
                end[check_and_cast<MPCP*>(msg)->getSrcAddr()]=simTime() + check_and_cast<MPCP*>(msg)->getBitLength()*data_rate;
//                end[check_and_cast<MPCP*>(msg)->getSrcAddr()]=simTime() + check_and_cast<MPCP*>(msg)->getBitLength()*pow(10,-9);
            }
            else
            {
                //EV << " onu" << check_and_cast<MyPacket*>(msg)->getSrcAddr() << endl;
                start[check_and_cast<MyPacket*>(msg)->getSrcAddr()]=simTime();
                end[check_and_cast<MyPacket*>(msg)->getSrcAddr()]=simTime() + check_and_cast<MyPacket*>(msg)->getBitLength()*data_rate;
//                end[check_and_cast<MyPacket*>(msg)->getSrcAddr()]=simTime() + check_and_cast<MyPacket*>(msg)->getBitLength()*pow(10,-9);
            }

            send(msg, "oltUp$o");
        }
    }

    //?? need to delete pointer??
    /* No.
     * While message has been send out of the gate,
     * this message no longer belong to splitter module.
     */

}
