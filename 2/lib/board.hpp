#include "wire.hpp"

#pragma once

#include <variant>
#include <vector>
#include <expected>
#include <string_view>

namespace circuit{
	class Board;
}

class circuit::Board {
private:
	std::vector<std::variant<InWire, OutWire>> interconnect;
public:
	Board() = default;

	Board(const Board& other) noexcept = default; // TODO Conditional noexcept everywhere here
	Board(Board&& other) noexcept = default;
	Board& operator= (const Board&) noexcept = default;
	Board& operator= (Board&&) noexcept = default;

	~Board() = default;

	template <std::ranges::viewable_range  t_R>
	Board(t_R&& r);

	//  Any funcitons that access elements of interconnect are not noexcept befause the user is expected to handle such cases
	
	auto operator+=(std::variant<InWire, OutWire>&&) & -> Board&; // intent to return ref to this, category should be preserved
	//auto operator+=(this auto&& self, std::variant<InWire, OutWire>&&) -> decltype(std::forward<std::remove_reference<decltype(self)>>(self)); // intent to return ref to this, category should be preserved

	auto operator[](this auto&& self, const size_t) -> decltype(std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.at(std::declval<decltype(0llu)>())) ; // accesing temporary's member should enable move

	std::expected<void, std::string_view> add_link(const size_t , const size_t); // ^ Reference collapsing +

	std::expected<void, std::string_view> rm_link(const size_t);	// + deducing this ^

	std::expected<void, std::string_view> remove(const size_t);

	void sort(); // Make this conditional noexcept based on when std::sort throws according to cppref pages in lms.dozen

	std::expected<void, std::string_view> moveuv(const size_t src, const std::pair<float, float>);

	decltype(std::declval<const decltype(Board::interconnect)>())& vec() const noexcept;
	const decltype(std::declval<const decltype(Board::interconnect)>().begin()) begin() const noexcept;
	const decltype(std::declval<const decltype(Board::interconnect)>().end()) end() const noexcept;

	template <class ...Ts>
	struct overloads : Ts... {using Ts::operator()...;}; // See std::variant<...Type>::visit example on cppreference.com
};
