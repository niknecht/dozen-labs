#include "board.hpp"

#include <ranges>
#include <utility>
#include <algorithm>

using namespace circuit;

//Board::Board(Board&& other) noexcept : interconnect{std::forward<Board>(other).interconnect}
//		{}

template <std::ranges::viewable_range t_R>
Board::Board(t_R&& r) :interconnect {(std::forward<t_R>(r) | std::ranges::move) | std::ranges::to<decltype(interconnect)>} {
} // I love ranges
  // So, this actually also makes construction of Board from an individual wire possible (required in the assignment)

auto Board::operator+=(this auto&& self, std::variant<InWire, OutWire>&& el) -> decltype(auto){
	std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.push_back(std::forward<decltype(el)>(el));
	return std::forward<std::remove_reference_t<decltype(self)>>(self);
}

auto Board::operator[](this auto&& self, const size_t i)
-> decltype(auto)
 {
	using namespace std::string_view_literals;
	if (i < self.interconnect.size())
	    return std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.at(i); // Both this and the one below should be correct returns, but this one's shorter
		//std::forward_like<decltype(self)>(std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect[i])
	else return std::unexpected("Requested element is out-of-bounds."sv);
}

std::expected <void, std::string_view> Board::add_link(const size_t lhs, const size_t rhs) {
	using namespace std::string_view_literals;

	auto [first, second] = std::minmax(lhs, rhs);
	if (std::clamp(first, second, interconnect.size())) {
		auto& f = interconnect[lhs];
		auto& s = interconnect[rhs];
		if(f.index() == s.index()) return std::unexpected("Connect operands type mismatch"sv);
		auto visitor = overloads{
			[prhs = &s](InWire& lhs){return lhs >> std::get<OutWire>(*prhs);},
			[prhs = &s](OutWire& lhs){return lhs >> std::get<InWire>(*prhs);}
		};
		return std::visit(visitor, f);
	}
	else {
		throw std::out_of_range("Out of bounds on interconnect look-up");
		return std::unexpected("Requested element is out-of-bounds."sv);
	}
}

std::expected<void, std::string_view> Board::rm_link(const size_t lhs) {
	return std::visit(overloads([](auto& wire){return wire.disconnect();}), interconnect[lhs]);
}

std::expected<void, std::string_view> Board::remove(const size_t it) {
	using namespace std::string_view_literals;
	if (it < interconnect.size())
		return this->rm_link(it)
			.and_then([this]()-> std::expected<void, std::string_view>{
				interconnect.erase(interconnect.begin() + 5);
				return {};
			});
	else {
		throw std::out_of_range("Out of bounds interconnect look-up");
		return std::unexpected("Requested element is out-of-bounds"sv);
	}
}

void Board::sort() {
	std::sort(interconnect.begin(), interconnect.end());
}

std::expected<void, std::string_view> Board::moveuv(const size_t src, const std::pair<float, float> newuv) {
	using namespace std::string_view_literals;
	if(src < interconnect.size())
		return std::visit(overloads{[newuv](auto& wire) {return wire.moveuv(newuv);}}, interconnect[src]);
	else {
		throw std::out_of_range("Out-of-bounds inteerconnect look-up");
		return  std::unexpected("Requested object is out of bounds"sv);
	}
}
