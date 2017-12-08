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

#ifndef MPCPTOOLS_H_
#define MPCPTOOLS_H_

#include <inttypes.h>
#include "traffic_gen/Messages/EPON_messages_m.h"

class MPCPTools {
public:
    MPCPTools();
    virtual ~MPCPTools();
    uint64_t nsToSimTime(uint64_t time);
    uint64_t bitsToNS16(uint64_t bits, uint8_t gigRate);
    uint64_t bytesToNS16(uint64_t bytes, uint8_t gigRate);
};

#endif /* MPCPTOOLS_H_ */
