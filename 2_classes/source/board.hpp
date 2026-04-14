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
	Board(std::span<std::variant<InWire, OutWire>>);
	Board(std::variant<InWire, OutWire>&&); // std::forward into the agreggate constructor

	Board& operator+=(std::variant<InWire, OutWire>&&);

	std::expected<std::variant<InWire, OutWire>, std::string_view> operator[](const size_t) noexcept;

	std::expected<void, std::string_view> add_link(const size_t , const size_t) noexcept;

	std::expected<void, std::string_view> rm_link(const size_t) noexcept;

	std::expected<void, std::string_view> remove(const size_t) noexcept;

	std::expected<void, std::string_view> sort();

	std::expected<void, std::string_view> move(const size_t src, const std::pair<float, float>) noexcept;
};
