#pragma once

#include "../requirement.hpp"

#include <string_view>
#include <vector>

#include "../../../subway/subway.hpp"

namespace sub {

class ReqLineExists : public sub::Requirement {
private:
	//static const RequirementTag<sub::ReqLinesExist> tag_;
	//static RequirementPriority priority_;

	RequirementPriority priority() const noexcept override;

	std::string required_line_;
public:
	size_t type() const noexcept override;
	std::vector<std::string_view> required_lines() const noexcept;
	
	explicit ReqLineExists(const std::string_view, const std::string_view);

	bool test(const Subway&) const override;
};
static_assert(sub::is_Req<ReqLineExists>);

// TODO: Declare all interaction functions here

}


// Interaction functions for this requirement with all kinds of stations
// Keep these here, so that they exist in the main's translation block
#include "../../../station/directStation/directStation.hpp"
#include "../../../station/multiLineStation/multiLineStation.hpp"
#include "../../../station/transitNodeStation/transitNodeStation.hpp"

#include <algorithm>
#include <ranges>


//template<>
template<>
bool sub::TryFixStationCallable<sub::DirectStation, sub::ReqLineExists>::operator()(this sub::DirectStation* it, const sub::ReqLineExists& r);
template<>
bool sub::TryFixStationCallable<sub::MultiLineStation, sub::ReqLineExists>::operator()(this sub::MultiLineStation* it, const sub::ReqLineExists& r);
template<>
bool sub::TryFixStationCallable<sub::TransitNodeStation, sub::ReqLineExists>::operator()(this sub::TransitNodeStation* it, const sub::ReqLineExists& r) noexcept(false);

template<>
bool sub::VerifyStationCallable<sub::DirectStation, sub::ReqLineExists>::operator()(this const sub::DirectStation* const it, const ReqLineExists& r);
template<>
bool sub::VerifyStationCallable<sub::MultiLineStation, sub::ReqLineExists>::operator()(this const sub::MultiLineStation* const it, const ReqLineExists& r);
template<>
bool sub::VerifyStationCallable<sub::TransitNodeStation, sub::ReqLineExists>::operator()(this const sub::TransitNodeStation* const it, const ReqLineExists& r);

// TODO Explicit this Subway or someth, make this use getters of Subway and Stations to test the requirement
// And add this to the concept
//bool sub::ReqLineExists::test(this const Subway*);
