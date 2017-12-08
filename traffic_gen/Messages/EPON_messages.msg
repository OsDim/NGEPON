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

//
// TODO generated message class
//
cplusplus {{
#include <inttypes.h> 
}}
packet MyPacket
{
	uint16_t SrcAddr;
    uint16_t DestAddr;
    uint16_t Priority;
    simtime_t txEnd;
    simtime_t txStart;

	bool lastPkt=false;
	uint32_t cycleTag;
}
packet MPCP extends MyPacket
{
    uint16_t opcode;
    uint32_t ts;
}

packet RTTReg extends MPCP 
{
    simtime_t Rtt;
//    int ByteLength; // eth_header(14) + payload(1500) + FCS(4) = 1518
	bool energySaving;    
}

packet MPCPAutoDiscovery extends MPCP 
{
    // use for rtt test
    simtime_t rtt;
}

packet MPCPGate extends MPCP 
{
    simtime_t StartTime;
    uint16_t Length;
    uint32_t downLength;
    uint32_t queueLength[2];
    
    // version 0
//    simtime_t powerSavingStartTime;
    simtime_t powerSavingTime;
    uint16_t powerSavingMode;
    
    // version 1
    simtime_t pwsTime[2];
    uint16_t pwsMode[2];
    
    // loading aware
    double downQueueLoading;
    
}

packet MPCPReport extends MPCP 
{
    uint32_t QInfo;
    simtime_t powerSavingStartTime;
    bool sleepAck=false;
    bool dozeAck=false;
    bool invisible=false;
    uint32_t FinalGrantLen;
}
