#pragma once

#include "../stationCRTP.hpp"

namespace sub{

class DirectStation : public sub::StationCRTP<DirectStation> {
private:
	std::string line_;
public:
	std::vector<std::string_view> lines() const;
	sub::station_req_fn_ret_t req() const;
	explicit DirectStation(const std::string_view name, const std::string_view line);
};

}

static_assert(sub::is_Station<sub::DirectStation>);
