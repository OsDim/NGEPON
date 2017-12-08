// Reference to OMNeT++ Manual - Simple Modules
// page. 59
//
//
//

#include "traffic_gen/Messages/EPON_messages_m.h"
#include <stdlib.h>
#include <time.h>
#include <omnetpp.h>
#include <vector>
#include <string>
#include <sstream>
#include <common/MPCP_codes.h>
#include <map>
#include <fstream>
using namespace  std;
class CoreNetwork : public cSimpleModule
{
    public:
        CoreNetwork();
        ~CoreNetwork();
    private:
        double offered_load, max_rate, local_throughput, interpacket_time;
        cMessage *sendTrafficEvent,
                 *nextArrivalEvent;
        cQueue txQueue;
        vector<uint16_t> destVec;
        vector<uint32_t> downBytes;
        uint16_t onuSize;
        double highPriorityRatio;
        string int2str(int & i);
        void findONUID();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(CoreNetwork);

CoreNetwork::CoreNetwork(){
    sendTrafficEvent = NULL;
    nextArrivalEvent = NULL;
}

CoreNetwork::~CoreNetwork(){
    cancelAndDelete(sendTrafficEvent);
    cancelAndDelete(nextArrivalEvent);
    txQueue.clear();
}

void CoreNetwork::initialize()
{
    sendTrafficEvent = new cMessage("sendTrafficEvent");
    nextArrivalEvent = new cMessage("nextArrivalEvent");
    scheduleAt(simTime(), nextArrivalEvent);
    onuSize = simulation.getModuleByPath("EPON.splitter")->gateSize("onuDown$o");
    findONUID();
    offered_load = par("offered_load");
    max_rate = par("max_rate");
    highPriorityRatio = par("high_priority_ratio");
    local_throughput = offered_load * max_rate;
    interpacket_time = ((((MIN_FRAME_LEN + MAX_FRAME_LEN) / 2) * 8) / local_throughput);

    for (uint16_t i=0; i!=onuSize; i++){
        downBytes.push_back(0);
    }

}

void CoreNetwork::handleMessage(cMessage *msg)
{
    if (msg == nextArrivalEvent)
    {
        MyPacket *job = new MyPacket("traffic");
        job->setTimestamp();

        int16_t hpRatio = highPriorityRatio*100;
        uint16_t dest = intuniform(0, onuSize-1, this->getIndex());
        uint32_t len = intuniform(MIN_FRAME_LEN, MAX_FRAME_LEN, dest);

        len = 791;

        uint16_t pri = intuniform(1, 100, dest);

        job->setByteLength(len);   // <= Edit
        job->setDestAddr(destVec[dest]);       // <= Destination rnd?  uniform

        if (pri>100-hpRatio)
            job->setPriority(0);
        else
            job->setPriority(1);

//        downBytes[dest]+=len;
        txQueue.insert(job);
        scheduleAt(simTime() + 0.00075, nextArrivalEvent);

        //scheduleAt(simTime() + exponential(interpacket_time, this->getIndex()), nextArrivalEvent);
        if (!sendTrafficEvent->isScheduled())
            scheduleAt(simTime(), sendTrafficEvent);
    }

    if (msg == sendTrafficEvent) {
        if (txQueue.empty())
            return;
        else
        {
            MyPacket * pkt = check_and_cast<MyPacket *>(txQueue.pop());
            downBytes[pkt->getDestAddr()-2]+=pkt->getByteLength();
            send(pkt, "ethOut$o");
            scheduleAt(simTime()+pkt->getBitLength()*pow(10,-9), sendTrafficEvent);   // 100 Mbps.. -8 is correct

        }
    }
}

void CoreNetwork::finish(){
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir="/home/chia/results/";
#endif
    stringstream path;
    path << dir << "CoreNetwork.txt";
    ofstream out ;
    out.open(path.str().c_str(), ios::out | ios::app);
    for (uint16_t i=0; i!=onuSize; i++) {
        out << "ONU[" << i << "] downstream: " << downBytes[i]/pow(2,20)*8 << "M bit" << endl;
    }
    out << "\n\n";
    out.close();
}

string CoreNetwork::int2str(int & i){
    string str;
    stringstream ss(str);
    ss << i;
    return ss.str();
}

void CoreNetwork::findONUID(){
    for (int i=0; i!=onuSize; i++)
    {
        string path = "EPON.onu[]";
        path.insert(9, int2str(i));
        cModule *onuMdl = simulation.getModuleByPath(path.c_str());
        destVec.push_back(onuMdl->getId());
    }
}
