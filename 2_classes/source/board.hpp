#include "wire.hpp"

#include <span>
#include <variant>
#include <vector>
#include <expected>
#include <string_view>

class Board {
private:
	std::vector<std::variant<InWire, OutWire>> interconnect;
public:
	Board(const Board& other) = default;
	Board(Board&& other);
	Board& operator= (const Board&) = default;
	Board&& operator= (Board&&);

	~Board() = default;

	template <std::ranges::viewable_range  t_R>
	Board(t_R&& r);
	//Board(std::variant<InWire, OutWire>&&); // std::forward into the agreggate constructor

	auto operator+=(this auto&& self, std::variant<InWire, OutWire>&&) -> decltype(auto); // rationale: intent to return ref to this, category should be preserved

	auto operator[](this auto&& self, const size_t) noexcept -> decltype(auto); // accesing temporary's member should enable move

	std::expected<void, std::string_view> add_link(const size_t , const size_t) noexcept; // Reference collapsing +

	std::expected<void, std::string_view> rm_link(const size_t) noexcept;	// + deducing this

	std::expected<void, std::string_view> remove(const size_t) noexcept;

	void sort(); // Make this conditional noexcept based on when std::sort throws according to cppref pages in lms.dozen

	std::expected<void, std::string_view> moveuv(const size_t src, const std::pair<float, float>) noexcept;

	template <class ...Ts>
	struct overloads : Ts... {using Ts::operator()...;}; // See std::variant<...Type>::visit example on cppreference.com
};
