#include"command.hpp"
#include "../lib/board.hpp"

#include <algorithm>

command::cmd_t::cmd_t(circuit::Board& device) : board{device} {}

command::etch::etch(circuit::Board& device, std::variant<InWire, OutWire> wire) : cmd_t{device}, w{wire} {
}

std::expected<void, std::string_view> command::etch::operator()() {
	board.get().operator+=(std::move(w));
	return {};
}

command::integrate::integrate(circuit::Board& device, size_t pos1, size_t pos2) :cmd_t{device}, pos1{pos1}, pos2{pos2}
{}

std::expected<void, std::string_view> command::integrate::operator()() {
	if(pos1 + 1 > board.get().vec().size())
		return std::unexpected("Error: The position requested in integrate's first arg does not exist.");
	else if (pos2 + 1 > board.get().vec().size())
		return std::unexpected("Error: The position requested in integrate's second arg does not exist.");
	else if (board.get()[pos1].index() == board.get()[pos2].index())
		return std::unexpected("Error: In integrate call - wire type mismatch.");
	else
		return board.get().add_link(pos1, pos2);
}
