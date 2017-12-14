/*
 * DBA.cc
 *
 *  Created on: 2014/3/24
 *      Author: chienson
 */

#include "DBA.h"
#include <fstream>
#include <time.h>
#include <sstream>

Define_Module(DBA);

DBA::DBA() {
    sendGateEvent = NULL;
    sendDownstreamEvent = NULL;
}
DBA::~DBA() {
    cancelAndDelete(sendGateEvent);
    cancelAndDelete(sendDownstreamEvent);
    //delete sendGateEvent;
    //delete sendDownstreamEvent;
}

void DBA::initialize() {
    upI = gateHalf("Up$i", cGate::INPUT);
    upO = gateHalf("Up$o", cGate::OUTPUT);
    downI = gateHalf("Down$i", cGate::INPUT);
    downO = gateHalf("Down$o", cGate::OUTPUT);
    up2I = gateHalf("Up2$i", cGate::INPUT);
    up2O = gateHalf("Up2$o", cGate::OUTPUT);
    down2I = gateHalf("Down2$i", cGate::INPUT);
    down2O = gateHalf("Down2$o", cGate::OUTPUT);
    up3I = gateHalf("Up3$i", cGate::INPUT);
    up3O = gateHalf("Up3$o", cGate::OUTPUT);
    down3I = gateHalf("Down3$i", cGate::INPUT);
    down3O = gateHalf("Down3$o", cGate::OUTPUT);
    up4I = gateHalf("Up4$i", cGate::INPUT);
    up4O = gateHalf("Up4$o", cGate::OUTPUT);
    down4I = gateHalf("Down4$i", cGate::INPUT);
    down4O = gateHalf("Down4$o", cGate::OUTPUT);
    targetIdx = 0;
    cycleCount = 0;

    meanAvgReqLen = 0;
    meanMinReqLen = 10000;
    meanMaxReqLen = 0;

    //for credit based
    lastReqLen = 0;
    crdReqLen = 0;
    lastCrdReqLen = 0;
    arvReqLen = 0;
    //winSize=1; //actual window size set here, its max size 100 is defined in DBA.h

    curCycleTime = simTime();
    lastCycleTime = simTime();
    onutbl = dynamic_cast<ONUTable *>(simulation.getModuleByPath(
            "EPON.olt.onuTable"));
    sendGateEvent = new cMessage("sendGateEvent");
    sendDownstreamEvent = new cMessage("sendDownstreamEvent");
    sendCH1DownstreamEvent = new cMessage("sendCH1DownstreamEvent");
    sendCH2DownstreamEvent = new cMessage("sendCH2DownstreamEvent");
    sendCH3DownstreamEvent = new cMessage("sendCH3DownstreamEvent");
    sendCH4DownstreamEvent = new cMessage("sendCH4DownstreamEvent");

    cModule *epon = simulation.getModuleByPath("EPON");
    onuSize = epon->par("sizeOfONU").longValue();
    version = epon->par("version").longValue(); // 0: Larry ; 1: MY version
    queueLimit = epon->par("queueLimit").longValue();
    algo = epon->par("algo").longValue();
    creditBased = epon->par("creditBased").boolValue();
    creditRatio = epon->par("creditRatio").doubleValue();
    setMTW = epon->par("setMTW").longValue();
    winSize = epon->par("winSize").longValue();

    //get the sim-time-limit for calculate result, Example : "TotalBit / sim-time-limit" for average bit per-second.
    const char *s = ev.getConfig()->getConfigValue("sim-time-limit");
    cout << "sim-Time-Limit : " << s << endl;
    simTimeLimit = atoi(s) - 1;
    cout << "sim-Time-Limit(int) : " << simTimeLimit ;

    //new

    down_data_rate = par("down_data_rate");
    up_data_rate = par("up_data_rate");

    nxtGateFail = false;
    GUARD_INTERVAL = 1 * pow(10, -6);//1us
    cout << "GUARD_INTERVAL :" << GUARD_INTERVAL;
    MINUNIT = 1 * pow(10, -12);
    srand((unsigned)time(NULL)) ;

    // for dynamic

    GATE_cycle[onuSize];
    totalQueueSize[onuSize];

    dyRatio = epon->par("dyRatio").doubleValue();
    downThreshold = epon->par("downThreshold").doubleValue();
    //normalizeDown = 10 / (par("down_data_rate").doubleValue()); // unUse don't know what to do.
    modes = epon->par("modes").longValue();
    multiMTW = epon->par("multiMTW").doubleValue();
    MTW_algo = epon->par("MTW_algo").boolValue();
    HP_must_empty = epon->par("high_priority_must_empty").boolValue();
    asymmetric_flow = epon->par("asymmetric_flow").boolValue();
    randomChannelAssign = epon->par("randomChannelAssign").boolValue();
    randomOrRR = epon->par("randomOrRR").boolValue();
    tuningTime = epon->par("tuningTime").doubleValue() ;
    active_ch = 1;
    last_active_ch = 1 ;
    cycleAllPacketSize = 0;
    lastHPArrivalTime = 0;
    lastLPArrivalTime = 0;
    oneChtime = 0;
    twoChtime = 0;
    threeChtime = 0;
    fourChtime = 0;
    preidx = -1;
    firstgetidx = true;
    afteronecycle = false;
    targetidx = 0;
    lastChangeChtime = 0;
    onutransmitchannel.reserve(onuSize);
    tempVec.reserve(onuSize) ;
    arrivalTime.reserve(onuSize) ;
    onurequestsize.reserve(onuSize) ;

    notSleepOnu = 0 ;

    averageChannelCapacity = 0 ;
    requestSize = 0 ;

    for (uint16_t i = 0; i != onuSize; i++) {
        onutransmitchannel[i] = 1;
        onurequestsize[i] = 0 ;
    }
    //WATCH_VECTOR(pktVec);
    for (uint32_t i = 0; i != onuSize; i++) // Initialize qMap's High Priority, Low Priority Queues
            {
        //for credit based
        for (uint32_t j = 0; j != winSize; j++) {
            window[i][j] = 0;
            lastWindow[i][j] = 0;
        }
        widx[i] = 0;
        avgArvRate[i] = 0;

        cQueue q0, q1;
        qMap[i].push_back(q0);
        qMap[i].push_back(q1);
        byteMap[i].push_back(0);    // for high
        byteMap[i].push_back(0);    // for low
        dropCount[i].push_back(0);
        downLink[i].push_back(0);
        downLink[i].push_back(0);

        isUpEnd[i] = true;
        isDownEnd[i] = true;
        isWaitReport[i] = false;
        hasTransmitDown[i] = false ;

        accumulateDownstreamBytes[i] = 0;
        accumulateFromCoreNet[i] = 0;

        minReqLen[i] = 10000;  //any big number
        maxReqLen[i] = 0;
        avgReqLen[i] = 0;
        countReqZero[i] = 0;
        countReqAll[i] = 0;

        // for dynamic
        downRate[i] = 0;
        GATE_cycle[i] = 0;
        totalQueueSize[i] = 0;
        lastGATE[i] = simTime();
        newGATE[i] = simTime();

    }

    totalSize = 0;
    doubleSize = 0;
    delaySize = 0;
    cout << "MTU: " << MTU << endl;

}

int DBA::Getchannel() {
    return active_ch;
}


void DBA::handleMessage(cMessage *msg) {
    // --Self message--
    if (msg->isSelfMessage()) {
        if (msg == sendGateEvent)
            upStreamScheduler();
        else if (msg == sendDownstreamEvent) { //Driven by getDownstreamData() first transmit data
            MyPacket *pkt, *pkt2, *pkt3, *pkt4;
            uint32_t destIdx, destIdx2, destIdx3, destIdx4;
            //cout << "active channel :" << active_ch << endl ;
            if (active_ch >= 1) {
                pkt = readyVec[0].second;
                destIdx = pkt->getDestAddr() - 2;
            }

            if (active_ch >= 2) {
                pkt2 = readyVec2[0].second;
                destIdx2 = pkt2->getDestAddr() - 2;
            }

            if (active_ch >= 3) {
                //cout << "vector3 size : " << readyVec3.size() << endl ;
                pkt3 = readyVec3[0].second;
                destIdx3 = pkt3->getDestAddr() - 2;
            }

            if (active_ch == 4) {
                pkt4 = readyVec4[0].second;
                destIdx4 = pkt4->getDestAddr() - 2;
            }

            // record each onu data complete transmission time


            if (pkt->getKind() == MPCP_TYPE)
                nxtGateProcess(destIdx);
            if (active_ch >= 2 && pkt2->getKind() == MPCP_TYPE)
                nxtGateProcess(destIdx2);
            if (active_ch >= 3 && pkt3->getKind() == MPCP_TYPE)
                nxtGateProcess(destIdx3);
            if (active_ch >= 4 && pkt4->getKind() == MPCP_TYPE)
                nxtGateProcess(destIdx4);


            else { //force gate?

                if (simTime() > 1) {
                    accumulateDownstreamBytes[pkt->getDestAddr() - 2] +=
                            pkt->getByteLength(); // <==================================== statistic
                    downLink[pkt->getDestAddr() - 2][pkt->getPriority()] +=
                            pkt->getByteLength();
                    if (active_ch >= 2) {
                        accumulateDownstreamBytes[pkt2->getDestAddr() - 2] +=
                                pkt2->getByteLength();
                        downLink[pkt2->getDestAddr() - 2][pkt2->getPriority()] +=
                                pkt2->getByteLength();
                    }
                    if (active_ch >= 3) {
                        accumulateDownstreamBytes[pkt3->getDestAddr() - 2] +=
                                pkt3->getByteLength();
                        downLink[pkt3->getDestAddr() - 2][pkt3->getPriority()] +=
                                pkt3->getByteLength();
                    }
                    if (active_ch >= 4) {
                        accumulateDownstreamBytes[pkt4->getDestAddr() - 2] +=
                                pkt4->getByteLength();
                        downLink[pkt4->getDestAddr() - 2][pkt4->getPriority()] +=
                                pkt4->getByteLength();
                    }
                }
            }

            if (pkt->getLastPkt()) {
                isDownEnd[destIdx] = true;
                //if (destIdx == 18 && simTime() > 0.78 ) cout << "ONU 18 Last Packet ! " << "Time : "<< simTime() + (pkt->getBitLength() * down_data_rate)<< " trans to onu "<< endl ;
                if (downEnd[destIdx] != simTime() + pkt->getBitLength() * down_data_rate - MINUNIT && algo == 1) { // has error
                    cout << " error: downEnd[" << destIdx << "]=" << downEnd[destIdx] << " lastPkt end=" << simTime() + pkt->getBitLength() * down_data_rate << endl;
                    endSimulation();
                }
            }
            if ( active_ch >= 2 ) {
              if (pkt2->getLastPkt()) {
                isDownEnd[destIdx2] = true;
                //if (destIdx2 == 18 && simTime() > 0.78 ) cout << "ONU 18 Last Packet ! " << "Time : "<< simTime() + (pkt2->getBitLength() * down_data_rate)<< " trans to onu "<< endl ;
                if (downEnd[destIdx2] != simTime() + pkt2->getBitLength() * down_data_rate - MINUNIT && algo == 1) { // has error
                    cout << " error: downEnd[" << destIdx2 << "]=" << downEnd[destIdx2] << " lastPkt end=" << simTime() + pkt2->getBitLength() * down_data_rate << endl;
                    endSimulation();
                }
              }
            }
            if ( active_ch >= 3 ) {
              if (pkt3->getLastPkt()) {
                isDownEnd[destIdx3] = true;
                //if (destIdx3 == 7 ) cout << "ONU 7 Last Packet ! " << "Time : "<< simTime() + (pkt3->getBitLength() * down_data_rate)<< " trans to onu "<< endl ;
                if (downEnd[destIdx3] != simTime() + pkt3->getBitLength() * down_data_rate - MINUNIT && algo == 1) { // has error
                  cout << " error: downEnd[" << destIdx3 << "]=" << downEnd[destIdx3] << " lastPkt end=" << simTime() + pkt3->getBitLength() * down_data_rate << endl;
                  endSimulation();
                }
              }
            }
            if ( active_ch >= 4 ) {
              if (pkt4->getLastPkt()) {
                isDownEnd[destIdx4] = true;

                if (downEnd[destIdx4] != simTime() + pkt4->getBitLength() * down_data_rate - MINUNIT && algo == 1) { // has error
                  cout << " error: downEnd[" << destIdx4 << "]=" << downEnd[destIdx4] << " lastPkt end=" << simTime() + pkt4->getBitLength() * down_data_rate << endl;
                  endSimulation();
                }
              }
            }

            // send and erase transmit packet

            nxtTxEnd = simTime() + (down_data_rate) * pkt->getBitLength(); // need mod
            //cout << " Transmit channel 1 !!!!!!!!!!!!!!!!!!!!!!" << endl;
            //cout << "destIdx : " << destIdx << " Kind : "<<pkt->getKind() << endl ;
            send(pkt, downO);
            readyVec.erase(readyVec.begin());

              scheduleAt(nxtTxEnd, sendCH1DownstreamEvent); // Duration to avoid collision in real channel

            if (active_ch >= 2) {

                nxtTxEnd2 = simTime() + (down_data_rate) * pkt2->getBitLength(); // need mod
                //cout << " Transmit channel 2 !!!!!!!!!!!!!!!!!!!!!!" << endl;
                //cout << "destIdx2 : " << destIdx2 << " Kind : "<<pkt2->getKind() << endl ;
                send(pkt2, down2O);

                readyVec2.erase(readyVec2.begin());

                  scheduleAt(nxtTxEnd2, sendCH2DownstreamEvent); // Duration to avoid collision in real channel
            }

            if (active_ch >= 3) {
                nxtTxEnd3 = simTime() + (down_data_rate) * pkt3->getBitLength(); // need mod
                        //cout << " Transmit channel 3 !!!!!!!!!!!!!!!!!!!!!!" << endl ;

                send(pkt3, down3O);
                readyVec3.erase(readyVec3.begin());

                  scheduleAt(nxtTxEnd3, sendCH3DownstreamEvent); // Duration to avoid collision in real channel
            }
            if (active_ch >= 4) {
                nxtTxEnd4 = simTime() + (down_data_rate) * pkt4->getBitLength(); // need mod
                        //cout << " Transmit channel 4 !!!!!!!!!!!!!!!!!!!!!!" << endl ;

                send(pkt4, down4O);
                readyVec4.erase(readyVec4.begin());

                  scheduleAt(nxtTxEnd4, sendCH4DownstreamEvent); // Duration to avoid collision in real channel
            }

        } else if (msg == sendCH1DownstreamEvent
                || msg == sendCH2DownstreamEvent
                || msg == sendCH3DownstreamEvent
                || msg == sendCH4DownstreamEvent) { // Driven by senDownstreamEvent()
            uint16_t channel = 0;
            if (msg == sendCH1DownstreamEvent && !readyVec.empty())
                channel = 1;
            else if (msg == sendCH2DownstreamEvent && !readyVec2.empty())
                channel = 2;
            else if (msg == sendCH3DownstreamEvent && !readyVec3.empty())
                channel = 3;
            else if (msg == sendCH4DownstreamEvent && !readyVec4.empty())
                channel = 4;

            if (channel != 0) {
                //cout << "event start ! " << endl;
                MyPacket * pkt;
                uint32_t destIdx;
                if (channel == 1) {
                    pkt = readyVec[0].second;
                    destIdx = pkt->getDestAddr() - 2;
                } else if (channel == 2) {
                    pkt = readyVec2[0].second;
                    destIdx = pkt->getDestAddr() - 2;
                } else if (channel == 3) {
                    pkt = readyVec3[0].second;
                    destIdx = pkt->getDestAddr() - 2;
                } else {
                    pkt = readyVec4[0].second;
                    destIdx = pkt->getDestAddr() - 2;
                }

                //if ( destIdx == 28 && simTime() > 1 )
                  //cout << "destIdx : " << destIdx << " Kind : "<< pkt->getKind() << endl ;
                // record each onu data complete transmission time
                if (channel == 1 && pkt->getKind() == MPCP_TYPE)
                    nxtGateProcess(destIdx);
                else if (channel == 2 && pkt->getKind() == MPCP_TYPE)
                    nxtGateProcess(destIdx);
                else if (channel == 3 && pkt->getKind() == MPCP_TYPE)
                    nxtGateProcess(destIdx);
                else if (channel == 4 && pkt->getKind() == MPCP_TYPE)
                    nxtGateProcess(destIdx);
                else  //force gate?
                {

                    if (simTime() > 1) {
                        accumulateDownstreamBytes[pkt->getDestAddr() - 2] +=
                                pkt->getByteLength(); // <==================================== statistic
                        // for test
                        downLink[pkt->getDestAddr() - 2][pkt->getPriority()] +=
                                pkt->getByteLength();


                    }
                }

                if (pkt->getLastPkt()) {
                    //cout << "destIdx : " << destIdx << " last packet "<< endl ;
                    isDownEnd[destIdx] = true;
                   // if (destIdx == 18 && simTime() > 0.78 ) cout << "ONU 18 Last Packet ! " << "Time : "<< simTime() + (pkt->getBitLength() * down_data_rate)<< " trans to onu "<< endl ;
                    if (downEnd[destIdx]
                            != simTime() + pkt->getBitLength() * down_data_rate
                                    - MINUNIT && algo == 1) { // has error
                        cout << " error: downEnd[" << destIdx << "]="
                                << downEnd[destIdx] << " lastPkt end="
                                << simTime()
                                        + pkt->getBitLength() * down_data_rate
                                << endl;
                        cout << " WTF?!" << endl;
//                if (downEnd[destIdx]!= simTime()+pkt->getBitLength()*pow(10,-9)- MINUNIT && algo==1) {
//                    cout << " error: downEnd[" << destIdx << "]=" << downEnd[destIdx] << " lastPkt end=" << simTime() + pkt->getBitLength()*pow(10,-9) << endl;
                        endSimulation();
                    }
                }

                //pkt->getDestAddr()
                // to determine this packet use which channel to transmit
                //if ( !pkt->getLastPkt() ) { // not the last packet
                if (channel == 1) {

                    nxtTxEnd = simTime()
                            + (down_data_rate) * pkt->getBitLength(); // need mod
                    //cout << " Transmit channel 11 !!!!!!!!!!!!!!!!!!!!!!"<< endl ;


                    send(pkt, downO);
                    readyVec.erase(readyVec.begin());

                      scheduleAt(nxtTxEnd, sendCH1DownstreamEvent); // Duration to avoid collision in real channel
                }

                else if (channel == 2) {
                    nxtTxEnd2 = simTime()
                            + (down_data_rate) * pkt->getBitLength(); // need mod
                    //cout << " Transmit channel 22 !!!!!!!!!!!!!!!!!!!!!!<< endl;

                    send(pkt, down2O);
                    readyVec2.erase(readyVec2.begin());


                      scheduleAt(nxtTxEnd2, sendCH2DownstreamEvent); // Duration to avoid collision in real channel
                }

                else if (channel == 3 ) {
                    nxtTxEnd3 = simTime()
                            + (down_data_rate) * pkt->getBitLength(); // need mod
                                    //cout << " Transmit channel 3 !!!!!!!!!!!!!!!!!!!!!!" << endl ;

                    send(pkt, down3O);
                    readyVec3.erase(readyVec3.begin());

                      scheduleAt(nxtTxEnd3, sendCH3DownstreamEvent); // Duration to avoid collision in real channel
                }

                else if ( channel == 4 ){
                    nxtTxEnd4 = simTime()
                            + (down_data_rate) * pkt->getBitLength(); // need mod
                                    //cout << " Transmit channel 4 !!!!!!!!!!!!!!!!!!!!!!" << endl ;

                    send(pkt, down4O);
                    readyVec4.erase(readyVec4.begin());

                      scheduleAt(nxtTxEnd4, sendCH4DownstreamEvent); // Duration to avoid collision in real channel
                }



            } // channel no packet to transmit


            if (readyVec.empty() && readyVec2.empty() && readyVec3.empty()
                    && readyVec4.empty()) {
                //cout << "go next transmit cycle !" << endl;
                if ( active_ch >= 1 ) cancelEvent(sendCH1DownstreamEvent) ;
                if ( active_ch >= 2 ) cancelEvent(sendCH2DownstreamEvent) ;
                if ( active_ch >= 3 ) cancelEvent(sendCH3DownstreamEvent) ;
                if ( active_ch >= 4 ) cancelEvent(sendCH4DownstreamEvent) ;
                scheduleAt(simTime(), sendGateEvent);  // trigger next transmit cycle
            }

        }

    } // is self message
    else //network message
    {
        if (msg->getArrivalGate() == gate("gen")) // Store and classify MyPacket
            classify(msg);
        else if (strcmp(msg->getName(), "Ready") == 0) {    // " Polling Start "
            delete msg;
            upStreamScheduler();

        } else if (strcmp(msg->getClassName(), "MPCPReport") == 0) {
            recvReport(check_and_cast<MPCPReport *>(msg));
            delete msg;
        }
    }
}

void DBA::finish() {
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir = "/home/you/results/";
#endif
    stringstream path;
    path << dir << "DBA.txt";
    ofstream out;
    out.open(path.str().c_str(), ios::out | ios::app);
//    for (uint16_t i=0; i!=onuSize; i++) {
//        out <<  i << " DL: " << (double)accumulateDownstreamBytes[i]*8/pow(10,6) << "M bits"
//            << " Core net: " << (double)accumulateFromCoreNet[i]*8/pow(10,6) << "M bits;"
//            << " q[0] length= " << qMap[i][0].length() << "(" << byteMap[i][0] << " Bytes);"
//            << " q[1] length= " << qMap[i][1].length() << "(" << byteMap[i][1] << "Bytes);";
//        out << " dropCount: " << dropCount[i][0] + dropCount[i][1] << endl;
//    }

    for (uint16_t i = 0; i != onuSize; i++) {
        out << i << " DL: "
                << (double) accumulateDownstreamBytes[i] * 8 / pow(10, 6)
                << "M bits" << " Core net: "
                << (double) accumulateFromCoreNet[i] * 8 / pow(10, 6)
                << "M bits;" << " q[0] length= " << qMap[i][0].length() << "("
                << byteMap[i][0] * 8 / pow(10, 6) << " MBits);"
                << " q[1] length= " << qMap[i][1].length() << "("
                << byteMap[i][1] * 8 / pow(10, 6) << " MBits);";
        out << " dropCount: " << dropCount[i][0] + dropCount[i][1] << "; "
                << "down[0]: " << downLink[i][0] * 8 / pow(10, 6) << " MBits);"
                << "down[1]: " << downLink[i][1] * 8 / pow(10, 6) << " MBits);"
                << endl;
    }

    out << " cycle count = " << cycleCount << ", avg cycleTime = "
            << cycle_accu / cycleCount << " , cycle accu = " << cycle_accu
            << endl;
    out << "\n\n\n";
    out.close();

    //measure the credit Size
    stringstream path2;
    path2 << dir << "ReqLen.txt";
    ofstream out2;
    out2.open(path2.str().c_str(), ios::out | ios::app);

    //mark loading
    cModule *locNet = simulation.getModuleByPath("EPON.localNetwork[0]"), *gen =
    simulation.getModuleByPath("EPON.trafficGen[0]");
    double upLoad = locNet->par("offered_load"), downLoad = gen->par("offered_load");
    out2 << upLoad * 100 << "% " << downLoad * 100 << "% "
            << " minReqLen  avgReqLen  maxReqLen countReqZero countReqAll countReqZeroRatio"
            << endl;

    for (uint16_t i = 0; i != onuSize; i++) {
        out2 << i << " " << minReqLen[i] << " " << avgReqLen[i] << " "
                << maxReqLen[i] << " " << countReqZero[i] << " "
                << countReqAll[i] << " "
                << (double) countReqZero[i] / countReqAll[i] << "\n";
    }
    out2 << " meanMinReqLen=" << meanMinReqLen << " meanAvgReqLen="
            << meanAvgReqLen << " meanMaxReqLen" << meanMaxReqLen << endl;
    out2 << "\n\n\n";
    out2.close();

    stringstream path3;
    path3 << dir << "DownQueueSize.txt";
    ofstream out3;
    out3.open(path3.str().c_str(), ios::out | ios::app);

    out3 << upLoad * 100 << "%  " << downLoad * 100 << "% " << endl;

    for (uint16_t i = 0; i != onuSize; i++) {
        out3 << i << " AvgDownQueueSize: " << totalQueueSize[i] / GATE_cycle[i]
                << " MB, GATE cycle: " << GATE_cycle[i] << endl;
    }

    out3 << "\n\n\n" << endl;
    out3.close();

    stringstream path4;
    path4 << dir << "AvgUseChannel.txt";
    ofstream out4;
    out4.open(path4.str().c_str(), ios::out | ios::app);
    out4 << upLoad * 100 << "%  " << downLoad * 100 << "% " << endl;

    simtime_t avgChannaltime = 1;
    //cout << fourChtime << " " <<threeChtime <<" "<< twoChtime <<" "<< oneChtime << endl ;
    if (fourChtime > 0) // means use four channel
        avgChannaltime = (1 * oneChtime + 2 * twoChtime + 3 * threeChtime + 4 * fourChtime + active_ch * (simTime() - lastChangeChtime)) / simTimeLimit;
    else if (threeChtime > 0)  // means use three channel
        avgChannaltime = (1 * oneChtime + 2 * twoChtime + 3 * threeChtime + active_ch * (simTime() - lastChangeChtime)) / simTimeLimit;
    else if (twoChtime > 0) // means use two channel
        avgChannaltime = (1 * oneChtime + 2 * twoChtime + active_ch * (simTime() - lastChangeChtime)) / simTimeLimit;

    if ( lastChangeChtime < 1 ) avgChannaltime = active_ch * (simTime() - 1 ) / simTimeLimit;
    if (  avgChannaltime > 4 ) avgChannaltime = 4 ;

    out4 << "avgUseChannal " << avgChannaltime << endl;

    out4 << "\n\n\n" << endl;
    out4.close();

    // ------------ delete
    qMap.clear();
    readyVec.clear();
    readyVec2.clear();
    readyVec3.clear();
    readyVec4.clear();
    tempVec.clear();

}
//---
void DBA::syncState(uint32_t idx, bool dozeAck, bool sleepAck) {
    simtime_t singleTripDelay = onutbl->getEntry(idx)->getRTT() / 2,
            sendReportTime = simTime() - singleTripDelay + MINUNIT;
    if (sleepAck) {
        simtime_t downEndTime = downEnd[idx] + singleTripDelay + MINUNIT,
                lastTransmitTime =
                        (sendReportTime > downEndTime) ?
                                (simTime() - sendReportTime) :
                                (simTime() - downEndTime), remainTime =
                SLEEPTIME * pow(10, -3) - lastTransmitTime;
        onutbl->clockONUTimer(idx, SLEEP, remainTime);

       //if (idx == 18 && simTime() > 0.78 )
          //cout << "t=" << simTime() << " OLT sync onu[" << idx << "] and think wake up at = " << simTime()+remainTime << endl;
    } else if (dozeAck) {
        simtime_t remainTime = DOZETIME * pow(10, -3)
                - (simTime() - sendReportTime);
        onutbl->clockONUTimer(idx, DOZE, remainTime); // version 1 will ignore remainTime
    } else if (onutbl->getEntry(idx)->getState() != ACTIVE) // Early wake up
        onutbl->earlyWakeProcess(idx, onutbl->getEntry(idx)->getState()); // cancel timer and change state
}

void DBA::recvReport(MPCPReport * rep) {
    uint32_t onuIdx = rep->getSrcAddr() - 2;

    //if ( onuIdx == 18 && simTime() > 0.78 ) cout << "recv ONU 18 report at time : " << simTime() << endl ;
    //cout <<"time :  "<< simTime() << " recv report form : " << onuIdx << endl ;

    //for credit based
//    cout << "get Final GrantLen = " << rep->getFinalGrantLen() << endl << endl;

    finalGrantLen = rep->getFinalGrantLen();
    recvRepoTime = simTime();

    onurequestsize[onuIdx] = rep->getRequestLen() ;

    if (rep->getInvisible() == false) // receive REPORT and it's real
            {
        bool sleepAck = rep->getSleepAck(), dozeAck = rep->getDozeAck();
        //if (sleepAck || dozeAck)
            //cout << "DBA: onu[" << onuIdx << "] t=" << simTime() << " decide to " << (sleepAck? "sleep mode\n":"doze mode\n");

        syncState(onuIdx, dozeAck, sleepAck);
    }
//    else
//        cout << "DBA: onu[" << onuIdx << "] t=" << simTime() << " recv invisible REPORT" << "\n";

    isUpEnd[onuIdx] = true;
    //isWaitReport[onuIdx] = false ;
    if ( hasTransmitDown[onuIdx] && isUpEnd[onuIdx]) {
    //if ( isWaitReport[onuIdx]) {
        isWaitReport[onuIdx] = false ;
        if (!sendGateEvent->isScheduled())
          scheduleAt(simTime() + MINUNIT, sendGateEvent);
    }
    // Check error
    // simTime => recieve REPORT time
    upEnd[onuIdx] = simTime() ;

    if (upEnd[onuIdx] < simTime() - 2 * pow(10, -12)) {
        cout << "simtime=" << simTime() << " upEnd[" << onuIdx << "]="
                << upEnd[onuIdx] << " exceed:" << upEnd[onuIdx] - simTime()
                << endl;
        endSimulation();
    }

    //bool sleepAck = rep->getSleepAck(), dozeAck = rep->getDozeAck();

//    if (simTime()> 4.09968133  && simTime()< 4.100261803 && onuIdx ==4){
//        cout << "(receREPORT)" << rep->getInvisible() << "t= " << simTime() << "   onuSTATE " << onutbl->getEntry(onuIdx)->getState() << " " << sleepAck << ","<< dozeAck<< endl;
//        if ((sleepAck || dozeAck))
//            cout << "DBA: onu[" << onuIdx << "] t=" << simTime() << " decide to " << (sleepAck? "sleep mode\n":"doze mode\n");
//
//    }
}
void DBA::upStreamScheduler()   // call by event: sendGateEvent
{
    //if (isUpEnd[targetIdx] && isDownEnd[targetIdx])
    bool allcomplete = true ;
    //cout << "here is upStreamScheduler ! " << endl;
    for( uint32_t index = 0 ; index < onuSize; index++ ) {
        uint16_t curMode = onutbl->getEntry(index)->getState();

        if ( !isDownEnd[index] ) allcomplete = false ;
        if ( curMode == ACTIVE && (  !isUpEnd[index] || isWaitReport[index] )) {

            allcomplete = false ;
            //isWaitReport[index] = true;
            //cout << "index : " << index << " is waiting " <<" upend? "<<isUpEnd[index] << " downend? " << isDownEnd[index] << " iswaitreport ? "<< isWaitReport[index] << endl ;

        }

        /*
        if (index == 18  && simTime() >  0.78 )
            cout << "INDEX : 18 "<< curMode <<" upend? "<<isUpEnd[index] << " downend? " << isDownEnd[index] << " iswaitreport ? "<< isWaitReport[index] << endl ;
        */
    }


    if ( allcomplete ) {
        //cout << "Time : " << simTime() << " Polling start ! " << endl ;
        notSleepOnu = 0 ;
        cycleAllPacketSize = 0 ;
        for (targetIdx = 0; targetIdx < onuSize; targetIdx++) {

            isUpEnd[targetIdx] = false;
            isDownEnd[targetIdx] = false;
            isWaitReport[targetIdx] = false;
            hasTransmitDown[targetIdx] = false ;
            polling(targetIdx);

        }
        //targetIdx+1 == onuSize ? targetIdx =0: targetIdx++;
        targetIdx = 0;
        if (targetIdx == 0) {
            curCycleTime = simTime() - lastCycleTime;
            lastCycleTime = simTime();
            if (simTime() > 1) {
                cycle_accu = simTime().dbl() - 1;
//                cycle_accu += curCycleTime.dbl();
                cycleCount++;
            }
        }
    }

    /*
    else {
        if( !allcomplete ) {
          if (!sendGateEvent->isScheduled())
            scheduleAt(simTime(), sendGateEvent);
        }
    }

    else {
        isWaitReport[targetIdx] = true;
        //if (isUpEnd[targetIdx]) //marked by clare
        //cout << "DBA [" << targetIdx << "] Upstream is end; Predict downEnd=" << downEnd[targetIdx] << endl; //marked by clare
        //if (isDownEnd[targetIdx]) //marked by clare
        //cout << "DBA [" << targetIdx << "] Downstream is end; Predict upEnd=" << upEnd[targetIdx] << endl; //marked by clare
    }
    */
}

void DBA::polling(uint32_t idx) {
    //uint32_t reqLength = onutbl->getEntry(idx)->getComTime().length;
    uint32_t reqLength = onurequestsize[idx] ;
    //if ( idx == 3 && requestSize < reqLength ) requestSize = reqLength ; //not sure what is it. unused.

//    cout << "onu[" << idx << "]" << " reqLength=" << reqLength << endl;
//    cout << "last cycle finalGrantLen in poll OLT side = " << finalGrantLen << endl;

    lastReqLen = reqLength;
//    cout << "onu[" << idx << "] lastReqLen = " << lastReqLen << endl;
    lastUpBW = reqLength;

    //reserve this and check
    //uint32_t crdReqLen= 0;
    arvReqLen = reqLength - lastReqLen + finalGrantLen;

//    cout << "onu[" << idx << "] arvReqLen = reqLength - lastReqLen + finalGrantLen = " << arvReqLen << endl;
//    cout << "onu[" << idx << "] waitTime = " << waitTime << endl;

//    if (waitTime != 0)
//    cout << "onu[" << idx << "] arvReqLen/ waitTime = " << arvReqLen/ waitTime << endl;

    //store recent arrival rate for each onu[idx] and put into its own cycle buffer
    //In first cycle, the wait time of ONU 0 is 0. It causes error nan (not a number).
    //Ignore the cases of 0 waiting time
    double waitTimeInDouble = waitTime.dbl();
    //to avoid case (1)first cycle wait time is 0 and (2)suddenly arrive packets after traffic is silent a while and staring again with minimum time unit 1e-012
    //case(1) induces divide by 0 i.e. not a number (nan)
    //case(2) invokes very large arrival rate result in giving MTW in alg to certain ONU that wastes bandwidth
    if (waitTimeInDouble > 1e-012)
        //buffers the averaged arrival rate avgArvRate within waiting time
        window[idx][widx[idx]] = arvReqLen / waitTimeInDouble;

//    cout << "onu[" << idx << "] arvReqLen " << arvReqLen << endl;
//    cout << "onu[" << idx << "] waitTime " << waitTimeInDouble << endl;
//    cout << "onu[" << idx << "] currentWinRate = arvReqLen / waitTime = " << window[idx][widx[idx]] << endl << endl;

    double sumRate = 0;
    for (uint16_t i = 0; i != winSize; ++i)
    //for (uint16_t i=0; i!=2; ++i) //testing only 1 window
            {
        //cout << "before sumRate = " << sumRate << endl;
//            cout << "onu[" << idx << "] Window in loop " << window[idx][i] << endl;
        sumRate = sumRate + window[idx][i];
        //cout << "after sumRate = " << sumRate << endl;

    }
    //cout << "winSize = " << winSize << endl;
    //cout << "sumRate = " << sumRate << endl;

    avgArvRate[idx] = sumRate / winSize;
    //avgArvRate[idx] = sumRate / 1; //testing only 1 window

    //replace old avgArvRate with a new avgArvRate
    //avgArvRate[idx] = avgArvRate[idx] - lastWindow[idx][widx[idx]] + window[idx][widx[idx]];
//    cout << "onu[" << idx << "] avgArvRate = " << avgArvRate[idx] << endl;

    //record old window[idx][widx[idx]]
    lastWindow[idx][widx[idx]] = window[idx][widx[idx]];
    //cout << "onu[" << idx << "] last window " << lastWindow[idx][widx[idx]] << endl;

    //cycling window
    widx[idx] = (widx[idx] + 1) % winSize;
    //widx[idx] = 0; //testing 1 window

    //int32_t RTW;
    //double creditRatio = 0;
    //cout << "onu[" << idx << "]" << " pre-avgReqLen=" << avgReqLen[idx] << endl;

    //count number of any size request
    countReqAll[idx]++;

    //min and max requestLength
    if (reqLength > maxReqLen[idx]) {
        maxReqLen[idx] = reqLength;
        meanMaxReqLen = reqLength; //max request length among all ONUs
    }
    if (reqLength < minReqLen[idx]) {
        minReqLen[idx] = reqLength;
        meanMinReqLen = reqLength; //min request length among all ONUs
    }
    if (reqLength == 0) {
        countReqZero[idx]++;
    }

    //average requestLength
    avgReqLen[idx] = (avgReqLen[idx] + reqLength) / 2;
    meanAvgReqLen = (meanAvgReqLen + reqLength) / 2;
    //cout << "onu[" << idx << "]" << " avgReqLen[" << idx << "]=" << avgReqLen[idx] << endl;

    //IF CREDIT
    //v1
    //crdReqLen = reqLength + (reqLength*creditRatio) + minCrdLen;

    //v2: max( minCrdLen, reqLength * ( 1 + creditRatio ) ;minCrdLen is for case of request size is zero to prevent giving a credit size of 0
    //if ( minCrdLen <  reqLength * ( 1 + creditRatio ) )
    //    crdReqLen = reqLength * ( 1 + creditRatio );
    //else
    //    crdReqLen = minCrdLen;

    //v3: credit by mean value of request length, here creditRatio = 1 (half distribution size) / 2 (full distribution size )
    //max(reqLenght, avgReqLen * creditRatio)
    //if (reqLength < avgReqLen * creditRatio)
    //    crdReqLen = avgReqLen * creditRatio;
    //else  //no quota case
    //    crdReqLen = reqLength;
    //v4: predict credit by avgArvRate with window size 10
    //first part is of current request =reqLength,
    //the second part is the size of prediction for next service cycle

    //double predictFn = avgArvRate[idx] *  waitTimeInDouble / winSize;
    double predictFn = avgArvRate[idx] * waitTimeInDouble;
//    cout << "waitTime in double" << waitTimeInDouble << endl;
//    cout << "avgArvRate[idx] = " << avgArvRate[idx] << endl;
//    cout << "avgArvRate[idx]  * waitTime = " << avgArvRate[idx] * waitTimeInDouble << endl;

    if (creditBased)
        crdReqLen = reqLength + (predictFn) * (1 + creditRatio);
    else
        crdReqLen = reqLength;

    //cout << "current giving crdReqLen=" << crdReqLen << endl;

    //check MTW
    if (asymmetric_flow) {
        if (idx < 8) {
            if ((crdReqLen > MTU_high) || (setMTW == 1))
                crdReqLen = MTU_high;
        } else {
            if ((crdReqLen > MTU_low) || (setMTW == 1))
                crdReqLen = MTU_low;
        }
    } else if (crdReqLen > MTU) {
        crdReqLen = MTU; //0.2msx100Mbps=2500bytes/MTU
    }

//    cout << "final giving crdReqLen=" << crdReqLen << endl;
//    cout << "current predict len = predictFn = avgArvRate[idx] * waitTime = "  << predictFn << endl;
//    cout << "final crdReqLen = reqLength + predictFn * ( 1 + creditRatio) = " << crdReqLen << endl << endl;

    grantUpLen[idx] = crdReqLen;
    //record last time given credit size
    lastCrdReqLen = crdReqLen;
    //cout << "current grantUpLen[" << idx << "]=" <<  grantUpLen[idx] << endl;
//    cout << "OLT polling get ONU[" << idx <<"] request length + length*credit =" << reqLength << "+" << reqLength*creditRatio << "=" << crdReqLen << endl;
//    cout << "MTU=" << MTU << " creditRatio=" << creditRatio << " reqLength=" << reqLength << " grantUpLength[" << idx << "]=" << grantUpLen[idx] << endl;

    uint16_t curMode = onutbl->getEntry(idx)->getState();

    downloading(idx); // decide loading whether exceed threshold

    if (curMode != SLEEP/*onutbl->getEntry(idx)->getState()!=SLEEP*/) { //onutable is active or doze
//        downloading(idx);
        uint32_t downQueueSize = byteMap[idx][0] + byteMap[idx][1];
        bool downQueueLight = true;
        if ( dynamic_mode) {
            if (MTW_algo)
                downQueueLight = (downQueueSize < multiMTW * MTU) ? true : false;
            if (downQueueLight && HP_must_empty)
                downQueueLight = (qMap[idx][0].isEmpty()) ? true : false;
        }else
            downQueueLight = true;

        //if ( qMap[idx][0].isEmpty() ) cout <<"idx : "<< idx <<" no hp arrival !" << endl ;
        //else cout << "idx : "<< idx <<" has hp arrival !!!!!!!!!!" << endl ;

        //cout << "[" << idx << "]" <<  downRate[idx] << ", " << downThreshold << ", " <<  downQueueLight << ", " << downQueueSize << endl;

        if (modes == dynamic_mode && downRate[idx] <= downThreshold && downQueueLight) {
//            cout << curMode << ",  " << grntUpLen[idx] << endl;
//            grantDownLen[idx] = 0;
//            cout << "[" << idx << "]" << byteMap[idx][0] << " + " << byteMap[idx][1] << ", " << byteMap[idx][0] + byteMap[idx][1] << endl;
//            cout << "[" << idx << "]" << byteMap[idx][0] << " + " << byteMap[idx][1] << ", " <<  downQueueSize << endl;

            if (curMode == ACTIVE && grantUpLen[idx] == 0) // upstream is light.
                grantDownLen[idx] = 0;
            else if (curMode == DOZE) {
                grantDownLen[idx] = 0;
            } else {  // upstream not light.
                grantDownLen[idx] = 0;
                /*
                grantDownLen[idx] = getDownstreamData(idx);
                cycleAllPacketSize = cycleAllPacketSize + grantDownLen[idx] ;
                */
            }
        } else {
            notSleepOnu ++ ;
            grantDownLen[idx] = getDownstreamData(idx); // getDownstreamData() put traffic to tempVec
            //cout << "idx : " << idx <<" grant : "<< grantDownLen[idx] << endl ;
            cycleAllPacketSize = cycleAllPacketSize + grantDownLen[idx] ;
        }
//        if (onutbl->getEntry(idx)->getState()==ACTIVE)
//            cout << "t=" << simTime() << "OLT check onu["<< idx <<"] is ACTIVE" << endl;
//        if (onutbl->getEntry(idx)->getState()==DOZE)
//            cout << "t=" << simTime() << "OLT check onu["<< idx <<"] is DOZE" << endl;
//        cout << "t=" << simTime() << " OLT check onu["<< idx <<"] grant up and downlen as requested size, next will send Gate" << endl;
//        cout << "t=" << simTime() << " OLT check onu["<< idx <<"] grantUpLen=" << grantUpLen[idx] <<  " grantDownLen =" << grantDownLen[idx] << endl;
    } else { //onutable is sleep

        grantDownLen[idx] = 0;
//        cout << "t=" << simTime() << "OLT check onu["<< idx <<"] is sleep and grantDownlen=0, next will sendGate" << endl;
    }

//    if (simTime()> 4.099 && simTime()<4.12 && idx==4 /*&& (reqLength!=0 && grantUpLen[idx]!=0)*/){
//    cout << "(Polling)  " << simTime() << "   " ;
//    cout << "onu[" << idx << "] " << curMode << "   reqLength=" << reqLength<< " grant=" << grantUpLen[idx]<< endl;}

    //Gate can be sent with grantDownLen=0 when
    //sendGateMessage(idx, grantUpLen[idx], grantDownLen[idx]); // put GATE to readyVec //control msg
    /*
    if ( idx == 2 ) {
        cout << "grant : " << grantDownLen[idx] << "mode : "<< curMode << endl ;
        cout <<  "simtime : " << simTime() << " get sleep arrival" << onutbl->getSleepArrivalTime(idx) << endl ;

    }
    */
    if (idx+1 == onuSize ) { // last onu

        //cout << "cycleAllPacketSize:" << cycleAllPacketSize << endl;
        //cout << "cycleTime: " << SIMTIME_DBL(newGATE[idx] - lastGATE[idx]) << endl;
        //if (firstgetidx) {

        last_active_ch = active_ch ;
        //if( last_active_ch < 1 ) last_active_ch = 1 ;
        averageChannelCapacity = (((double) cycleAllPacketSize) * 8.0) / 50000000; // onu * 0.006 cycle time, 128 ONU if cycleTime still 2ms , then every ONU 0.015625ms
        active_ch = ceil( averageChannelCapacity ) ;
        if ( active_ch > 4 ) active_ch = 4 ;
        if ( active_ch < 1 ) active_ch = 1 ;

        //active_ch = 4 ;

        //packet_rate = (((double) cycleAllPacketSize) * 8.0)/ SIMTIME_DBL(newGATE[31] - newGATE[0])  ;

        //  firstgetidx = false ;
        //}
        // else

          //packet_rate = (((double) cycleAllPacketSize) * 8.0)/ SIMTIME_DBL(newGATE[idx] - lastGATE[idx]) ;

        //cout << "active_ch : " << active_ch << endl;
        activeChannel() ;
        if ( randomChannelAssign ) { // if randomChannelAssign is FALSE, use Hungarian.
            //random
            int count = ceil(onuSize/active_ch);
            hasSelect.assign(active_ch,count);
        } else {
            //Hungarian.
            onuCost.clear() ;
            onuCost.assign(onuSize,0) ;
            costMatrix.clear() ;
            costMatrix.assign(active_ch, onuCost ) ;
            //cout << "do produce cost " << endl ;
            produceCost();
            //cout << "do Hungarian " << endl ;
            hungarian() ;
        }

        for ( idx = 0 ; idx < onuSize ; idx ++ ) {

          if ( randomChannelAssign ){
              sendGateMessage(idx, grantUpLen[idx], grantDownLen[idx]); // put GATE to readyVec //control msg
          } else {
              assignChannel = channelAssignment[idx].second ;
              sendGateMessage(channelAssignment[idx].first, grantUpLen[channelAssignment[idx].first], grantDownLen[channelAssignment[idx].first]);
          }
          //cout << "idx : " << idx << "sendgate " << endl ;
        }
        //cout << " sendDownstreamEvent !" << endl;
        if (!sendDownstreamEvent->isScheduled()) //data traffic
            scheduleAt(simTime(), sendDownstreamEvent); // gate is sended in function sendGateMessage()
    }
}

void DBA::nxtGateProcess(uint32_t idx)  // call by event: sendDownstreamEvent
        {

    //uint32_t nxtIdx=idx+1;
    //(nxtIdx == onuSize)? nxtIdx=0 : nxtIdx=nxtIdx;
    //cout << "here is next gate process " << endl;
    //if (idx != -1) {

        simtime_t nextGate,
//              ns=1*pow(10,-9),
                idxRTT = onutbl->getEntry(idx)->getRTT();

        //nextIdxRTT=onutbl->getEntry(nxtIdx)->getRTT();
        upEnd[idx] = simTime() + (64 + grantUpLen[idx] + 64) * 8 * up_data_rate
                + idxRTT - MINUNIT; //check ok
//    upEnd[idx]   = simTime() + (64+grantUpLen[idx]+64)*8*ns + idxRTT - MINUNIT; //check ok
//    cout << "upEnd[" << idx << "]=" << "simtime:" << simTime() << "+8*down_data_rate*(G,R," << grantUpLen[idx] << ") + RTT:" << idxRTT << "-MINUNIT\n"; //marked by clare

        downEnd[idx] = simTime() + (64 + grantDownLen[idx]) * 8 * down_data_rate
                - MINUNIT;  // at least... may be insert
        uint16_t curMode = onutbl->getEntry(idx)->getState();
        //if ( idx == 7 && simTime() > 0.55 ) cout << " ONU 7 downEnd : " <<  downEnd[idx] << endl ;
        if ( curMode == ACTIVE ) {
            isWaitReport[idx] = true ;
            hasTransmitDown[idx] = true ;
        }
//    downEnd[idx] = simTime() + (64+grantDownLen[idx])*8*ns - MINUNIT ;  // at least... may be insert

//      cout << "t=" << simTime() << " upEnd[" << idx << "]=" << upEnd[idx] << endl; //marked by clare
//      cout << "t=" << simTime() << " downEnd[" << idx << "]=" << downEnd[idx] << endl;

        // (1) The best time is consider no downstream. => optimize upstream channel's utilization
        //  nextGate     = upEnd[idx] - nextIdxRTT + MINUNIT - 64*8*down_data_rate + GUARD_INTERVAL;  //check ok

        //timeSetter();
        // NextGate is not very accurate, because ONU (1)should get REPORT first, then do gate.
        /*
         if (algo==0 && nextGate <= downEnd[idx] ) // insert Gate
         {
         for (uint32_t i=0; i!=readyVec.size(); ++i)
         {
         if (readyVec[i].second->getKind() == MPCP_TYPE) continue;  // CANT"T SPLIT GATE
         MyPacket * pkt = readyVec[i].second;
         simtime_t pktStart = readyVec[i].first,
         pktEnd   = pktStart + pkt->getBitLength()*down_data_rate ;
         //                      pktEnd   = pktStart + pkt->getBitLength()*pow(10,-9);
         if (nextGate <= pktEnd && nextGate > pktStart) {
         splitPkt(i, nextGate);
         break;
         }
         }
         }
         else if (algo==1 && nextGate <= downEnd[idx]) {
         // (2) Consider downstream. => after the N-1 target downstream end.
         nextGate = downEnd[idx]+MINUNIT;
         }
         */
        //cout<< "nxt gate process down !" << endl ;
    //} // if idx != -1
      // (3) We have another one nextGate condition => After one cycle.. need to wait REPORT received( need to know ONU's require)


    //else
        //scheduleAt(simTime(), sendGateEvent);


    //scheduleAt(nextGate, sendGateEvent);    // let it run in first cycle.... later, need wait transmit end.
//    cout << "DBA_nxtG: [" << idx << "] nxtGateTime=" << nextGate << endl; //marked by clare
}

void DBA::splitPkt(int idx, simtime_t nextGate) {
    simtime_t effectStart = readyVec[idx].first;
    MyPacket* effectPkt = readyVec[idx].second;
    MyPacket* pkt1 = effectPkt->dup();
    MyPacket* pkt2 = effectPkt->dup();

    if (effectPkt->getKind() == MPCP_TYPE) {
        cout << "CANT't split GATE!\n";
        endSimulation();
    }

    int64_t totalSize = effectPkt->getBitLength();
    int64_t pkt1Size = pow(10, 9) * (nextGate - effectStart).dbl() + 1; // +1 to let pkt2 and gate contention
    int64_t pkt2Size = totalSize - pkt1Size;

    pkt1->setBitLength(pkt1Size);
    pkt2->setBitLength(pkt2Size);

    if (pkt1->getLastPkt())
        pkt1->setLastPkt(false);    // last is packet_2

    delete effectPkt;
    readyVec[idx].second = pkt1;
    readyVec.insert(readyVec.begin() + idx + 1,
            make_pair(readyVec[idx].first, pkt2));
    pkt2->setName("MyPacket_split");
    pkt2->setLastPkt(true);

}

void DBA::sendGateMessage(uint32_t idx, uint32_t grantUpLen,
        uint32_t downLength) {
    MPCPGate * gt = new MPCPGate("MPCPGate", MPCP_TYPE);
    gt->setOpcode(MPCP_GATE);
    gt->setDestAddr(onutbl->getEntry(idx)->getLLID());
    gt->setStartTime(onutbl->getEntry(idx)->getComTime().start); // current not use
    gt->setLength(grantUpLen);                                          // ��
    gt->setDownLength(downLength);  // current downstream data
    gt->setByteLength(64);
    gt->setTimestamp();
    gt->setCycleTag(cycleCount);
    gt->setQueueLength(0, qMap[idx][0].length());
    gt->setQueueLength(1, qMap[idx][1].length());
//------------ set transmit channel ---------
    uint16_t channel = 1;

    if (randomChannelAssign ) {
        if (randomOrRR){//if true , use Random Assignment, else us RR assignment
            int index = rand()%active_ch ;
            while( hasSelect[index] < 0 )
                index = rand()%active_ch ;
            hasSelect[index] -- ;
            channel = index + 1 ;
            gt->setTransmitChannel(channel);
            onutransmitchannel[idx] = channel;
        }else{
            //this is round robin
            if (active_ch == 1) {
                gt->setTransmitChannel(1);
            } else if (active_ch == 2) {
                channel = idx % 2 + 1;
            } else if (active_ch == 3) {
                channel = idx % 3 + 1;
            } else {
                channel = idx % 4 + 1;
            }
            //cout << "idx : " << idx << "channel : " << channel << endl ;
            gt->setTransmitChannel(channel);
            onutransmitchannel[idx] = channel;
        }
    }else{
        //Hungarian
        channel = assignChannel + 1 ;
        gt->setTransmitChannel(channel);
        onutransmitchannel[idx] = channel;
    }

// ----------- loading aware ----------------
    gt->setDownQueueLoading(downRate[idx]);

    downLength == 0 ? gt->setLastPkt(true) : gt->setLastPkt(false);

    uint16_t curMode = onutbl->getEntry(idx)->getState();

    //for credit based
    sendGateTime = simTime();
    waitTime = sendGateTime - recvRepoTime;

    if (version == 0) // Larry mode
            {
        if (grantUpLen == 0 && curMode == ACTIVE) {
            if (downLength == 0) {
                gt->setPowerSavingMode(SLEEP);
                gt->setPowerSavingTime(SLEEPTIME * pow(10, -3));
            } else {
                gt->setPowerSavingMode(DOZE);
                gt->setPowerSavingTime(DOZETIME * pow(10, -3));
            }
        } else if (curMode == ACTIVE)
            gt->setName("MPCPGate");
        else if (curMode != ACTIVE)
            gt->setName("Force Gate");
    } else if (version == 1) {
        if (curMode != SLEEP && downLength == 0) {
            // Let ONU make decision.
            if (curMode == ACTIVE)
                gt->setName("MPCPGate");
            else if (curMode == DOZE && downLength == 0) // Instruct ONU to power saving //this may cause problem ONU in sleep but OLT change to doze while pkt arrive and then send pkts
                    {
                gt->setName("Force Gate");
                simtime_t sleepDuration = SLEEPTIME * pow(10, -3)
                        + onutbl->getEntry(idx)->getRTT() / 2
                        + 64 * 8 * down_data_rate;
//                simtime_t sleepDuration = SLEEPTIME*pow(10,-3) + onutbl->getEntry(idx)->getRTT()/2 + 64*8*pow(10,-9);
//                cout << "rtt/2=" << onutbl->getEntry(idx)->getRTT()/2 << endl;

                onutbl->clockONUTimer(idx, SLEEP, sleepDuration);
//                cout << "t=" << simTime() << " OLT set onu[" << idx <<"] sleep and wakeuptime=" << sleepDuration << endl;
//                endSimulation();
                gt->setPowerSavingMode(SLEEP);
                gt->setPowerSavingTime(SLEEPTIME * pow(10, -3));
            }

        }
        //byclare
        if (curMode == DOZE && downLength != 0) {
            gt->setName("Force Gate");
            onutbl->clockONUTimer(idx, DOZE, -1);
            gt->setPowerSavingMode(DOZE);
            gt->setPowerSavingTime(3);
        }
    }

    if (algo == 0) {   // insert algo
        readyVec.insert(readyVec.end(), tempVec[idx].begin(), tempVec[idx].end());
        if (!readyVec.empty() && readyVec[readyVec.size()-1].second->getKind() != MPCP_TYPE) // Find a space to insert GATE
        {
            for (uint32_t i = 0; i != readyVec.size(); i++) {
                if (readyVec[i].second->getKind() == MPCP_TYPE)
                    continue;
                else {
                    readyVec.insert(readyVec.begin() + i,
                            make_pair(simTime(), gt));
                    break;
                }
            }
        } else
            readyVec.push_back(make_pair(simTime(), gt)); //send(gt, downO);
    } else if (algo == 1) {   // append algo

        tempVec[idx].insert(tempVec[idx].begin(), make_pair(simTime(), gt));
        if (channel == 1)
            readyVec.insert(readyVec.end(), tempVec[idx].begin(), tempVec[idx].end());
        else if (channel == 2 )
            readyVec2.insert(readyVec2.end(), tempVec[idx].begin(), tempVec[idx].end());
        else if (channel == 3 )
            readyVec3.insert(readyVec3.end(), tempVec[idx].begin(), tempVec[idx].end());
        else if (channel == 4 )
            readyVec4.insert(readyVec4.end(), tempVec[idx].begin(), tempVec[idx].end());
    }
    tempVec[idx].clear();
}

int DBA::getDownstreamData(uint32_t idx) {
    uint32_t pri = 0, nextSize = 0, totalSize = 0;


    if (qMap[idx][0].isEmpty() && qMap[idx][1].isEmpty())
        return 0;

//if ( preidx > idx ) afteronecycle = true ;
    //preidx = idx ;
    pri = 0;
    nextSize = 0;

//    // calculate ( data / curcycle ) and confirm need how many channel to send
    while (pri < 2)  // 0,1 < 2
    {

        if (!qMap[idx][pri].isEmpty()) {
            nextSize = ((MyPacket *) qMap[idx][pri].front())->getByteLength();
            totalSize += nextSize;

            if (asymmetric_flow) {

                if (idx < 8) //heavy node
                        {
                    if (pri == 0) { // high priority
                        if (totalSize <= MTU_high) {
                            byteMap[idx][pri] -= nextSize;
                            MyPacket * pkt = check_and_cast<MyPacket*>(
                                    qMap[idx][pri].pop());
                            tempVec[idx].push_back(make_pair(simTime(), pkt));
                            arrivalTime[idx] = pkt->getArrivalTime() ;

                            pkt->setCycleTag(cycleCount);
                        } else {
                            totalSize -= nextSize;
                            break;
                        }
                    } else { // low priority
                        if (totalSize <= MTU_all) {
                            byteMap[idx][pri] -= nextSize;
                            MyPacket * pkt = check_and_cast<MyPacket*>(
                                    qMap[idx][pri].pop());
                            tempVec[idx].push_back(make_pair(simTime(), pkt));

                            pkt->setCycleTag(cycleCount);
                        } else {
                            totalSize -= nextSize;
                            break;
                        }
                    }
                } else // light node
                {
                    if (pri == 0) { // high priority
                        if (totalSize <= MTU_low) {
                            byteMap[idx][pri] -= nextSize;
                            MyPacket * pkt = check_and_cast<MyPacket*>(qMap[idx][pri].pop());
                            tempVec[idx].push_back(make_pair(simTime(), pkt));
                            arrivalTime[idx] = pkt->getArrivalTime() ;
                            pkt->setCycleTag(cycleCount);
                        } else {
                            totalSize -= nextSize;
                            break;
                        }
                    } else { // low priority
                        if (totalSize <= MTU_low) {
                            byteMap[idx][pri] -= nextSize;
                            MyPacket * pkt = check_and_cast<MyPacket*>(qMap[idx][pri].pop());
                            tempVec[idx].push_back(make_pair(simTime(), pkt));

                            pkt->setCycleTag(cycleCount);
                        } else {
                            totalSize -= nextSize;
                            break;
                        }
                    }
                }
            } // if (asymmetric_flow  ) {

            else { // symmetric_flow
                uint32_t dyMTU = MTU ;

//
//                if ( downRate[idx] > 300000 ) dyMTU = MTU * 2 ;
//                if ( downRate[idx] > 500000 ) dyMTU = MTU * 3 ;
//                if ( downRate[idx] > 600000 ) dyMTU = MTU * 3.5 ;
//                if ( downRate[idx] > 700000 ) dyMTU = MTU * 4 ;
//                if ( downRate[idx] > 800000 ) dyMTU = MTU * 4.5 ;
//                if ( downRate[idx] > 900000 ) dyMTU = MTU * 5 ;
//                if ( downRate[idx] > 1000000 ) dyMTU = MTU * 5.5 ;
//                if ( downRate[idx] > 1200000 ) dyMTU = MTU * 6 ;
//

                //cout << "MTW : " << dyMTU << endl ;
                if (totalSize <= dyMTU) {
                    byteMap[idx][pri] -= nextSize;
                    MyPacket * pkt = check_and_cast<MyPacket*>(qMap[idx][pri].pop());
                    tempVec[idx].push_back(make_pair(simTime(), pkt));
                    if ( pri == 0 ) arrivalTime[idx] = pkt->getArrivalTime() ;
                    pkt->setCycleTag(cycleCount);
                } else {
                    totalSize -= nextSize;
                    break;
                }

            }
        }// if ( !qMap[idx][pri].isEmpty() )
        //if( downRate[idx] <= downThreshold ) break ; // light loading only trans high priority packet
        else
            pri++;
    }
    tempVec[idx].back().second->setLastPkt(true);
    return totalSize;
}

void DBA::timeSetter() {
    simtime_t nxtStart = simTime();
    for (uint32_t i = 0; i != readyVec.size(); i++) {
        readyVec[i].first = nxtStart;
        readyVec[i].second->setTxStart(nxtStart);
        readyVec[i].second->setTxEnd(
                nxtStart + readyVec[i].second->getBitLength() * down_data_rate
                        - MINUNIT);
        nxtStart += readyVec[i].second->getBitLength() * down_data_rate;
        /*
        readyVec2[i].first = nxtStart;
        readyVec2[i].second->setTxStart(nxtStart);
                readyVec[i].second->setTxEnd(
                        nxtStart + readyVec[i].second->getBitLength() * down_data_rate
                                - MINUNIT);
                nxtStart += readyVec[i].second->getBitLength() * down_data_rate;

                readyVec[i].first = nxtStart;
                        readyVec[i].second->setTxStart(nxtStart);
                        readyVec[i].second->setTxEnd(
                                nxtStart + readyVec[i].second->getBitLength() * down_data_rate
                                        - MINUNIT);
                        nxtStart += readyVec[i].second->getBitLength() * down_data_rate;

                        readyVec[i].first = nxtStart;
                                readyVec[i].second->setTxStart(nxtStart);
                                readyVec[i].second->setTxEnd(
                                        nxtStart + readyVec[i].second->getBitLength() * down_data_rate
                                                - MINUNIT);
                                nxtStart += readyVec[i].second->getBitLength() * down_data_rate;
         */
//        readyVec[i].second->setTxEnd(nxtStart+readyVec[i].second->getBitLength()*pow(10,-9) - MINUNIT);
//        nxtStart += readyVec[i].second->getBitLength()*pow(10,-9);
    }
}

void DBA::classify(cMessage * msg) {
    /*
     *  We don't need to update "downlength" every time, it will increase cpu overhead.
     *  And, we only use update "downlength" while we are ready polling.
     */

    MyPacket * mypacket = check_and_cast<MyPacket*>(msg);
    uint16_t dest = mypacket->getDestAddr(), pri = mypacket->getPriority(),
            pktLen = mypacket->getByteLength();
    mypacket->setLastPkt(false);

    if (byteMap[dest - 2][pri] + pktLen < queueLimit)   // in Bytes
            {
        qMap[dest - 2][pri].insert(msg); // onu idx:17 .... will -2 to map to index 15
        byteMap[dest - 2][pri] += pktLen;
        if (simTime() > 1)
            accumulateFromCoreNet[dest - 2] += pktLen; // <=============================================================statistic
    } else {
        dropCount[dest - 2][pri]++;
        delete mypacket;
    }
}

void DBA::downloading(uint16_t idx) {
    uint32_t pktSize = (byteMap[idx][0] + byteMap[idx][1]) * 8;
    //uint16_t pktLength = (qMap[idx][0].length() + qMap[idx][1].length());

    //cout << " idx : " << idx <<"pktsize : " << pktSize << " bits " << " interTime : " << simTime() - newGATE[idx] << endl ;
    double loading = pktSize / pow(10, 6) / (simTime() - newGATE[idx]); // normalizeDown;

    //cout << "loading : " << loading << endl ;
    GATE_cycle[idx]++;

    int size = (byteMap[idx][0] + byteMap[idx][1]);
    totalQueueSize[idx] = totalQueueSize[idx] + size / pow(10, 6);

//    cout << "[" << idx << "], cycle: " << GATE_cycle[idx] << ", " << totalQueueSize[idx] << "(" << pktSize << ")" << endl;

    if (downRate[idx] == 0)
        downRate[idx] = loading;
    else
        downRate[idx] = dyRatio * loading + (1 - dyRatio) * downRate[idx];

    lastGATE[idx] = newGATE[idx];
    newGATE[idx] = simTime();

}

void DBA::activeChannel() {
    if ( active_ch != last_active_ch ) { // record channel time
        if ( simTime() >= 1 ) {
          if( last_active_ch == 1 ) {
              oneChtime = oneChtime + (simTime() - lastChangeChtime);
              //cout << "record one ! " << endl ;
          }
          else if( last_active_ch == 2 ) {
              twoChtime = twoChtime + (simTime() - lastChangeChtime);
              //cout << "record two ! " << endl ;
          }
          else if(  last_active_ch == 3 ) {
              threeChtime = threeChtime + (simTime() - lastChangeChtime);
             // cout << "record three ! " << endl ;
          }
          else if(  last_active_ch == 4 ) {
              fourChtime = fourChtime + (simTime() - lastChangeChtime);
              //cout << "record four ! " << endl ;
          }
        }

        lastChangeChtime = simTime();
    }



}

void DBA::produceCost() {
    vector< pair<uint32_t, simtime_t> > hpArrival ;
    vector< pair<uint32_t, uint32_t> > packetSize ;
    //tempVec[idx].push_back(make_pair(simTime(), pkt));
    for( uint32_t i = 0 ; i < onuSize ; i ++ ) {
        hpArrival.push_back( make_pair( i ,arrivalTime[i]) ) ;
        packetSize.push_back( make_pair( i ,grantDownLen[i]) ) ;
    }

    for( uint32_t i = 0 ; i < onuSize ; i ++ ) {
        for( uint32_t j = 0 ; j < onuSize-1 ; j ++ ) {
            if ( hpArrival[j].second > hpArrival[j+1].second ) { //
                swap( hpArrival[j], hpArrival[j+1] ) ;
            }
            //channel capacity balance
            if ( packetSize[j].second < packetSize[j+1].second ) { // big-> middle-> small
                swap( packetSize[j], packetSize[j+1] ) ;
            }
        }
    }

    double timeGap = ( SIMTIME_DBL(hpArrival[onuSize-1].second)-SIMTIME_DBL(hpArrival[0].second) ) / 100 ;
    // timeGap between First packet and last packet;
    /*cout << "31 : " << hpArrival[31].second << "  0 : " << hpArrival[0].second << endl ;
    //cout << "timeGap : " << timeGap << endl ;*/

    for( uint32_t i = 0 ; i < onuSize ; i ++ ) { // put each onu cost into cost matrix ex:1~32
        uint32_t arrivalTimeID = hpArrival[i].first ; // first = onuID , index i  =  onuID cost(0~onuSize-1) , tuning cost = 12
        uint32_t packetSizeID = packetSize[i].first ;
        costMatrix[0][arrivalTimeID] = 0 ;
        costMatrix[0][packetSizeID] = 0 ;
        if ( timeGap > 0 )costMatrix[0][arrivalTimeID] = costMatrix[0][arrivalTimeID] + ( ( SIMTIME_DBL( hpArrival[i].second - hpArrival[0].second ) / timeGap ) ) ;
        //cout << "time cost : " << ( ( SIMTIME_DBL( hpArrival[i].second - hpArrival[0].second ) / timeGap ) ) << endl ;
        //costMatrix[0][i] = costMatrix[0][i] + ( ((double) grantDownLen[i] / (double)cycleAllPacketSize ) * 100) ;
        //cout << "grant : " << grantDownLen[packetSizeID] << endl ;
        //cout << "onu "<< i << " size cost : " << ( ( (double)grantDownLen[i] / (double)cycleAllPacketSize ) * 100) << endl ;
        if( onutransmitchannel[arrivalTimeID] != 1 && cycleAllPacketSize != 0 )
            costMatrix[0][arrivalTimeID] = costMatrix[0][arrivalTimeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) ) * 100 ) / 2  ; // tuning time
        if( onutransmitchannel[packetSizeID] != 1 && cycleAllPacketSize != 0 )
            costMatrix[0][packetSizeID] = costMatrix[0][packetSizeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) )  * 100 ) / 2  ; // tuning time
        //cout << " tuning cost : " << ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) )  * 100 ) / 2 << endl ;
        if ( active_ch >= 2 ) {
            costMatrix[1][arrivalTimeID] = 0 ;
            costMatrix[1][packetSizeID] = 0 ;
            if ( timeGap > 0 )costMatrix[1][arrivalTimeID] = costMatrix[1][arrivalTimeID] + ( ( SIMTIME_DBL( hpArrival[i].second - hpArrival[0].second ) / timeGap ) * 0.2 ) ;
            if( onutransmitchannel[arrivalTimeID] != 2 )
                costMatrix[1][arrivalTimeID] = costMatrix[1][arrivalTimeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) ) * 100 ) / 2  ; // tuning time
            if( onutransmitchannel[packetSizeID] != 2 )
                costMatrix[1][packetSizeID] = costMatrix[1][packetSizeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) ) * 100 ) / 2  ; // tuning time
        }
        if ( active_ch >= 3 ) {
            costMatrix[2][arrivalTimeID] = 0 ;
            costMatrix[2][packetSizeID] = 0 ;
            if ( timeGap > 0 )costMatrix[2][arrivalTimeID] = costMatrix[2][arrivalTimeID] + ( ( SIMTIME_DBL( hpArrival[i].second - hpArrival[0].second ) / timeGap ) * 0.2  ) ;
            //costMatrix[2][packetSizeID] = costMatrix[2][packetSizeID] + ( ( (double)grantDownLen[packetSizeID] / (double)cycleAllPacketSize ) * 100 ) ;
            if( onutransmitchannel[arrivalTimeID] != 3 )
                costMatrix[2][arrivalTimeID] = costMatrix[2][arrivalTimeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) ) * 100 ) / 2  ; // tuning time
            if( onutransmitchannel[packetSizeID] != 3 )
                costMatrix[2][packetSizeID] = costMatrix[2][packetSizeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) ) * 100 ) / 2  ; // tuning time
        }
        if ( active_ch >= 4 ) {
            costMatrix[3][arrivalTimeID] = 0 ;
            costMatrix[3][packetSizeID] = 0 ;
            if ( timeGap > 0 )costMatrix[3][arrivalTimeID] = costMatrix[3][arrivalTimeID] + ( ( SIMTIME_DBL( hpArrival[i].second - hpArrival[0].second ) / timeGap ) ) ;
            //costMatrix[3][packetSizeID] = costMatrix[3][packetSizeID] + ( ( (double)grantDownLen[packetSizeID] / (double)cycleAllPacketSize ) * 100 ) ;
            if( onutransmitchannel[arrivalTimeID] != 4 )
                costMatrix[3][arrivalTimeID] = costMatrix[3][arrivalTimeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) ) * 100 ) / 2  ; // tuning time
            if( onutransmitchannel[packetSizeID] != 4 )
                costMatrix[3][packetSizeID] = costMatrix[3][packetSizeID] + ( tuningTime / ( (double)cycleAllPacketSize / (double)active_ch / 0.4*pow(10,-10) ) * 100 ) / 2  ; // tuning time
        }

    }

    /*cout << " cost Matrix : " << endl ;
    for( uint32_t i = 0; i < active_ch ; i ++ ) {
        for( uint32_t j = 0 ; j < onuSize ; j ++ ) {
            cout << costMatrix[i][j] <<", " ;
        }

        cout << endl ;
    }

    */

}

void DBA::hungarian() {
    //==========creat ONU assigment map ===========================================================
    int Times = 0;
    int capacity = ((double)cycleAllPacketSize / (double)active_ch )* 4 ;

    vector<uint32_t> channelCapacity ;
    channelCapacity.clear() ;
    channelCapacity.assign(active_ch,capacity) ;

    vector<pair<int, int>>::iterator it;
    channelAssignment.clear();

    HungarianAlgorithm HungAlgo;
    vector<int> assignment;

    int fullChannel = 0 ;
    bool clockwise = true ;

    double cost;
    while( channelAssignment.size() < onuSize ){

        Times++;
        assignment.clear();
        cost = HungAlgo.Solve(costMatrix, assignment);
        for (unsigned int x = 0; !assignment.empty() && channelAssignment.size() < onuSize ; x++){

            if ( x == active_ch ) x = 0 ;

            //cout << x << "," << assignment[x] << " ";
            // X:Channel的ID ,
            //assignment[x]:OUN的ID

            //make sure no re repeat, if already assignment, do not again
            // -1 mean no yet assignment
            int ass = assignment[0];//assignment[x]:OUN的ID
            //cout << "ONU : " << ass << "wait assign " << endl ;
            if ( channelCapacity[x] > grantDownLen[ass]  ) {
                for(int i = 0; i < costMatrix.size(); i++){
                    costMatrix[i][ass] = 500;
                }
                //cout << "ONU:" << ass << " to Chnnel:" << x << endl;
                it = find_if(channelAssignment.begin(), channelAssignment.end(),
                [&ass](pair<int, int> const& elem){return elem.first == ass;});
                if(it == channelAssignment.end()){
                    channelAssignment.push_back(make_pair(ass,x));
                    channelCapacity[x] = channelCapacity[x] - grantDownLen[ass] ;//unknow
                    assignment.erase(assignment.begin()) ;
                }else{
                    assignment.erase(assignment.begin());
                }
            }else{
                fullChannel ++;
                if ( fullChannel >= active_ch ) { // all channel is full
                    //cout << "all channel is full " << endl ;
                    //cout << "channelAssignment size :" << channelAssignment.size() << endl ;
                    for(int i = 0; i < costMatrix.size(); i++) {
                        costMatrix[i][ass] = 500;
                    }
                    it = find_if(channelAssignment.begin(), channelAssignment.end(),
                    [&ass](pair<int, int> const& elem){return elem.first == ass;});
                    if(it == channelAssignment.end()){
                        //cout << "ONU:" << ass << " to Channel:" << x << endl;
                        channelAssignment.push_back(make_pair(ass,x));
                        //channelCapacity[x] = channelCapacity[x] - grantDownLen[ass] ;
                        assignment.erase(assignment.begin()) ;
                        //cout << "assignment size :" << assignment.size() << endl ;
                    }else assignment.erase(assignment.begin()) ;
                }
                if (fullChannel>100) {
                    cout << "has loop !" << endl ;
                    endSimulation() ;
                }
            }
        }

        if ( Times > 130) {
            cout << " cost Matrix : " << endl ;
            for( uint32_t i = 0; i < active_ch ; i ++ ) {
                for( uint32_t j = 0 ; j < onuSize ; j ++ ) {
                    cout << costMatrix[i][j] <<", " ;
                }
                cout << endl ;
            }
            endSimulation() ;
        }

    }
}


