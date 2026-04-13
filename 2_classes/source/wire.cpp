#include "wire.hpp"

Basic_Wire::Basic_Wire(std::pair<float, float>arg) noexcept :uv(arg)
{}

template<typename... t_args>
InWire::InWire(t_args&&... args) noexcept // Args outlive the functon call
requires(std::is_constructible_v<Basic_Wire, t_args...>) : Basic_Wire(std::forward<t_args>(args)...)
{
}

template<typename... t_args>
OutWire::OutWire(t_args&&... args) noexcept // Standard forwarding constructor (idk, blame chatgpt)
requires(std::is_constructible_v<Basic_Wire, t_args...>) : Basic_Wire(std::forward<t_args>(args)...)
{}

template<typename... t_args>
AXIPacket<InWire, OutWire> InWire::make_tethered(t_args&&... args) noexcept
requires(std::is_constructible_v<Basic_Wire, t_args...>)
{
	return AXIPacket{*this, std::forward<t_args>(args)...}; // RVO
}
template<typename... t_args>
AXIPacket<OutWire, InWire> OutWire::make_tethered(t_args&&... args) noexcept
requires(std::is_constructible_v<Basic_Wire, t_args...>) 
{
	return AXIPacket{*this, std::forward<t_args>(args)...}; // RVO
}

template<class Base, class Product>
template<typename... t_args>
AXIPacket<Base, Product>::AXIPacket(Base& owner, t_args&&... args)
requires(std::is_constructible_v<Product, t_args...>) : slave(owner), slub(std::forward<t_args>(args)...)
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
AXIPacket<Base, Product>::~AXIPacket() { // Clock!
	if(master && slave)
		slave.get().tethered = this->slub;
}
// usage:
// board.push_back<>(make_tethered(board[5]))
// make_tethered returns axi_packet (owns slub)
// move the axi_packet into newwire ctor arg from the appropriate/applicable axi_packet
// and you're done

template<class Base, class Product>
auto AXIPacket<Base, Product>::get_slub() const noexcept-> const decltype(slub)& { //noexcept
	return this->slub;
}

template<class Base, class Product>
void AXIPacket<Base, Product>::set_transmitter(const decltype(master) trueMaster) noexcept {
	this->master = trueMaster;
}
