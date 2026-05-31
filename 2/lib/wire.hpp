#include <functional>
#include <optional>
#include <string_view>
#include <expected>

#pragma once

/*!
 * @file
 * @authors Nikita Vitkovskiy
 * @copyright (c) 2026 Nikita Vitkovkiy
 * @license CC0-1.0
 */

class Basic_Wire;
class InWire;
class OutWire;
template<class, class>
class AXIPacket;


class Basic_Wire {
protected:
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

	void moveuv(std::pair<float, float> newuv); // Checking the coordinate validity is responsibility of the user class. 
						    // Basic_Wire class guarantees that uv is {[0.f;1.f], [-1.f, 0.f]} at all times
	std::pair<float, float> getuv() const noexcept;
};


class InWire : public Basic_Wire {
	friend AXIPacket<InWire, OutWire>;
	friend OutWire;
	using AXIPacket = AXIPacket<InWire, OutWire>;
private:
	std::optional<std::reference_wrapper<OutWire>> tethered;
	
	InWire& connect(OutWire&) noexcept; // Agh, shoulda called it integrate
public:
	InWire(auto&&... args) noexcept
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>) : Basic_Wire(std::forward<decltype(args)>(args)...)
	{
	}

	InWire(InWire&& w);// TODO: this prolly causes the weird vector behavious unless reserved. Fix this, and fix the OutWire
	InWire(const InWire&);
	InWire& operator=(const InWire&);
	InWire& operator=(InWire&&);

	AXIPacket make_tethered(auto&&... args) noexcept //!< @example InWire a{0.5f, -0.8f}; OutWire b = a.make_tethered(0.2f, -4.5f);
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>); /*!< @param Paramters that wire of the opposite direction can be constructed from
	@return Returns a stub of the wire of the opposite direction, which then can be  used to construct the wire itself.
	Newly created wire is created to the wire it was created for, and the wire it was created for is also connected to the new wire by the end of the call.
	It is guaranteed that there's only 1 copy of AXIPacket created.*/

	bool is_tethered() const noexcept;
	std::optional<std::reference_wrapper<const OutWire>> tethered_view() const noexcept;

	std::expected<void, std::string_view> operator>> (OutWire& other) noexcept;
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
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>) : Basic_Wire(std::forward<decltype(args)>(args)...)
	{}

	OutWire(OutWire&&) ;
	OutWire(const OutWire&) ;
	OutWire& operator=(const OutWire&);
	OutWire& operator=(OutWire&&) ;

	AXIPacket make_tethered(auto&&... args) noexcept //!< @example OutWire a{0.5f, -0.8f}; InWire b = a.make_tethered(0.2f, -4.5f);
	requires(std::is_constructible_v<Basic_Wire, decltype(args)...>); /*!< @param Paramters that wire of the opposite direction can be constructed from
	@return Returns a stub of the wire of the opposite direction, which then can be  used to construct the wire itself.
	Newly created wire is created to the wire it was created for, and the wire it was created for is also connected to the new wire by the end of the call.
	It is guaranteed that there's only 1 copy of AXIPacket created.*/

	bool is_tethered() const noexcept;
	std::optional<std::reference_wrapper<const InWire>> tethered_view() const noexcept;

	std::expected<void, std::string_view> operator>> (InWire& other) noexcept;
	std::expected<void, std::string_view> disconnect();

	OutWire(::AXIPacket<InWire, OutWire>&&);
};


template <class Base, class Product> // <slave, master>
class AXIPacket { // AXI-S protocol
private:
	Product slub; // TUSER
	std::optional<std::reference_wrapper<Base>> slave; // TUSER + TREADY -> set on create
	std::optional<std::reference_wrapper<Product>> master; // TDATA + TVALID -> make transmittion whenever there's a handshake on destruction
	// Newly created Wire transmitts its adress to the old slave
public:
	auto get_slub(this auto&& self) noexcept -> decltype(auto){  // Use expected for incorrect ABI usage
		return std::forward<std::remove_reference_t<decltype(self)>>(self).slub;
	}
	void set_transmitter(const decltype(master)) noexcept;   // Use exceptions for design errors within the Wire classes
								// Specifically here, there's nothing to go wrong
	AXIPacket(Base&, auto&&... args)
	requires(std::is_constructible_v<Product, decltype(args)...>);

	AXIPacket(AXIPacket&&); // Do I want to move an optional that is a reference_wrapper?
	AXIPacket(const AXIPacket&) = default;

	~AXIPacket();
};

auto InWire::make_tethered(auto&&... args) noexcept -> AXIPacket //!< @example InWire a{0.5f, -0.8f}; OutWire b = a.make_tethered(0.2f, -4.5f);
requires(std::is_constructible_v<Basic_Wire, decltype(args)...>) {
	return AXIPacket{*this, std::forward<decltype(args)>(args)...}; // RVO + C++17 guaranteed copy ellision
}
auto OutWire::make_tethered(auto&&... args) noexcept -> AXIPacket
requires(std::is_constructible_v<Basic_Wire, decltype(args)...>) 
{
	return AXIPacket{*this, std::forward<decltype(args)>(args)...}; // RVO + C++17 guaranteed copy ellision
}

template<class Base, class Product>
AXIPacket<Base, Product>::AXIPacket(Base& owner, auto&&... args)
requires(std::is_constructible_v<Product, decltype(args)...>) : slave(owner), slub(std::forward<decltype(args)>(args)...)
{
}

template<class Base, class Product>
AXIPacket<Base, Product>::AXIPacket(AXIPacket&& other) : slave(other.slave), master(other.master), slub(std::move(other.slub))
{}

template <class Base, class Product>
AXIPacket<Base, Product>::~AXIPacket() { // Clocking event!
	if(master && slave)
		slave.value().get().tethered = this->slub;
}

template<class Base, class Product>
void AXIPacket<Base, Product>::set_transmitter(const decltype(master) trueMaster) noexcept {
	this->master = trueMaster;
}
