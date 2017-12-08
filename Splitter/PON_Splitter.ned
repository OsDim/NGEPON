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

package mytest20140210.Splitter;

simple PON_Splitter
{
    parameters:
        @node();
        @display("i=device/hub");
        
        int haltOn = default(10);
        double up_data_rate;
    gates:
        inout oltUp;
        inout oltDown;
        inout onuUp[];
        inout onuDown[];
        inout oltUp2;
        inout oltDown2;
        inout onuUp2[];
        inout onuDown2[];
        inout oltUp3;
        inout oltDown3;
        inout onuUp3[];
        inout onuDown3[];
        inout oltUp4;
        inout oltDown4;
        inout onuUp4[];
        inout onuDown4[];
}
