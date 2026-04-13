#include <any>
#include <functional>
#include <optional>

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
	//StreamBus<InWire, OutWire> factory;
public:
	template<typename... t_args>
	InWire(t_args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

	//InWire(const InWire&) = default;
	InWire(InWire&&) = default;

	template<typename... t_args>
	AXIPacket make_tethered(t_args&&...)
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

	InWire(const ::AXIPacket<OutWire, InWire>&&);
};

class OutWire : public Basic_Wire {
	friend AXIPacket<OutWire, InWire>;
	using AXIPacket = AXIPacket<OutWire, InWire>;
private:
	//StreamBus<OutWire, InWire> factory;
public:
	template<typename... t_args>
	OutWire(t_args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

	//OutWire(const OutWire&);
	OutWire(OutWire&&) = default;

	template<typename... t_args>
	AXIPacket make_tethered(t_args&&...)
	requires(std::is_constructible_v<Basic_Wire, t_args...>);

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
	const decltype(slub)&         get_slub() const noexcept;
	const decltype(slave.value()) get_reciever() const noexcept(false); // Use expected for incorrect ABI usage
	void     set_transmitter(const decltype(master)) const noexcept(false); // Use exceptions for design errors within the Wire classes
										// Specifically here, just exit if this gets thrown, or leave unhandeled
	template<typename... t_args>
	AXIPacket(Base&, t_args&&...)
	requires(std::is_constructible_v<Product, t_args...>);

	AXIPacket(AXIPacket&&); // Do I want to move an optional that is a reference_wrapper?
	AXIPacket(const AXIPacket&) = default;

	~AXIPacket();
};
