#include <typeindex>
#include <utility>
#include <optional>
#include <expected>
#include <string_view>
#include <memory>
#include <type_traits>


#pragma once


template<class> class Wire;
class basic_InWire;
class basic_OutWire;

template<class Direction>
class Wire {
private:
	using OppositeDirection = std::conditional<std::is_same_v<Direction, basic_InWire>, basic_OutWire, basic_InWire>;
	const Direction it;
	std::optional<OppositeDirection> connected;
public:
	Wire() = delete;
	Wire(const Wire&) = default;

	Wire(std::pair<float, float>);
	
	~Wire();


	Wire& operator>>(Wire&);
};

class basic_Wire {
private:
	basic_Wire();
	std::pair<float, float> uv; // uv [0;1][0;1]
};

class basic_InWire : basic_Wire {
private:
	basic_InWire(std::pair<float, float>);
};

class basic_OutWire{
private:
	basic_OutWire(std::pair<float, float>);
};
