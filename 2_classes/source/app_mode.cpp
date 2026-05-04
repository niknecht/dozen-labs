#include "app_mode.hpp"

#include <iostream>

std::ostream& cli::operator<< (std::ostream& cout, const std::variant<InWire, OutWire>& v) {
	const auto uniformCoordinates = std::visit(circuit::Board::overloads([](const auto& w)constexpr noexcept{auto y = w.getuv(); return y;}), v);
	
	char t {std::visit(circuit::Board::overloads([](const InWire&){return 'i';}, [](const OutWire&){return 'o';}), v)};

	return cout << t << "{" << uniformCoordinates.first << "," << uniformCoordinates.second << "}";
	
}

std::ostream& cli::operator<< (std::ostream& cout, const circuit::Board& b) {
	for(auto& it : b.data())
		cout << it;
	return cout;
	
}

constexpr inline std::string_view app::help() {
	return
		"Synopsis:\n"
		"classes -M{cli|gui} [-h]\n"
		"-M - set the output method"
		"-h - print this message"
		"gui is not supported as of the current version.\n";
}



bool cli::cli_mode::refresh_and_wait() {
	return true;
}
