#pragma once

#include "../../station.hpp"

namespace sub {

/*! @brief This is a type of station that cannot have connection of its own.
 *
 * Even though DirectStation cannot form and control connections of its own, it was decided that it can be connected to by other types of stations for simplicity.
 */
class DirectStation : public Station {
public:
	std::vector<sub::Requirement*> req() override;
	bool verify(const std::vector<sub::Requirement>& check) override;
};

}
