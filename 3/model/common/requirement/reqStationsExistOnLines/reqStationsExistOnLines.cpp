#include "reqStationsExistOnLines.hpp"

#include <typeinfo>
#include <stdexcept>

size_t sub::ReqStationsExistOnLines::type() const noexcept {
	return typeid(ReqStationsExistOnLines).hash_code();
}

sub::RequirementPriority sub::ReqStationsExistOnLines::priority() const noexcept {
	return sub::RequirementPriority::ReqStationsExistOnLines;
}

sub::ReqStationsExistOnLines::ReqStationsExistOnLines(std::string_view src, std::vector<std::pair<std::string_view, std::string_view>> trans) : Requirement(src), transits(trans.begin(), trans.end())
{
	using namespace std::string_literals;
	if(transits.size() > 3)
		throw std::out_of_range("Run-time error: Too many transitions for any station required by "s + std::string(src) + "."s);
	else if(transits.empty())
		throw std::invalid_argument("Run-time error: "s + std::string(src) + " created an empty stations exist requirement."s);

	for (const auto& it : trans) {
		if(it.second.empty())
			throw std::invalid_argument("Run-time error: "s + std::string(src) + " emmited a stations exist requirement containing an empty station string."s);
		else if(it.first.empty())
			throw std::invalid_argument("Run-time error: "s + std::string(src) + " emmited a stations exist requirement containing an empty line string."s);
	}
}

#include "../../../station/stationCRTP.hpp"
#include "../../../station/directStation/directStation.hpp"
#include "../../../station/multiLineStation/multiLineStation.hpp"
#include "../../../station/transitNodeStation/transitNodeStation.hpp"

template<>
bool sub::TryFixStationCallable<sub::DirectStation, sub::ReqStationsExistOnLines>::operator()(this sub::DirectStation* it, const sub::ReqStationsExistOnLines& r) {
	// TODO Check if this line stisfies one of the transfers (subway is supposed to redirect the same req to all the transfers manually)
	using namespace std::string_literals;
	//throw std::invalid_argument("Warning: "s + std::string(it->name()) + " (direct station) is not supposed to be a target of stations exist requirement, originating from "s + std::string(r.source()));
	for(auto tr : r){
		//auto iter = std::ranges::find(it->transfers(), l,
		//		[](const std::pair<std::string_view, std::string_view> p) constexpr noexcept {return p.first;}); // std::get<0> doesn't work for some reason
		const auto linesCp = it->lines();
		if((std::ranges::find(linesCp, tr.first) != linesCp.end()) && tr.second == it->name())
			return true;
	}
	return false;
}
template<>
bool sub::TryFixStationCallable<sub::MultiLineStation, sub::ReqStationsExistOnLines>::operator()(this sub::MultiLineStation* it, const sub::ReqStationsExistOnLines& r) {
	// TODO Check if this line stisfies one of the transfers (subway is supposed to redirect the same req to all the transfers manually)
	using namespace std::string_literals;
	//throw std::invalid_argument("Warning: "s + std::string(it->name()) + " (multi-line station) is not supposed to be a target of stations exist requirement, originating from "s + std::string(r.source()));
	for(auto tr : r){
		//auto iter = std::ranges::find(it->transfers(), l,
		//		[](const std::pair<std::string_view, std::string_view> p) constexpr noexcept {return p.first;}); // std::get<0> doesn't work for some reason
		const auto linesCp = it->lines();
		if((std::ranges::find(linesCp, tr.first) != linesCp.end()) && tr.second == it->name())
			return true;
	}
	return false;
}
template<>
bool sub::TryFixStationCallable<sub::TransitNodeStation, sub::ReqStationsExistOnLines>::operator()(this sub::TransitNodeStation* it, const sub::ReqStationsExistOnLines& r) {
	// TODO Check if this line stisfies one of the transfers (subway is supposed to redirect the same req to all the transfers manually)
	using namespace std::string_literals;
	//throw std::invalid_argument("Warning: "s + std::string(it->name()) + " (multi-line station) is not supposed to be a target of stations exist requirement, originating from "s + std::string(r.source()));
	for(auto tr : r){
		//auto iter = std::ranges::find(it->transfers(), l,
		//		[](const std::pair<std::string_view, std::string_view> p) constexpr noexcept {return p.first;}); // std::get<0> doesn't work for some reason
		const auto linesCp = it->lines();
		if((std::ranges::find(linesCp, tr.first) != linesCp.end()) && tr.second == it->name())
			return true;
	}
	return false;
}
