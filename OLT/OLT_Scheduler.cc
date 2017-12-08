/*
 * OLT_Scheduler.cc
 *
 *  Created on: 2014/3/24
 *      Author: chienson
 */

#include <omnetpp.h>
#include <string.h>
#include <vector>
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "common/MPCP_codes.h"
#include "ONUTable.h"
#include "ONU/PowerControler.h"
#include <fstream>
#include "DBA.h"

using namespace std;

class OLT_Scheduler : public cSimpleModule
{
public:
    OLT_Scheduler();
    ~OLT_Scheduler();
private:

    cGate *upI,*upO,*downI,*downO,
          *up2I,*up2O,*down2I,*down2O,
          *up3I,*up3O,*down3I,*down3O,
          *up4I,*up4O,*down4I,*down4O;
    ONUTable * onutbl;
    cMessage * sendDelayEventMessage;

    map<uint16_t, vector<uint32_t> > downBytes;
    map<uint16_t, vector<uint32_t> > packetCount;   // [0] high [1] low
    map<uint16_t, vector<simtime_t> > packetDelay;
    map<uint16_t, vector<simtime_t> > maxDelay;
    uint16_t onuSize;
    double distance;
    double data_rate;
    int channel ;
    simtime_t tuningtime ;
    bool needTuning ;
    uint32_t tuningcount ;
    vector<uint16_t> channelindex ;



    // for test
    uint32_t priorityCount;
    uint32_t highMPCP;

protected:
   virtual void initialize();
   virtual void handleMessage(cMessage *msg);
   virtual void finish();
// -----------------------------------------------
   virtual void sendMPCPDiscovery();
   virtual void processMPCP(MPCP *msg);
};


Define_Module(OLT_Scheduler);

OLT_Scheduler::OLT_Scheduler(){
    sendDelayEventMessage = NULL;
}

OLT_Scheduler::~OLT_Scheduler(){
    //cancelAndDelete(sendDelayEventMessage);
    delete sendDelayEventMessage;
}

void OLT_Scheduler::initialize()
{
    sendDelayEventMessage = new cMessage("sendDelayEventMessage");
    //-------------- Interface ---------------
    upI =gateHalf("Up$i", cGate::INPUT);
    upO =gateHalf("Up$o", cGate::OUTPUT);
    downI =gateHalf("Down$i", cGate::INPUT);
    downO =gateHalf("Down$o", cGate::OUTPUT);
    up2I =gateHalf("Up2$i", cGate::INPUT);
    up2O =gateHalf("Up2$o", cGate::OUTPUT);
    down2I =gateHalf("Down2$i", cGate::INPUT);
    down2O =gateHalf("Down2$o", cGate::OUTPUT);
    up3I =gateHalf("Up3$i", cGate::INPUT);
    up3O =gateHalf("Up3$o", cGate::OUTPUT);
    down3I =gateHalf("Down3$i", cGate::INPUT);
    down3O =gateHalf("Down3$o", cGate::OUTPUT);
    up4I =gateHalf("Up4$i", cGate::INPUT);
    up4O =gateHalf("Up4$o", cGate::OUTPUT);
    down4I =gateHalf("Down4$i", cGate::INPUT);
    down4O =gateHalf("Down4$o", cGate::OUTPUT);

    onutbl = dynamic_cast<ONUTable *> (simulation.getModuleByPath("EPON.olt.onuTable"));
    sendMPCPDiscovery();
    onuSize = simulation.getModuleByPath("EPON.splitter")->gateSize("onuDown$o");
    for (uint16_t i=0; i!=onuSize; i++) {
                                    // map[onuIdx][priority]
        downBytes[i].push_back(0);  // [0] => high
        downBytes[i].push_back(0);  // [1] => low
        packetCount[i].push_back(0);
        packetCount[i].push_back(0);
        packetDelay[i].push_back(0);
        packetDelay[i].push_back(0);
        maxDelay[i].push_back(0);
        maxDelay[i].push_back(0);

    }
    cModule *epon = simulation.getModuleByPath("EPON");
    distance = epon->par("distance").doubleValue(); // 20
    data_rate = par("down_data_rate");
    tuningtime = 0.0001 ;
    needTuning = false ;
    channelindex.reserve(onuSize) ;
    for(uint16_t i=0; i!=onuSize; i++) {
        channelindex[i] = 1 ;
    }


    // for test
    priorityCount=0;
    highMPCP=0;
    tuningcount = 0 ;
}

void OLT_Scheduler::handleMessage(cMessage *msg)
{
    // Upstream data
    cGate *fromGate = msg->getArrivalGate(); // get the packet from down stream or up steam
    if (fromGate==upI || fromGate==up2I || fromGate==up3I || fromGate==up4I || fromGate==gate("in") || fromGate==gate("in2"))
    {
        if ((msg->getKind()==MPCP_TYPE) )
        {
            MPCP* mpcp= check_and_cast<MPCP *>(msg);
            processMPCP(mpcp);
        }
        else if (msg->isPacket()) // Need statistic receive packet
        {
//            cout << "OLT t=" << simTime() << " recev " << msg
//                    << "(" << check_and_cast<MyPacket *>(msg)->getByteLength()
//                    << ") from onu[" << check_and_cast<MyPacket *>(msg)->getSrcAddr()-2 << "] "
//                    << " id:" << msg->getId()<< endl; //marked by clare
            delete msg;
        }
        else{
            delete msg;
        }
    }
    else if (fromGate==downI || fromGate==down2I || fromGate==down3I || fromGate==down4I) // Downstream gen data
    {
        if (msg->isPacket())    // Downstream "MPCP" or "Normal Packet"
        {
            MyPacket * pkt = check_and_cast<MyPacket *>(msg);

            uint32_t onuIdx = pkt->getDestAddr()-2;
            int    priority = pkt->getPriority();

            string halfpath = "EPON.onu[";
            stringstream path;
            path << halfpath << onuIdx << "].scheduler";
            cModule *onu = simulation.getModuleByPath(path.str().c_str());
            cModule *dba = simulation.getModuleByPath("EPON.olt.dba");
            channel = static_cast<DBA*>(dba)->Getchannel() ;

            simtime_t propagationDelay = distance/200000,
                      duration         = (pkt->getBitLength()*data_rate);

            //cout << "channel : " << channel << " duration : " << duration << endl ;
            //if ( channel == 2 ) endSimulation() ;
            if (msg->getKind()==MPCP_TYPE){
                if(simTime()>1 && priority==1)
                {
                    cout << "MPCP: " << highMPCP << endl;
                }

                MPCPGate * gt = check_and_cast<MPCPGate *>(msg);
                uint16_t channel = gt->getTransmitChannel() ;
                //cout << "onu channel : " << channelindex[onuIdx] << endl ;
                //cout << "channel to : " << channel << endl ;
                if( channel != channelindex[onuIdx] ) {
                    needTuning = true ;
                    channelindex[onuIdx] = channel ;
                    tuningcount ++ ;
                }
                else {
                    needTuning = false ;
                }

                //cout << " OLT t=" << simTime() << " send " << msg << "grant(" << check_and_cast<MPCPGate*>(msg)->getLength()
                //        << ") Downstream(" << check_and_cast<MPCPGate*>(msg)->getDownLength()
                //        << ") to " << path.str() << " id:" << msg->getId() << " cycleTag:"
                //        << check_and_cast<MyPacket*>(msg)->getCycleTag() << " lastPkt?" << check_and_cast<MyPacket*>(msg)->getLastPkt() << endl; //marked by clare
//                cout << " ONUTbl[" << onuIdx << "] mode=" << onutbl->getEntry(onuIdx)->getState() << endl;
//                if (onutbl->getEntry(onuIdx)->getState()==SLEEP) {
//                    cout << " ONUTbl[" << onuIdx << "] Sleep arrival t=" << onutbl->getSleepArrivalTime(onuIdx) << endl;
//                }
//                string str_pre = "EPON.onu[";
//                stringstream str_all;
//                str_all << str_pre << onuIdx << "].powerControler";
//                PowerControler *onuCtrl = dynamic_cast<PowerControler * >(simulation.getModuleByPath(str_all.str().c_str()));
//                cout << " ONUCtrl[" << onuIdx << "] mode=" << onuCtrl->getEnergyMode() << endl;
//                cout << "Arrival sleep t=" << onuCtrl->getSleepArrivalTime() << " sleep time left=" << onuCtrl->getSleepTime() << endl;
            }
            else{
                if (simTime()>1) {
                    downBytes[onuIdx][priority]+=pkt->getByteLength();
                    packetCount[onuIdx][priority]++;

                    if ( needTuning )
                      packetDelay[onuIdx][priority]+= ( simTime() - pkt->getTimestamp() ) + tuningtime ;
                    else

                        packetDelay[onuIdx][priority]+= simTime() - pkt->getTimestamp() ;

                    if (simTime() - pkt->getTimestamp() > maxDelay[onuIdx][priority]) // maximum delay
                        maxDelay[onuIdx][priority]=simTime() - pkt->getTimestamp();
                }
//                cout << " OLT t=" << simTime() << " send " << msg << "(" << check_and_cast<cPacket*>(msg)->getByteLength()
//                     << ")" << " to " << path.str() << " id:" << msg->getId()
//                     << " cycleTag:" << check_and_cast<MyPacket*>(msg)->getCycleTag()
//                     << " lastPkt?" << check_and_cast<MyPacket*>(msg)->getLastPkt() << endl; //marked by clare

            }
            // test


            if( fromGate==downI )
              sendDirect(pkt, propagationDelay, duration, onu, "in");
            else if(  fromGate==down2I )
              sendDirect(pkt, propagationDelay, duration, onu, "in2");
            else if(  fromGate==down3I )
              sendDirect(pkt, propagationDelay, duration, onu, "in3");
            else
              sendDirect(pkt, propagationDelay, duration, onu, "in4");


        }
    }
}

void OLT_Scheduler::finish()
{
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir="/home/you/results/";
#endif
    stringstream path;
    path << dir << "OLT_Scheduler.txt";
    ofstream out ;
    out.open(path.str().c_str(), ios::out | ios::app);
    cModule *locNet = simulation.getModuleByPath("EPON.localNetwork[0]");
    double upLoad = locNet->par("offered_load");

    cModule *gen = simulation.getModuleByPath("EPON.trafficGen[0]");
    double downLoad = gen->par("offered_load");
    out << upLoad*100 << "% " << downLoad*100 << "% Mbits" << endl ;
    for (uint16_t i=0; i!=onuSize; i++) {

        simtime_t high_pktDelay, low_pktDelay = -1;

        if (packetCount[i][0]==0)
            high_pktDelay = 0;
        else
            high_pktDelay = packetDelay[i][0] / packetCount[i][0];// 0 = high 1 = low, actual is high_pktDelay ,
        if (packetCount[i][1]==0)
            low_pktDelay = 0;
        else
            low_pktDelay = packetDelay[i][1] / packetCount[i][1];


//        out << i << " " << (double)downBytes[i][0]*8/1000000 << " " << (double)downBytes[i][1]*8/1000000 << " "
//            << packetDelay[i][0] / packetCount[i][0] << " " << packetDelay[i][1] / packetCount[i][1] << " " << maxDelay[i][0] << " " << maxDelay[i][1] << "\n";

        out << i << " " << (double)downBytes[i][0]*8/1000000 << " " << (double)downBytes[i][1]*8/1000000 << " "
                    << high_pktDelay << " " << low_pktDelay  << " " << maxDelay[i][0] << " " << maxDelay[i][1] << "\n";


    }


//    for (uint16_t i=0; i!=onuSize; i++) {
//        out << i << " " << (double)downBytes[i][0]*8/1000000 << " " << (double)downBytes[i][1]*8/1000000 << " "
//            << packetDelay[i][0] << " " <<  packetCount[i][0] << " "
//            << packetDelay[i][1] << " " <<  packetCount[i][1] << " "
//            << maxDelay[i][0] << " " << maxDelay[i][1] << "\n";
//
//    }
    out << "\n\n\n";
    out.close();

    stringstream path2;
    path2 << dir << "Tuning count.txt";
    ofstream out2 ;
    out2.open(path2.str().c_str(), ios::out | ios::app);
    out2 << upLoad*100 << "% " << downLoad*100 << "% Mbits" << endl ;
    out2 << "tuning count : " << tuningcount << endl ;

    out2.close();

    priorityCount=0;
}
// ------------------------------------------------------------------------------------
void OLT_Scheduler::sendMPCPDiscovery()
{
    MPCPAutoDiscovery *autoDisc = new MPCPAutoDiscovery();
    autoDisc->setKind(MPCP_TYPE);
    autoDisc->setOpcode(MPCP_DISCOVER);
    autoDisc->setTimestamp();
    autoDisc->setByteLength(64);
    send(autoDisc, downO);
}

void OLT_Scheduler::processMPCP(MPCP *msg)
{
    switch (msg->getOpcode())
    {
        case MPCP_REGISTER: // This case have done by other module(ONUtable)... won't process here.
        {
            delete msg;
            break;
        }
        case MPCP_REPORT:   // Do updating ONUTable
        {
            MPCPReport * rep = check_and_cast<MPCPReport * >(msg);
//            cout << "OLT t=" << simTime()
//                 << " recev REPORT (invisible?" << rep->getInvisible() << ") req:" << "" << rep->getQInfo() << " from onu[" << rep->getSrcAddr()-2 << "] " << " id:" << rep->getId()<< endl; //marked by clare
            send( rep, upO);
            onutbl->updateTable(rep);
            break;
        }
    };

}

