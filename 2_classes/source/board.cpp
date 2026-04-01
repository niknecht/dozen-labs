#include "board.hpp"

Board::Board(std::span<std::variant<basic_InWire, basic_OutWire>> interconnect_src)
	:interconnect(std::from_range, interconnect_src) {}

Board::Board(const std::variant<basic_InWire, basic_OutWire>& it) 
	:interconnect(it) {}

//Board& Board::operator+=(std::variant<basic_InWire, basic_OutWire>) {}
