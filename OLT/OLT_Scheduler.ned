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

simple OLT_Scheduler
{
    parameters:
        double down_data_rate;
    gates:
		inout Up;
        inout Down;
        inout Up2;
        inout Down2;
        inout Up3;
        inout Down3;
        inout Up4;
        inout Down4;
		input in @directIn;
		input in2 @directIn;
		input in3 @directIn;
		input in4 @directIn;
}