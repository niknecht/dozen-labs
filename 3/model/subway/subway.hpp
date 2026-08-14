#pragma once

#include "../station/station.hpp"
#include"../common/traits/requirementTraits.hpp"

#include <memory>
#include <string>
#include <type_traits>
//#include <atomic>

namespace sub {
/*! @brief This is a Repository pattern class that stores Stations, searches and verifies them.
* 
* The verification process is broke down into two parts: (i) iterating over the list of stations, gathering requirements, and (ii) iterating over the list of requirements, checking whether each one of them (except for the ones that place requirements onto stations that are already in the delete list) is satisfied and adding stations.
* It is guaranteed that no two threads would be checking requirements for the same station, because stations must only get deleted if there is no possibility of fixing.
*/
class Subway {
private:
	std::unordered_map<std::string, std::unique_ptr<Station>> name_map;
	std::unordered_map<Station*, std::pair<std::string, std::string>> line_name_cache;
	std::unordered_map<Station*, std::pair<std::string, std::string>> transfer_name_cache;

	std::vector<std::string_view> listDelete;// TODO Check of the views are valid
	std::vector<std::string_view> listTryFix;
	
	void do_delete() noexcept;

	std::vector<std::unique_ptr<Requirement>> verify() const noexcept;

	bool test(const is_Req auto&) const;

	void tryFix() noexcept;
public:
	/*! Flexible station addition without performance penalties.*/
	template<typename RawT_Station>
	void addStation(std::string_view name, auto&&... args) noexcept
		requires(std::is_constructible_v<RawT_Station, decltype(name), std::remove_reference_t<decltype(args)>...>) {
		try {
		if(name_map.find(std::string(name)) == name_map.end())
			name_map[name, std::make_unique<RawT_Station>(name, args...)];
		}
		catch(...) { //TODO
		}
	} // Perfect forward a Station (unique_ptr?) here
};


}
