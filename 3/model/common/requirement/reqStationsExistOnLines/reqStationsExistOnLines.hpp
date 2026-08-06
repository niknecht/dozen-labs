#pragma once

#include "../requirement.hpp"

#include <vector>

namespace sub {

class ReqStationsExistOnLines : public sub::Requirement {
private:
	const std::vector<std::pair<std::string_view, std::string_view>> transits;
public:
	size_t type() const noexcept;
	sub::RequirementPriority priority() const noexcept;

	ReqStationsExistOnLines(std::string_view src, std::vector<std::pair<std::string_view, std::string_view>>);

	decltype(auto) begin() const noexcept {return transits.begin();}
	decltype(auto) end() const noexcept {return transits.end();}
};

}
