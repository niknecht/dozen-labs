#include "station.hpp"
//#include "stationCRTP.hpp"

#include <utility>


// Note: the two classes above generate all nessesaty specializations for all the external polymorphism related
// boilerplate in the app. No further specialization is required on the behalf of station clases nor requirement classes.

sub::Station::Station(const Station&s) :pimpl{s.pimpl->clone()}
{}

sub::Station& sub::Station::operator=(const Station& s) {
	Station oth{s};
	std::swap(*this, oth);
	return *this;
}

// Type erasure boilerplate: external polymorphism boilerplate (Station:: dispatching definitions)
std::string_view sub::Station::name() const {
	return pimpl->name();
}
std::vector<std::string_view> sub::Station::lines() const {
	return pimpl->lines();
}
bool sub::Station::tryFix(const Requirement& r) {
	return pimpl->tryFix(r);
}
bool sub::Station::verify(const Requirement& r) const {
	return pimpl->verify(r);
}
sub::station_req_fn_ret_t sub::Station::req() const {
	return pimpl->req();
}

// Double dispatch boilerplate on the Station's side
//bool Station::verify(const sub::Requirement&)

// Stations aren't responsible for providing any FREE tryFix and verify implementations.
// Defining interaction between each new requirement is responsibility of the requirement itself.
// Stations are only liable of imposing required restrainsts on requirement classes.
