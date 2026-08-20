#include "transitNodeStation.hpp"

#include "../../common/requirement/reqStationsExistOnLines/reqStationsExistOnLines.hpp"

std::vector<std::string_view> sub::TransitNodeStation::lines() const noexcept {
	return {line};
}

std::vector<std::unique_ptr<sub::Requirement>> sub::TransitNodeStation::req() const {
	std::vector<std::unique_ptr<sub::Requirement>> req;
	for (const auto& t : this->transfers())
		req.push_back(std::make_unique<sub::ReqStationExistsOnLine>(name(), std::pair<std::string_view, std::string_view>(t.first, t.second)));
	return req;
}

std::vector<std::pair<std::string_view, std::string_view>> sub::TransitNodeStation::transfers() const noexcept {
	return {transfers_.begin(), transfers_.end()};
}

sub::TransitNodeStation::TransitNodeStation(std::string_view name, std::vector<std::pair<std::string_view, std::string_view>> transfers) 
	: StationCRTP<TransitNodeStation>(name), 
	  transfers_{transfers.begin(), transfers.end()}
{}
