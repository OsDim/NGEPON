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

#include <ONUTable.h>

Define_Module(ONUTable);

ONUTable::ONUTable(){

}
ONUTable::~ONUTable(){
    for (uint16_t i=0; i!=onuSize; i++) {
        cancelAndDelete(sleepTimeOutEventVec[i]);
        cancelAndDelete(dozeTimeOutEventVec[i]);
        //delete sleepTimeOutEventVec[i];
        //delete dozeTimeOutEventVec[i];
    }
}

void ONUTable::initialize()
{
    cModule *epon = simulation.getModuleByPath("EPON");
    onuSize = epon->par("sizeOfONU").longValue();
    version = epon->par("version").longValue();


    WATCH_VECTOR(tbl);

    for (uint16_t i=0; i!=onuSize; i++) {
        stringstream sleepName, dozeName;
        sleepName << "sleepTimeOutEvent[" << i << "]";
        dozeName << "dozeTimeOutEvent[" << i << "]";
        TimeOutMsg * sleepTimeOutEvent = new TimeOutMsg(sleepName.str().c_str());
        TimeOutMsg * dozeTimeOutEvent = new TimeOutMsg(dozeName.str().c_str());
        sleepTimeOutEvent->setKind(TIMEOUT_TYPE);
        dozeTimeOutEvent->setKind(TIMEOUT_TYPE);
        sleepTimeOutEvent->setOnuIdx(i);
        dozeTimeOutEvent->setOnuIdx(i);
        sleepTimeOutEventVec.push_back(sleepTimeOutEvent);
        dozeTimeOutEventVec.push_back(dozeTimeOutEvent);
    }

}

void ONUTable::handleMessage(cMessage *msg)
{
    if (msg->getKind()==TIMEOUT_TYPE)   // timerEvent
    {
        timeOutHandler(check_and_cast<TimeOutMsg * >(msg));
    }
    else if ( strcmp(msg->getClassName(),"RTTReg") == 0 )        // Register ONU
    {

        regONU(check_and_cast<RTTReg*>(msg));
        if ( getTableSize()== onuSize)
        {                      // When all ONU have been registered
            cMessage *ready = new cMessage("Ready");
            sendDirect(ready, simulation.getModuleByPath("EPON.olt.dba"), "in");
        }
    }
}

void ONUTable::finish()
{

}

// -----------------------------------------------------------------------------
void ONUTable::timeOutHandler(TimeOutMsg * tom)
{
    uint16_t idx = tom->getOnuIdx();
    if (version == 0)   // Larry version
    {
        if (sleepTimeOutEventVec[idx] == tom) { // When sleep time is over
            clockONUTimer(idx,  DOZE, DOZETIME*pow(10,-3));
        }
        else if (dozeTimeOutEventVec[idx] == tom)
            clockONUTimer(idx,  ACTIVE, -1);
    }
    else if (version == 1)  // MY version
    {
        if (sleepTimeOutEventVec[idx] == tom)
            clockONUTimer(idx,  DOZE, -1);
    }
}

void ONUTable::earlyWakeProcess(uint32_t idx, uint16_t mode)
{
    if (mode == SLEEP)
        cancelEvent(sleepTimeOutEventVec[idx]);
    else if (mode == DOZE)
        cancelEvent(dozeTimeOutEventVec[idx]);

    getEntry(idx)->setState(ACTIVE);
}

void ONUTable::clockONUTimer(uint32_t idx, uint16_t mode, simtime_t clockTime)
{
    Enter_Method("clockONUTimer()");
    getEntry(idx)->setState(mode);  // Predict state change on ONU
    if (mode==DOZE)
    {
        if (version==0) {
            take(dozeTimeOutEventVec[idx]);
            scheduleAt(simTime()+clockTime, dozeTimeOutEventVec[idx]);
        } // version 1 will only change state
        else if (version==1) {
            //  Do nothing, but stay in doze
        }
//        cout << "t=" << simTime() << " OLT assume onu[" << idx << "] dozing... to " << clockTime+simTime() << endl;
    }
    else if (mode==SLEEP) {
        //  Exception for < 1sec , when time reach and ONU decide to SLEEP
        if (sleepTimeOutEventVec[idx]->isScheduled()) cancelEvent(sleepTimeOutEventVec[idx]);
        take(sleepTimeOutEventVec[idx]);
        scheduleAt(simTime()+clockTime, sleepTimeOutEventVec[idx]);

//        cout << "t=" << simTime() << " OLT assume onu[" << idx << "] sleeping... to " << clockTime+simTime() << endl;
    }

}

ONUTableEntry * ONUTable::getEntry(uint32_t idx){
    if (idx<0 || idx>=tbl.size()) return NULL;
    return &tbl[idx];
}

ONUTableEntry * ONUTable::getEntry(uint16_t llid){
    for (uint32_t i=0; i!=tbl.size(); i++)
        if (getEntry(i)->getLLID() == llid)
            return &tbl[i];
    return NULL;
}

void ONUTable::regONU(RTTReg *msg)
{
    uint16_t llid = msg->getSrcAddr();
    simtime_t rtt = msg->getRtt();

    CommitedTime t;
    t.length=0;     // initial MTW
    t.start=0;
    ONUTableEntry entry;
    entry.setRTT(rtt);
    entry.setLLID(llid);
    entry.setComTime(t);
    entry.setGrantTime(t);
    tbl.push_back(entry);

    delete msg;
}

void ONUTable::updateTable(MPCPReport * msg)
{
    uint16_t llid = msg->getSrcAddr();
    ONUTableEntry * oldEntry = getEntry(llid);

    CommitedTime t;
    t.length =msg->getQInfo();  // Update ONU's queue info
    t.start=0;
    oldEntry->setComTime(t);
    oldEntry->setLastUpdate(simTime());

//    delete msg;   // DONE BY DBA
}
