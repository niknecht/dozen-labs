#pragma once

#include <stdint.h>
#include <string>

namespace sub{
class Requirement;
}

namespace sub {

/*! @brief Class that manages requirements posed by one station onto another. These can later be checked by the Subway class.
 * 
 * The guarantee is given that cheking requirements of lower priority does not break already checked requirements of higher priorities.
 * Copies are allowed, it is reqsonsibility of the subway class to check that a pointer to the same source doesn't get written into the delete list twice.
 * Source is refered to by string as a more robust and safer identity that already has all the infrustructure for working with it.
 * Requirement sorting has to account for both priority and the source name. This is because, when processed in parallel, each thread has to get all the requirements on the same priority for one station.
 * Co-dependent requirements are prohiboted because they would violate the priority guarantee and therefore require special requirement classes. But this should not occur in a typical metro anyway.
 */
enum class RequirementPriority : uint32_t {
	ReqLinesExist,
	ReqStationExistsNotOnLine
};

class Requirement {
protected:
	std::string source; //!< We must be able to perform a search by name even on a deleted station (for example, my a direct rm command)
protected:
	Requirement(const std::string_view src);
public:
	bool operator<(const Requirement& other) const noexcept;
	bool operator==(const Requirement& other) const noexcept;
	bool operator<=>(const Requirement& other) const noexcept; 

	Requirement() = delete;
	Requirement(const Requirement&) = default;
	Requirement(Requirement&&) = default;
	Requirement& operator=(const Requirement&) = default;
	Requirement& operator=(Requirement&&) = default;
	~Requirement() = default;

	virtual constexpr RequirementPriority kind() const noexcept = 0; // !< This is nessesary so that comparisons work, and so that the source can tryFix the unsatisfied requirement, which means it has to know the requirement kind.
};
}
