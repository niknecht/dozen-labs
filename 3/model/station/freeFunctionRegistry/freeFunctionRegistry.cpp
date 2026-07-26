#include "freeFunctionRegistry.hpp"

#include <typeinfo>

size_t sub::pair_hash::operator()(const std::pair<size_t, size_t> v) const {
	return std::hash<size_t>()(v.first) 
	     ^ std::hash<size_t>()(v.second);
}

void sub::FreeFunctionRegistry::write(Key const addr, Val const val) {
	registry[addr] = val;
}

const sub::FreeFunctionRegistry::Val& sub::FreeFunctionRegistry::read(Key const addr) {
	return registry.find(addr)->second; // Check for nullptr
}

template<sub::is_Station LWord, sub::is_Req RWord>
sub::FreeFunctionRegistrar<LWord, RWord>::FreeFunctionRegistrar(sub::FreeFunctionRegistry& reg, void*& f) //:registrar_{reg} 
{
	reg.write({typeid(LWord).hash_code(), typeid(RWord).hash_code()}, f);
	//static Registrar registrar_{reg, f};
}

//template<sub::is_Station LWord, sub::is_Req RWord>
//sub::FreeFunctionRegistrar<LWord, RWord>::FreeFunctionRegistrar::Registrar::Registrar(sub::FreeFunctionRegistry& reg, void*& f) {
//}
