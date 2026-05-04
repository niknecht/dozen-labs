#include "../lib/board.hpp"

#pragma once

#include <string_view>
#include <string>
#include <iostream>

namespace cli {
	class cli_mode;

	std::ostream& operator<<(std::ostream& cout, const std::variant<InWire, OutWire>& w);

	std::ostream& operator<<(std::ostream& cout, const circuit::Board& b);
}

enum class op_t {
	
};

class app {
private:
	circuit::Board device;
public:
	constexpr inline static std::string_view help();
	virtual bool refresh_and_wait() = 0;
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

	bool refresh_and_wait() override;
	void diagramo() const override;
	std::expected<op_t, std::string_view> accepti() const override;
	std::expected<void, std::string_view> act(op_t op) const override;
	void erro(std::string_view) override;

	~cli_mode() = default;
	
};
