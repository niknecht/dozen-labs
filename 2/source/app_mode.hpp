#include "../lib/board.hpp"
#include "command.hpp"

#pragma once

#include <string_view>
#include <string>
#include <iostream>
#include <memory>

namespace app {
	class app_t;
}
namespace cli {
	class cli_mode;

	std::ostream& operator<<(std::ostream& cout, const std::variant<InWire, OutWire>& w);
	std::ostream& operator<<(std::ostream& cout, const circuit::Board& b);
}

enum class op_t {
	
};

// This class implements command design pattern
class app::app_t {
protected:
	circuit::Board device;
	std::vector<std::shared_ptr<command::cmd_t>> req;
public:
	// High-level methods
	constexpr inline static std::string_view help();
	virtual bool refresh_and_wait() = 0; // All errors are handled at this level
	virtual void diagramo() const = 0;
	virtual std::expected<void, std::string_view> accepti() = 0; // Assigns Command_t in vec TODO If invalid input (unexpected), accept again
	virtual std::expected<void, std::string_view> act() = 0;
	virtual void erro(std::string_view) = 0;

};

class cli::cli_mode : public app::app_t {
private:
	std::string buf;
public:
	cli_mode() = default;

	bool refresh_and_wait() override;
	void diagramo() const override;
	std::expected<void, std::string_view> accepti() override;
	std::expected<void, std::string_view> act() override;
	void erro(std::string_view) override;

	~cli_mode() = default;
};
