#include "reqStationsExistOnLines.hpp"

#include <typeinfo>
#include <stdexcept>

size_t sub::ReqStationExistsOnLine::type() const noexcept {
	return typeid(ReqStationExistsOnLine).hash_code();
}

sub::RequirementPriority sub::ReqStationExistsOnLine::priority() const noexcept {
	return sub::RequirementPriority::ReqStationsExistOnLines;
}

sub::ReqStationExistsOnLine::ReqStationExistsOnLine(std::string_view src, std::pair<std::string_view, std::string_view> trans) : Requirement(src), transfer({trans.first, trans.second})
{
	using namespace std::string_literals;
	//if(transits.size() > 3)
	//	throw std::out_of_range("Run-time error: Too many transitions for any station required by "s + std::string(src) + "."s);
	//else if(transits.empty())
	//	throw std::invalid_argument("Run-time error: "s + std::string(src) + " created an empty stations exist requirement."s);

	for (const auto& it : {trans}) {
		if(it.second.empty())
			throw std::invalid_argument("Run-time error: "s + std::string(src) + " emmited a stations exist requirement containing an empty target station name string."s);
		else if(it.first.empty())
			throw std::invalid_argument("Run-time error: "s + std::string(src) + " emmited a stations exist requirement containing an empty target station line string."s);
	}
}

#include "../../../station/stationCRTP.hpp"
#include "../../../station/directStation/directStation.hpp"
#include "../../../station/multiLineStation/multiLineStation.hpp"
#include "../../../station/transitNodeStation/transitNodeStation.hpp"

template<>
bool sub::TryFixStationCallable<sub::DirectStation, sub::ReqStationExistsOnLine>::operator()(this sub::DirectStation* it, const sub::ReqStationExistsOnLine& r) {
	// TODO Check if this line stisfies one of the transfers (subway is supposed to redirect the same req to all the transfers manually)
	using namespace std::string_literals;
	throw std::invalid_argument("Warning: "s + std::string(it->name()) + " (direct station) is not supposed to be a source of stations exist requirement, targeting "s + std::string(r.source()));
	
	return true;
}
template<>
bool sub::TryFixStationCallable<sub::MultiLineStation, sub::ReqStationExistsOnLine>::operator()(this sub::MultiLineStation* it, const sub::ReqStationExistsOnLine& r) {
	using namespace std::string_literals;
	throw std::invalid_argument("Warning: "s + std::string(it->name()) + " (multi-line station) is not supposed to be a source of stations exist requirement, targeting "s + std::string(r.source()));

	return true;
}
template<>
bool sub::TryFixStationCallable<sub::TransitNodeStation, sub::ReqStationExistsOnLine>::operator()(this sub::TransitNodeStation* it, const sub::ReqStationExistsOnLine& r) {
	// TODO Check if this line stisfies one of the transfers (subway is supposed to redirect the same req to all the transfers manually)
	using namespace std::string_literals;

	if(r.source() != it->name()) [[unlikely]]
		throw std::invalid_argument("Run-time error: "s + std::string(r.source()) + "'s failed requirement has been sent back to "s + std::string(it->name()) + " (a multi-line station)."s);

	const auto transfersCp = it->transfers();
	for(const auto& l : it->lines())
		if(std::pair{it->name(), l} == r.transfer)
			return false;

	it->remove_transfer(r.transfer.second);
	return true;
}

template<>
bool sub::VerifyStationCallable<sub::DirectStation, sub::ReqStationExistsOnLine>::operator()(this const sub::DirectStation* self, const sub::ReqStationExistsOnLine& r) {
	using namespace std::string_literals;
	if(self->name() != r.transfer.second) [[unlikely]]
		throw std::invalid_argument("Run-time error: A requirement with "s + std::string(r.transfer.second) + " as a target sent to "s + std::string(self->name()) + " (a direct station)."s);

	const auto linesCp = self->lines();
	if((std::ranges::find(linesCp, r.transfer.first) != linesCp.end()) && r.transfer.second == self->name())
		return true;
	else return false;
}
template<>
bool sub::VerifyStationCallable<sub::MultiLineStation, sub::ReqStationExistsOnLine>::operator()(this const sub::MultiLineStation* self, const sub::ReqStationExistsOnLine& r) {
	using namespace std::string_literals;
	if(self->name() != r.transfer.second) [[unlikely]]
		throw std::invalid_argument("Run-time error: A requirement with "s + std::string(r.transfer.second) + " as a target sent to "s + std::string(self->name()) + " (a multi-line station)."s);

	const auto linesCp = self->lines();
	if((std::ranges::find(linesCp, r.transfer.first) != linesCp.end()) && r.transfer.second == self->name())
		return true;
	else return false;
}
template<>
bool sub::VerifyStationCallable<sub::TransitNodeStation, sub::ReqStationExistsOnLine>::operator()(this const sub::TransitNodeStation* self, const sub::ReqStationExistsOnLine& r) {
	using namespace std::string_literals;
	if(self->name() != r.transfer.second) [[unlikely]]
		throw std::invalid_argument("Run-time error: A requirement with "s + std::string(r.transfer.second) + " as a target sent to "s + std::string(self->name()) + " (a transit node station)."s);

	const auto linesCp = self->lines();
	if((std::ranges::find(linesCp, r.transfer.first) != linesCp.end()) && r.transfer.second == self->name())
		return true;
	else return false;
}

