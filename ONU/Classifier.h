/*
 * Classifier.h
 *
 *  Created on: 2014/6/26
 *      Author: Andy
 */

#include <omnetpp.h>
#include "traffic_gen/Messages/MyPacket_m.h"
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "common/MPCP_codes.h"
#include "ONU/PowerControler.h"
#include <fstream>

#define dual_mode      2
#define tri_mode       3
#define dynamic_mode   4

using namespace std;

class PoewrControler;

class Classifier: public cSimpleModule {
public:
    Classifier();
    ~Classifier();

private:
    // Queue size > 2 will cause error; you should rewrite "respGrant"+"classify"
    // readyQueue stored the last "Report data"
    cQueue queues[2], vqueues[2], readyQueue, remainQueue;
    uint32_t queueLimit, qc0, qc1
             , lastReqLen, finalGrantLen;   // max limit & queue capacity
    uint32_t onuSize;
    vector<uint32_t> onuUpBytes;


    cGate *upI,*upO,*downI,*downO,
          *up2I,*up2O,*down2I,*down2O,
          *up3I,*up3O,*down3I,*down3O,
          *up4I,*up4O,*down4I,*down4O;
    cMessage * sendUpstreamEvent;
    cMessage * sleepTimeoutEvent;
    cMessage * checkHPQueue ;

    PowerControler * pwCtrler;
    bool goToSleep;
    int version;    // larry's or My version
    int wakeupSize;
    int modes;
    double delayBound;
    double up_data_rate, down_data_rate;
    double normalizeUp, normalizeDown;

//  ---------------   dynamic ----------------
    double dynamicPoint, dyRatio;
    double upQueueLoading;
    double upThreshold, downThreshold;
    simtime_t lastREPORT, newREPORT;
    double multiMTW;
    bool MTW_algo;
    bool HP_must_empty ;
    int onuStayChannel ;
    bool needTuning ;
    int REPORT_cycle;
    double totalQueueSize;

// -------------------------------------------
    bool getNeedSleep() {return goToSleep;}
    void setNeedSleep(bool b) {goToSleep=b;}    //???????
    void classify(MyPacket *pkt);
    uint32_t respGrant(uint32_t grantSize);
    void upstreamScheduler();
    MyPacket * sendReport(int qInfo, int length, uint16_t mode);
    void earlyWakeUp();
    void recvGateProcess(MPCPGate *gt);
    uint32_t currentQSize();

//  ---------------   dynamic ----------------
    void upLoading();
    void checkLoading();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};
