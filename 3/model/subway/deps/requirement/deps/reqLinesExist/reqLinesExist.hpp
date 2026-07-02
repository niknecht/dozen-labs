#pragma once

#include "../../requirement.hpp"

#include <vector>

namespace sub {

/*! @brief Requires from Subway that one or a number of lines exist.
 *
 * This can safely be checked first, since placing a station in a delete list for any other reason doesn't undo existance of a line.
 */
class ReqLinesExist : public Requirement {
private:
	static const RequirementPriority priority {sub::RequirementPriority::ReqLinesExist};
	std::vector<std::string> required_lines;
public:
	ReqLinesExist() = delete;
	ReqLinesExist(const ReqLinesExist&) = default;
	ReqLinesExist(ReqLinesExist&&) = default;
	ReqLinesExist& operator=(const ReqLinesExist&) = default;
	ReqLinesExist& operator=(ReqLinesExist&&) = default;
	~ReqLinesExist() = default;

	ReqLinesExist(std::vector<std::string_view> required_lines, std::string_view source);
	
	constexpr RequirementPriority kind() const noexcept override;
};

}
