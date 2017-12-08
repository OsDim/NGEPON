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

package mytest20140210;

import mytest20140210.traffic_gen.*;
import mytest20140210.ONU.*;
import mytest20140210.OLT.*;
import mytest20140210.Splitter.*;

channel EPONChannel extends ned.DatarateChannel
{
    // Do both plz useing {datarate = 1Gbps; delay=50000ns;}
    // 5km delay is 0.00005 1sec & 50ms & 50,000ns
    datarate = 100Gbps;	// Only calculate "msg.getByteLength()"
    //delay = 25000ns;	// Calculate by yourself from distence 5 kilo meters
    					// 5km =>25ms;(OLT->Split, Split->ONU) 10km =>50ms;(OLT->Split->ONU)	
}

channel Ethernet extends ned.DatarateChannel
{
//    datarate = 100Mbps;
	datarate = 100Gbps;
}

network EPON
{
    parameters:
        @display("bgb=835,485");
        int sizeOfONU;
        int version;
        int queueLimit;
        int algo;
        int awakeSize;
        int modes;
        double distance;
        double delayBound;
        double maxESTimer;
        double creditRatio;
        int minCrdLen;
        int setMTW;
        int winSize;
        bool trafficPoisson;
        bool creditBased;
        double dynamicPoint;
        double dyRatio;
        double downThreshold;
        double upThreshold;
        double multiMTW;
        bool MTW_algo;
        bool high_priority_must_empty ;
        bool asymmetric_flow ;
        double multiple_of_flow ;
        bool randomChannelAssign ;
		bool randomOrRR ;
        double tuningTime ;
        
     
    submodules:
        onu[sizeOfONU]: EPON_ONU {
            @display("p=516,307");
        }
        localNetwork[sizeOfONU]: LocalNetwork {
            @display("p=603,328");
        }
        trafficGen[sizeOfONU]: TrafficGen {
            @display("p=76,61");
        }
        splitter: PON_Splitter {
            @display("p=290,232");
        }
        olt: EPON_OLT {
            @display("p=89,153");
        }

    //        gen:Generator ;
    connections allowunconnected:
        // Olt to Splitter
        splitter.oltUp$o --> EPONChannel --> olt.eponUp$i;
        splitter.oltDown$i <-- EPONChannel <-- olt.eponDown$o;

        for i=0..sizeOfONU-1 {
            // ONUs to Splitter
            onu[i].eponUp$o --> EPONChannel --> splitter.onuUp$i++;
            splitter.onuDown$o++ --> EPONChannel --> onu[i].eponDown$i;
            // Local to ONU
//            localNetwork[i].ethUp$o --> {  datarate = 100Mbps; } --> onu[i].ethUp$i;
//            onu[i].ethDown$o --> {  datarate = 100Mbps; } --> localNetwork[i].ethDown$i;
            localNetwork[i].ethUp$o --> {  datarate = 100Gbps; } --> onu[i].ethUp$i;
            onu[i].ethDown$o --> {  datarate = 100Gbps; } --> localNetwork[i].ethDown$i;
        }
}


