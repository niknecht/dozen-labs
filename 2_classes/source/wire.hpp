#include <any>
#include <functional>
#include <optional>
#include <string_view>
#include <expected>

class Basic_Wire;
class InWire;
class OutWire;
template<class, class>
class AXIPacket;

// Yep, the fourth rewrite, yes, yes, seriously. This one uses AXI4-Stream

class Basic_Wire {
private:
	std::pair<float, float> uv;
public:
	Basic_Wire(std::pair<float, float>) noexcept;
};


class InWire : public Basic_Wire {
	friend AXIPacket<InWire, OutWire>;
	using AXIPacket = AXIPacket<InWire, OutWire>;
private:
	std::optional<std::reference_wrapper<OutWire>> tethered;
public:
	template<typename... t_args>
	InWire(t_args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

	InWire(InWire&&) = default;

	template<typename... t_args>
	AXIPacket make_tethered(t_args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

	bool is_tethered() const noexcept;
	OutWire& operator>> (OutWire& other);
	InWire& connect(OutWire&);
	std::expected<void, std::string_view> disconnect();

	InWire(::AXIPacket<OutWire, InWire>&&);
};

class OutWire : public Basic_Wire {
	friend AXIPacket<OutWire, InWire>;
	using AXIPacket = AXIPacket<OutWire, InWire>;
private:
	std::optional<std::reference_wrapper<InWire>> tethered;
public:
	template<typename... t_args>
	OutWire(t_args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

	OutWire(OutWire&&) = default;

	template<typename... t_args>
	AXIPacket make_tethered(t_args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

	bool is_tethered() const noexcept;
	InWire& operator>> (InWire& other);
	OutWire& connect(InWire&);
	std::expected<void, std::string_view> disconnect();

	OutWire(::AXIPacket<InWire, OutWire>&&);
};

// Make In/OutWire constructible from whareve BasicWire is constructible from

// This is also class agnostic, so you can create whaterever new wires, by just passing either std::any to Product
// TODO Implement whatever I just wrote above with std::visit prolly
template <class Base, class Product> // <slave, master>
class AXIPacket { // AXI-S protocol Valcum tu FPGA
private:
	Product slub; // TUSER
	std::optional<std::reference_wrapper<Base>> slave; // TUSER + TREADY -> set on create
	std::optional<std::reference_wrapper<Product>> master; // TDATA + TVALID -> make transmittion whenever there's a handshake on destruction
	// Newly created Wire transmitts its adress to the old slave
public:
	auto get_slub() const noexcept -> const decltype(slub)&;  // Use expected for incorrect ABI usage
	void set_transmitter(const decltype(master)) noexcept;   // Use exceptions for design errors within the Wire classes
								// Specifically here, there's nothing to go wrong
	template<typename... t_args>
	AXIPacket(Base&, t_args&&...)
	requires(std::is_constructible_v<Product, t_args...>);

	AXIPacket(AXIPacket&&); // Do I want to move an optional that is a reference_wrapper?
	AXIPacket(const AXIPacket&) = default;

	~AXIPacket();
};
