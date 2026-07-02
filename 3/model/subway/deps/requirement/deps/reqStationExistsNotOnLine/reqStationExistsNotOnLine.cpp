#include "reqStationExistsNotOnLine.hpp"

#include <stdexcept>

constexpr sub::RequirementPriority sub::ReqStationExistsNotOnLine::kind() const noexcept {
	return priority;
}

sub::ReqStationExistsNotOnLine::ReqStationExistsNotOnLine(std::vector<std::pair<std::string_view, std::string_view>> s_l, std::string_view src)
	: Requirement(src), station_line(s_l.begin(), s_l.end()) {
	if(s_l.size() > 3)
		throw std::range_error("Runtime error: Excessive requirement on TransitNode connections.");
}
