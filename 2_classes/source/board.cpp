/*#include "board.hpp"

Board::Board(std::span<std::variant<basic_InWire, basic_OutWire>> interconnect_src)
	:interconnect(std::from_range, interconnect_src) {}

Board::Board(const std::variant<Wire<basic_InWire>, Wire<basic_OutWire>>& it) 
	:interconnect({it})
{
}

Board& Board::operator+=(const std::variant<basic_InWire, basic_OutWire>& it) {
	this->interconnect.push_back(it);
	return *this;
}

std::variant<Wire<basic_InWire>>
// Use std::visit for anything direction-agnostic
//Board& Board::operator+=(std::variant<basic_InWire, basic_OutWire>) {}
/*/
