#include "wire.hpp"

#pragma once

#include <variant>
#include <vector>
#include <expected>
#include <string_view>

/*!
 * @file
 * @authors Nikira Vitkovskiy
 * @copyright (c) 2026 Nikita Vitkovkiy
 * @license CC0-1.0: 2026
 * */

namespace circuit{
	class Board;
}

/*!
 *
 * This is an owning vector wrapper for the vector of variants of InWire, OutWire speicialization. The expectation is that
 * errors are handled locally, at the point where they are critical, except for when passing arguments to a constructor, in
 * that case all values are expected to be valid and sane. It should be expected that NO error checking or sanity checks are
 * performed by constructors.
 *
 * Classes InWire, and OutWire are not polimorphic to most methods of Board. Implementing InWire and OutWire in a polymorphic way
 * is non-trivial due to different return types of 'tethered''s getters and types of the 'tethered' members. This observation 
 * dictates the use of visit pattern together with a vector of variants, making the architecture simpler, though, making some 
 * functions harder to read to a reader who is not used to such pattern.*/
class circuit::Board {
private:
	std::vector<std::variant<InWire, OutWire>> interconnect;
public:
	Board() : interconnect{} {interconnect.reserve(3);} // <-- The hardest question in programming (is this

	Board(const Board& other) noexcept = default; // TODO Conditional noexcept everywhere here
	Board(Board&& other) noexcept = default;
	Board& operator= (const Board&) noexcept = default;
	Board& operator= (Board&&) noexcept = default;

	~Board() = default;

	// @param viable_range 
	template <std::ranges::viewable_range  t_R>
	Board(t_R&& r); // Reference collapsing

	//  Any funcitons that access elements of interconnect are not noexcept befause the user is expected to handle such cases
	
	//auto operator+=(std::variant<InWire, OutWire>&&) & -> Board&; // intent to return ref to this, category should be preserved
	auto operator+=(this auto&& self, std::variant<InWire, OutWire>&& el) -> decltype(std::forward<std::remove_reference_t<decltype(self)>>(self)) // intent to return ref to this, category should be preserved
	{
		std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.push_back(std::forward<decltype(el)>(el));
		return std::forward<std::remove_reference_t<decltype(self)>>(self);
	}
	//auto operator[](const size_t i) & -> std::variant<InWire, OutWire>&;
	auto operator[](this auto&& self, size_t i) -> decltype(std::forward<Board>(self).interconnect.at(i)) // accesing temporary's member should enable move
	{
		return std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.at(i);
	}

	std::expected<void, std::string_view> add_link(const size_t , const size_t); // ^ Reference collapsing +

	std::expected<void, std::string_view> rm_link(const size_t);	// + deducing this ^

	std::expected<void, std::string_view> remove(const size_t);

	void sort(); // Make this conditional noexcept based on when std::sort throws according to cppref pages in lms.dozen

	std::expected<void, std::string_view> moveuv(const size_t src, const std::pair<float, float>);

	decltype(std::declval<const decltype(Board::interconnect)>())& vec() const noexcept;
	const decltype(std::declval<const decltype(Board::interconnect)>().begin()) begin() const noexcept;
	const decltype(std::declval<const decltype(Board::interconnect)>().end()) end() const noexcept;

	// @see See std::variant<...Type>::visit example on cppreference.com
	template <class ...Ts>
	struct overloads : Ts... {using Ts::operator()...;}; // See std::variant<...Type>::visit example on cppreference.com
};
