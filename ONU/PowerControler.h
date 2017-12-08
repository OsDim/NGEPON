/*
 * PowerControler.h
 *
 *  Created on: 2014/5/4
 *      Author: chienson
 */
#ifndef _POWERCONTROLER_H_
#define _POWERCONTROLER_H_

#include <omnetpp.h>
#include "traffic_gen/Messages/MyPacket_m.h"
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "common/MPCP_codes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define dual_mode      2
#define tri_mode       3
#define dynamic_mode   4

using namespace std;

class PowerControler: public cSimpleModule
{
public:
    PowerControler();
    ~PowerControler();

    void changeMode(uint16_t mode, simtime_t startTime ,simtime_t duration);
    void wakeUpProcess();

    uint32_t getEnergyMode(){return energyMode;}
    string getEnergyModeStr();
    void setRecvEnd(bool b) {recvEnd=b;}
    bool isRecvEnd() {return recvEnd;}
    void setTransEnd(bool b) {transEnd=b;}
    bool isTransEnd() {return transEnd;}
    simtime_t getSleepTime(){return Y_TimeUpEvent->getArrivalTime()- simTime();}
    simtime_t getSleepArrivalTime(){return Y_TimeUpEvent->getArrivalTime();}
    void setUploading(double loading) {upRate = loading;}
    double getUploading() {return upRate;}
    void setRemainAcitve(bool ActiveMode) {remainActive = ActiveMode;}
    bool getRemainAcitve() {return remainActive;}


private:
    uint32_t energyMode, onuSize;
    cMessage * Y_TimeUpEvent,* Z_TimeUpEvent,
             * startSleepEvent, *startDozeEvent,
             * maxESEvent;
    simtime_t IDLETHRESHOLD, MINUNIT,
              DOZE_START_TIME;
    uint16_t onuIdx;
    int version;
    double maxESTimer; //added by clare
    // ================== statistic ===================
    simtime_t lastActive, lastDoze, lastSleep,
              doze_accu, sleep_accu, active_accu;

    bool recvEnd, transEnd;

// -----------dynamic mode------------------------
    int modes;
    double dynamicPoint, upRate;
    double upThreshold;

    bool remainActive;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
