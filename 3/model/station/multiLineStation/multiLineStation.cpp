#include "multiLineStation.hpp"

#include "../../common/requirement/reqLinesExist/reqLinesExist.hpp"

sub::MultiLineStation::MultiLineStation(const std::string_view n, const std::vector<std::string_view>& ls) :StationCRTP<MultiLineStation>(n), lines_{ls.begin(), ls.end()}
{
	if(ls.size() > 3zu)
		throw std::out_of_range("Error: Stations with transitions to other lines can only have up to 3 transitions.");
}

std::vector<std::unique_ptr<sub::Requirement>> sub::MultiLineStation::req() const {
	sub::station_req_fn_ret_t req(this->lines().size());
	for(const auto& l : this->lines())
		req.push_back(
#ifdef THRS
		std::make_unique<FutureRequirementModel<ReqLineExists>>(std::async(std::launch::async, [this](){
			return sub::ReqLineExists(this->name(), line) 
#else
				std::make_unique<sub::ReqLineExists>(this->name(), l) 
#endif
#ifdef THRS
		;}))
#endif
		);
	return req;
}

std::vector<std::string_view> sub::MultiLineStation::lines() const {
	return {lines_.begin(), lines_.end()};
}
