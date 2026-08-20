#pragma once

#include "../requirement.hpp"
#include "../../../subway/subway.hpp"

#include <vector>

namespace sub {

// NOTE: Requirements must be readily verifiable (that is, they must be sendable to the subway's verify function with no transformations required)
class ReqStationExistsOnLine : public sub::Requirement {
private:
public:
	const std::pair<std::string_view, std::string_view> transfer;

	size_t type() const noexcept override;
	sub::RequirementPriority priority() const noexcept override;

	ReqStationExistsOnLine(std::string_view src, std::pair<std::string_view, std::string_view>);

	bool test(const Subway&) const override;

	//decltype(auto) begin() const noexcept {return transits.begin();}
	//decltype(auto) end() const noexcept {return transits.end();}
};

}


#include "../../../station/stationCRTP.hpp"
#include "../../../station/directStation/directStation.hpp"
#include "../../../station/multiLineStation/multiLineStation.hpp"
#include "../../../station/transitNodeStation/transitNodeStation.hpp"
#include "../../../subway/subway.hpp"

template<>
bool sub::TryFixStationCallable<sub::DirectStation, sub::ReqStationExistsOnLine>::operator()(this sub::DirectStation*, const sub::ReqStationExistsOnLine&);
template<>
bool sub::TryFixStationCallable<sub::MultiLineStation, sub::ReqStationExistsOnLine>::operator()(this sub::MultiLineStation* it, const sub::ReqStationExistsOnLine& r);
template<>
bool sub::TryFixStationCallable<sub::TransitNodeStation, sub::ReqStationExistsOnLine>::operator()(this sub::TransitNodeStation* it, const sub::ReqStationExistsOnLine& r);

//template<>
//bool sub::Subway::test(const ReqStationExistsOnLine&) const;
