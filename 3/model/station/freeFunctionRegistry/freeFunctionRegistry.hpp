#pragma once

#include <utility>
#include <unordered_map>
#include <typeindex>

#include "../../common/traits/stationTraits.hpp"
#include "../../common/traits/requirementTraits.hpp"

#include "freeFunctionRegistry.inl"

namespace sub {

class pair_hash {
public:
	size_t operator()(const std::pair<size_t, size_t>) const;
};

/*! Registry that stores ptrs to type erasure's free functions.
 *
 * @see registry
 * @see type erasure
 * @see runtime multi-dispatch
*/
class FreeFunctionRegistry {
	using Key = std::pair<std::size_t, std::size_t>;
	using Val = void*;
private:
	std::unordered_map<Key, Val, pair_hash> registry;
public:
	void write(Key const addr, Val const cont); //!< No safeguards because (a) this is all done automatically through types, and (b) the map will catch duplicates
	const Val& read(Key const addr); //!< No safeguards because everything is done automatically
};

/*! Helper class who's ctor binds a function to a register. Each callable in the station's interace is
 * supposed to have a static binder object of this type.
 *
 * Each station callable (that defines an interface) that wants to be registered is supposed to have a static variable of this helper template class.
 * The user is not supposed to add functions, only to add functionality on the existing framwork by pulling class specific
 * business logic into already defined existing interfaces. The user is not supposed to change the interface of a generic station
 * class. This tradeoff allows open sets of types on both ends, wich is where the extandability is planned.
 *
 * @see The hardest question in programming*/
template<sub::is_Station LWord, sub::is_Req RWord>
struct FreeFunctionRegistrar {
	constexpr FreeFunctionRegistrar(FreeFunctionRegistry& reg, void* fun);//!< Allows reusability without declaring seperate class for each registry
};

}

// ------------------------ Template definitions ---------------------------------

#include <typeinfo>

template<sub::is_Station LWord, sub::is_Req RWord>
constexpr sub::FreeFunctionRegistrar<LWord, RWord>::FreeFunctionRegistrar(sub::FreeFunctionRegistry& reg, void* f) //:registrar_{reg} 
{
	reg.write({typeid(LWord).hash_code(), typeid(RWord).hash_code()}, f);
	//static Registrar registrar_{reg, f};
}
