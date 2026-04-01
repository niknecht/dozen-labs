#include "wire.hpp"

#include <vector>
#include <span>
#include <variant>

class Board {
private:
	std::vector<std::variant<basic_InWire, basic_OutWire>> interconnect;

public:
	Board() = delete;
	explicit Board(std::span<std::variant<basic_InWire, basic_OutWire>> interconnect_src);
	explicit Board(const std::variant<basic_InWire, basic_OutWire>&);

	Board& operator+=(std::variant<basic_InWire, basic_OutWire>);
};
