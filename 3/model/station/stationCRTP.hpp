#pragma once

#include <string>
#include <memory>
#include <functional>

#include "../common/traits/stationTraits.hpp"
#include "../common/traits/requirementTraits.hpp"
#include "freeFunctionRegistry/freeFunctionRegistry.hpp"
#include "station.hpp"

namespace sub {

extern FreeFunctionRegistry registry_tryFix;
extern FreeFunctionRegistry registry_verify;

// TODO Write deduction guides for the parameters so that the specializations definitions would look exactly as they
// would for a member function previously
template<is_Station S, is_Req R>
class TryFixStationCallable;

template<is_Station S, is_Req R>
class VerifyStationCallable;

/*! Station CRTP base.
 *
 * CRTP is chosen here as a replacement for virtual functions because it allows template arguments in signature.
 * The compile-time mechanism allows functions that define interaction of this station type with all requirement types 
 * (known at compile time) to be declared at compile time, enforcing correct use of requirement-station mechanism at compile time.
 * The lack of interaction between different types of stations makes it very easy to erase station types, making CRTP all the more viable option.
 * New station types should not have new public functions to follow the typical interface.
 *
 * @see CRTP
 */
template <typename Station>
class StationCRTP {
	std::string m_name;
	std::unique_ptr<Station> m_it;
public:
	std::string_view name() const noexcept; //!< Non-polymorphic behaviour, doesn't dispatch to the derived
	std::vector<std::string_view> lines() const noexcept; //!< Polymorphic lines() getter. Defined automatically for every base. Defining dispatch-to method is the reqponsibility of the derivees.
	sub::station_req_fn_ret_t req() const noexcept; //!< Polymorphic request to get a list of requirements from this station to the rest of the stations and the subway itself. Defining the actual dispatch-to method is resposibility of the station derivitees.
	//bool tryFix(const is_Req auto&); //!< Polymorphic tryFix method. Defined automatically for every base. Defining dispatch-to method is the reqponsibility of the requirements.
	//bool verify(const is_Req auto&) const; //!< Polymorphic verify method. Defined automatically for every base. Defining dispatch-to method is the reqponsibility of the requirements.
	template<is_Req R>
	static TryFixStationCallable<Station, R> tryFix; //!< Its operator() accepts explicit this, hence, the object itself can be static
	
	template<is_Req R>
	static VerifyStationCallable<Station, R> verify; //!< Its operator() accepts explicit this, hence, the object itself can be static

	StationCRTP<Station>(const std::string_view n); //!< Non-polymorphic behaviour, required in order to have non-polymorphic name() getter

	//static_assert(is_Station<Station>);
};

template<is_Station S, is_Req R>
class TryFixStationCallable {
private:
	static constexpr const FreeTryFixFunctor<S, R> func {func};
	//! (i) Constexpr everything (ii) blame Sam Hyde
	static constexpr const FreeFunctionRegistrar<S, R> register_{registry_tryFix, reinterpret_cast<void*>(&func.tryFix)}; //TODO Only register if not registered already.
public:
	using TryFix = bool(S&, const Requirement&); //!< Use this to cast the retrieved from the registry function before call
	bool operator()(this S*, const R&); //!< Actual model logics
};

template<is_Station S, is_Req R>
class VerifyStationCallable {
private:
	static constexpr const FreeVerifyFunctor<S, R> func {func}; //!< @see The hardest question in programming
	//! (i) Constexpr everything (ii) blame Sam Hyde
	static constexpr const FreeFunctionRegistrar<S, R> register_{registry_verify, reinterpret_cast<void*>(&func.verify)}; //TODO Control that all type hashes are for remove_const and remove_reference
public:
	using Verify = bool(const S&, const Requirement&); //!< Use this to cast the retrieved from the registry function before call
	bool operator()(this const S* const, const R&); //!< Actual model logics
};


}

// -------------------------- Template definitions --------------------------------------


// CRTP boilerplate: Non polymorphic behaviour (base intialization)
template<typename Station>
sub::StationCRTP<Station>::StationCRTP(const std::string_view n) :m_name(n) {
	if(n.empty())
		throw std::invalid_argument("Run-time error: Bad station name.");
}

// CRTP boilerplate: Non polymorphics beahaviour
template<typename Station>
std::string_view sub::StationCRTP<Station>::name() const noexcept {
	return m_name;
}

// CRTP boilerplate: Compile-time polymorphism (dispatch to derived)
template<typename Station>
std::vector<std::string_view> sub::StationCRTP<Station>::lines() const noexcept {
	return static_cast<const Station*>(this)->lines();
}// NOTE: Defining actual lines() implementation is responsibility of each derivee.
template<typename Station>
sub::station_req_fn_ret_t sub::StationCRTP<Station>::req() const noexcept{
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
