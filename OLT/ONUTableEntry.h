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

#ifndef ONUTABLEENTRY_H_
#define ONUTABLEENTRY_H_

#include "omnetpp.h"
#include "common/MPCP_codes.h"
using namespace std;

typedef struct commitedTime{
    simtime_t start;
    uint32_t length;
} CommitedTime;

class ONUTableEntry
{

protected:
    simtime_t RTT, lastUpdate;
    uint16_t LLID, // ONU index     //    vector<uint16_t> LLIDs; // why vector??? (pon)
             state;
    CommitedTime comTimes, grantTimes;
    uint32_t downLength;
    bool energySaving;
public:
    ONUTableEntry();
    ~ONUTableEntry();

    // ----------------------getter----------------------
    simtime_t getLastUpdate(){return lastUpdate;}
    simtime_t getRTT(){return RTT;}
    uint16_t getLLID(){return LLID;}
    CommitedTime getComTime(){return comTimes;}
    CommitedTime getGrantTime(){return grantTimes;}
    uint32_t getDownLength(){return downLength;}
    uint16_t getState(){return state;}
    // ----------------------setter----------------------
    void setLastUpdate(simtime_t t){lastUpdate = t;}
    void setRTT(simtime_t rtt){RTT = rtt;}
    void setLLID(uint16_t id){LLID = id;}
    void setComTime(CommitedTime t){comTimes = t;}
    void setGrantTime(CommitedTime t){grantTimes = t;}
    void setDownLength(uint32_t len){downLength = len;}
    void setState(uint16_t s){state = s;}
    // --------------------------------------------------
    ONUTableEntry& operator=(const ONUTableEntry &en);
    friend std::ostream & operator<<(std::ostream &out, const ONUTableEntry &en);


};

#endif /* ONUTABLEENTRY_H_ */
