#pragma once

#include "../stationCRTP.hpp"
#include "../../common/traits/stationTraits.hpp"

namespace sub{

class TransitNodeStation : public StationCRTP<TransitNodeStation>{
private:
	std::string line;
	std::vector<std::pair<std::string, std::string>> transfers_;
public:
	std::vector<std::string_view> lines() const noexcept;
	std::vector<std::unique_ptr<sub::Requirement>> req() const;
	std::vector<std::pair<std::string, std::string>>& transfers() const noexcept;
	void remove_transfer(const decltype(transfers_.begin())) noexcept;

	explicit TransitNodeStation(std::string_view name, std::vector<std::pair<std::string_view, std::string_view>> transfers);
};
static_assert(is_Station<TransitNodeStation>);

}
