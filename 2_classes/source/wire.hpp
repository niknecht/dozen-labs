#include <utility>
#include <optional>
#include <expected>
#include <string_view>
#include <memory>
#include <functional>
#include <type_traits>

#pragma once

constexpr static decltype(auto) g_signalSpeed {5lu};

// Static interface CRTP
template <class>
class Wire;
class InWire;
class OutWire;

template <class Derived, typename ...constructableToWire_t>	
concept is_constructible_to_Wire = 
	requires(Derived D){ D(std::declval<constructableToWire_t>()...);};

template <class Derived>
class Wire {
private:
	std::pair<float, float> uv;
	class Tethered_t;
public:
	Wire() = delete;
	Wire(const Wire<Derived>&);
	Wire& operator=(const Wire<Derived>&);

	explicit Wire(const std::pair<float, float>) noexcept;

	bool operator==(const Wire&); // Making this a template auto would equate Wire to whatever
	bool operator> (const Wire&); // a Wire can be constructed from
	auto operator<=>(const Wire&); // which is, obviously, very wrong

	std::expected<void, std::string_view> disconnect(); // Tha powa of CRTP!! Derived.tethered

	~Wire() = default;

	auto make_tethered(auto...) -> // But here, the construction is implied
	std::expected<std::remove_reference_t<Wire<Derived>::Tethered_t> &&, std::string_view>
	requires(is_constructible_to_Wire<Derived>); // Also, I don't have to write a deduction guide thanks to auto
	
	Derived&& operator=(Derived&&);
		// Okay, now this is a hack, but the assignment itself doesn't allow a conventional method
	std::expected<void, std::string_view> operator>>(const Tethered_t& newTethered);
};

// These handle the  tethered
class InWire : public Wire<InWire> {
private:
	using Tethered_t = OutWire;
	std::optional<std::reference_wrapper<Tethered_t>> tethered;
public:
};

class OutWire : public Wire<OutWire> {
private:
	using Tethered_t = InWire;
	std::optional<std::reference_wrapper<Tethered_t>> tethered;
public:

};

// TODO Use factory pattern for make_tethered if possible

// TODO make Derived Derived::operator=(Derived moved&&) {*this = std::forward<Derived>(moved); return static_cast(this->tethered.tethered = *this);} return std::move(...)
// TODO Add constuctors to the derived classes (maybe use same concept that is already defined)
// TODO Tests: type validity
// 1) copy doesn't end up with tethered type mismatch
//
// TODO: Use range_value_t in board class i guess (or just let it implicitly convert to span?)
