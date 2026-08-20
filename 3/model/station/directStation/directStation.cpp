#include "directStation.hpp"
#include "../../common/requirement/reqLinesExist/reqLinesExist.hpp"

std::vector<std::string_view> sub::DirectStation::lines() const {
	return {line};
}

std::vector<std::unique_ptr<sub::Requirement>> sub::DirectStation::req() const {
	std::vector<std::unique_ptr<sub::Requirement>> req(1zu);
	req.push_back(std::make_unique<sub::ReqLineExists>(this->name(), line));
	return req;
}

sub::DirectStation::DirectStation(const std::string_view name, const std::string_view lines) : sub::StationCRTP<DirectStation>{name}, line{lines} 
{}
