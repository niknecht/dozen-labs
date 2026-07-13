#include "station.hpp"

#include <utility>

// CRTP boilerplate: Non polymorphic behaviour (base intialization)
template<typename Station>
sub::StationCRTP<Station>::StationCRTP(const std::string_view n) :m_name(n) {}

// CRTP boilerplate: Non polymorphics beahaviour
template<typename Station>
std::string_view sub::StationCRTP<Station>::name() const {
	return m_name;
}

// CRTP boilerplate: Compile-time polymorphism (dispatch to derived)
template<typename Station>
std::vector<std::string_view> sub::StationCRTP<Station>::lines() const {
	return static_cast<const Station>(*this).lines();
}// NOTE: Defining actual lines() implementation is responsibility of each derivee.
template<typename Station>
bool sub::StationCRTP<Station>::tryFix(const is_Req auto& r) {
	return (static_cast<Station*>(this))->tryFix(r);
}// NOTE: Defining actual tryFix for all specific station derivees is resposibility of each new requirement class 
template<typename Station>
bool sub::StationCRTP<Station>::verify(const is_Req auto& r) const {
	return static_cast<const Station>(*this).verify(r);
}// NOTE: Defining actual verify for all specific station derivees is resposibility of each new requirement class

// Type erasure boilertplate: external polymorphism free functions
bool sub::tryFix(is_Station auto& s, const is_Req auto& r) {
	return s.tryFix(r);
}
bool sub::verify(const is_Station auto& s, const is_Req auto& r) {
	return s.verify(r);
}
std::string_view sub::name(const is_Station auto& s) {
	return s.name();
}
std::vector<std::string_view> sub::lines(const is_Station auto& s) {
	return s.lines();
}

// Type erasure boilerplate: external polymorphism boilerplate (StationModel:: dispatching difinitions)
template<typename StationT> requires(sub::is_Station<StationT>)
bool sub::StationModel<StationT>::tryFix(const sub::Requirement& it) {
	return tryFix(obj, it);
};
template<typename StationT> requires(sub::is_Station<StationT>)
bool sub::StationModel<StationT>::verify(const sub::Requirement& it) const {
	return verify(obj, it);
}
template<typename StationT> requires(sub::is_Station<StationT>)
std::string_view sub::StationModel<StationT>::name() const {
	return name(obj);
}
template<typename StationT> requires(sub::is_Station<StationT>)
std::vector<std::string_view> sub::StationModel<StationT>::lines() const {
	return lines(obj);
}

// Note: the two classes above generate all nessesaty specializations for all the external polymorphism related
// boilerplate in the app. No further specialization is required on the behalf of station clases nor requirement classes.

// Type erasure boilerplate: prototype copy boilerplate
template <typename StationT> requires(sub::is_Station<StationT>)
std::unique_ptr<sub::StationConcept> sub::StationModel<StationT>::clone() const {
	return std::make_unique<StationModel<StationT>>(*this);
}

sub::Station::Station(const Station&s) :pimpl{s.pimpl->clone()}
{}

sub::Station& sub::Station::operator=(const Station& s) {
	Station oth{s};
	std::swap(*this, oth);
	return *this;
}

// Type erasure boilerplate: prototype initialization
template<typename StationT> requires(sub::is_Station<StationT>)
sub::StationModel<StationT>::StationModel(StationT s) :obj{std::move(s)}
{}

template <typename StationT>
sub::Station::Station(StationT s) :pimpl(std::make_unique(std::move(s)))
{}

// Type erasure boilerplate: external polymorphism boilerplate (Station:: dispatching definitions)
std::string_view sub::Station::name() const {
	return pimpl->name();
}
std::vector<std::string_view> sub::Station::lines() const {
	return pimpl->lines();
}
bool sub::Station::tryFix(const is_Req auto& r) {
	return pimpl->tryFix(r);
}
bool sub::Station::verify(const is_Req auto& r) const {
	return pimpl->verify(r);
}

// Stations aren't responsible for providing any FREE tryFix and verify implementations.
// Defining interaction between each new requirement is responsibility of the requirement itself.
// Stations are only liable of imposing required restrainsts on requirement classes.
