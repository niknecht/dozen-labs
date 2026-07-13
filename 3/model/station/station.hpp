#pragma once

#include <memory>

namespace sub {
class Station;
}

#include "../common/requirement/requirement.hpp"

namespace sub {
class Station;

class StationConcept;

template<typename>
class StationCRTP;

/*! Stations and requirements are closely coupled, restraints on ones must onlo restrict others,
 * most restraints are done on the requirement classes, as they are the less important part of design: 
 * restricting stations further would in theory enforce bigger changes to all other classes every time a 
 * requirement is added, than restricting requirements would, and therefore requirements are supposed to submit to whatever
 * changes have been done to station classes, but not vice versa, and the station classes are allowed to change more or less freely.
 * Of cource, in this case this descision is not as important as adding requirements should not change interface of a station anyway.
 *
 * @see is_Req concept
 */
template<typename sQM>
concept is_Station = std::derived_from<sQM, StationCRTP<sQM>> && requires(const sQM& s) {
	//{sQM(std::string{})} -> std::convertible_to
	//<StationCRTP<sQM>>;
	{s.name()} -> std::convertible_to<std::string_view>;
	{s.lines()} -> std::convertible_to<std::vector<std::string_view>>;
};

/* Station CRTP base.
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
	std::string_view name() const; //!< Non-polymorphic behaviour, doesn't dispatch to the derived
	std::vector<std::string_view> lines() const; //!< Polymorphic lines() getter. Defined automatically for every base. Defining dispatch-to method is the reqponsibility of the derivees.
	bool tryFix(const is_Req auto&); //!< Polymorphic tryFix method. Defined automatically for every base. Defining dispatch-to method is the reqponsibility of the requirements.
	bool verify(const is_Req auto&) const; //!< Polymorphic verify method. Defined automatically for every base. Defining dispatch-to method is the reqponsibility of the requirements.

	StationCRTP<Station>(const std::string_view n); //!< Non-polymorphic behaviour, required in order to have non-polymorphic name() getter

	static_assert(is_Station<Station>);
};

/*class DirectStation :public StationCRTP<DirectStation> {
	std::string m_line;
};*/

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
template<>
bool StationCRTP<DirectStation>::verify<ReqLinesExist>(const ReqLinesExist&) const {
	return false;
}*/
// ^ New requirement types are supposed to do this ^
//static_assert(is_Station<StationCRTP<DirectStation>>);

//! These have to be in the global namespace in order for concepts to use them, and for Station declarations to use the concepts.
class StationConcept {
public:
	virtual ~StationConcept() = default;

	virtual std::string_view name() const = 0; //!< Getters are essential in the type erasure approach, else type would not be really erased
	virtual std::vector<std::string_view> lines() const = 0; //!< Getters are essential in the type erasure approach, else type would not be really erased
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
	bool tryFix(const sub::Requirement& it) override; //!< @see double dispatch
	bool verify(const sub::Requirement& it) const override; //!< @see double dispatch
	std::unique_ptr<StationConcept> clone() const override; //!< @see Prototype

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
	friend bool tryFix(is_Station auto& s, const is_Req auto&); //!< @see External polymorphism
	friend bool verify(const is_Station auto& s, const is_Req auto&); //!< @see External polymorphism
public:
	template<typename StationT>
	Station(StationT); //!< @see Bridge

	Station(const Station&); //!< @see Prototype
	Station& operator=(const Station&); //!< @see Prototype

	Station(Station&&) = default; //!< NOTE: moved from objects are left invalid
	Station& operator=(Station&&) = default; //!< NOTE: moved from objects are left invalid

	std::string_view name() const; //!< Unfortunately, by definition of what type erasure is, this has to also be implemented like all other methods.
	std::vector<std::string_view> lines() const; //!< This could be just virtual, but tryFix and verify could not.

	/*! Public interfaces to externally polymorphic type erased classes
	 *
	 * Note that all of business logic free function declatations would be generated automatically, 
	 * thus causing a compiler error if left undefined
	 *
	 * @see External polymorhism
	 * */
	bool tryFix(const is_Req auto&);

	/*! Public interfaces to externally polymorphic type erased classes
	 *
	 * Note that all of business logic free function declatations would be generated automatically, 
	 * thus causing a compiler error if left undefined
	 *
	 * @see External polymorhism
	 * */
	bool verify(const is_Req auto&) const;
};


}
