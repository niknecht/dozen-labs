#include "../lib/board.hpp"
#include "command.hpp"

#pragma once

#include <string_view>
#include <string>
#include <iostream>
#include <memory>

/*!
 * @file
 * @authors Nikira Vitkovskiy
 * @copyright (c) 2026 Nikita Vitkovkiy
 * @license CC0-1.0
 */

namespace app {
	class app_t;
}
namespace cli {
	class cli_mode;

	std::ostream& operator<<(std::ostream& cout, const std::variant<InWire, OutWire>& w);
	std::ostream& operator<<(std::ostream& cout, const circuit::Board& b);
}


/*!
 * The class that encapsulates some app classes, enabling polimorphism to cli mode and gui mode.
*/
class app::app_t {
protected:
	circuit::Board device;
	std::vector<std::shared_ptr<command::cmd_t>> req;

	virtual std::expected<void, std::string_view> accepti() = 0; //!< Call this to 
	virtual std::expected<void, std::string_view> act() = 0; //!< This calls back of the 
	virtual void diagramo() const = 0; //!< This refreshes the diagram of current circuit.
public:
	constexpr inline static std::string_view help() noexcept; //!< Pretty self-explanatory. Returns a view to the help string.
	virtual bool refresh_and_wait() = 0; //!< This is the top level function. All errors are handles here.
	//static void erro(std::string_view);
};

class cli::cli_mode : public app::app_t {
private:
	std::string buf;
	
	static std::expected<void,std::string_view> erro(std::string_view); //!< This is a callable that prints the error message.

	std::expected<void, std::string_view> accepti() override;
	std::expected<void, std::string_view> act() override;
	void diagramo() const override;
public:
	cli_mode() = default;

	bool refresh_and_wait() override;

	~cli_mode() = default;
};
