/*
 * DBA.h
 *
 *  Created on: 2014/4/6
 *      Author: chienson
 */

#include <omnetpp.h>
#include <string.h>
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "traffic_gen/Messages/MyPacket_m.h"
#include "common/MPCP_codes.h"
#include "ONUTableEntry.h"
#include "ONUTable.h"
#include <vector>
#include <map>
#include "Hungarian.h"
#include <algorithm>
#include <math.h>

#define dual_mode      2
#define tri_mode       3
#define dynamic_mode   4

using namespace std;

class DBA : public cSimpleModule
{
public:
    DBA();
    ~DBA();
    int active_ch ;
    int last_active_ch ;
    double packet_rate ;
    int Getchannel() ;

private:
    double down_data_rate, up_data_rate;
    simtime_t GUARD_INTERVAL;
    map<uint16_t, uint32_t> accumulateDownstreamBytes, accumulateFromCoreNet;
    cGate *upI,*upO,*downI,*downO,
              *up2I,*up2O,*down2I,*down2O,
              *up3I,*up3O,*down3I,*down3O,
              *up4I,*up4O,*down4I,*down4O;
    cMessage *sendGateEvent;
    cMessage *sendDownstreamEvent;
    cMessage *sendCH1DownstreamEvent ;
    cMessage *sendCH2DownstreamEvent ;
    cMessage *sendCH3DownstreamEvent ;
    cMessage *sendCH4DownstreamEvent ;
    vector< pair<simtime_t, MyPacket*> > readyVec;
    vector< pair<simtime_t, MyPacket*> > readyVec2;
    vector< pair<simtime_t, MyPacket*> > readyVec3;
    vector< pair<simtime_t, MyPacket*> > readyVec4;

    vector< vector< pair<simtime_t, MyPacket*> > > tempVec;   // for cycle i's jth's ONU

    vector<double> onuCost ;
    vector< vector < double> > costMatrix ;
    vector< pair <int, int> > channelAssignment;
    vector<simtime_t> arrivalTime ;
    vector<uint16_t> onutransmitchannel ; // to know each onu use which channel to transmit
    vector<int>hasSelect ; // for random channel assignment
    vector<uint32_t> onurequestsize ; // record every time onu request upstream size

    bool nxtGateFail, nxtGateFailAgain;
    simtime_t MINUNIT;

    simtime_t nxtTxEnd, nxtTxEnd2, nxtTxEnd3, nxtTxEnd4, lastCycleTime, curCycleTime , lastHPArrivalTime , lastLPArrivalTime,
              oneChtime , twoChtime , threeChtime , fourChtime, lastChangeChtime;
    uint32_t cycleCount, onuSize, targetIdx, meanAvgReqLen, meanMinReqLen, meanMaxReqLen, simTimeLimit;

    double cycle_accu;

    //For credit based
    bool creditBased;
    simtime_t waitTime, sendGateTime, recvRepoTime;
    uint32_t winSize, lastReqLen, lastCrdReqLen, crdReqLen, finalGrantLen;
    uint16_t widx[256]; //onu size =16 //max onu size =256
    double window[256][100],lastWindow[256][100]; //reserved window size=100 actual setting in DBA.cc should be less than here.
    double arvReqLen, avgArvRate[256]; //onu size =16

// -------- dynamic ----------
    double dyRatio, downThreshold;
    double downRate[256];
    //double normalizeDown;
    int modes;
    double multiMTW;
    bool MTW_algo;
    bool HP_must_empty ;
    int GATE_cycle[256];//maybe don't have change every time, just give a enough number of ONU?
    double totalQueueSize[256];//same
    bool asymmetric_flow ;
    bool firstgetidx ;
    bool afteronecycle ;
    bool randomChannelAssign ;
    bool randomOrRR ;
    uint32_t AllpacketLength , cycleAllPacketSize;
    double s_time ;
    uint16_t assignChannel ;
    int notSleepOnu ;
    double tuningTime ;

    double averageChannelCapacity ;
    int requestSize ;

    ONUTable * onutbl;

    map<uint32_t, bool> isDownEnd, isUpEnd, isWaitReport, hasTransmitDown ;
    map<uint32_t, simtime_t> upStart, upEnd, downStart, downEnd;
    map<uint32_t, uint32_t> grantDownLen, grantUpLen, avgReqLen, minReqLen, maxReqLen, countReqZero, countReqAll;

    map<uint16_t, vector<cQueue> > qMap;
    map<uint16_t, vector<uint32_t> > byteMap;
    map<uint16_t, vector<uint32_t> > dropCount; // map <index, priority[]>
    map<uint16_t, vector<uint32_t> > downLink;


    int16_t algo;
    int version;
    uint32_t preidx ;
    uint32_t firstserviceidx ;
    uint32_t targetidx ;
    uint32_t queueLimit;
    double creditRatio;
    int setMTW;
    int minCrdLen;

    //--------------------------------------

    uint32_t lastUpBW;
    uint16_t BWidx[256];
    double BWindow[256][100],lastBWindow[256][100];
    double arvUpBW, avgArvUpBW[256];
    simtime_t lastGATE[256], newGATE[256];

    // --------------Functions--------------
    void syncState(uint32_t idx, bool dozeAck, bool sleepAck);
    void recvReport(MPCPReport * rep);
    void nxtGateProcess(uint32_t onuIdx) ;
    void splitPkt(int idx, simtime_t nextGate);
    void timeSetter();
    void classify(cMessage * msg);
    void activeChannel() ;
    void upStreamScheduler();
    void polling(uint32_t idx);

    void sendGateMessage(uint32_t idx, uint32_t grantUpLen, uint32_t downLength);
    int getDownstreamData(uint32_t idx);
    int returnChannelDataindex( uint16_t channel ) ;
    void produceCost() ;
    void hungarian() ;

    simtime_t timeCalc(int pktbytes, simtime_t rtt, int txrate){return pktbytes*8 / (txrate*pow(10,9)) + rtt ;}
    // Access twice 2*(all the packets in bytes)*8*ns + RTT // IPACT Guard: 5us
    // sendDirect once*(all the packets in bytes)*8*ns + RTT // IPACT Guard: 5us

// -------- dynamic ----------
    void downloading(uint16_t idx);


    double totalSize, doubleSize;
    double delaySize;


protected:
   virtual void initialize();
   virtual void handleMessage(cMessage *msg);
   virtual void finish();
};



