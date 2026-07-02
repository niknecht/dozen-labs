#pragma once

#include "../../requirement.hpp"

namespace sub {

/*! @brief Requires from Subway that one or a number of lines exist.
 */
class ReqLinesExist : public Requirement {
private:
	std::vector<std::string_view> required_lines;
};

}
