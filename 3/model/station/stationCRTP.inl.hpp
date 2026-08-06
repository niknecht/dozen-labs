#include <utility>
#include <exception>

#include "stationCRTP.hpp"

// CRTP boilerplate: Non polymorphic behaviour (base intialization)
template<typename Station>
sub::StationCRTP<Station>::StationCRTP(const std::string_view n) :m_name(n) {
	if(n.empty())
		throw std::invalid_argument("Bad station name.");
}

// CRTP boilerplate: Non polymorphics beahaviour
template<typename Station>
std::string_view sub::StationCRTP<Station>::name() const {
	return m_name;
}

// CRTP boilerplate: Compile-time polymorphism (dispatch to derived)
template<typename Station>
std::vector<std::string_view> sub::StationCRTP<Station>::lines() const {
	return static_cast<const Station*>(this)->lines();
}// NOTE: Defining actual lines() implementation is responsibility of each derivee.
template<typename Station>
std::vector<std::unique_ptr<sub::Requirement>> sub::StationCRTP<Station>::req() const {
	return static_cast<const Station*>(this)->req();
} // NOTE: Defining the actual req() implementation is responsibility of each derivee.
/*template<typename Station>
bool sub::StationCRTP<Station>::tryFix(const is_Req auto& r) {
	return (static_cast<Station*>(this))->tryFix(r);
}// NOTE: Defining actual tryFix for all specific station derivees is resposibility of each new requirement class 
template<typename Station>
bool sub::StationCRTP<Station>::verify(const is_Req auto& r) const {
	return static_cast<const Station*>(this)->verify(r);
}*/// NOTE: Defining actual verify for all specific station derivees is resposibility of each new requirement class
