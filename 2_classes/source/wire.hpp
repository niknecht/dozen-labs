#include <utility>
#include <functional>

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


template <class Base, class Product>
class WireFactory {
private:
	basic_Wire temp_stub; // idc good enough for govt work 
			      // and the compiler will bitch if I put a fully fledged Base var in here
	std::reference_wrapper<Base> owner;
	std::reference_wrapper<Product> stub;
public:
	WireFactory() = delete;
	WireFactory(Base&);
	WireFactory(WireFactory&&);

	~WireFactory();
};


// No can use CRTP cause then I get a loop of incomplete types
class InWire : basic_Wire {
	friend WireFactory<InWire, OutWire>;
private:
	WireFactory<InWire, OutWire> factory;
public:
	InWire(WireFactory<OutWire, InWire>);

	template<auto... t_args>
	decltype(factory)&& make_tethered(decltype(t_args)...) // Here the construction is implied
	requires(std::is_constructible_v<OutWire, decltype(t_args)...>); // Set the stubuv inside WireFactory with constructed.uv

	InWire(WireFactory<OutWire, InWire>&&);
};

class OutWire : basic_Wire {
	friend WireFactory<OutWire, InWire>;
private:
	WireFactory<OutWire, InWire> factory;
public:
	OutWire(WireFactory<InWire, OutWire>);

	template<auto... t_args>
	decltype(factory)&& make_tethered(decltype(t_args)...)
	requires(std::is_constructible_v<InWire, decltype(t_args)...>);

	OutWire(WireFactory<InWire, OutWire>&&);
};
