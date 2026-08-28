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
