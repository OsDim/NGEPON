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

#include <MPCPtools/MPCPTools.h>

MPCPTools::MPCPTools() {
    // TODO Auto-generated constructor stub

}

MPCPTools::~MPCPTools() {
    // TODO Auto-generated destructor stub
}

uint64_t MPCPTools::nsToSimTime(uint64_t time)
{
    int scale= (-9 -simTime().getScaleExp())/3;
    // Clock Granularity is 16ns
    return time * (scale*1000);
}

uint64_t MPCPTools::bitsToNS16(uint64_t bits, uint8_t gigRate){
    uint64_t rate = gigRate * pow(10,9);
    // tx time in seconds
    double txtime = ((double)bits)/rate;
    // tx time in ns
    txtime*=pow(10,9);
    // ns16
    return (txtime/16);
}

uint64_t MPCPTools::bytesToNS16(uint64_t bytes, uint8_t gigRate){
    return bitsToNS16(bytes*8, gigRate);
}
