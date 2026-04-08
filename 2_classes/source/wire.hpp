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
	template<typename... args>
	InWire(args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, args...>);

	InWire(InWire&&) = default;


	template<typename... args>
	AXIPacket make_tethered(args&&...)
	requires(std::is_constructible_v<Basic_Wire, args...>);

	InWire(AXIPacket&&);
};

class OutWire : public Basic_Wire {
	friend AXIPacket<OutWire, InWire>;
	using AXIPacket = AXIPacket<OutWire, InWire>;
private:
	//StreamBus<OutWire, InWire> factory;
public:
	template<typename... args>
	OutWire(args&&...) noexcept
	requires(std::is_constructible_v<Basic_Wire, args...>);

	OutWire(OutWire&&) = default;

	template<typename... args>
	AXIPacket make_tethered(args&&...)
	requires(std::is_constructible_v<Basic_Wire, args...>);

	OutWire(AXIPacket&&);
};

// Make In/OutWire constructible from whareve BasicWire is constructible from

// This is also class agnostic, so you can create whaterever new wires, by just passing either std::any to Product
// TODO Implement whatever I just wrote above with std::visit prolly
template <class Base, class Product> // <slave, master>
class AXIPacket { // AXI-S protocol Valcum tu FPGA
private:
	Product slub; // TUSER
	std::optional<std::reference_wrapper<Base>> slave; // TUSER + TREADY -> set on create
	std::optional<std::reference_wrapper<Product>> master; // TDATA + TVALID -> destruction whenever there's a handshake
	// Newly created Wire transmitts its adress to the old slave
public:
	decltype(slub) get_slub() const noexcept;
	decltype(slave) get_reciever() const noexcept;
	decltype(master) get_transmitter() const noexcept;

	template<typename... args>
	AXIPacket(Base&, args...)
	requires(std::is_constructible_v<Product, args...>);

	AXIPacket(AXIPacket&&); // Do I want to move an optional that is a reference_wrapper?
	AXIPacket(const AXIPacket&);

	~AXIPacket();
};
