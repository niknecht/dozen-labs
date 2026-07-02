#pragma once

#include <stdint.h>

namespace sub{
class Requirement;
}

#include "../../../station/station.hpp"

namespace sub {

/*! @brief Class that manages requirements posed by one station onto another. These can later be checked by the Subway class.
 * 
 * The guarantee is given that cheking requirements of lower priority does not break already checked requirements of higher priorities.
 */
enum class RequirementPriority : uint32_t {
	ReqLinesExist,
	ReqStationExistsNotOnLine
};

class Requirement {
private:
	static RequirementPriority priority;
	Station* source;
public:
	bool operator<(const Requirement& other);
	bool operator>(const Requirement& other);
	bool operator==(const Requirement& other);
};
}
