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

	std::ostream& operator<<(std::ostream& cout, const std::variant<InWire, OutWire>& w); //!< @see cli::cli_mode::diagramo()
	std::ostream& operator<<(std::ostream& cout, const circuit::Board& b); //!< @see cli::cli_mode::diagramp()
}


/*!
 * The class that encapsulates some app classes, enabling polimorphism to cli mode and gui mode.
*/
class app::app_t {
protected:
	circuit::Board device;
	std::vector<std::shared_ptr<command::cmd_t>> req;

	virtual std::expected<void, std::string_view> accepti() = 0; //!< Call this to record a command to the command queue
	virtual std::expected<void, std::string_view> act() = 0; //!< This calls back the last command in the command queue
	virtual void diagramo() const = 0; //!< This refreshes the diagram of the current circuit.
public:
	constexpr inline static std::string_view help() noexcept; //!< Pretty self-explanatory. Returns a view to the help string.
	virtual bool refresh_and_wait() = 0; //!< This is the top level function. All errors are handles here.
	//static void erro(std::string_view);
};

class cli::cli_mode : public app::app_t {
private:
	std::string buf;
	
	static std::expected<void,std::string_view> erro(std::string_view); //!< This is a callable that prints the error message.

	std::expected<void, std::string_view> accepti() override; //!< @see app::app_t::accepti()
	std::expected<void, std::string_view> act() override; //!< @see app::app_t::accepti()
	void diagramo() const override; /*! @see app::app_t::outputi()
					  @brief Outputs the diagram as <index> i <in-wire coordinate> [-> <connected in-wire coordinate> o]
									...
													   <out-wire coordinate> o <index>
					*/
public:
	cli_mode() = default;

	bool refresh_and_wait() override; //!< @see app::app_t::refresh_and_wait()

	~cli_mode() = default;
};
