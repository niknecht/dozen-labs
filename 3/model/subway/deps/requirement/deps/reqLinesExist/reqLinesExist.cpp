#include "reqLinesExist.hpp"

#include <stdexcept>

sub::ReqLinesExist::ReqLinesExist(std::vector<std::string_view> req_lines, std::string_view src) 
	: Requirement(src), required_lines(req_lines.begin(), req_lines.end()) 
{
	if(req_lines.size() > 4)
		throw std::range_error("Runtime error: Attempt to form an excessive requirement.");
}

constexpr sub::RequirementPriority sub::ReqLinesExist::kind() const noexcept {
	return this->priority;
}
