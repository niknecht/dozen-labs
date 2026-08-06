#include "../requirement.hpp"

#include <string_view>
#include <vector>

namespace sub {

class ReqLinesExist : public sub::Requirement {
private:
	//static const RequirementTag<sub::ReqLinesExist> tag_;
	//static RequirementPriority priority_;

	RequirementPriority priority() const noexcept override;

	std::vector<std::string> requiredLines;
public:
	size_t type() const noexcept override;
	std::vector<std::string_view> required_lines() const noexcept;
	
	explicit ReqLinesExist(const std::string_view, const std::vector<std::string_view>&);
};
static_assert(sub::is_Req<ReqLinesExist>);

// TODO: Declare all interaction functions here

}
