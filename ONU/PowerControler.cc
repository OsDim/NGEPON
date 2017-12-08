/*
 * PowerControler.cc
 *
 *  Created on: 2014/5/2
 *      Author: chienson
 */

#include "PowerControler.h"

Define_Module(PowerControler);

PowerControler::PowerControler() {
    Y_TimeUpEvent = NULL;
    Z_TimeUpEvent = NULL;
    startDozeEvent = NULL;
    startSleepEvent = NULL;
    maxESEvent = NULL;
}
PowerControler::~PowerControler() {
    cancelAndDelete(Y_TimeUpEvent);
    cancelAndDelete(Z_TimeUpEvent);
    cancelAndDelete(startDozeEvent);
    cancelAndDelete(startSleepEvent);
    cancelAndDelete(maxESEvent);
    //delete Y_TimeUpEvent;
    //delete Z_TimeUpEvent;
    //delete startDozeEvent;
    //delete startSleepEvent;
    //delete maxESEvent;
}

void PowerControler::initialize() {

    energyMode = ACTIVE;
    lastActive = simTime();

    MINUNIT = 1 * pow(10, -12);

    Y_TimeUpEvent = new cMessage("Y_TimeUpEvent");
    Z_TimeUpEvent = new cMessage("Z_TimeUpEvent");
    startDozeEvent = new cMessage("startDozeEvent");
    startSleepEvent = new cMessage("startSleepEvent");
    maxESEvent = new cMessage("maxESEvent");

    onuIdx = getParentModule()->getId() - 2;
    active_accu = 0;
    doze_accu = 0;
    sleep_accu = 0;

    WATCH(doze_accu);
    WATCH(sleep_accu);
    WATCH(energyMode);

//    classifier = dynamic_cast<Classifier *>(simulation.getModuleByPath(path.str().c_str()));

    cModule *epon = simulation.getModuleByPath("EPON");
    onuSize = epon->par("sizeOfONU").longValue();
    version = epon->par("version").longValue(); // 0: Larry ; 1: MY version
    maxESTimer = epon->par("maxESTimer").doubleValue(); //added by clare

    // dynamic
    modes = epon->par("modes").longValue();
    dynamicPoint = epon->par("dynamicPoint").doubleValue();
    upThreshold = epon->par("upThreshold").doubleValue();
//    dyRatio = epon->par("dynRatio").doubleValue();

    remainActive = false;

}

void PowerControler::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        if (msg == Y_TimeUpEvent)   // Sleep to Doze
        {
            if (simTime() > 1)
                sleep_accu += SLEEPTIME * pow(10, -3); // 10^-3 * 20 = 0.02 = 20 ms

            if (modes == dual_mode) { //dual-mode
                changeMode(ACTIVE, simTime(), 0);
            } else if (modes == tri_mode) { //tri-mode
                changeMode(DOZE, simTime(), DOZETIME * pow(10, -3));
            } else if (modes == dynamic_mode) {
                if (upRate>upThreshold || remainActive == true)
                    changeMode(ACTIVE, simTime(), 0);
                else
                    changeMode(DOZE, simTime(), DOZETIME * pow(10,-3));
            }
        } else if (msg == Z_TimeUpEvent) // D -> A
          {
            if (simTime() > 1)
                doze_accu += DOZETIME * pow(10, -3);
            changeMode(ACTIVE, simTime(), 0);

        } else if (msg == startDozeEvent) {    // D
//            lastDoze = simTime();

            energyMode = DOZE;
            DOZE_START_TIME = simTime();
//            if (simTime()>1 && lastDoze>1)
//                active_accu += simTime()-lastActive;
        } else if (msg == startSleepEvent) {   // S
//            lastSleep = simTime();
            //if ( getParentModule()->getId()-2 == 7 && simTime() > 0.55 ) cout << "ONU 7 sleep at : " << simTime() << endl ;

            if (version == 1 && energyMode == DOZE) {
                doze_accu += simTime() - DOZE_START_TIME;
            }
            energyMode = SLEEP;
//            if (simTime()>1)
//                active_accu += simTime()-lastActive;
        } else if (msg == maxESEvent){
            // reach maximum MPCP timeout
            wakeUpProcess();
        }

    } else    //    if (msg->arrivedOn("in"))
    {
        if (strcmp(msg->getName(), "Awake") == 0) // Do early wake up
        {
            wakeUpProcess();
            delete msg;
        } else {
            cout << "ERROR in pwctrl, Wat msg?=" << msg << " is lost??\n";
            endSimulation();
        }
    }
}

void PowerControler::finish() {
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir="/home/you/results/";
#endif
    stringstream path;
    path << dir << "PwCtrl.txt";
    ofstream out;
    out.open(path.str().c_str(), ios::out | ios::app);

    string halfpath = "EPON.localNetwork[";
    stringstream locPath;
    locPath << halfpath << getParentModule()->getId() - 2 << "]";
    cModule *locNet = simulation.getModuleByPath(locPath.str().c_str());
    double upLoad = locNet->par("offered_load");

    cModule *gen = simulation.getModuleByPath("EPON.trafficGen[0]");
    double downLoad = gen->par("offered_load");

    cModule *epon = simulation.getModuleByPath("EPON");
    int algo = epon->par("algo").longValue();

    if (getParentModule()->getId() - 2 == 0) {
        out << upLoad * 100 << "% " << downLoad * 100 << "%" << " Ver:"
                << version << " Algo:" << algo << endl;
    }
//    if (energyMode == ACTIVE)
//        active_accu+= simTime()-lastActive;
//    else if (energyMode == DOZE) {
//        if (version==0)
//            doze_accu+= simTime()-Z_TimeUpEvent->getTimestamp();
//        else if (version==1)
//            doze_accu+= simTime()-Z_TimeUpEvent->getTimestamp();
//    }
//    else if (energyMode ==SLEEP)
//        sleep_accu+= simTime()-Y_TimeUpEvent->getTimestamp();

//    out << " " << getParentModule()->getId()-2 << " " << active_accu << " " << doze_accu << " " << sleep_accu << endl;
    out << " " << getParentModule()->getId() - 2 << " " << doze_accu << " "
            << sleep_accu << endl;

    if (getParentModule()->getId() - 1 == onuSize)
        out << "\n\n\n";

    out.close();
}

void PowerControler::changeMode(uint16_t mode, simtime_t startTime, simtime_t duration)
                          // (1) sleep (2) doze (3) early wake
{

//    if (simTime()>4.099 && simTime()< 4.9503 && getParentModule()->getId()-2==4)
//        cout << "(BBchange Mode)t= " << simTime() << " ["<< getParentModule()->getId() - 2 << "] " << energyMode << endl;

    if (version == 1) {
        if (energyMode == ACTIVE && (mode == DOZE || mode == SLEEP)) { // A -> (D,S)
            scheduleAt(simTime() + maxESTimer, maxESEvent);

//            cout << "maxESTimer" << maxESTimer << endl;

        }

//        if ((simTime() + maxESTimer) ==4.099681339)
//                        cout << "t= " << simTime() << "+" << maxESTimer << "maxESTIMER" << endl;
    }
    if (mode == DOZE)  // Doze(Z)
    {
        take(startDozeEvent);
        take(Z_TimeUpEvent);
        scheduleAt(startTime, startDozeEvent);
        if (version == 0) {
            if (Z_TimeUpEvent->isScheduled()) {
                cout << "Z_TIMER has been scheduled\n";
                endSimulation();
            }
            scheduleAt(startTime + duration, Z_TimeUpEvent);
        }


//        if(getParentModule()->getId()-2 ==4)
//        cout << "onu[" << getParentModule()->getId()-2 << "] doze: " << simTime() << endl;
    }
    else if (mode == SLEEP)    // Sleep(Y)
    {
        if (Y_TimeUpEvent->isScheduled()) {
            cout << "t= " << simTime() << ", onu[" << getParentModule()->getId()-2 << "] Y_TIMER has been scheduled\n" << endl;
            endSimulation();
        }
        take(startSleepEvent);
        take(Y_TimeUpEvent);
        scheduleAt(startTime, startSleepEvent);
        scheduleAt(startTime + duration, Y_TimeUpEvent);
        //energyMode = SLEEP;
//        cout << "onu[" << getParentModule()->getId()-2 << "] sleep: " << startTime << " to " << startTime + duration << endl;

    }
    else if (mode == ACTIVE)   // Early wake up
    {
        if (simTime() > 1)
            lastActive = simTime();
        energyMode = ACTIVE;

        if (version == 1)
            cancelEvent(maxESEvent);

//        cout << "onu[" << getParentModule()->getId()-2 << "] wearly wake up: " << simTime() << endl;
    }
    else {
        cout << "ERROR in PowerControler changeMode(" << mode << ", "
                << duration << ")\n";
        endSimulation();
    }

//    if (simTime()>4.099 && simTime()< 4.9503 && getParentModule()->getId()-2==4)
//        cout << "(change Mode)t= " << simTime() << " ["<< getParentModule()->getId() - 2 << "] " << mode << endl;
}

string PowerControler::getEnergyModeStr() {
    switch (energyMode) {
    case ACTIVE:
        return "ACTIVE";
        break;
    case DOZE:
        return "DOZE";
        break;
    case SLEEP:
        return "SLEEP";
        break;
    }
    return "not define energyMode";
}

void PowerControler::wakeUpProcess() {
    if (getEnergyMode() == SLEEP) {
        simtime_t saveTime = simTime() - Y_TimeUpEvent->getSendingTime() + MINUNIT;
        if (simTime() > 1)
            sleep_accu += saveTime;
        cancelEvent(Y_TimeUpEvent);

//        cout << "t=" << simTime() << " [" << onuIdx << "] Early wake up from SLEEP save time = " << saveTime << endl;

    } else if (getEnergyMode() == DOZE) {
        if (version == 0) {
            simtime_t saveTime = simTime() - Z_TimeUpEvent->getSendingTime() + MINUNIT;
            if (simTime() > 1)
                doze_accu += saveTime;
            cancelEvent(Z_TimeUpEvent);

//            cout << "t=" << simTime() << " [" << onuIdx << "] Early wake up from DOZE save time = " << saveTime << endl;

        }
        else if (version == 1) {
            if (simTime() > 1)
                doze_accu += simTime() - DOZE_START_TIME;
        }

    }
//    if (version == 1)
//        cancelEvent(maxESEvent);

//    if (simTime()==4.099681339)
//        cout << "error" << endl;

    changeMode(ACTIVE, simTime(), 0);

    remainActive = true;
}
