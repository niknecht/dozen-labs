#include "app_mode.hpp"
#include "../lib/wire.hpp"


#include <iostream>
#include <variant>
#include <string>
#include <string_view>
#include <ranges>
#include <print>

/*!
 * @file
 * @authors Nikira Vitkovskiy
 * @copyright (c) 2026 Nikita Vitkovkiy
 * @license CC0-1.0
 */

std::ostream& cli::operator<< (std::ostream& cout, const std::variant<InWire, OutWire>& v) {
	const auto uniformCoordinates = std::visit(circuit::Board::overloads([](const auto& w)constexpr noexcept{auto y = w.getuv(); return y;}), v);
	
	char t {std::visit(circuit::Board::overloads([](const InWire&){return 'i';}, [](const OutWire&){return 'o';}), v)};

	return cout << t << "{" << uniformCoordinates.first << "," << uniformCoordinates.second << "}";
	
}

std::ostream& cli::operator<< (std::ostream& cout, const circuit::Board& b) {
	for(auto& it : b)
		cout << it;
	return cout;
	
}

constexpr inline std::string_view app::app_t::help() {
	return
		"Synopsis:\n"
		"classes -M{cli|gui} [-h]\n"
		"-M - set the output method"
		"-h - print this message"
		"gui is not supported as of the current version.\n";
}

void cli::cli_mode::diagramo() const {
	std::vector<std::optional<std::pair<size_t, std::reference_wrapper<const InWire>>>> in;
	
	//std::vector<std::variant<InWire, OutWire>> local_dev = device.vec(); // UI finctions. Don't care about performance
	for(auto it = device.begin(); it < device.end(); ++it)
		in.push_back(std::visit(circuit::Board::overloads(
						[it, begin = device.begin()](const InWire& w)
							->std::optional<std::pair<size_t, std::reference_wrapper<const InWire>>>
							{return {{std::distance(begin, it), w}};},
						[](const OutWire&)
							->std::optional<std::pair<size_t, std::reference_wrapper<const InWire>>>
							{return {};})
					, *it));

	std::vector<std::optional<std::pair<size_t, std::reference_wrapper<const OutWire>>>> outNotConnected;
	for(auto it = device.begin(); it < device.end(); ++it)
		outNotConnected.push_back(std::visit(circuit::Board::overloads(
						[](const InWire&)->std::optional<std::pair<size_t, std::reference_wrapper<const OutWire>>>{return {};},
						[it, begin = device.begin()](const OutWire& w)->std::optional<std::pair<size_t, std::reference_wrapper<const OutWire>>>
							{	         using returnType = std::optional<std::pair<size_t, std::reference_wrapper<const OutWire>>>;
								return (w.is_tethered()
										? returnType{} 
										: returnType{{std::distance(begin, it), w}});
							})
					, *it));

	std::cout << "  In                  Out\n";
	for(const auto& it : in)
		if(it.has_value()) {
			std::cout << "@ " << it.value().first << " " << it.value().second.get();
			if(it.value().second.get().tethered_view().has_value())
				std::cout  << "  ->  " << it.value().second.get().tethered_view().value().get() << '\n';
			else 
				std::cout << '\n';
		}

	for(const auto& it : outNotConnected)
		if(it.has_value())
			std::cout << "                 " << it.value().second.get() << " @ " << it.value().first << '\n';
}

std::expected<void, std::string_view> cli::cli_mode::accepti() {
	std::string query;
	std::getline(std::cin, query);
	std::vector<std::string_view> request_tokens; // a view over request



	auto request = query | std::views::chunk_by([](auto a, auto b){return (!std::isspace(a) && !isspace(b));});

	for(auto token : request){
		auto str = token.begin();
		auto strlen = std::ranges::distance(token);
		
		if(strlen == 0)
			continue;
		std::string_view str_view = std::string_view(&*str, static_cast<size_t>(strlen));
		if(str_view.find_first_not_of(" \t\n\r") == std::string_view::npos)
			continue;
		
		request_tokens.push_back(str_view);
	}

	using namespace std::string_view_literals;
	if(request_tokens[0uz] == "etch"sv){
		using namespace std::string_view_literals;

		char* strtof_success {};

		const auto x_token = request_tokens[1uz];
		float x = std::strtof(x_token.data(), &strtof_success);
		if(!strtof_success || !*strtof_success) return std::unexpected("Syntax error: Invalid float format in the first argument to etch. Only ASCII numbers and the dot are allowed."sv);
		else if(x < 0.f) return std::unexpected("Error: Invalid first argument to the etch expression. X coordinate must be not negative."sv);

		const auto y_token = request_tokens[2uz];

		float y = std::strtof(y_token.data(), &strtof_success);
		if(!strtof_success || !*strtof_success) return std::unexpected("Syntax error: Invalid float format in the second argument to etch. Only ASCII numbers and the dot are allowed."sv);
		else if(y > 0.f) return std::unexpected("Error: Invalid second argument to the etch expression. Y coordinate must be not positive."sv);
	
		auto type_token = request_tokens[3uz];
		char type;
		if(type_token=="i"sv)
			type = 'i';
		else if(type_token=="o"sv)
			type = 'o';
		else return std::unexpected("Syntax error: Bad wire direction. Must be either i or o."sv);

		std::variant<InWire, OutWire> wire = 
			(type == 'i' 
			 ? std::variant<InWire, OutWire>(InWire(std::pair{x, y})) 
			 : OutWire(std::pair{x, y}));

		req.push_back(std::make_shared<command::etch>(this->device, wire));
		return {};
	}
	else if(request_tokens[0uz] == "integrate"sv){
		auto pos1 {~0x0uz}, pos2 {~0x0uz};

		const auto pos1_token {request_tokens[1uz]};
		auto [ptr1, err1] = std::from_chars(pos1_token.data(), pos1_token.data() + pos1_token.size(), pos1);//std::strtoull(request_tokens[1uz].data(), &stoull_success, 10);
		if(err1 != std::errc{} || ptr1 != pos1_token.data() + pos1_token.size())
			return std::unexpected("Syntax error: Invalid unsigned format in the first argument to integrate. Only positive ASCII numbers are allowed."sv);
		
		const auto pos2_token {request_tokens[2uz]};
		auto [ptr2, err2] = std::from_chars(pos2_token.data(), pos2_token.data() + pos2_token.size(), pos2);//std::strtoull(request_tokens[1uz].data(), &stoull_success, 10);
		if(err2 != std::errc{} || ptr2 != pos2_token.data() + pos2_token.size())
			return std::unexpected("Syntax error: Invalid unsigned format in the second argument to integrate. Only positive ASCII numbers are allowed."sv);
		// Checking if pos is a valid index in device is the callable's resposibility

		if(pos1 == pos2)
			return std::unexpected("Error: Can't attach a wire to itself!");

		req.push_back(std::make_shared<command::integrate>(device, pos1, pos2));
		return {};
	}
	else if(request_tokens[0uz] == "dntegrate"sv){
		auto pos {~0x0uz};
		const auto pos_token {request_tokens[1uz]};
		const auto [ptr, err] = std::from_chars(pos_token.data(), pos_token.data() + pos_token.size(), pos);
		if(err != std::errc{} || ptr != pos_token.data() + pos_token.size())
			return std::unexpected("Syntax error: Invalid unsigned format in the argument to dntegrate. Only positive ASCII numbers are allowed."sv);

		req.push_back(std::make_shared<command::dntegrate>(device, pos));
		return {};
	}
	else if(request_tokens[0uz] == "move"sv){
		auto pos {~0x0uz};
		const auto pos_token {request_tokens[1uz]};
		const auto [ptr, err] = std::from_chars(pos_token.data(), pos_token.data() + pos_token.size(), pos);
		if(err != std::errc{} || ptr != pos_token.data() + pos_token.size())
			return std::unexpected("Syntax error: Invalid unsigned format in the argument to dntegrate. Only positive ASCII numbers are allowed."sv);

		char* strtof_success {};

		const auto x_token = request_tokens[1uz];

		float x = std::strtof(x_token.data(), &strtof_success);
		if(!strtof_success || !*strtof_success) return std::unexpected("Syntax error: Invalid float format in the second argument to move. Only ASCII numbers and the dot are allowed."sv);
		else if(x < 0.f) return std::unexpected("Error: Invalid third argument to the etch expression. X coordinate must be not negative."sv);

		const auto y_token = request_tokens[2uz];

		float y = std::strtof(y_token.data(), &strtof_success);
		if(!strtof_success || !*strtof_success) return std::unexpected("Syntax error: Invalid float format in the third argument to move. Only ASCII numbers and the dot are allowed."sv);
		else if(y > 0.f) return std::unexpected("Error: Invalid third argument to the move expression. Y coordinate must be not positive."sv);
		
		req.push_back(std::make_shared<command::move>(device, pos, std::pair{x, y}));
		return {};
	}
	else if(request_tokens[0uz] == "remove"sv) {
		auto pos {~0x0uz};
		const auto pos_token {request_tokens[1uz]};
		const auto [ptr, err] = std::from_chars(pos_token.data(), pos_token.data() + pos_token.size(), pos);
		if(err != std::errc{} || ptr != pos_token.data() + pos_token.size())
			return std::unexpected("Syntax error: Invalid unsigned format in the argument to dntegrate. Only positive ASCII numbers are allowed."sv);
	
		req.push_back(std::make_shared<command::remove>(device, pos));
		return {};
	}

	return std::unexpected("No ctrl paths satisfied the expression.");
}

std::expected<void, std::string_view> cli::cli_mode::act() {
	auto temp = (*req.back())();
	req.pop_back();
	return temp;
}

std::expected<void, std::string_view> cli::cli_mode::erro(std::string_view msg) {
	std::print("{}\n", msg);
	return std::unexpected(msg); // Propagate this so that we know the inital call failed
}

bool cli::cli_mode::refresh_and_wait() {
	while(accepti().or_else(this->erro))
		if(act().or_else(this->erro))
			diagramo();

	return true;
}
