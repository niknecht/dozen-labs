#include "wire.hpp"

#include <type_traits>

template <class Base, class Product>
WireFactory<Base, Product>::WireFactory(Base& base) :owner{base}{
}

template <auto... args>
decltype(std::declval<InWire>().factory)&& InWire::make_tethered(decltype(args)...)
requires(std::is_constructible_v<OutWire, decltype(args)...>) 
{
	this->factory.set_temp_stub(OutWire(args...));
	return std::move(this->factory);
}
template <auto... args>
decltype(std::declval<OutWire>().factory)&& OutWire::make_tethered(decltype(args)...)
requires(std::is_constructible_v<InWire, decltype(args)...>) 
{
	this->factory.set_temp_stub(InWire(args...));
	return std::move(this->factory);
}

template <class Base, class Product>
void WireFactory<Base, Product>::set_temp_stub(const std::any& it) {
	this->temp_stub = it;
}
template <class Base, class Product>
void WireFactory<Base, Product>::set_stub(const decltype(stub)& it) {
	this->stub = it;
}

template <class Base, class Product>
WireFactory<Base, Product>::WireFactory(WireFactory&& moved) :owner(moved.owner)
							     ,stub(moved.stub)
							     ,temp_stub(std::move(moved.temp_stub)){
}

InWire::InWire(WireFactory<OutWire, InWire>&& that) :InWire(std::any_cast<InWire>(std::forward<std::any>(that.move_temp_stub()))){
	that.set_stub(*this);
	this->tethered = that.get_owner();
}
OutWire::OutWire(WireFactory<InWire, OutWire>&& that) :factory(*this), tethered(that.get_owner()){
	that.set_stub(*this);
}

//template<class Base, class Product>
//WireFactory<Base, Product>::WireFactory(WireFactory&&) = default;

//InWire from WireFactory<OutWire, InWire>&& ctor
InWire::InWire(WireFactory<OutWire, InWire>&& stub) :InWire(std::any_cast<InWire>(stub.temp_stub)) {
	factory.owner = *this;
	tethered = stub.owner;
	stub.stub = *this;
}
// OutWire from WireFactory<InWire, OutWire>&& ctor
OutWire::OutWire(WireFactory<InWire, OutWire>&& stub) :factory(*this), tethered(stub.owner){
	stub.stub = *this;
}
// stub.owner.get().tethered = *this;

//template ~WireFactory()
/*template<class Base, class Product>
WireFactory<Base, Product>::~WireFactory() {
	this->owner.get().tethered = this->stub;
}*/

template<class Base, class Product>
WireFactory(Base&) -> WireFactory<Base, std::remove_reference<decltype(std::declval<Base>().tethered.get())>>;//Base, std::conditional<std::is_same_v<Base, InWire>, OutWire, InWire>>
//requires(std::is_same_v<Base, InWire> || std::is_same_v<Base, OutWire>){};

template<class Base, class Product>
WireFactory<Base, Product>::WireFactory(Base& base) :owner(base), stub(owner.get().tethered)
{

}
