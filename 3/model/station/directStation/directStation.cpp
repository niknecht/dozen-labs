#include "directStation.hpp"
#include "../../common/requirement/reqLinesExist/reqLinesExist.hpp"
#include "../../common/traits/stationTraits.hpp"

std::vector<std::string_view> sub::DirectStation::lines() const {
	return {line};
}

sub::station_req_fn_ret_t sub::DirectStation::req() const {
	sub::station_req_fn_ret_t req(1zu);
	req.push_back(
#ifdef THRS
		std::make_unique<FutureRequirementModel<ReqLineExists>>(std::async(std::launch::async, [this](){
			return 
#endif
				sub::ReqLineExists(this->name(), line) 
#ifdef THRS
		;}))
#endif
	);
	return req;
}

sub::DirectStation::DirectStation(const std::string_view name, const std::string_view lines) : sub::StationCRTP<DirectStation>{name}, line{lines} 
{}
