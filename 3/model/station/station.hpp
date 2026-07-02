#pragma once

namespace sub {
class Station;
}

#include "../subway/deps/requirement/requirement.hpp"

#include <string>
#include <vector>

namespace sub {

/*! @brief Class that represents a station entry.
 *
 * Owning lines were replaced with std::string line entry in each Station, as an unnessasary complication that would requier  a lot of complexity in verification.
 */
class Station {
private:
	std::string name;
	std::string line;
public:
	virtual std::vector<sub::Requirement*> req() = 0;
	virtual bool verify(const std::vector<sub::Requirement>& check) = 0;
};

}
