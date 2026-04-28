#include "board.hpp"

#include <ranges>
#include <utility>
#include <algorithm>
#include <iterator>

//Board::Board(Board&& other) : interconnect((std::span(other.interconnect) | std::ranges::move) | std::ranges::to<decltype(interconnect)>)
//		{}

template <std::ranges::viewable_range t_R>
Board::Board(t_R&& r) :interconnect {(std::forward<t_R>(r) | std::ranges::move) | std::ranges::to<decltype(interconnect)>} {
} // I love ranges
  // So, this actually also makes construction of Board from an individual wire possible (required in the assignment)

auto Board::operator+=(this auto&& self, std::variant<InWire, OutWire>&& el) -> decltype(auto){
	std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.push_back(std::forward<decltype(el)>(el));
	return std::forward<std::remove_reference_t<decltype(self)>>(self);
}

auto Board::operator[](this auto&& self, const size_t i) noexcept 
-> decltype(auto)
 {
	using namespace std::string_view_literals;
	if (i < self.interconnect.size())
	    return std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect[i]; // Both this and the one below should be correct returns, but this one's shorter
		//std::forward_like<decltype(self)>(std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect[i])
	else return std::unexpected("Requested element is out-of-bounds."sv); // TODO Throw out_of_bounds here
}

std::expected <void, std::string_view> Board::add_link(const size_t lhs, const size_t rhs) noexcept { // TODO Add C++23 conditionals to noexcept
	using namespace std::string_view_literals;

	auto [first, second] = std::minmax(lhs, rhs);
	if (std::clamp(first, second, interconnect.size())) {
		auto& f = interconnect[lhs];
		auto& s = interconnect[rhs];
		if(f.index() == s.index()) return std::unexpected("Connect operands type mismatch"sv);
		auto visitor = overloads{
			[prhs = &s](InWire& lhs){return lhs >> std::get<OutWire>(*prhs);},
			[prhs = &s](OutWire& lhs){return lhs >> std::get<InWire>(*prhs);} // TODO Make connect() return an expected Board& and an unexpected if already connected
		};
		return std::visit(visitor, f);
	}
	else return std::unexpected("Requested element is out-of-bounds."sv); // TODO Throw out_of_bounds here
}

std::expected<void, std::string_view> Board::rm_link(const size_t lhs) noexcept {
	return std::visit(overloads([](auto& wire){return wire.disconnect();}), interconnect[lhs]);
}

std::expected<void, std::string_view> Board::remove(const size_t it) noexcept {
	using namespace std::string_view_literals;
	if (it < interconnect.size())
		return this->rm_link(it)
			.and_then([this]()-> std::expected<void, std::string_view>{
				interconnect.erase(interconnect.begin() + 5);
				return {};
			});
	else return std::unexpected(""sv);
}

void Board::sort() {
	std::sort(interconnect.begin(), interconnect.end());
}

std::expected<void, std::string_view> Board::moveuv(const size_t src, const std::pair<float, float>) noexcept {
	using namespace std::string_view_literals;
	if(src >= interconnect.size())
		return  std::unexpected("Requested object is out of bounds"sv); // TODO Throw here
}

// TODO: Rule of 5 for the Board class
