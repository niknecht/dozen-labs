#include "reqLinesExist.hpp"

#include <typeinfo>
#include <vector>
#include <stdexcept>

sub::ReqLinesExist::ReqLinesExist(const std::string_view src, const std::vector<std::string_view>& lines) : Requirement{src}, requiredLines{lines.begin(), lines.end()}
{
	using namespace std::string_literals;
	if(lines.empty()) [[unlikely]]{
		throw std::invalid_argument("Run-time error: Empty line exist requirement emmited by "s + std::string(src) + ".");
	}
	else if (lines.size() > 3) [[unlikely]]{
		throw std::invalid_argument("Run-time error: Empty line exist requirement emmited by "s + std::string(src) + ".");
	}

	for (const auto& it : lines)
		if (it.empty()) [[unlikely]]
			throw std::out_of_range("Run-time error: "s + std::string(src) + " emmitted bad requirement: empty line names are not allowed."s);
}

size_t sub::ReqLinesExist::type() const noexcept{
	return typeid(ReqLinesExist).hash_code();
}

sub::RequirementPriority sub::ReqLinesExist::priority() const noexcept {
	return sub::RequirementPriority::ReqLinesExist;
}

std::vector<std::string_view> sub::ReqLinesExist::required_lines() const noexcept {
	return {requiredLines.begin(), requiredLines.end()};
}

// Interaction functions for this requirement with all kinds of stations
// Keep these here, so that they exist in the main's translation block
#include "../../../station/directStation/directStation.hpp"
#include "../../../station/multiLineStation/multiLineStation.hpp"
#include "../../../station/transitNodeStation/transitNodeStation.hpp"

#include <algorithm>
#include <ranges>

namespace sub{
/*! Local uset-defined function that moves generic verification code for ReqLinesExist.
 *
 * The overhead of this should be near zero unless there's an exception.*/
template<is_Station S>
static void constexpr inline check_source_station_against_lines_exist(const ReqLineExists& r, const S* it) noexcept(false) {
	// Generic station checks
	using namespace std::string_literals;
	if (r.required_lines().empty()) [[unlikely]] // Speculative execution negates the overhead of all the checks.
		throw std::invalid_argument("Run-time error: "s + std::string(r.source()) + "'s ("s + typeid(S).name() + " station) requirement"s);
	if(r.required_lines().size() != it->lines().size()) [[unlikely]]
		throw std::invalid_argument("Run-time error: "s + std::string(r.source()) + " (a " +typeid(S).name()+" station) has emmited an excessive requirement.");
	for(const auto& it : it->lines())
		if (std::find(r.required_lines().begin(), r.required_lines().end(), it) == r.required_lines().end()) [[unlikely]]
			throw std::invalid_argument("Run-time error: "s + std::string(r.source()) + " (a "+typeid(S).name()+" station) has emmited an excessive requirement.");
	if (r.source() != it->name()) [[unlikely]]
		throw std::invalid_argument("Run-time error: "s + std::string(r.source()) + "'s "+typeid(S).name()+" station) requirement has been redirected to "s + std::string(it->name()) + "."s);
}
}

//template<>
template<>
bool sub::TryFixStationCallable<sub::DirectStation, sub::ReqLineExists>::operator()(this sub::DirectStation* it, const sub::ReqLineExists& r) {
	sub::check_source_station_against_lines_exist<std::remove_pointer_t<decltype(it)>>(r, it);
	// TODO Check relevance, if not relevant -- throw
	// TODO If relevant, return falsea

	// Logics
	return false;
}
template<>
bool sub::TryFixStationCallable<sub::MultiLineStation, sub::ReqLineExists>::operator()(this sub::MultiLineStation* it, const sub::ReqLineExists& r) {
	using namespace std::string_literals;
	sub::check_source_station_against_lines_exist<std::remove_pointer_t<decltype(it)>>(r, it);

	// Logics
	return false;
}
template<>
bool sub::TryFixStationCallable<sub::TransitNodeStation, sub::ReqLineExists>::operator()(this sub::TransitNodeStation* it, const sub::ReqLineExists& r) noexcept(false) {
	sub::check_source_station_against_lines_exist<std::remove_pointer_t<decltype(it)>>(r, it);
	throw(std::invalid_argument("Warning: TransitNodeStation should not emmit ReqLinesExist: use sub::ReqStationsExistOnLines."));

	// Logics
	for(const auto i : it->lines())
		if(std::find(r.required_lines().begin(), r.required_lines().end(), i) != r.required_lines().end())
			return false;
	for(auto l : r.required_lines()){
		auto iter = std::ranges::find(it->transfers(), l,
				[](const std::pair<std::string_view, std::string_view> p) constexpr noexcept {return p.first;}); // std::get<0> doesn't work for some reason
		if(iter != std::ranges::end(it->transfers())) [[likely]]
			it->remove_transfer(iter->second);
	}
	return true;
}

template<>
bool sub::VerifyStationCallable<sub::DirectStation, sub::ReqLineExists>::operator()(this const sub::DirectStation* const it, const ReqLineExists& r) {
	using namespace std::string_literals;
	throw std::invalid_argument("Run-time error: Requirement of type 'lines exist' from "s + std::string(r.source()) + " is not directed to "s + std::string(it->name())\
			+ " (DirectStation). Note: Requirements of type 'lines exist' don't have a recipient, they must be handled by the Subway class itself."s);
	return true;
}
template<>
bool sub::VerifyStationCallable<sub::MultiLineStation, sub::ReqLineExists>::operator()(this const sub::MultiLineStation* const it, const ReqLineExists& r) {
	using namespace std::string_literals;
	throw std::invalid_argument("Run-time error: Requirement of type 'lines exist' from "s + std::string(r.source()) + " is not directed to "s + std::string(it->name())\
			+ " (multi-line station). Note: Requirements of type 'lines exist' don't have a recipient, they must be handled by the Subway class itself."s);
	return true;
}
template<>
bool sub::VerifyStationCallable<sub::TransitNodeStation, sub::ReqLineExists>::operator()(this const sub::TransitNodeStation* const it, const ReqLineExists& r) {
	using namespace std::string_literals;
	throw std::invalid_argument("Run-time error: Requirement of type 'lines exist' from "s + std::string(r.source()) + " is not directed to "s + std::string(it->name())\
			+ ". Note: Requirements of type 'lines exist' don't have a recipient, they must be handled by the Subway class itself."s);
	return true;
}
