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

package mytest20140210.traffic_gen;

simple TrafficGen
{
    parameters:
        @display("i=green/source");
       
        double offered_load = default(0.25);
        double high_priority_ratio = default(0.5);
        double max_rate;
        double rate;
        bool trafficPoisson = default(false);
	
	 //--------------------------------------------------------
        bool self_similar = default(false);
        bool asymmetric_flow = default(false) ;
		double pareto_rate = default(100 * 1000 * 1000);
		double pareto_alpha_on = default(1.4);
		double pareto_alpha_off = default(1.2);
		double pareto_beta_on = default(1);
		double multiple_of_flow = default(3.3);
	
	
    gates:
        inout genGate @directIn;
}