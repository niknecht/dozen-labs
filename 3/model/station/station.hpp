#pragma once

#include <memory>

namespace sub {
class Station;
}

#include "../common/requirement/requirement.hpp"

namespace sub {
class Station;

namespace sub1{
template<is_Req Req>
class ReqStationStrategy {
public:
	bool verify(const Station&, const Req&) const noexcept;
	bool tryFix(Station&);
};

/*! This is a type erased station class. 
 *
 * Type erasure is nessesary because instances of the would be stored as the base class. Apart from that, each station class
 * needs to have strategies of working with different Requirements according to the strategy design pattern, virtual templated 
 * functions are not allowed in C++.
 *
 * @see Type erasure design pattern
 * @see Bridge design pattern
 * @see Prototype design pattern
 * @see Strategy design pattern
 * */
class Station {
private:
	class StationConcept {};

	template<class StationT>
	class StationModel : public StationConcept{
		friend bool verify(const StationT&, const is_Req auto&); //!< Defining specializations these is resposibility of requirements.
		friend bool tryFix(StationT&, const is_Req auto&); //!< Defining specializations of these is resposibility of requirements.
	
		bool do_verify(const is_Req auto&);
		bool tryFix(const is_Req auto&);

	};

	bool do_verify(const is_Req auto&);
	bool do_tryFix(const is_Req auto&);

	std::unique_ptr<StationConcept> pimpl;
};

template<>
bool Station::do_tryFix(const Requirement&) {
	return false;
}

}
// -----------------------------------------------------------------------------------------------------------------------------------------------
// _______________________________________________________________________________________________________________________________________________
template <typename Station>
class StationCRTP {
	std::unique_ptr<Station> it;
public:
	bool tryFix(const is_Req auto&);
	bool verify(const is_Req auto&) const;
};

class DirectStation : public StationCRTP<DirectStation> {

};

/*! Type erased station 
 *
 * @example Works!! This below is responsibility of the Requirements*/
template<>
template<>
bool StationCRTP<DirectStation>::tryFix<Requirement>(const Requirement&) {
	return false;
}
template<>
template<>
bool StationCRTP<DirectStation>::verify<Requirement>(const Requirement&) const {
	return false;
}
// ^ New requirement types are supposed to do this ^

class Station {
	class StationConcept {
		virtual bool tryFix(const sub::Requirement&);	
		virtual bool verify(const sub::Requirement&) const;
	};

	// TODO: Require all station to have tryFix and verify through concepts
	// TODO CRTP for ~(~i~n place o~f~)~ double dispatch
	// TODO Type erasure (with only constructors) for storing the objects


	// StationT is any CRTP. This is the main buf of type erasure is that erased typed don't have to have the same base class
	template<class StationT>
	class StationModel : StationConcept {
		bool tryFix(const sub::Requirement& it) override {
			tryFix(obj, it);
		};
		bool verify(const sub::Requirement& it) const override {
			verify(obj, it);
		}

		StationT obj;
	};

	std::unique_ptr<StationConcept> pimpl;
};

//!< _ @example _
bool tryFix(DirectStation& s, is_Req auto); //!< @see External polymorphism
bool verify(const DirectStation& s, is_Req auto); //!< @see External polymorphism
// ^ New station types are supposed to do this ^
// TODO Declare these for all stations at once with concepts

//!< @example
bool tryFix(DirectStation& s, Requirement r) {
	s.tryFix(r);
}
bool verify(const DirectStation& s, Requirement r) {
	s.verify(r);
}
// ^ New requirement types are supposed to do this ^
// Actually, this could probably be done either by every new station with templates,
// or for all stations at once with concepts + templates
// TODO: But the problem is, if requirements are stored polymorphically as base class, calling 
// Requirement req {concreteReqRvalue}; Station s{...}; s.verify(req); Will call verify version for the Requirement base class
// Need either another type erasure on Requirement too, or, more interestingly, double dispatch.
// TODO Double dispatch the sucker
// Or... Do I need double dispatch thx to type erasure? Single dispatch is prolly enough

}

