#pragma once

#define THRS // WARNING: Remove this once done eveloping multithread

namespace sub{
template<typename>
class StationCRTP;

class Requirement;
struct FutureRequirementConcept;
}

#include <concepts>
#include <vector>
#include <string_view>
#include <memory>
#include <future>


/*! Concept relying on is_Req concept demanding a lot of interaction, that defines what a station is.
 *
 * Stations and requirements are closely coupled, restraints on ones must onlo restrict others,
 * most restraints are done on the requirement classes, as they are the less important part of design: 
 * restricting stations further would in theory enforce bigger changes to all other classes every time a 
 * requirement is added, than restricting requirements would, and therefore requirements are supposed to submit to whatever
 * changes have been done to station classes, but not vice versa, and the station classes are allowed to change more or less freely.
 * Of cource, in this case this descision is not as important as adding requirements should not change interface of a station anyway.
 *
 * @see is_Req concept */
namespace sub {

#ifdef THRS
using station_req_fn_ret_t = std::vector<std::unique_ptr<sub::FutureRequirementConcept>>;
#else
using station_req_fn_ret_t = std::vector<std::unique_ptr<Requirement>>;
#endif

template<typename sQM>
concept is_Station = std::derived_from
<sQM, StationCRTP<sQM>> && requires(const sQM& s) {
	//{sQM(std::string{})} -> std::convertible_to
	//<StationCRTP<sQM>>;
	{s.name()} -> std::convertible_to<std::string_view>;
	{s.lines()} -> std::convertible_to<std::vector<std::string_view>>;
	{s.req()} -> std::convertible_to<station_req_fn_ret_t>;
};
}
