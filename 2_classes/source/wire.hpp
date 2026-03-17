#include <utility>
#include <optional>
#include <expected>
#include <string_view>
#include <memory>
#include <type_traits>

#pragma once

constexpr static decltype(auto) g_signalSpeed {5lu};

template<class> class Wire;
class basic_Wire;
using basic_InWire = basic_Wire; // This means all the Wire type checks are now
using basic_OutWire = basic_Wire; //done by the built-in compile-time type checking in C++.
				//  It's impossible to mismatch wire directions no manual checking needed
template<class Direction>
class Wire {
private:
	static_assert(std::is_same_v<Direction, basic_InWire> || std::is_same_v<Direction, basic_OutWire>);
	using OppositeDirection = std::conditional<std::is_same_v<Direction, basic_InWire>, basic_OutWire, basic_InWire>;
	const Direction it;
	std::optional<std::unique_ptr<OppositeDirection>> connected;
public:
	Wire() = delete;
	Wire(const Wire&) = default;
	Wire& operator=(const Wire&);

	Wire(std::pair<float, float> uv) noexcept(false);
	
	~Wire();


	Wire& operator>>(Wire<OppositeDirection>& that) noexcept;

	bool operator==(const Wire&) const;
	bool operator>(const Wire&) const;
	auto operator<=>(const Wire&) const;

	std::expected<float, std::string_view> time(const Wire&) const noexcept;
	std::expected<void, std::string_view> make_connect(const std::pair<float, float>); // Makes unique
};

class basic_Wire {
private:
	basic_Wire(const std::pair<float, float>) noexcept(false);
	std::pair<float, float> uv; // uv [0;1][0;1]
	
	bool operator==(const basic_Wire&) const;
	bool operator>(const basic_Wire&) const;
	auto operator<=>(const basic_Wire&) const;
};
