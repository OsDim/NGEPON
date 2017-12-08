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

#include "Classifier.h"

#include <queue>

Define_Module(Classifier);
Classifier::Classifier() {
    sendUpstreamEvent = new cMessage();
    sleepTimeoutEvent = new cMessage();
    checkHPQueue = new cMessage() ;
}
Classifier::~Classifier() {
    cancelAndDelete(sendUpstreamEvent);
    cancelAndDelete(sleepTimeoutEvent);
    cancelAndDelete(checkHPQueue);
    //delete sendUpstreamEvent;
    //deleteModule();
}

void Classifier::initialize() {
    upO = gateHalf("Up$o", cGate::OUTPUT);      // to ONU_Scheduler
    upI = gateHalf("Up$i", cGate::INPUT);       // from gen
    downO = gateHalf("Down$o", cGate::OUTPUT);
    downI = gateHalf("Down$i", cGate::INPUT);   // from ONU_Scheduler
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

    qc0 = 0, qc1 = 0;
    WATCH(qc0);
    WATCH(qc1);

    string halfpath = "EPON.onu[";
    stringstream path;
    path << halfpath << getParentModule()->getId() - 2 << "].powerControler";
    pwCtrler = dynamic_cast<PowerControler *>(simulation.getModuleByPath(path.str().c_str()));
    onuUpBytes.push_back(0);

    cModule *epon = simulation.getModuleByPath("EPON");
    version = epon->par("version").longValue(); // 0: Larry ; 1: MY version
    queueLimit = epon->par("queueLimit").longValue();
    wakeupSize = epon->par("awakeSize").longValue();
    delayBound = epon->par("delayBound").doubleValue();
    modes = epon->par("modes").longValue();
    onuSize = epon->par("sizeOfONU").longValue();
    up_data_rate = par("up_data_rate");
    down_data_rate = par("down_data_rate");

    onuStayChannel = 1 ;


    needTuning = false ;

//----------   dynamic  ---------------
    dynamicPoint = epon->par("dynamicPoint").doubleValue();
    dyRatio = epon->par("dyRatio").doubleValue();
    downThreshold = epon->par("downThreshold").doubleValue();
    upThreshold = epon->par("upThreshold").doubleValue();
    normalizeDown = 1 / (par("down_data_rate").doubleValue());
    normalizeUp = 10 / (par("up_data_rate").doubleValue());
    multiMTW = epon->par("multiMTW").doubleValue();
    MTW_algo = epon->par("MTW_algo").boolValue();
    HP_must_empty = epon->par("high_priority_must_empty").boolValue();
    upQueueLoading = 0;
    lastREPORT = simTime();
    newREPORT = simTime();
    REPORT_cycle = 0;
    totalQueueSize = 0;

}

void Classifier::handleMessage(cMessage *msg) {
    // --Self Message--
    if (msg->isSelfMessage()) {
        if (msg == sendUpstreamEvent && !readyQueue.isEmpty()) {
            cModule *olt = simulation.getModuleByPath("EPON.olt");
            MyPacket * pkt = check_and_cast<MyPacket *>(readyQueue.pop());

            pkt->setSrcAddr(getParentModule()->getId());
            pkt->setDestAddr(olt->getId());

            if (pkt->getKind() != MPCP_TYPE)
                onuUpBytes[0] += pkt->getByteLength(); // Statistic receive bytes


            if ( onuStayChannel == 1)
              send(pkt, upO);
            else if ( onuStayChannel == 2)
              send(pkt, up2O);
            else if ( onuStayChannel == 3)
              send(pkt, up3O);
            else if ( onuStayChannel == 4)
              send(pkt, up4O);

            scheduleAt(simTime(), sendUpstreamEvent);
        } else if (strcmp(msg->getName(), "wakeEvent") == 0) {
            earlyWakeUp();
            delete msg;
        }
        else if(msg == sleepTimeoutEvent)
        {
//            cout << "sleep time out" << endl;
            if (modes!=dynamic_mode)
                cancelAndDelete(sleepTimeoutEvent);
            checkLoading();
        }

        else if ( msg == checkHPQueue ) {
            if ( !queues[0].isEmpty() )
              earlyWakeUp();

        }
    }
    // --Network Message--
    if (msg->getArrivalGate() == upI || msg->getArrivalGate() == up2I || msg->getArrivalGate() == up3I || msg->getArrivalGate() == up4I) {   // Upstream from gen.
        classify(check_and_cast<MyPacket *>(msg)); // Storing and classify to hQueue & lQueue

    } else if (msg->getArrivalGate() == downI || msg->getArrivalGate() == down2I || msg->getArrivalGate() == down3I || msg->getArrivalGate() == down4I) { // Receive grant_req and transmit packets.
        if (msg->getKind() == MPCP_TYPE) {   // receive MPCPGate
            recvGateProcess(check_and_cast<MPCPGate *>(msg));
//            goToSleep=false;
        }
        else {  // MyPacket downstream
            if (check_and_cast<MyPacket*>(msg)->getLastPkt()) {
                pwCtrler->setRecvEnd(true);
                //if ( getParentModule()->getId()-2 == 7 && simTime() > 0.55 ) cout << "recv onu 7 last packet at : " << simTime() << endl ;
            }

//            if (check_and_cast<MyPacket *>(msg)->getLastPkt() && goToSleep && pwCtrler->isTransEnd() && pwCtrler->isRecvEnd()) {
//                pwCtrler->changeMode(SLEEP, simTime(), SLEEPTIME*MS);
//            }
        }
        delete msg;
    }
}

void Classifier::finish() {
#ifdef WIN32
    string dir="C:\\results\\";
#endif
#ifdef __linux__
    string dir="/home/you/results/";
#endif
    stringstream path;
    path << dir << "Classifier.txt";
    ofstream out;
    out.open(path.str().c_str(), ios::out | ios::app);

    out << "[" << getParentModule()->getId() - 2 << "] upstream: "
            << 8 * onuUpBytes[0] << " bits(" << 8 * onuUpBytes[0]/pow(10,6) << " Mbits) q[0]=" << qc0 << " q[1]="
            << qc1 << endl;

    if (getParentModule()->getId() - 1 == onuSize)
        out << "\n\n\n";
    out.close();


    stringstream path3;
    path3 << dir << "UpQueueSize.txt";
    ofstream out3;
    out3.open(path3.str().c_str(), ios::out | ios::app);

    if (getParentModule()->getId()-2==0) {
        cModule *locNet = simulation.getModuleByPath("EPON.localNetwork[0]"), *gen = simulation.getModuleByPath("EPON.trafficGen[0]");
        double upLoad = locNet->par("offered_load"), downLoad = gen->par("offered_load");
        out3 << upLoad*100 << "% " << downLoad*100 << "%" << endl ;
    }

    out3 << getParentModule()->getId()-2 << " AvgUpQueueSize: " << totalQueueSize/REPORT_cycle << " KB, REPORT cycle: " << REPORT_cycle << endl;


    if (getParentModule()->getId() - 1 == onuSize)
    out3 << "\n\n\n" << endl;


    out3.close();
}

// ---------------------------------------

void Classifier::recvGateProcess(MPCPGate *gt) {

    uint32_t currentUpLength = 0;
    //bool respGrantDone=false;
    uint32_t grantLength = gt->getLength();
    uint32_t onuState = pwCtrler->getEnergyMode(),
            recommandMode = gt->getPowerSavingMode();
    simtime_t MINUNIT = 1 * pow(10, -12),  // minimal unit
//    NS = 1 * pow(10, -9),    // Nanosecond
    MS = 1 * pow(10, -3);    // Millisecond
    idx = getParentModule()->getId() - 2;
    //if ( idx == 28 && simTime() > 1 ) cout << "onu 28 recv gate ! " << endl ;
    simtime_t startSaving;      //record start saving time
    double downQueueLoading = gt->getDownQueueLoading();

    bool upQueueEmpty = (queues[0].isEmpty() && queues[1].isEmpty()) ? true : false;
    bool downQueueEmpty = (gt->getQueueLength(0) == 0 && gt->getQueueLength(1) == 0) ? true : false;
    uint32_t upQueueSize = qc0 + qc1;
    uint32_t downQueueLength = gt->getQueueLength(0) + gt->getQueueLength(1);

    uint16_t channel = gt->getTransmitChannel() ;
    //cout << "channel " << channel << endl ;

    if ( onuStayChannel != channel ) { // need tuning time
        needTuning = true ;
        onuStayChannel = channel ;

    }

    // high priority packet must empty
    bool upQueueLight =  true , downQueueLight = true ;
    if (dynamic_mode) {
            if ( MTW_algo ) {
                if( upQueueSize>multiMTW*MTU ) {    // 1*MTU??
                    upQueueLight = false;
                }

            }
            if ( upQueueLight && HP_must_empty ){

                if ( !queues[0].isEmpty()) {
                  upQueueLight = false ;
                  //cout << "ONU : " << idx << " has hp arrival !!!!!" << endl ;
                }

                //else cout << "ONU : " << idx << " no hp arrival !" << endl ;

            }
            if ( downQueueLight && HP_must_empty ){

                if ( gt->getQueueLength(0) != 0 )
                  downQueueLight = false ;

            }


    }


    //In higher load, the packets are accumulated in ONU queues during the period from sending request to receive grant quota by OLT.
    //calculate updated request length here
    //After examining the the result of currrentQSize() and qc0+qc1, they are same. But the explicitly call currentQSize() invokes the output file such as schedule.txt having unlimited figures.
    //currentUpLength = currentQSize();
    currentUpLength = qc0 + qc1;

//    if(simTime()> 4.6568 && getParentModule()->getId()-2==20 && simTime()< 4.683){
//                      cout << "(receGATE)"<< onuState << " t="<< simTime() << ",  " << ((MyPacket *) queues[0].front())->getTimestamp() << endl;
//                      cout << "qc0 : " << qc0 << endl;
//    }

    if (onuState == ACTIVE)
    {
//        cout << "ONU active" << endl;
        upLoading();

        if (pwCtrler->getRemainAcitve()==true){
            upQueueLight = false;
        }

    }

//    if (simTime()> 4.09968133  && simTime()< 4.100261803 && getParentModule()->getId()-2==4)
//        cout << "(receGATE)t= " << simTime() << "   onuSTATE " << onuState << endl;


//    cout << "At ONU[" << idx << "], received gated msg: grantLength=" << grantLength << endl;
//    cout << "At ONU[" << idx << "], current queue size by qc0+qc1=" << qc0+qc1 << endl;

    if (grantLength > currentUpLength) //Here grantLength=reqLength+creditRatio*reqLenght i.e. credited Length
        grantLength = currentUpLength;  //Final grant length
//    cout << "t = " << simTime() << ", At ONU[" << getParentModule()->getId()-2 << "] received gate msg, queue[0] + queue[1]=" << queues[0].length() << "+" << queues[1].length() << "=" << queues[0].length()+queues[1].length() << endl;

//    cout << "t = " << simTime() << ", At ONU[" << getParentModule()->getId()-2 << "], current queue size=" << currentUpLength << endl;

    finalGrantLen = grantLength;
//    cout << "At ONU[" << idx << "], final grant length (actual requirement)  in ONU = " << finalGrantLen << endl;


//    simtime_t upEnd = simTime() + (64 + grantLength) * 8 * up_data_rate + MINUNIT;
//    simtime_t downEnd = simTime() + gt->getDownLength() * 8 * down_data_rate + MINUNIT;
//
//    cout << "[" << getParentModule()->getId()-2 << "] upEnd " << upEnd << endl
//         << "[" << getParentModule()->getId()-2 << "] downEnd: " << downEnd << endl;


    if (onuState == ACTIVE) {
        simtime_t startAfterReport = simTime() + 64 * 8 * up_data_rate + MINUNIT;
//        simtime_t startAfterReport = simTime() + 64 * 8 * NS + MINUNIT;
        // Larry's... wait GATE command to change mode.

        //cout << upQueueLight <<"  UPloading : " << upQueueLoading << " threshold : " << upThreshold << endl ;
        if (version == 0 && (recommandMode == DOZE || recommandMode == SLEEP) && upQueueEmpty) {
            if (recommandMode == SLEEP)
                pwCtrler->changeMode(SLEEP, startAfterReport, SLEEPTIME * MS);
            else
                pwCtrler->changeMode(DOZE, startAfterReport, DOZETIME * MS);
            sendReport(0, 64, recommandMode);
        }


        else if (version == 1 && (upQueueEmpty || (upQueueLight &&  upQueueLoading <= upThreshold && modes==dynamic_mode)) )
//        else if (version == 1 && (upQueueEmpty || (upQueueLight && modes==dynamic_mode)) )  // Up queue is empty
                //gzrule
                //else if (version==1 && (upQueueEmpty || grantLength==0 ))  // Up queue is empty or granted lengths=0
        {        // ONU make decision
//            readyQueue = remainQueue;
//            remainQueue.clear();
//            cout << "At ONU[" << getParentModule()->getId()-2 << "]  up queue is empty" << endl << endl;
//            if (qc0!=0)
//                cout << qc0 << endl;
//            if(grantLength!=0)
//                cout << "grant: " << grantLength << endl;
            //record start saving time
            if (gt->getDownLength() == 0){  // no upstream and no downstream pkts, A->S
                //startSaving =  simTime()+( 64 + gt->getLength() )*8*NS + MINUNIT;   // After Report send out
                startSaving = simTime() + (64 + grantLength) * 8 * up_data_rate + MINUNIT; // After Report send out
//                startSaving = simTime() + (64 + grantLength) * 8 * NS + MINUNIT; // After Report send out
            }
            else {
                //no upstream but has downstream, A->D

                startSaving = simTime() + ( gt->getDownLength()+64 * 8) * down_data_rate + MINUNIT;         // After Recived last down packet
                if( needTuning ) {
                    //startSaving = startSaving + 0.0001 ;
                    needTuning = false ;
                }
//
            }


            //send report with start-saving-time
            //no up and no down-->ACTIVE to SLEEP
            if (gt->getDownLength() == 0 && ( downQueueEmpty || (downQueueLight && downQueueLoading<=downThreshold && modes == dynamic_mode)) ) {
//            if (gt->getDownLength() == 0 && ( downQueueEmpty || (downQueueLight && modes == dynamic_mode)) ) {
//                cout << "At ONU[" << idx << "] down queue size="<< gt->getDownLength() << " up queue size=" << qc0+qc1 << " and ONU ready to sleep and grantLength= " << grantLength << endl << endl;
//                cout << "At ONU[" << idx << "] startSaving sleep time=" << startSaving << " sleepDuration=" << SLEEPTIME*MS << endl;

//                if (modes == dynamic_mode)
//                {
//                    respGrant(grantLength);
//                    cout << grantLength << endl;
//                }
                sendReport(0, 64, SLEEP);
                //if( getParentModule()->getId() - 2 == 28 && simTime() > 1 ) cout << "ONU go to sleep at time : " << simTime() << endl ;
//                sendReport(qc0+qc1, 64, SLEEP); //queue size =qc0+qc1, it may be 0 for none credited based or not 0 for credit based
                startSaving = simTime() + ( gt->getDownLength()+64) * 8 * down_data_rate + MINUNIT + 0.00011 ;

                //if ( idx == 18 && simTime() > 0.78 )

                //cout << "idx : " << idx <<"active change in sleep in time : " << startSaving <<endl ;

                pwCtrler->changeMode(SLEEP, startSaving, SLEEPTIME * MS);

                scheduleAt(startSaving+0.00001, checkHPQueue);

                scheduleAt(startSaving+SLEEPTIME*MS, sleepTimeoutEvent);



//                cout << "[" << getParentModule()->getId()-2 << "] " << "sleep start: " << startSaving << endl;
                //Note that put cout below sendReport and pwCtler cause no output as following line
//                cout << "At ONU[" << idx << "] down queue size="<< gt->getDownLength() << " up queue size=" << currentQSize() << " and ONU go to sleep" << endl << endl;
//                cout << "At ONU[" << idx << "] down queue size="<< gt->getDownLength() << " and ONU go to sleep" << endl << endl;
             }

            else { //no up but has down-->ACTIVE to DOZE
                if (modes == dual_mode) { //dual-mode
                    respGrant(grantLength);
                    sendReport(0, 64, ACTIVE);
                }
                else if (modes == tri_mode || modes == dynamic_mode) { //tri-mode
//                    cout << "[" << getParentModule()->getId()-2 << "] ";
//                    cout << qc0 << " + " << qc1 << ",   "  << upQueueLoading << endl;

//                    if (downQueueEmpty && upQueueEmpty)
//                        cout << "Queue Empty" << endl;
//                    else if (downQueueLight && upQueueLight)
//                        cout << "Queue Light" << endl;

                        sendReport(0, 64, DOZE);
                        pwCtrler->changeMode(DOZE, startSaving, 0);

                        scheduleAt(startSaving+0.00001, checkHPQueue);
                        //if ( idx == 7 && simTime() > 0.55 )
                      //cout << "idx : " << idx << "active change in doze " << endl ;


//                    cout << "At ONU[" << idx << "] down queue size="<< gt->getDownLength() << " up queue size = " << qc0+qc1 << " and ONU go to doze" << endl;

                }
            }
        }
        else {   //upQueue is not empty->has up->stay at ACTIVE
//            readyQueue = remainQueue;
//            remainQueue.clear();
        //cout << "At ONU[" << getParentModule()->getId()-2 << "], final grant legth =" << grantLength << endl;
//            cout << "At ONU[" << idx << "] current queue size=" << qc0+qc1 << " ready to grant length=" << grantLength <<endl;

//            cout << "At ONU[" << idx << "] downQueueSize="<< gt->getDownLength() << " upQueueSize=" << qc0+qc1 << " and ONU stay in active" << endl << endl;

            respGrant(grantLength); // return upstream size; after sending allowed up length-> equivalent no up stream pkt
            //Note cout the line below cause the file Scheduler.txt unlimited result, may be divided by 0
//            cout << "At ONU[" << idx << "] after granted the size=" << grantLength << " current queue size is = " << currentQSize() << endl;
//            cout << "At ONU[" << idx << "] after granted the size=" << grantLength << " current up queue size is = " << qc0+qc1 << " , qc0=" << qc0 << ", qc1= " << qc1  << endl;
//            if (downQueueEmpty)
//                cout << "DownQueue is Empty" << endl;

            //after respGrant immediately go to power saving instead keeping in Active
            //now equivalent no up stream pkt,  send report with start-saving-time
            //record start saving time

            //if (upstreamTotal > grantLength) {cout << "sendUp > grantLength\n";endSimulation();}
//            cout << "At ONU[" << idx << "] after granted, report queue size of q0+q1 to OLT is=" << qc0+qc1 << " , qc0=" << qc0 << ", qc1= " << qc1 << endl;
            sendReport(qc0 + qc1, 64, ACTIVE);
            //if( getParentModule()->getId() - 2 == 28 && simTime() > 1 ) cout << "ONU stay active at time : " << simTime() << endl ;
//            if(simTime()> 4.6568 && getParentModule()->getId()-2==20 && simTime()< 4.683)
//                cout <<"t= " << simTime() << "  check error ACTIVE " << qc0+qc1<< endl;
        }

        pwCtrler->setRemainAcitve(false);
    }




    else // DOZE or SLEEP //here should receive force gate
    {
        //DOZE
        if (version == 1) { // power saving ONU should only follow OLT command to change its next state
//            cout << "t=" << simTime() << " ONU["<< idx <<"] receivedforce gate" << endl;
//            if( onuState==DOZE && gt->getDownLength()==0 && downQueueEmpty) {
            if ( recommandMode == DOZE && gt->getDownLength() == 0 && ( downQueueEmpty || (downQueueLight && downQueueLoading <= downThreshold && modes == dynamic_mode ) ) ){
//            if (recommandMode == DOZE && gt->getDownLength() == 0 && downQueueEmpty) {
//                pwCtrler->changeMode(SLEEP, simTime() + 2*MINUNIT,  SLEEPTIME*MS );  // Don't report just sleep......
                if( needTuning ) {
                  pwCtrler->changeMode(SLEEP, simTime(), SLEEPTIME * MS); // Don't report just sleep......
                  needTuning = false ;
                }
                else pwCtrler->changeMode(SLEEP, simTime(), SLEEPTIME * MS); // Don't report just sleep......
//                cout << "t=" << simTime() << " ONU["<< idx <<"] receivedforce gate and start sleep" << endl;
                //cout << " doze change to sleep " << endl ;
            }
            else{
                // remain doze
            }

        }


        //SLEEP
        // Invisible REPORT, can detect by power saving mode
//        cout << "At ONU[" << idx << "] in sleep will report its current queue size =" << qc0+qc1 << " , qc0=" << qc0 << ", qc1= " << qc1 << endl;
//        sendReport(qc0+qc1, 64, -1);
        sendReport(0, 64, -1);
    }

    scheduleAt(simTime() + MINUNIT, sendUpstreamEvent);
}

void Classifier::earlyWakeUp() {
//    pwCtrler->getEnergyMode() == ACTIVE ?
//            true : sendDirect(new cMessage("Awake"), pwCtrler, "in");
    if (!cMessage("Awake").isScheduled())
      sendDirect(new cMessage("Awake"), pwCtrler, "in");

    if (pwCtrler->getEnergyMode() != ACTIVE){
        pwCtrler->setRemainAcitve(true);

        if (getParentModule()->getId()-2 == 20 && simTime() > 4.656 && simTime()< 4.683){
            cout << "(earlyWakeUp)wakeup to Acitve from " << pwCtrler->getEnergyMode()
                 << "[" << getParentModule()->getId()-2 << "] "<< "wake Up, t= " << simTime()
                 << "First pkt:  " <<((MyPacket *) queues[0].front())->getTimestamp() << endl;
        }
//    cout << "[" << getParentModule()->getId()-2 << "] wakeup process , t=" << simTime() << endl;
    }

//    if(pwCtrler->getEnergyMode() == ACTIVE)
//    {
//
//    }
//    else
//    {
////        checkLoading();
//        sendDirect(new cMessage("Awake"), pwCtrler, "in");
        cancelEvent(sleepTimeoutEvent);
////        cout << "early wake up" << endl;
////        checkLoading();
//    }

//    if(pwCtrler->getEnergyMode() != ACTIVE)
//        cout << "early wakeUp" << endl;
}

void Classifier::classify(MyPacket *pkt) {

//    if (getParentModule()->getId()-2 == 4 && simTime() > 4.099 && simTime() < 4.9503 && pkt->getPriority()==0){
//        cout << "pkt Time" << pkt->getTimestamp() << endl;
//        cout << "Mode : " << pwCtrler->getEnergyMode() << endl;
//    }


    switch (pkt->getPriority()) {
    case 0:     // "High priority data arrived\n"
        if (qc0 < queueLimit) {
            qc0 += pkt->getByteLength();
            queues[0].insert(pkt);
//            if (queues[0].length() > wakeupSize)
//                earlyWakeUp();
            simtime_t headTimestamp =
                    ((MyPacket *) queues[0].front())->getTimestamp();


//            if(simTime()> 4.6568 && getParentModule()->getId()-2==20 && simTime()< 4.683)
//                cout << "(classify)"<< headTimestamp <<  "  " << pwCtrler->getEnergyMode() << endl;


            if (simTime() - headTimestamp >= delayBound) //10*pow(10,-3) = 10ms
            {
                if (!cMessage("wakeEvent").isScheduled())
                scheduleAt(simTime(), new cMessage("wakeEvent"));
                // can refine code
            } else {
                // power saving
            }
        } else
            delete pkt;

        break;
    case 1:     // "Low priority data arrived\n"
        if (qc1 < queueLimit) {
            qc1 += pkt->getByteLength();
            queues[1].insert(pkt);
        } else
            delete pkt;
        break;
    default:    // "Cannot classify "
        break;
    }
    //cout << "At ONU[" << getParentModule()->getId()-2 << "] pkt just arrived, the length of queue[0] + queue[1]=" << queues[0].length() << "+" << queues[1].length() << "=" <<  (queues[0].length() + queues[1].length()) << endl;

//    double qd0 = qc0 / 1000000, qd1 = qc1 / 1000000;
//    EV << "queue0 capacity" << qd0 << "MB" << endl << "queue1 capacity" << qd1
//              << "MB" << endl;
}

uint32_t Classifier::respGrant(uint32_t grantSize) {
    //  (1)Calculate "nextRequest Length" (2)Put next upstream data to remainQueue
    uint32_t qlen = sizeof(queues) / sizeof(cQueue), index = 0, // Index of H-queue(0) or L-queue(1)
            nextSize = 0, totalSize = 0;


//    if (getParentModule()->getId()-2 == 4 && simTime() > 4.099 && simTime() < 4.9503){
//        cout << "(respGrant)t= " << simTime() << " , "<< pwCtrler->getEnergyMode() << endl;
//        cout << "grant First pkt:  " <<((MyPacket *) queues[0].front())->getTimestamp() << "  totalSize: " << grantSize << endl;
//    }

    // --Aggregate packets to tmp & tmp size is lower than Grant size.--
    while (index < qlen) // 0,1 < 2
    {
        if (queues[index].isEmpty())
            index++;
        else {
            nextSize = ((MyPacket *) queues[index].front())->getByteLength();

            //uint32_t ql=0;
            //ql =  queues[0].getLength();
            //cout << "high priority queue length=" << ql << endl;

//            cout << "queue[" << index << "] length=" << queues[index].length() << endl;

            if (totalSize + nextSize <= grantSize) {
                totalSize += nextSize;
                cPacket *tmpPkt = check_and_cast<cPacket *>(
                        queues[index].pop());

                //remainQueue.insert(tmpPkt);
                readyQueue.insert(tmpPkt);

                if (index == 0)
                    qc0 -= tmpPkt->getByteLength();
                else if (index == 1)
                    qc1 -= tmpPkt->getByteLength();
            } else {

                break;
            }
        }
    }

    return totalSize;
}

MyPacket * Classifier::sendReport(int qInfo, int length, uint16_t mode) {
    cModule *olt = simulation.getModuleByPath("EPON.olt");

    MPCPReport * rep = new MPCPReport();

    uint32_t onuState = pwCtrler->getEnergyMode();
    if (onuState != ACTIVE)
        rep->setInvisible(true);

    if (mode == DOZE)
        rep->setDozeAck(true);
    else if (mode == SLEEP)
        rep->setSleepAck(true);

    rep->setTimestamp();
    rep->setDestAddr(olt->getId());
    rep->setSrcAddr(getParentModule()->getId());
    rep->setKind(MPCP_TYPE);
    rep->setOpcode(MPCP_REPORT);
    rep->setQInfo(qInfo);
    rep->setByteLength(length);
    rep->setLastPkt(true);
    rep->setFinalGrantLen(finalGrantLen);

    readyQueue.insert(rep);
    return rep;
}

void Classifier::upLoading()
{
    uint32_t pktSize = (qc0 + qc1)*8;
//    uint16_t pktLength = queues[1].length();
//    uint16_t pktLength = queues[0].length() + queues[1].length();

    double loading = pktSize/pow(10,6)/(simTime()-newREPORT) ;// normalizeUp;

    totalQueueSize = totalQueueSize + (qc0+qc1)/pow(10,6);
    REPORT_cycle++;

//    if(simTime() > 4.099 && getParentModule()->getId()-2==4 && REPORT_cycle<5420){
//        cout << "(loading aware function)[" << getParentModule()->getId()-2 << "]: "<< REPORT_cycle << ", t = " << simTime()
//             << ", qc0= " << qc0 << "," << queues[0].length() << " qc1= " <<  qc1<< endl;
////        << ", qc0+qc1= " << qc0+qc1 << endl;
//
//    }


    if (upQueueLoading==0)
        upQueueLoading = loading;
    else
        upQueueLoading = dyRatio*loading + (1-dyRatio)*upQueueLoading;

    pwCtrler->setUploading(upQueueLoading);
//    if (upQueueLoading < upThreshold)
////    if (qc0+qc1 > 15000)
//    if ((simTime()-newREPORT) *pow(10,3) > 0.3)
//    cout << "[" << getParentModule()->getId()-2 << "] pktLength: "
//         << pktLength << "(" << qc0+qc1 <<  " bytes)"
//         << ", uploading: " << loading << " M/s" << endl
//         << "duration: " /*<< simTime() << " - " << newREPORT << " = "*/ << (simTime()-newREPORT) *pow(10,3) << " ms"<< endl;

//    if (qc0!=0){
//        simtime_t delay =  simTime() - ((MyPacket *)queues[0].front())->getTimestamp();
//        cout << delay*pow(10,3) << " ms" << endl;
//    }

    lastREPORT = newREPORT;
    newREPORT = simTime();
}

void Classifier::checkLoading()
{
    uint32_t pktSize = (qc0 + qc1)*8;
//    uint16_t pktLength = queues[1].length();
//    uint16_t pktLength = queues[0].length() + queues[1].length();

    double loading = pktSize/pow(10,6)/(simTime()-newREPORT) ;// normalizeUp;

    if (upQueueLoading==0)
        upQueueLoading = loading;
    else
        upQueueLoading = dyRatio*loading + (1-dyRatio)*upQueueLoading;

//    if (pktLength > MTU)
//        pwCtrler->setUploading(upThreshold+1);
//    else
        pwCtrler->setUploading(upQueueLoading);
//            cout << "[" << getParentModule()->getId()-2 << "] sleep time out , t= " << simTime() << endl;

//    cout << "[" << getParentModule()->getId()-2 << "]"
//         << "pktLength: " << pktLength
//         << "loading: " << loading << " M/s"<< endl
//         << "duration: " /*<< simTime() << " - " << newREPORT << " = "*/ << (simTime()-newREPORT)*pow(10,3) << "ms" << endl;
        simtime_t highDelay;
        if(queues[0].length()>0)
            highDelay = simTime() - ((MyPacket *) queues[0].front())->getTimestamp();

        if (upQueueLoading > upThreshold /*|| highDelay > 50*pow(10,-3)*/)
        {
            pwCtrler->setRemainAcitve(true);
        }
}

