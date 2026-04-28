#include "app_mode.hpp"

std::ostream& cli::operator>> (const std::ostream& cout, const std::variant<InWire, OutWire>& v) {
	//std::visit(circuit::Board::overloads([&cout](auto& wire){return cout << wire..first << wire.uv.second;}), v);
}

void cli::cli_mode::diagramo() const {
}
