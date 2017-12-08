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

package mytest20140210.OLT;

module EPON_OLT
{
    parameters:

        @display("i=device/router;bgb=363,225");
    gates:
        inout ethUp;
        inout ethDown;
        inout eponUp;
        inout eponDown;
        inout eponUp2;
        inout eponDown2;
        inout eponUp3;
        inout eponDown3;
        inout eponUp4;
        inout eponDown4;

    submodules:
        onuTable: ONUTable;
        olt_Scheduler: OLT_Scheduler {
            @display("p=134,183");
        }
        dba: DBA {
            @display("p=134,65");
        }
    connections allowunconnected:
		// --- Upstream ---
        olt_Scheduler.Up$i <-- eponUp$i;
        dba.Up$i <-- olt_Scheduler.Up$o;
        ethUp$o <-- dba.Up$o;
        
        olt_Scheduler.Up2$i <-- eponUp2$i;
        dba.Up2$i <-- olt_Scheduler.Up2$o;
        
        olt_Scheduler.Up3$i <-- eponUp3$i;
        dba.Up3$i <-- olt_Scheduler.Up3$o;
        
        olt_Scheduler.Up4$i <-- eponUp4$i;
        dba.Up4$i <-- olt_Scheduler.Up4$o;
        
        

        // --- Downstream ---
        ethDown$i --> dba.Down$i;
        dba.Down$o --> olt_Scheduler.Down$i;
        olt_Scheduler.Down$o --> eponDown$o;
        
        
        dba.Down2$o --> olt_Scheduler.Down2$i;
        olt_Scheduler.Down2$o --> eponDown2$o;
        
        dba.Down3$o --> olt_Scheduler.Down3$i;
        olt_Scheduler.Down3$o --> eponDown3$o;
        
        dba.Down4$o --> olt_Scheduler.Down4$i;
        olt_Scheduler.Down4$o --> eponDown4$o;
}
