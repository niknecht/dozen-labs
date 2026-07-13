#pragma once

namespace sub {
/*! @brief This is a Repository pattern class that stores Stations, searches and verifies them.
* 
* The verification process is broke down into two parts: (i) iterating over the list of stations, gathering requirements, and (ii) iterating over the list of requirements, checking whether each one of them (except for the ones that place requirements onto stations that are already in the delete list) is satisfied and adding stations.
* It is guaranteed that no two threads would be checking requirements for the same station, because stations must only get deleted if there is no possibility of fixing.
*/
class Subway {
private:
public:
};


}
