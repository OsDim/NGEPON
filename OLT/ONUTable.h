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

#ifndef ONUTABLE_H_
#define ONUTABLE_H_

#include <omnetpp.h>
#include "ONUTableEntry.h"
#include "traffic_gen/Messages/EPON_messages_m.h"
#include "traffic_gen/Messages/MyPacket_m.h"
#include <vector>
#include <fstream>

using namespace std;

class ONUTable : public cSimpleModule
{
protected:
    // -------------------------------------------------- Override baisc component
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    vector<ONUTableEntry> tbl;
public:
    ONUTable();
    ~ONUTable();

    virtual ONUTableEntry * getEntry(uint32_t idx);
    virtual ONUTableEntry * getEntry(uint16_t llid);

    virtual int getTableSize(){ return tbl.size();}
    virtual void regONU(RTTReg * msg);
    virtual void updateTable(MPCPReport * msg);
    void clockONUTimer(uint32_t idx, uint16_t mode, simtime_t clockTime);
    void earlyWakeProcess(uint32_t idx, uint16_t mode);
    void timeOutHandler(TimeOutMsg * tom);
    simtime_t getSleepArrivalTime(uint32_t idx){return sleepTimeOutEventVec[idx]->getArrivalTime();}

private:
    int version;
    int onuSize;
    vector<TimeOutMsg * > sleepTimeOutEventVec,
                        dozeTimeOutEventVec;
};

#endif /* ONUTABLE_H_ */
