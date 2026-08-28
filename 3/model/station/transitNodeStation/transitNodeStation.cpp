#include "transitNodeStation.hpp"

#include "../../common/requirement/reqStationsExistOnLines/reqStationsExistOnLines.hpp"
#include "../../common/requirement/reqLinesExist/reqLinesExist.hpp"


std::vector<std::string_view> sub::TransitNodeStation::lines() const noexcept {
	return {line_};
}

sub::station_req_fn_ret_t sub::TransitNodeStation::req() const {
	std::vector<std::unique_ptr<sub::Requirement>> req(2zu);
	req.push_back(
#ifdef THRS
		std::make_unique<FutureRequirementModel<ReqLineExists>>(std::async(std::launch::async, [this](){
			return sub::ReqLineExists(this->name(), line) 
#else
		std::make_unique<sub::ReqLineExists>(this->name(), line_) 
#endif
#ifdef THRS
		;}))
#endif
	);
	for (const auto& t : this->transfers())
		req.push_back(
#ifdef THRS
		std::make_unique<FutureRequirementModel<ReqStationExistsOnLine>>(std::async(std::launch::async, [this](){
			return sub::ReqStationExistsOnLine(name(), std::pair<std::string_view, std::string_view>(t.first, t.second))
#else
			std::make_unique<sub::ReqStationExistsOnLine>(name(), std::pair<std::string_view, std::string_view>(t.first, t.second))
#endif
#ifdef THRS
		;}))
#endif
		);
	return req;
}

std::vector<std::pair<std::string_view, std::string_view>> sub::TransitNodeStation::transfers() const noexcept {
	return {transfers_.begin(), transfers_.end()};
}

sub::TransitNodeStation::TransitNodeStation(std::string_view name, std::string_view line, std::vector<std::pair<std::string_view, std::string_view>> transfers) 
	: StationCRTP<TransitNodeStation>(name), 
	  line_{line}, transfers_{transfers.begin(), transfers.end()}
{
	using namespace std::string_literals;
	if(transfers_.size() > 3zu)
		throw std::out_of_range("Run-time error: Too many transfers per a transit node station"s + std::string(this->name()) + ". Only up to three are allowed."s);
	else if(line_.empty())
		throw std::out_of_range("Run-time error: Bad line name for "s + std::string(this->name()) + ". Empty names are not allowed."s);
}

void sub::TransitNodeStation::remove_transfer(const std::string_view name) noexcept {
	this->transfers_.erase(std::ranges::find(transfers_, name, [](const auto& pair){return pair.second;}));
}
