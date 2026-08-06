#include "transitNodeStation.hpp"

#include "../../common/requirement/reqStationsExistOnLines/reqStationsExistOnLines.hpp"

std::vector<std::string_view> sub::TransferNodeStation::lines() const noexcept {
	return {line};
}

std::vector<std::unique_ptr<sub::Requirement>> sub::TransferNodeStation::req() const {
	std::vector<std::unique_ptr<sub::Requirement>> req;
	req.push_back(std::make_unique<sub::ReqStationsExistOnLines>(name(), std::vector<std::pair<std::string_view, std::string_view>>(transfers_.begin(), transfers_.end())));
	return req;
}

std::vector<std::pair<std::string_view, std::string_view>> sub::TransferNodeStation::transfers() const noexcept {
	return {transfers_.begin(), transfers_.end()};
}

sub::TransferNodeStation::TransferNodeStation(std::string_view name, std::vector<std::pair<std::string_view, std::string_view>> transfers) 
	: StationCRTP<TransferNodeStation>(name), 
	  transfers_{transfers.begin(), transfers.end()}
{}
