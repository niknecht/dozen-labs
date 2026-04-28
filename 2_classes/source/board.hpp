#include "wire.hpp"

#include <variant>
#include <vector>
#include <expected>
#include <string_view>

class Board {
private:
	std::vector<std::variant<InWire, OutWire>> interconnect;
public:
	Board(const Board& other) noexcept = default; // TODO Conditional noexcept everywhere here
	Board(Board&& other) noexcept = default;
	Board& operator= (const Board&) noexcept = default;
	Board& operator= (Board&&) noexcept = default;

	~Board() = default;

	template <std::ranges::viewable_range  t_R>
	Board(t_R&& r);

	//  Any funcitons that access elements of interconnect are not noexcept befause the user is expected to handle such cases
	
	auto operator+=(this auto&& self, std::variant<InWire, OutWire>&&) -> decltype(auto); // rationale: intent to return ref to this, category should be preserved

	auto operator[](this auto&& self, const size_t) -> decltype(auto); // accesing temporary's member should enable move

	std::expected<void, std::string_view> add_link(const size_t , const size_t); // ^ Reference collapsing +

	std::expected<void, std::string_view> rm_link(const size_t);	// + deducing this ^

	std::expected<void, std::string_view> remove(const size_t);

	void sort(); // Make this conditional noexcept based on when std::sort throws according to cppref pages in lms.dozen

	std::expected<void, std::string_view> moveuv(const size_t src, const std::pair<float, float>);

	template <class ...Ts>
	struct overloads : Ts... {using Ts::operator()...;}; // See std::variant<...Type>::visit example on cppreference.com
};
