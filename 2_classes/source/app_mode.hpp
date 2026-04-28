#include "../lib/board.hpp"

#pragma once

#include <string_view>
#include <string>
#include <iostream>

namespace cli {
	class cli_mode;

	std::ostream& operator>>(const std::ostream& cout, const std::variant<InWire, OutWire>& w);
}

enum class op_t {
	
};

class app {
private:
	circuit::Board device;
public:
	virtual bool refresh_and_wait() const = 0;
	virtual void diagramo() const = 0;
	virtual std::expected<op_t, std::string_view> accepti() const = 0;
	virtual std::expected<void, std::string_view> act(op_t op) const = 0;
	virtual void erro(std::string_view) = 0;
};

class cli::cli_mode : public app {
private:
	std::string buf;
public:
	cli_mode() = default;

	bool refresh_and_wait() const override;
	void diagramo() const override;
	std::expected<op_t, std::string_view> accepti() const override;
	std::expected<void, std::string_view> act(op_t op) const override;
	void erro(std::string_view) override;

	~cli_mode() = default;
	
};
