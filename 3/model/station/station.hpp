#pragma once

#include <memory>
#include <vector>

#include "freeFunctionRegistry/freeFunctionRegistry.hpp"

namespace sub {
class Station;
class StationConcept;
}


namespace sub {
class Station;

class Requirement;


template<typename>
class StationCRTP;

}

#include "../common/requirement/requirement.hpp"
#include "../common/traits/stationTraits.hpp"
#include "../common/traits/requirementTraits.hpp"

namespace sub {


/*! Type erased station 
 *
 * @example*/
/*
 * @example Works!! This below is responsibility of the Requirements*/
/*template<>
bool DirectStation::tryFix<ReqLinesExist>(const ReqLinesExist&) { // These two specializations will usually for the same type, but this is just for an example
	return false;
} //TODO Above is actually correct, below is not, below just dispatch to the derived method (defined like above). That could be defined automatically.
template<>
template<> // <typename Station>
bool StationCRTP<DirectStation>::verify<ReqLinesExist>(const ReqLinesExist& r) const {
	return false; // return static_cast<const Station>(*this).verify(r);
}*/
// ^ New requirement types are supposed to do this ^
//static_assert(is_Station<StationCRTP<DirectStation>>);

//! These have to be in the global namespace in order for concepts to use them, and for Station declarations to use the concepts.
class StationConcept {
public:
	virtual ~StationConcept() = default;

	virtual std::string_view name() const = 0; //!< Getters are essential in the type erasure approach, else type would not be really erased
	virtual std::vector<std::string_view> lines() const = 0; //!< Getters are essential in the type erasure approach, else type would not be really erased
	virtual std::vector<std::unique_ptr<sub::Requirement>> req() const = 0; //!< inteface to the underlying Station class' req method
	virtual bool tryFix(const sub::Requirement&) = 0;	//!< @see double dispatch
	virtual bool verify(const sub::Requirement&) const = 0; //!< @see double dispatch
	virtual std::unique_ptr<StationConcept> clone() const = 0; //!< @see Prototype
};

//! These have to be in the global namespace in order for concepts to use them, and for Station declarations to use the concepts.
template<class StationT> requires(is_Station<StationT>)
class StationModel : StationConcept {
public:
	StationModel(StationT);

	std::string_view name() const override; //!< Getters are essential in the type erasure approach, else type would not be really erased. Because of nature of the Concept class in type erasure pattern, unfortunately, this has to be virtual.
	std::vector<std::string_view> lines() const override; //!< Getters are essential in the type erasure approach, else type would not be really erased
	std::vector<std::unique_ptr<sub::Requirement>> req() const override; //!< This returns a moved list of requirements to the subway system
	bool tryFix(const sub::Requirement& it) override; //!< @see double dispatch
	bool verify(const sub::Requirement& it) const override; //!< @see double dispatch
	std::unique_ptr<StationConcept> clone() const override; //!< @see Prototype
	
	/* NOTE: Double dispatch cannot happen before nor after the call-on-pimpl-stage of type erasure
	*  because StationConcept has virtual methods that cannot be templates and therefore cannot 
	*  be defined for all specific requirements at once (must use Requirement base) and cannot
	*  happen after because dispatch functions themselves must be virtual and therefore cannot
	*  be defined for all specific stations at once. Not having these very simple, declared automatically
	*  with correctness-of-definitions and safety guaranteed by concepts, would involve type erasing
	*  Requirements, which is a lot more error-prone. For this reason double dispatch must involve calling 
	*  the concrete method specification on obj deirectly, but Station concept doesn't have obj, and if it did,
	*  it would've been a template, meaning the function calling it, which is a virtual function would've had to
	*  be a template too. This is why double dispatch here is not an option, dispatch must happen at run time
	*  with a wrapper directly checking obj's type thorugh a virtual, and force reinterpret cast into whatever
	*  requirement type tag it recieves. */

	StationT obj;
};



/*! Type erased station CRTP.
 *
 * Copy is done through clone() model's member function. Move assignment through swap. Move ctor copies.
 *
 * @see Type erasure
 */
class Station {
private:
	std::unique_ptr<StationConcept> pimpl;

	template<typename StationT>
	StationT clone(); //!< @see Prototype

	friend std::string_view name(const is_Station auto& s); // @see External polymorphism
	friend std::vector<std::string_view> lines(const is_Station auto& s); //@see External polymorphism
	friend std::vector<std::unique_ptr<sub::Requirement>> req(const is_Station auto& s);
public:
	template<is_Req R>
	friend bool tryFix(is_Station auto& s, const Requirement&); //!< @see External polymorphism
	template<is_Req R>
	friend bool verify(const is_Station auto& s, const Requirement&); //!< @see External polymorphism

	template<typename StationT>
	Station(StationT); //!< @see Bridge

	Station(const Station&); //!< @see Prototype
	Station& operator=(const Station&); //!< @see Prototype

	Station(Station&&) = default; //!< NOTE: moved from objects are left invalid
	Station& operator=(Station&&) = default; //!< NOTE: moved from objects are left invalid

	std::string_view name() const; //!< Unfortunately, by definition of what type erasure is, this has to also be implemented like all other methods.
	std::vector<std::string_view> lines() const; //!< This could be just virtual, but tryFix and verify could not.
	std::vector<std::unique_ptr<sub::Requirement>> req() const;

	/*! Public interfaces to externally polymorphic type erased classes
	 *
	 * Note that all of business logic free function declatations would be generated automatically, 
	 * thus causing a compiler error if left undefined
	 *
	 * @see External polymorhism
	 * */
	bool tryFix(const Requirement&);

	/*! Public interfaces to externally polymorphic type erased classes
	 *
	 * Note that all of business logic free function declatations would be generated automatically, 
	 * thus causing a compiler error if left undefined
	 *
	 * @see External polymorhism
	 * */
	bool verify(const Requirement&) const;
};

/*! @see The hardest question in programming*/
template<is_Station S, is_Req R>
struct FreeTryFixFunctor {
	static const FreeTryFixFunctor& it;

	bool operator()(S& s, const Requirement& r) const {return FreeTryFixFunctor/*<S, R>*/::tryFix(s, r);}
	static bool tryFix(S& s, const Requirement& r) {return it(s, r);}
};
//template<is_Station S, is_Req R>
//bool FreeTryFixFunctor<S, R>::tryFix(S& s, const Requirement& r) {return FreeTryFixFunctor::it(s, r);}

template<is_Station S, is_Req R>
struct FreeVerifyFunctor {
	static const FreeVerifyFunctor& it;

	bool operator()(const S& s, const Requirement& r) const {return FreeVerifyFunctor/*<S, R>*/::verify(s, r);}
	static bool verify(const S& s, const Requirement& r) {return it(s, r);}

	FreeVerifyFunctor() = delete;
};

}

// ---------------------------- Template defintions --------------------------------------------------

namespace sub {
extern sub::FreeFunctionRegistry registry_tryFix;
extern sub::FreeFunctionRegistry registry_verify;
}

// Type erasure boilerplate: prototype initialization
template<typename StationT> requires(sub::is_Station<StationT>)
sub::StationModel<StationT>::StationModel(StationT s) :obj{std::move(s)}
{}

template <typename StationT>
sub::Station::Station(StationT s) :pimpl(std::make_unique(std::move(s)))
{}

// Type erasure boilerplate: prototype copy boilerplate
template <typename StationT> requires(sub::is_Station<StationT>)
std::unique_ptr<sub::StationConcept> sub::StationModel<StationT>::clone() const {
	return std::make_unique<StationModel<StationT>>(*this); //TODO std::exchange maybe?????
}

// Type erasure boilerplate: external polymorphism boilerplate (StationModel:: dispatching difinitions)
template<typename StationT> requires(sub::is_Station<StationT>)
bool sub::StationModel<StationT>::tryFix(const sub::Requirement& r) {
	const std::pair key = {typeid(Station).hash_code(), r.type()};
	const auto func = reinterpret_cast<bool(*)(StationT&, const Requirement&)>(sub::registry_tryFix.read(key));
	return func(*obj, r); // Func is guaranteed to exist because its existance is bound to the existance of the tryFix callable for this specific req station pair.
};
template<typename StationT> requires(sub::is_Station<StationT>)
bool sub::StationModel<StationT>::verify(const sub::Requirement& r) const {
	const std::pair key = {typeid(Station).hash_code(), r.type()};
	const auto func = reinterpret_cast<bool(*)(const StationT&, const Requirement&)>(sub::registry_verify.read(key));
	return func(*obj, r); // Func is guaranteed to exist because its existance is bound to the existance of the verify callable for this specific req station pair.
}
template<typename StationT> requires(sub::is_Station<StationT>)
std::string_view sub::StationModel<StationT>::name() const {
	return name(obj);
}
template<typename StationT> requires(sub::is_Station<StationT>)
std::vector<std::string_view> sub::StationModel<StationT>::lines() const {
	return lines(obj);
}
template<typename StationT> requires(sub::is_Station<StationT>)
std::vector<std::unique_ptr<sub::Requirement>> sub::StationModel<StationT>::req() const {
	return req(obj);
}

// Type erasure boilertplate: external polymorphism free functions
template<sub::is_Req R>
bool sub::tryFix(is_Station auto& s, const Requirement& r) { // TODO: VIRTUAL double dipatch of is_Station auto& s calls s.tryFix(*this); ()
	return s.tryFix(dynamic_cast<R&>(r)); // NOTE: Double dispatch requires at least one concrete type, which is only possible on the external polymophism stage out of 4 stages of type erasure (Station -> pimpl -> ext polymorph of obj -> obj's specific behaviour)
}
template<sub::is_Req R>
bool sub::verify(const is_Station auto& s, const Requirement& r) {
	return s.tryFix(dynamic_cast<R&>(r)); // NOTE: Double dispatch requires at least one concrete type, which is only possible on the external polymophism stage out of 4 stages of type erasure (Station -> pimpl -> ext polymorph of obj -> obj's specific behaviour)
}
