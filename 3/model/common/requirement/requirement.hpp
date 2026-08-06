#pragma once

#include "../../common/traits/requirementTraits.hpp"

namespace sub{
//template<typename>
class Requirement;
}

#include <stdint.h>
#include <string>

namespace sub {
class Station;
class StationConcept;
template<class>
class StationCRTP;
}
//#include "../../station/station.hpp"

namespace sub {


enum class RequirementPriority : uint32_t {
	ReqLinesExist,
	ReqStationsExistOnLines
};

/*! A class that exposes Requirement concrete type through type's hash at runtime. This is useful for dynamic registry lookup for multidispatch. */
//template<typename R>
//class RequirementTag {
	//struct NumericalTag {
	//	size_t tag;
	//	NumericalTag();// : tag{typeid(R).hash_code()} {}
	//};
//	static size_t tag_;
//public:
//	static size_t tag();// {return tag_.tag;}
//};

//extern bool tryFix(is_Station auto& s, const is_Req auto&); //!< @see External polymorphism
//extern bool verify(const is_Station auto& s, const is_Req auto&); //!< @see External polymorphism



//#include "../../station/station.hpp"


/* @brief Class that manages requirements posed by one station onto another. These can later be checked by the Subway class.
 * 
 * The guarantee is given that cheking requirements of lower priority does not break already checked requirements of higher priorities.
 * Copies are allowed, it is reqsonsibility of the subway class to check that a pointer to the same source doesn't get written into the delete list twice.
 * Source is refered to by string as a more robust and safer identity that already has all the infrustructure for working with it.
 * Requirement sorting has to account for both priority and the source name. This is because, when processed in parallel, each thread has to get all the requirements on the same priority for one station.
 * Co-dependent requirements are prohiboted because they would violate the priority guarantee and therefore require special requirement classes. But this should not occur in a typical metro anyway.
 *
 * @see Multi-dispatch
 */
class Requirement{
private:
	std::string source_;
	virtual RequirementPriority priority() const = 0;
public:
	bool operator<(const Requirement& other) const noexcept; //!< Priority -> name comparion
	bool operator==(const Requirement& other) const noexcept; //!< Priority -> name comparison
	bool operator<=>(const Requirement& other) const noexcept; //!< Priority -> name comparison

	Requirement() = delete;
	explicit Requirement(std::string_view);

	Requirement(const Requirement&) = default;
	Requirement(Requirement&&) = default;
	Requirement& operator=(const Requirement&) = default;
	Requirement& operator=(Requirement&&) = default;
	virtual ~Requirement() = default;

	virtual size_t type() const = 0; //!< Returns a type hash that can be used in dynamic dispatch to select the correct free function
	std::string_view source() const noexcept;
	//static_assert(is_Req<Requirement>);
};

}
