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
class basic_InWire;
class basic_OutWire;

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


	Wire& operator>>(Wire& that) noexcept;

	bool operator==(const Wire&);
	bool operator>(const Wire&);
	auto operator<=>(const Wire&);

	std::expected<float, std::string_view> time(const Wire&) noexcept;
};

class basic_Wire {
private:
	basic_Wire() noexcept(false);
	std::pair<float, float> uv; // uv [0;1][0;1]
	
	bool operator==(const basic_Wire&);
	bool operator>(const basic_Wire&);
	auto operator<=>(const basic_Wire&);
};

class basic_InWire : public basic_Wire {
private:
	basic_InWire(std::pair<float, float>) noexcept(false);
};

class basic_OutWire : public basic_Wire{
private:
	basic_OutWire(std::pair<float, float>) noexcept(false);
};
