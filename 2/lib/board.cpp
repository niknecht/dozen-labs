#include "board.hpp"

#include <ranges>
#include <utility>
#include <algorithm>

/*!
 * @file
 * @authors Nikita Vitkovskiy
 * @copyright (c) 2026 Nikita Vitkovkiy
 * @license CC0-1.0
 */

using namespace circuit;

//Board::Board(Board&& other) noexcept : interconnect{std::forward<Board>(other).interconnect}
//		{}
/*
template <std::ranges::viewable_range t_R>
Board::Board(t_R&& r) 
requires(std::is_same_v<std::remove_reference_t<decltype(std::declval<t_R>()[0])>, std::variant<InWire, OutWire>>)
:interconnect {(std::forward<t_R>(r) | std::ranges::move) | std::ranges::to<decltype(interconnect)>}{
} // I love ranges
  // So, this actually also makes construction of Board from an individual wire possible (required in the assignment)
*/
/*auto Board::operator+=(this auto&& self, std::variant<InWire, OutWire>&& el) -> decltype(std::forward<std::remove_reference_t<decltype(self)>>(self)){
	std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.push_back(std::forward<decltype(el)>(el));
	return std::forward<std::remove_reference_t<decltype(self)>>(self);
}*/

/*auto Board::operator+=(std::variant<InWire, OutWire>&& el) & -> Board&{
	(*this).interconnect.push_back(std::forward<decltype(el)>(el));
	return *this;
}*/

/*template <typename Self>
auto Board::operator[](this Self&& self, size_t i)
-> decltype(std::forward<Board>(self).interconnect.at(i))
 {
	return std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect.at(i); // Both this and the one below should be correct returns, but this one's shorter
		//std::forward_like<decltype(self)>(std::forward<std::remove_reference_t<decltype(self)>>(self).interconnect[i])
	//else return std::unexpected("Requested element is out-of-bounds."sv);
}*/

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
		//throw std::out_of_range("Out of bounds on interconnect look-up");
		return std::unexpected("Requested element is out-of-bounds."sv);
	}
}

std::expected<void, std::string_view> Board::rm_link(const size_t lhs) {
	try {

		return std::visit(overloads([](auto& wire){return wire.disconnect();}), interconnect.at(lhs));

	} catch (const std::exception& e) {
		return std::unexpected(e.what());
	} catch (const std::string_view e) {
		return std::unexpected(e);
	} catch (...) {
		return std::unexpected("Unknown error thrown from rm_link");
	}
}

std::expected<void, std::string_view> Board::remove(const size_t it) {
	using namespace std::string_view_literals;
	if (it < interconnect.size())
		try {
		if(std::visit(overloads([](const auto& w){return w.is_tethered();}), (*this)[it]))
			return this->rm_link(it)
				.and_then([this, it]()-> std::expected<void, std::string_view>{
					interconnect.erase(interconnect.begin() + it);
					return {};
				});
		else {
			interconnect.erase(interconnect.begin() + it);
			return {};
		}
		} catch (const std::exception& e) {
			return std::unexpected{e.what()};
		} catch (std::string_view e) {
			return std::unexpected{e};
		} catch (...) {
			return std::unexpected("Unknown error thrown from remove");
		}
	else {
		//throw std::out_of_range("Out of bounds interconnect look-up");
		return std::unexpected("Requested element is out-of-bounds"sv);
	}
}

void Board::sort() {
	std::sort(interconnect.begin(), interconnect.end());
}

std::expected<void, std::string_view> Board::moveuv(const size_t src, const std::pair<float, float> newuv) {
	using namespace std::string_view_literals;
	if(src < interconnect.size()){
		std::visit(overloads{[newuv](auto& wire) {return wire.moveuv(newuv);}}, interconnect[src]);
		return {};
	}
	else {
		throw std::out_of_range("Out-of-bounds interconnect look-up");
		return  std::unexpected("Requested object is out of bounds"sv);
	}
}

decltype(std::declval<const decltype(Board::interconnect)>())& Board::vec() const noexcept {
//const auto& Board::data() const noexcept{
	return interconnect;
}

const decltype(std::declval<const decltype(Board::interconnect)>().begin()) Board::begin() const noexcept {
	return vec().begin();
}

const decltype(std::declval<const decltype(Board::interconnect)>().end()) Board::end() const noexcept {
	return vec().end();
}
