//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <omnetpp.h>
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "traffic_gen/Messages/MyPacket_m.h"
#include "common/MPCP_codes.h"
#include "MPCPtools/MPCPTools.h"
#include "ONU/PowerControler.h"
#include "ONU/Classifier.h"
#include "DBA.h"
#include <fstream>
#include <cmath>
using namespace std;

class Scheduler : public cSimpleModule
{
    public:

        Scheduler();
        ~Scheduler();
    private:

        uint32_t lastReportLength;
        int onuSize;
        double distance;
        double data_rate;
        int channel ;
        simtime_t tuningtime ;

        map<uint16_t, uint32_t> upBytes;
        map<uint16_t, uint32_t> packetCount;
        map<uint16_t, simtime_t> packetDelay;
        map<uint16_t, simtime_t> maxDelay;


        cMessage *sendUpstreamMessage;
        cGate *upI,*upO,*downI,*downO,
              *up2I,*up2O,*down2I,*down2O,
              *up3I,*up3O,*down3I,*down3O,
              *up4I,*up4O,*down4I,*down4O;

             // *downO; Scheduler -> Classifier
        cQueue txQueue;
        PowerControler * pwCtrler;
        Classifier * classifier;
        uint32_t accumulateRecvPacketSize, accumulateRecvPacketCount;
        // ----------------------------------------
        void sendUpstream();
        void sentReportProcess(MPCPReport *msg);
        void recvGateProcess(MPCPGate *msg);

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void processMPCP(MPCP *msg);
        virtual void finish();
};

Define_Module(Scheduler);

Scheduler::Scheduler(){
    sendUpstreamMessage=NULL;
}

Scheduler::~Scheduler(){
    cancelAndDelete(sendUpstreamMessage);
    //delete sendUpstreamMessage;
}

void Scheduler::initialize()
{
//    receivePacketCountSignal = registerSignal("receivePacketCount");
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

    txQueue.empty();
    sendUpstreamMessage = new cMessage("sendUpstreamMessage");


    string halfpath = "EPON.onu[";
    stringstream path;
    path << halfpath << getParentModule()->getId()-2 << "].powerControler";
    pwCtrler = dynamic_cast<PowerControler * > (simulation.getModuleByPath(path.str().c_str()));

    cModule *epon = simulation.getModuleByPath("EPON");
    onuSize = epon->par("sizeOfONU").longValue();
    distance = epon->par("distance").doubleValue();
    data_rate = par("up_data_rate");



    packetDelay[0]=0;
    packetCount[0]=0;
    packetDelay[1]=0;
    packetCount[1]=0;
    maxDelay[0]=0;
    maxDelay[1]=0;

    upBytes[0]=0;
    upBytes[1]=0;
    WATCH(txQueue);
}

void Scheduler::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        // --Self_Message--
        if (msg==sendUpstreamMessage && !txQueue.isEmpty()) // start Upstream
            sendUpstream();
    }
    else {
        // --Network Message--
        cGate *fromGate = msg->getArrivalGate();

        if (fromGate == upI || fromGate == up2I || fromGate == up3I || fromGate == up4I) { // Upstream
          if ( msg->isPacket() ) {
            txQueue.insert(msg);
            if (msg->getKind() == MPCP_TYPE) {   // Recv REPORT
                if (!sendUpstreamMessage->isScheduled())
                scheduleAt(simTime(), sendUpstreamMessage);
            }
          }
        }
        else if (fromGate == gate("in") || fromGate == gate("in2") || fromGate == gate("in3") || fromGate == gate("in4") ) // receive OLT Downstream
        {
            //cout << "ONU[" << getParentModule()->getId()-2 << "] t=" << simTime() << " Recv " << msg << "("
            //     << check_and_cast<MyPacket*>(msg)->getByteLength() << ") id:" << msg->getId()
            //     << " lastPkt?" << check_and_cast<MyPacket *>(msg)->getLastPkt() << endl; //marked by clare
            if (msg->getKind() == MPCP_TYPE) {   // Recv GATE
                processMPCP(check_and_cast<MPCP * >(msg));
                pwCtrler->setRecvEnd(false);
                pwCtrler->setTransEnd(false);
                // test
                //MPCPGate * gt = check_and_cast<MPCPGate *>(msg);
                //cout << "CH :" << gt->getTransmitChannel() << endl ;

                // test

            }
            else
            {// recv traffic
                MyPacket *pkt = check_and_cast<MyPacket *>(msg);
                accumulateRecvPacketSize+= pkt->getByteLength() ;
                accumulateRecvPacketCount+=1;

//                if (simTime()> 0.1920)
//                    endSimulation();
                if (pwCtrler->getEnergyMode()==SLEEP) {
                    cout << "onu[" << getParentModule()->getId()-2 << "] recv" << msg << " insleep" ;
                    cout << " t=" << simTime() << msg << " (" << pkt->getByteLength() << ") "
                         << " id:" << msg->getId() << " cycleTag:" << pkt->getCycleTag() << endl;
                    cout << " t=" << simTime() << " onu[" << getParentModule()->getId()-2 << "] last pkt?" << pkt->getLastPkt() << " sleep arrival=" << pwCtrler->getSleepArrivalTime() << endl;
                    endSimulation();
                }
                send(msg, downO);
            }
        }
        else if( fromGate == downI || fromGate == down2I || fromGate == down3I || fromGate == down4I ) // let's set first
        {
            if (strcmp(msg->getClassName(), "MPCPAutoDiscovery")==0) {
                processMPCP( check_and_cast<MPCP *>(msg) );
            }
            delete msg;
        }
    }
}

void Scheduler::finish()
{
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir="/home/you/results/";
#endif
    stringstream path;
    path << dir << "Scheduler.txt";
    ofstream out ;
    out.open(path.str().c_str(), ios::out | ios::app);

    if (getParentModule()->getId()-2==0) {
        cModule *locNet = simulation.getModuleByPath("EPON.localNetwork[0]"), *gen = simulation.getModuleByPath("EPON.trafficGen[0]");
        double upLoad = locNet->par("offered_load"), downLoad = gen->par("offered_load");
        out << upLoad*100 << "% " << downLoad*100 << "%" << " (Mbits) endTime=" << simTime() << endl ;
    }

    out << getParentModule()->getId()-2 << " "
            << (double)upBytes[0]*8/1000000 << " " << (double)upBytes[1]*8/1000000 << " "
            << packetDelay[0] / packetCount[0] << " " << packetDelay[1] / packetCount[1] << " " << maxDelay[0] << " " << maxDelay[1] << endl;


//    out << getParentModule()->getId()-2 << " "
//            << (double)upBytes[0]*8/1000000 << " " << (double)upBytes[1]*8/1000000 << " "
//            << packetDelay[0] << " " <<  packetCount[0] << " "
//            << packetDelay[1] <<  " " << packetCount[1] << " "
//            << maxDelay[0] << " " << maxDelay[1] << endl;


//    if (packetCount[0]==0 ) {
//
//        out << getParentModule()->getId()-2 << " "
//        << (double)upBytes[0]*8/1000000 << " " << (double)upBytes[1]*8/1000000 << " "
//        << 0 << " " << packetDelay[1] / packetCount[1] << " " << 0 << " " << maxDelay[1] << endl;
//    } else if (packetCount[1]==0 ) {
//        out << getParentModule()->getId()-2 << " "
//        << (double)upBytes[0]*8/1000000 << " " << (double)upBytes[1]*8/1000000 << " "
//        << packetDelay[0] / packetCount[0] << " " << 0 << " " << maxDelay[0] << " " << 0 << endl;
//    } else if (packetCount[1]==0 && packetCount[0]==0) {
//        out << getParentModule()->getId()-2 << " "
//        << (double)upBytes[0]*8/1000000 << " " << (double)upBytes[1]*8/1000000 << " "
//        << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
//    }
//
//    else {
//
//        out << getParentModule()->getId()-2 << " "
//        << (double)upBytes[0]*8/1000000 << " " << (double)upBytes[1]*8/1000000 << " "
//        << packetDelay[0] / packetCount[0] << " " << packetDelay[1] / packetCount[1] << " " << maxDelay[0] << " " << maxDelay[1] << endl;
//    }

    if (getParentModule()->getId()-2 == onuSize-1)
        out << "\n\n\n";
    out.close();
}

// --


void Scheduler::processMPCP(MPCP *msg)
{
    cModule *olt = simulation.getModuleByPath("EPON.olt");
    switch (msg->getOpcode())
    {
        case MPCP_DISCOVER:
        {
            RTTReg *reg = new RTTReg();
            reg->setKind(MPCP_TYPE);
            reg->setOpcode(MPCP_REGISTER);
            reg->setTimestamp(simTime());
            reg->setSrcAddr(getParentModule()->getId());
            reg->setDestAddr(olt->getId());
            reg->setByteLength(64);
//            reg->setRtt( 2*(simTime() - msg->getTimestamp() - 2*msg->getDuration()) );
            reg->setRtt(2*(distance/200000));

            reg->setEnergySaving(true);
            cModule *onutbl = simulation.getModuleByPath("EPON.olt.onuTable");
            sendDirect(reg, onutbl, "in");  // Avoid collision
            break;
        }
        case MPCP_GATE:
        {
            MPCPGate * gate = check_and_cast<MPCPGate * >(msg);
            send(gate, downO);
            break;
        }
        default:
            EV << "Unrecognized MPCP OpCode!!\n";
            endSimulation();
            break;
    };
}

void Scheduler::sendUpstream()
{
    cModule *olt_Scheduler = simulation.getModuleByPath("EPON.olt.olt_Scheduler");
    MyPacket *tmp = check_and_cast<MyPacket *>(txQueue.pop());
    uint16_t pri = tmp->getPriority();
    cModule *dba = simulation.getModuleByPath("EPON.olt.dba");
    channel = static_cast<DBA*>(dba)->Getchannel() ;
    simtime_t propagationDelay = distance/200000,
              packetDuration   = (tmp->getBitLength()*data_rate) ;
//              packetDuration   = tmp->getBitLength()*pow(10,-9);


    simtime_t nxtSendTime = simTime() + packetDuration;   // when simTime=0, size= 10, end at 9.... so next time is => simtime+10
    sendDirect(tmp, propagationDelay, packetDuration, olt_Scheduler, "in");

//    cout << "ONU[" << getParentModule()->getId()-2 << "] nxtSendTime: " << nxtSendTime << endl;



    if (tmp->getKind()==MPCP_TYPE) {
        pwCtrler->setTransEnd(true);
        // Decide mode?????????????????????????????? here
        // classifier
//        if( simTime()>4.099 && simTime()< 4.11 && getParentModule()->getId()-2==4){
//        MPCPReport * rep = check_and_cast< MPCPReport * > (tmp);
//        uint32_t qInfo = rep->getQInfo(); //marked by clare
//        cout << " ONU[" << getParentModule()->getId()-2 << "] t=" << simTime() << " send " << tmp << " invisible?" << check_and_cast<MPCPReport*>(tmp)->getInvisible() << " qinfo(" << qInfo << ")" << " id:" << tmp->getId()<< endl; //marked by clare
//
//        }
    }
    else {
        //cout << " ONU[" << getParentModule()->getId()-2 << "] t=" << simTime() << " send " << tmp << "(" << tmp->getByteLength() << ")" << " id:" << tmp->getId()<< endl; //marked by clare
        if (simTime()>1) {
            packetDelay[pri]+= simTime() - tmp->getTimestamp();

            if ((simTime() - tmp->getTimestamp()) > maxDelay[pri]){
                maxDelay[pri]=simTime() - tmp->getTimestamp();
//                if (maxDelay[0]> 20*pow(10,-3) && simTime()< 4.683){
//                    cout << tmp->getId() << " , t= " << simTime() << " , timeStamp= " << tmp->getTimestamp();
//                    cout << " [" << getParentModule()->getId()-2 << "] " ;
//                    cout << "max Hi pkt delay=" << maxDelay[0]*1000 << "ms,   max low pkt delay=" << maxDelay[1] *1000 << " ms"<< endl;
//                }
            }

            //cout << "max Hi pkt delay=" << maxDelay[0] << " max low pkt delay=" << maxDelay[1] << endl;
            packetCount[pri]++;
            upBytes[pri]+=tmp->getByteLength();
        }
    }

    if (!txQueue.isEmpty())
      scheduleAt(nxtSendTime, sendUpstreamMessage);

}


