#include <utility>
#include <functional>
#include <any>
#include <optional>

class InWire;
class OutWire;

class basic_Wire {
private:
	std::pair<float, float> uv;
public:
	bool operator==(const basic_Wire&);
	bool operator> (const basic_Wire&);
	bool operator<=>(const basic_Wire&);
};


// So when a newly created Wire is moved, its destructor is called
// Before old object is called, the factory's destructor is called
// ----------------------------------------
// Than the new object's move constructor is called
// Referenced in factory object is the old object, moved new object is valid
// Than the factory's move constructor is called
//
// moved new InWire constricted from InWireFactory<OutWire> (has OutWireFactory<InWire>)
// factory references old InWire location (invalid)
// need to save new location into the old jowner object
//
// Save OutWire into the new InWire, old InWireFactory (turned into InWire, moved), new OutWireFactory
//
// usage: InWireInst = OutWire.make_tethered(pairff);
// make tethered returns rvalue ref to its factory
// InWire constructor takes that rvalue ref to outwire's factory and 
// A copies the base to this tethered
// B uses pairff to create InWire with it
// C writes this into stub
// factory destructor writes stub into the OutWire tethered

// Usage: std::declval<Product> = make_tethered(constructor_args);
// TODO: So, in case I make a new Wire class that would have many connections to different types of wires
// (Maybe use std::visit)
template <class Base, class Product>
class WireFactory {
private:
	std::optional<std::reference_wrapper<Base>> owner; // owner is not always knwown at construction
	std::optional<std::reference_wrapper<Product>> stub; // Wrire tethered to owner source
	std::any temp_stub; // No can make this Base cause the type is incomplete 
public:
	WireFactory() = default;
	WireFactory(Base&);
	WireFactory(WireFactory&&);

	~WireFactory();

	void set_temp_stub(const std::any&);
	decltype(temp_stub)&& move_temp_stub();
	void set_stub(const decltype(stub)&);
	decltype(owner) get_owner() const noexcept;
};

class InWire : public basic_Wire {
	friend WireFactory<InWire, OutWire>;
	friend OutWire;
private:
	WireFactory<InWire, OutWire> factory;
	std::reference_wrapper<OutWire> tethered;
public:
	InWire(WireFactory<OutWire, InWire>);
	InWire(const InWire&);
	InWire(InWire&&);

	template<auto... t_args>
	decltype(factory)&& make_tethered(decltype(t_args)...) // Here the construction is implied
	requires(std::is_constructible_v<OutWire, decltype(t_args)...>); // Set the stubuv inside WireFactory with constructed.uv

	InWire(WireFactory<OutWire, InWire>&&);
};

class OutWire : public basic_Wire {
	friend WireFactory<OutWire, InWire>;
	friend InWire;
private:
	WireFactory<OutWire, InWire> factory;
	std::reference_wrapper<InWire> tethered;
public:
	OutWire(WireFactory<InWire, OutWire>);
	OutWire(const OutWire&);
	OutWire(OutWire&&);

	template<auto... t_args>
	decltype(factory)&& make_tethered(decltype(t_args)...)
	requires(std::is_constructible_v<InWire, decltype(t_args)...>);

	OutWire(WireFactory<InWire, OutWire>&&);
};
