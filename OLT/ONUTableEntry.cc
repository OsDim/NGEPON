////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU Lesser General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU Lesser General Public License for more details.
////
//// You should have received a copy of the GNU Lesser General Public License
//// along with this program.  If not, see http://www.gnu.org/licenses/.
////

#include <ONUTableEntry.h>

ONUTableEntry::ONUTableEntry() {
    LLID=0;
    RTT=simTime();
    lastUpdate=simTime();
    comTimes.length=0;
    comTimes.start=0;
    downLength=0;
    energySaving=false;
    state=ACTIVE;
}

ONUTableEntry::~ONUTableEntry() {
    // TODO Auto-generated destructor stub
}


ostream & operator<<(std::ostream &out, const ONUTableEntry &en)
{
    string state;
    if (en.state==ACTIVE)
        state = "ACTIVE";
    else if (en.state==DOZE)
        state = "DOZE";
    else if (en.state==SLEEP)
        state = "SLEEP";

    out << " LLID: "<< en.LLID
        << " LastUpdate: " << en.lastUpdate
        <<"  RTT: "<< en.RTT
        << " Start: "<< en.comTimes.start
        <<" Up Link Queue Length: "<< en.comTimes.length
        <<" Down Link Queue Length: "<< en.downLength
        <<" Power Saving State:" << state
        <<" UseEnergySaving?"<< en.energySaving
        << endl;

    return out;
}

ONUTableEntry& ONUTableEntry::operator=(const ONUTableEntry &en){
    LLID=en.LLID;
    lastUpdate=en.lastUpdate;
    RTT=en.RTT;
    comTimes=en.comTimes;
    downLength=en.downLength;
    energySaving=en.energySaving;
    state=en.state;
    return *this;
}
