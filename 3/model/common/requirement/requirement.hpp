#pragma once

namespace sub{
//template<typename>
class Requirement;
}

#include <stdint.h>
#include <string>
#include <memory>

namespace sub {
class Station;
}
#include <vector>

namespace sub {

template<typename R>
concept is_Req = (requires(const R& t, const Requirement& b, Station& s, const Station& sc) {
	{t <=> b} -> std::convertible_to<bool>;
	{t <=> t} -> std::convertible_to<bool>;
	{b <=> t} -> std::convertible_to<bool>;
	{t.dispatchTryFix(s)} -> std::convertible_to<bool>;
	{t.dispatchVerify(sc)} -> std::convertible_to<bool>;
} && std::is_base_of_v<Requirement, R>);

enum class RequirementPriority : uint32_t {
	ReqLinesExist,
	ReqStationExistsNotOnLine
};

//#include "../../station/station.hpp"


/* @brief Class that manages requirements posed by one station onto another. These can later be checked by the Subway class.
 * 
 * The guarantee is given that cheking requirements of lower priority does not break already checked requirements of higher priorities.
 * Copies are allowed, it is reqsonsibility of the subway class to check that a pointer to the same source doesn't get written into the delete list twice.
 * Source is refered to by string as a more robust and safer identity that already has all the infrustructure for working with it.
 * Requirement sorting has to account for both priority and the source name. This is because, when processed in parallel, each thread has to get all the requirements on the same priority for one station.
 * Co-dependent requirements are prohiboted because they would violate the priority guarantee and therefore require special requirement classes. But this should not occur in a typical metro anyway.
 *
 * @see Double dispatch
 */
class Requirement{
private:
	std::string source;
	RequirementPriority priority;
public:
	bool operator<(const Requirement& other) const noexcept; //!< Priority -> name comparion
	bool operator==(const Requirement& other) const noexcept; //!< Priority -> name comparison
	bool operator<=>(const Requirement& other) const noexcept; //!< Priority -> name comparison

	Requirement() = delete;
	Requirement(const Requirement&) = default;
	Requirement(Requirement&&) = default;
	Requirement& operator=(const Requirement&) = default;
	Requirement& operator=(Requirement&&) = default;
	~Requirement() = default;
protected:
	virtual bool dispatchTryFix(Station&s) const = 0; // {return s.tryFix(*this);} // TODO: override looks the same for all req
	virtual bool dispatchVerify(const Station&s) const = 0; // {return s.tryFix(*this);} // TODO: override this with auto in templates for all req classes

	//static_assert(is_Req<Requirement>);
};

//@example
class ReqLinesExist : public Requirement {
	std::vector<std::string> requiredLines;
public:
	virtual bool dispatchVerify(const Station&) const override; //{return false; }//s.verify(*this);}
	virtual bool dispatchTryFix(Station&) const override;// {return false; } //s.tryFix(*this);}
};
static_assert(is_Req<ReqLinesExist>);


/* @brief Class that manages requirements posed by one station onto another. These can later be checked by the Subway class.
 * 
 * The guarantee is given that cheking requirements of lower priority does not break already checked requirements of higher priorities.
 * Copies are allowed, it is reqsonsibility of the subway class to check that a pointer to the same source doesn't get written into the delete list twice.
 * Source is refered to by string as a more robust and safer identity that already has all the infrustructure for working with it.
 * Requirement sorting has to account for both priority and the source name. This is because, when processed in parallel, each thread has to get all the requirements on the same priority for one station.
 * Co-dependent requirements are prohiboted because they would violate the priority guarantee and therefore require special requirement classes. But this should not occur in a typical metro anyway.
 *
 * @see Double dispatch pattern for info on repairing stations
 *//*
class TypeErasedRequirement {
private:
	template<typename ReqT>
	class RequirementModel : public Requirement {
	private:
		ReqT obj;
	public:
		virtual bool dispatchVerify(const Station&) const override; //{return false; }//s.verify(*this);}
		virtual bool dispatchTryFix(Station&) const override;// {return false; } //s.tryFix(*this);}
	};
	std::unique_ptr<Requirement> pimpl; //!< @see Type erasure design pattern
public:
	*//*std::string source; //!< We must be able to perform a search by name even on a deleted station (for example, by a direct rm command)
protected:
	Requirement(const std::string_view src);
	virtual constexpr RequirementPriority kind() const noexcept = 0; // !< This is nessesary so that comparisons work, and so that the source can tryFix the unsatisfied requirement, which means it has to know the requirement kind. Polymorphism here allown easy comparisons 
public:
	bool operator<(const Requirement& other) const noexcept; //!< Priority -> name comparion
	bool operator==(const Requirement& other) const noexcept; //!< Priority -> name comparison
	bool operator<=>(const Requirement& other) const noexcept; //!< Priority -> name comparison

	Requirement() = delete;
	Requirement(const Requirement&) = default;
	Requirement(Requirement&&) = default;
	Requirement& operator=(const Requirement&) = default;
	Requirement& operator=(Requirement&&) = default;
	~Requirement() = default;
	*/
/*};*/

}
