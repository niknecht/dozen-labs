#pragma once

#include "../station/station.hpp"
#include"../common/traits/requirementTraits.hpp"
#include "../line/line.hpp"

#include <memory>
#include <string>
#include <type_traits>
#include<unordered_set>
#include <optional>
#include <atomic>

namespace sub {
/*! @brief This is a Repository pattern class that stores Stations, searches and verifies them.
* 
* The verification process is broke down into two parts: (i) iterating over the list of stations, gathering requirements, and (ii) iterating over the list of requirements, checking whether each one of them (except for the ones that place requirements onto stations that are already in the delete list) is satisfied and adding stations.
* It is guaranteed that no two threads would be checking requirements for the same station, because stations must only get deleted if there is no possibility of fixing.
*/
class Subway {
private:
	std::unordered_map<std::string, std::unique_ptr<Station>> name_map;
	std::unordered_map<std::pair<std::string, std::string>, Station*> line_name_cache;
	std::unordered_map<std::pair<std::string, std::string>, Station*> transfer_name_cache;

	class LineHash {
	public:
		size_t operator()(const Line& l) {return std::hash<std::string_view>()(l.name());}
	};
	std::unordered_set<sub::Line, LineHash> lines;

	std::vector<std::string_view> listDelete;// TODO Check of the views are valid
	std::vector<std::string_view> listTryFix;
	
	//void do_delete() noexcept;

	// do_test(const is_Req auto&) const;

#ifdef THRS
	//!< This returns true if there have been deletions, and false if there haven't. If this returns false, that means the current model is valid.
	bool remedy() noexcept(false);
#else
	bool verify() noexcept;
#endif

	//void tryFix(std::vector<std::unique_ptr<Requirement>>) noexcept;
public:
	void add_line(auto&&... args)
		requires(std::is_constructible_v<Line, decltype(args)...>) {
		lines.insert(Line{std::forward(args)...});
	}

	bool is_allowed_line(const std::string_view) const noexcept;

	/*! Flexible station addition without performance penalties.
	 *
	 * NOTE: Each station's ctor is supposed to take name string as the first argument.*/
	template<typename RawT_Station>
	void addStation(std::string_view name, auto&&... args) noexcept
		requires(std::is_constructible_v<RawT_Station, decltype(name), std::remove_reference_t<decltype(args)>...>) {
		if(name_map.find(std::string(name)) == name_map.end())
			name_map[name, std::make_unique<RawT_Station>(name, std::forward(args)...)];
	} // Perfect forward a Station (unique_ptr?) here
	
	std::optional<std::add_const_t<decltype(name_map.begin())>> findStation_byName(const std::string_view) const;

	std::optional<std::add_const_t<decltype(name_map.begin())>> findStation_byLineNamePair(const std::pair<std::string_view, std::string_view>) const;
};


}
