#include "wire.hpp"

Basic_Wire::Basic_Wire(std::pair<float, float>arg) noexcept :uv(arg)
{}

template<typename... t_args>
InWire::InWire(t_args&&... args) noexcept
requires(std::is_constructible_v<Basic_Wire, t_args...>) : Basic_Wire(std::forward<t_args>(args)...)
{
}

template<typename... t_args>
OutWire::OutWire(t_args&&... args) noexcept
requires(std::is_constructible_v<Basic_Wire, t_args...>) : Basic_Wire(std::forward<t_args>(args)...)
{}

template<typename... t_args>
AXIPacket<InWire, OutWire> InWire::make_tethered(t_args&&... args)
requires(std::is_constructible_v<Basic_Wire, t_args...>)
{
	return AXIPacket{*this, args...};
}
template<typename... t_args>
AXIPacket<OutWire, InWire> OutWire::make_tethered(t_args&&... args)
requires(std::is_constructible_v<Basic_Wire, t_args...>) 
{
	return AXIPacket{*this, args...};
}

template<class Base, class Product>
template<typename... t_args>
AXIPacket<Base, Product>::AXIPacket(Base&, t_args... args)
requires(std::is_constructible_v<Product, t_args...>) : slub(args...)
{
}

template<class Base, class Product>
AXIPacket<Base, Product>::AXIPacket(AXIPacket&&)
{}

template<class Base, class Product>
AXIPacket<Base, Product>::AXIPacket(const AXIPacket& other) : slub(other.slub)
{}
