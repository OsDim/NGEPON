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

package mytest20140210.ONU;

import mytest20140210.ONU.Classifier;
import mytest20140210.ONU.Scheduler;
import mytest20140210.ONU.PowerControler;

module EPON_ONU
{
    parameters:
        @display("i=device/modem;bgb=308,258");
        // int count @prompt("Enter count:");

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
        scheduler: Scheduler {
            @display("p=155,76");
        }
        classifier: Classifier {

            @display("p=155,176");
        }
        powerControler: PowerControler;
    connections allowunconnected:

        // --- Upstream ---
        classifier.Up$i <-- ethUp$i;
        scheduler.Up$i <-- classifier.Up$o;
        eponUp$o <-- scheduler.Up$o;
        
        scheduler.Up2$i <-- classifier.Up2$o;
        eponUp2$o <-- scheduler.Up2$o;
        
        scheduler.Up3$i <-- classifier.Up3$o;
        eponUp3$o <-- scheduler.Up3$o;
        
        scheduler.Up4$i <-- classifier.Up4$o;
        eponUp4$o <-- scheduler.Up4$o;

        // --- Downstream ---
        eponDown$i --> scheduler.Down$i;
        scheduler.Down$o --> classifier.Down$i;
        classifier.Down$o --> ethDown$o;
        eponDown2$i --> scheduler.Down2$i;
        scheduler.Down2$o --> classifier.Down2$i;
        
        eponDown3$i --> scheduler.Down3$i;
        scheduler.Down3$o --> classifier.Down3$i;
        
        eponDown4$i --> scheduler.Down4$i;
        scheduler.Down4$o --> classifier.Down4$i;
        

}
