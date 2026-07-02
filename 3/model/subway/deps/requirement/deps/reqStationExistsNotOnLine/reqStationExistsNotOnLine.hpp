#pragma once

#include "../../requirement.hpp"

#include <vector>

namespace sub {

/*! @brief This is an implementation of a Requirement mainly emmited by TransitNode station objects directed at its connections.
 *
 * Because this class is tailored to TransitNode, this requirement combines individual station_line requirements together. Since only requirements at the same priority emmited by different staition van be evaluated in parallel, this grouping has no performance penalties either.
*/
class ReqStationExistsNotOnLine : public Requirement {
private:
	const constexpr static RequirementPriority priority {RequirementPriority::ReqStationExistsNotOnLine};
	std::vector<std::pair<std::string, std::string>> station_line;
public:
	ReqStationExistsNotOnLine() = delete;
	ReqStationExistsNotOnLine(std::vector<std::pair<std::string_view, std::string_view>>, std::string_view);

	ReqStationExistsNotOnLine(const ReqStationExistsNotOnLine&) = default;
	ReqStationExistsNotOnLine(ReqStationExistsNotOnLine&&) = default;
	ReqStationExistsNotOnLine& operator=(const ReqStationExistsNotOnLine&) = default;
	ReqStationExistsNotOnLine& operator=(ReqStationExistsNotOnLine&&) = default;
	~ReqStationExistsNotOnLine() = default;

	constexpr RequirementPriority kind() const noexcept override;
};

}
