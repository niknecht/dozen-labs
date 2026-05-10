#include"command.hpp"
#include "../lib/board.hpp"

#include <string_view>
#include <expected>

/*!
 * @file
 * @authors Nikira Vitkovskiy
 * @copyright (c) 2026 Nikita Vitkovkiy
 * @license CC0-1.0
 */

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
	else if (std::visit(circuit::Board::overloads([](const auto& w){return w.is_tethered();}), (board).get().vec()[pos1]))
		return std::unexpected("Error: The wire at argument one's position is already connected.");
	else if (std::visit(circuit::Board::overloads([](const auto& w){return w.is_tethered();}), (board).get().vec()[pos2]))
		return std::unexpected("Error: The wire at argument two's position is already connected.");
	/*Random commentary moment: use of vector of variants in this case is dictated by different return types for
	 * some InWire and OutWire member callables. This means that InWire and OutWire classes are not truely polymorphic
	 * as the context in which they can be called is dectated by their specific derived type. This is why polimorphism-
	 * centered  vector of shared ptr's to base would have very tight limitations in this case, making vec of variants
	 * approach nessesary and most preactical here. tldr: These classes are not truely polymorphic to most critical operations,
	 * hence polimorphism approaches do not fit.*/
	else
		return board.get().add_link(pos1, pos2);
}

command::dntegrate::dntegrate(circuit::Board& device, size_t pos) : cmd_t{device}, pos{pos}
{}

std::expected<void, std::string_view> command::dntegrate::operator()() {
	if (pos + 1 > board.get().vec().size())
		return std::unexpected("Error: The position requested in dntegrate does not exist.");
	else if (std::visit(circuit::Board::overloads([](const auto& w){return w.is_tethered();}), board.get().vec()[pos]))
		return std::unexpected("Error: Cannot dntegrate a wire that is not connected!");

	else
		return board.get().rm_link(pos);
}
// TODO: Error checking resposibilities is a mess, I'll have to establish clear contracts for each functon in the 3rd lab.
// Too bad contracts are not supported in clang yet.

command::move::move(circuit::Board& device, size_t pos, std::pair<float, float> uv) : cmd_t{device}, uv{uv}, pos{pos}
{}

std::expected<void, std::string_view> command::move::operator()() {
	if (pos + 1 > board.get().vec().size())
		return std::unexpected("Error: The position requested in move does not exist.");
	
	return board.get().moveuv(pos, uv);
}

command::remove::remove(circuit::Board& device, size_t pos) : cmd_t{device}, pos{pos} 
{}

std::expected<void, std::string_view> command::remove::operator()() {
	if (pos + 1 > board.get().vec().size())
		return std::unexpected("Error: The position requested in remove does not exist.");

	else
		return board.get().remove(pos);
}
