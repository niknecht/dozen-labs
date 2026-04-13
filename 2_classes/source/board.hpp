/*#include "wire.hpp"

#include <vector>
#include <span>
#include <variant>

class Board {
private:
	std::vector<std::variant<Wire<basic_InWire>, Wire<basic_OutWire>>> interconnect;

public:
	Board() = delete;
	explicit Board(std::span<std::variant<Wire<basic_InWire>, Wire<basic_OutWire>>> interconnect_src);
	explicit Board(const std::variant<Wire<basic_InWire>, Wire<basic_OutWire>>&);

	Board& operator+=(const std::variant<Wire<basic_InWire>, Wire<basic_OutWire>>&);
};(\*/
