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

	Basic_Wire(Basic_Wire&&) = default; // This is just std::pair reallistically
	Basic_Wire(const Basic_Wire&) = default;
	Basic_Wire& operator=(const Basic_Wire&) = default;
	Basic_Wire& operator= (Basic_Wire&&) = default;

	bool operator<(const Basic_Wire&) const;
	bool operator==(const Basic_Wire&) const;
	bool operator<=>(const Basic_Wire&) const = default;

	std::expected<void, std::string_view> moveuv(std::pair<float, float> newuv);
};


class InWire : public Basic_Wire {
	friend AXIPacket<InWire, OutWire>;
	friend OutWire;
	using AXIPacket = AXIPacket<InWire, OutWire>;
private:
	std::optional<std::reference_wrapper<OutWire>> tethered;
	
	InWire& connect(OutWire&) noexcept; // Agh, shoulda called it intgrate
public:
	InWire(auto&&... args) noexcept
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>);

	InWire(InWire&&) = default;
	InWire(const InWire&) = default;
	InWire& operator=(const InWire&) = default;
	InWire& operator=(InWire&&) = default;

	AXIPacket make_tethered(auto&&... args) noexcept
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>);

	bool is_tethered() const noexcept;
	std::expected<void, std::string_view> operator>> (OutWire& other) noexcept; // TODO Add C++23 conditional noexcept here and everywhere else
	std::expected<void, std::string_view> disconnect();

	InWire(::AXIPacket<OutWire, InWire>&&);
};

class OutWire : public Basic_Wire {
	friend AXIPacket<OutWire, InWire>;
	friend InWire;
	using AXIPacket = AXIPacket<OutWire, InWire>;
private:
	std::optional<std::reference_wrapper<InWire>> tethered;

	OutWire& connect(InWire&) noexcept;
public:
	OutWire(auto&&... args) noexcept
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>);

	OutWire(OutWire&&) = default;
	OutWire(const OutWire&) = default;
	OutWire& operator=(const OutWire&) = default;
	OutWire& operator=(OutWire&&) = default;

	AXIPacket make_tethered(auto&&... args) noexcept
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>);

	bool is_tethered() const noexcept;
	std::expected<void, std::string_view> operator>> (InWire& other) noexcept;
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
	auto get_slub(this auto&& self) noexcept -> const decltype(slub)&;  // Use expected for incorrect ABI usage, TODO use explcit self parameter and deducing this
	void set_transmitter(const decltype(master)) noexcept;   // Use exceptions for design errors within the Wire classes
								// Specifically here, there's nothing to go wrong
	AXIPacket(Base&, auto&&... args)
	requires(std::is_constructible_v<Product, decltype(args)...>);

	AXIPacket(AXIPacket&&); // Do I want to move an optional that is a reference_wrapper?
	AXIPacket(const AXIPacket&) = default;

	~AXIPacket();
};
