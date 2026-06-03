// TODO: Put app into some namespace
// TODO: Put command into another template

#include <variant>
#include <functional>

#pragma once

/*!
 * @file
 * @authors Nikita Vitkovskiy
 * @copyright (c) 2026 Nikita Vitkovkiy
 * @license CC0-1.0
 */

#include "../lib/wire.hpp"
#include "../lib/board.hpp"

namespace command {
	template<class crtp>
	class cmdcrtp;
	class cmd_t;

	class etch;
	class integrate;
	class dntegrate;
	class move;
	class remove;
}

class command::cmd_t {
protected:
	std::reference_wrapper<circuit::Board> board; // Use Board interfaces to acces interconnect through the API
public:
	cmd_t(circuit::Board&); // TODO just set board here
	virtual std::expected<void, std::string_view> operator()() = 0;
};


class command::etch : public command::cmd_t {
private:
	std::variant<InWire, OutWire> w;
public:
	explicit etch(circuit::Board&, std::variant<InWire, OutWire>); // Copy ellision on pass

	etch& operator=(etch&) = default;
	etch& operator=(etch&&) = default;
	etch(etch&&) = default;
	etch(etch&) = default;

	std::expected<void, std::string_view> operator()() override;

	~etch() = default;
};

class command::integrate : public command::cmd_t {
private:
	std::size_t pos1, pos2;
public:
	explicit integrate(circuit::Board&, std::size_t, std::size_t);

	integrate& operator=(const integrate&) = default;
	integrate& operator=(integrate&&) = default;
	integrate(const integrate&) = default;
	integrate(integrate&&) = default;

	std::expected<void, std::string_view> operator()() override;

	~integrate() = default;
};

class command::dntegrate : public command::cmd_t {
private:
	std::size_t pos;
public:
	explicit dntegrate(circuit::Board&, std::size_t);

	dntegrate& operator=(const dntegrate&) = default;
	dntegrate& operator=(dntegrate&&) = default;
	dntegrate(const dntegrate&) = default;
	dntegrate(dntegrate&&) = default;

	std::expected<void, std::string_view> operator()() override;

	~dntegrate() = default;
};

class command::move : public command::cmd_t {
private:
	std::pair<float, float> uv;
	std::size_t pos;
public:
	explicit move(circuit::Board&, std::size_t, std::pair<float, float>);

	move& operator=(const move&) = default;
	move& operator=(move&&) = default;
	move(const move&) = default;
	move(move&&) = default;

	std::expected<void, std::string_view> operator()() override;

	~move() = default;
};

class command::remove : public command::cmd_t {
private:
	std::size_t pos;
public:
	explicit remove(circuit::Board&, std::size_t);

	remove& operator=(const remove&) = default;
	remove& operator=(remove&&) = default;
	remove(const remove&) = default;
	remove(remove&&) = default;

	std::expected<void, std::string_view> operator()() override;

	~remove() = default;
};
