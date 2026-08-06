#include "multiLineStation.hpp"

#include "../../common/requirement/reqLinesExist/reqLinesExist.hpp"

sub::MultiLineStation::MultiLineStation(const std::string_view n, const std::vector<std::string_view>& ls) :StationCRTP<MultiLineStation>(n), lines_{ls.begin(), ls.end()}
{
	if(ls.size() > 4zu)
		throw std::out_of_range("Error: Stations with transitions to other lines can only have up to 3 transitions.");
}

std::vector<std::unique_ptr<sub::Requirement>> sub::MultiLineStation::req() const {
	std::vector<std::unique_ptr<sub::Requirement>> req;
	req.push_back(std::make_unique<sub::ReqLinesExist>(this->name, std::vector<std::string_view>(lines_.begin(), lines_.end())));
	return req;
}

std::vector<std::string_view> sub::MultiLineStation::lines() const {
	return {lines_.begin(), lines_.end()};
}
