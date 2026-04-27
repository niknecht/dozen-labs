#include "wire.hpp"
#include <cmath>

Basic_Wire::Basic_Wire(std::pair<float, float>arg) noexcept :uv(arg)
{}

bool Basic_Wire::operator<(const Basic_Wire& other) const {
	const auto& [x,y] = uv;
	const auto& [x1, y1] = other.uv;
	constexpr auto mod = [](const decltype(uv.first)& x, const decltype(uv.second)& y){ return x*x + y*y;};
	return mod(x, y) < mod(x1, y1);
}
bool Basic_Wire::operator==(const Basic_Wire& other) const {
	return uv == other.uv;
}

InWire::InWire(auto&&... args) noexcept
requires(std::is_constructible_v<Basic_Wire, decltype(args)...>) : Basic_Wire(std::forward<decltype(args)>(args)...)
{
}

OutWire::OutWire(auto&&... args) noexcept // Standard forwarding constructor (idk, blame chatgpt)
requires(std::is_constructible_v<Basic_Wire, decltype(args)...>) : Basic_Wire(std::forward<decltype(args)>(args)...)
{}

AXIPacket<InWire, OutWire> InWire::make_tethered(auto&&... args) noexcept
requires(std::is_constructible_v<Basic_Wire, decltype(args)...>)
{
	return AXIPacket{*this, std::forward<decltype(args)>(args)...}; // RVO + C++17 guaranteed copy ellision
}
AXIPacket<OutWire, InWire> OutWire::make_tethered(auto&&... args) noexcept
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


InWire::InWire(::AXIPacket<OutWire, InWire>&& pkt) : InWire(std::move(pkt.get_slub()))
{
	pkt.set_transmitter(*this);
}
OutWire::OutWire(::AXIPacket<InWire, OutWire>&& pkt) : OutWire(std::move(pkt.get_slub()))
{
	pkt.set_transmitter(*this); // Handshake!!
}

template <class Base, class Product>
AXIPacket<Base, Product>::~AXIPacket() { // Clocking event!
	if(master && slave)
		slave.get().tethered = this->slub;
}
// usage:
// board.push_back<>(make_tethered(board[5]))
// make_tethered returns axi_packet (owns slub)
// move the axi_packet into newwire ctor arg from the appropriate/applicable axi_packet
// and you're done

template<class Base, class Product>
auto AXIPacket<Base, Product>::get_slub(this auto&&  self) noexcept-> const decltype(slub)& { //noexcept
	return std::forward<std::remove_reference_t<decltype(self)>>(self).slub;
}

template<class Base, class Product>
void AXIPacket<Base, Product>::set_transmitter(const decltype(master) trueMaster) noexcept {
	this->master = trueMaster;
}

OutWire& OutWire::connect(InWire& other) noexcept{
	this->tethered = other;
	return *this;
}
InWire& InWire::connect(OutWire& other) noexcept {
	this->tethered = other;
	return *this;
}

std::expected<std::reference_wrapper<InWire>, std::string_view> OutWire::operator>> (InWire& other) noexcept{
	using namespace std::string_view_literals;
	if(this->is_tethered() || other.is_tethered())
		return std::unexpected("Attemted reconnect before disconnecting."sv);
	this->connect(other);
	other.connect(*this);
	return other;
}
std::expected<std::reference_wrapper<OutWire>, std::string_view> InWire::operator>> (OutWire& other) noexcept{
	if(this->is_tethered() || other.is_tethered())
		return std::unexpected("Attemted reconnect before disconnecting.");
	this->connect(other);
	other.connect(*this);
	return other;
}

bool InWire::is_tethered() const noexcept {
	return static_cast<bool>(tethered);
}
bool OutWire::is_tethered() const noexcept {
	return static_cast<bool>(tethered);
}

std::expected<void, std::string_view> OutWire::disconnect() {
	using namespace std::literals::string_view_literals;

	if(!tethered) 
		return std::unexpected("Attempt to break a non-existent connection."sv);
	else
		if(tethered.value().get().is_tethered())
			return this->tethered.reset(), tethered.value().get().disconnect();
		else throw "\nCritical Design Error: Assimetrical connection\n"sv;
	
	return {};
}
std::expected<void, std::string_view> InWire::disconnect(){
	using namespace std::literals::string_view_literals;

	if(!tethered) 
		return std::unexpected("Attempt to break a non-existent connection."sv);
	else
		if(tethered.value().get().is_tethered())
			return this->tethered.reset(), tethered.value().get().disconnect();
		else throw "\nCritical Design Error: Assimetrical connection\n"sv;
	
	return {};
}
