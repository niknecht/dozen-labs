#include "wire.hpp"

#include <exception>
#include <cmath>

template <class Direction> Wire<Direction>::Wire(std::pair<float, float> uv) noexcept(false) : it(uv){};

template <Direction_t D>
basic_Wire<D>::basic_Wire(std::pair<float, float> uv) noexcept(false) 
	: 
		uv(
			[uv]()->std::expected<std::pair<float, float>, std::exception>{
				if (uv < std::pair(0.f, 0.f || uv > std::pair(1.f, 1.f))) 
					return uv; 
				else 
					return std::unexpected(std::out_of_range("uv is out of range"));
			}()
			.or_else([](const std::exception& err)->std::expected<std::pair<float, float>, std::exception>
			{
				throw err;
				return std::unexpected(err);
			})
			.value()
		) {}

template<class Direction> Wire<Direction>& Wire<Direction>::operator=(const Wire<Direction>& other) 
{
	it = other.it;
	connected = std::make_unique<OppositeDirection>(connected.get());
	return *this;
}
template<class Direction> Wire<Direction>::Wire(const Wire<Direction>& other)
	:
		connected(std::make_unique<OppositeDirection>(other.connected.get())),
		it(other.it){}

template<Direction_t D>
bool basic_Wire<D>::operator>(const basic_Wire& that) const {
	return this->uv.first*this->uv.first + this->uv.second*this->uv.second 
		> that.uv.first*that.uv.first + that.uv.second*that.uv.second;
}
template<Direction_t D>
bool basic_Wire<D>::operator==(const basic_Wire& that) const {
	return this->uv == that.uv;
}

template <class Direction> bool Wire<Direction>::operator>(const Wire& other) const {
	return it > other;
}
template <class Direction> bool Wire<Direction>::operator==(const Wire& other) const{
	return it == other;
}

template <class Direction>
std::expected<Wire<typename Wire<Direction>::OppositeDirection>&, std::string_view> 
	Wire<Direction>::operator>>(Wire<OppositeDirection>& that) noexcept
{
	this->connected.reset(std::make_unique<Wire<OppositeDirection>>(&that));
	that.connected.reset(std::make_unique<Wire<OppositeDirection>>(this->connected.get()));
	return that;
}

template <class Direction>
std::expected<float, std::string_view> Wire<Direction>::basic_time_to(const Wire<Direction>&) const noexcept
{
	static_assert(g_signalSpeed > 0.f);
	if(!this->connected.has_value())
		return std::unexpected("");
	return std::sqrt(this->it.uv.first * this->it.uv.first + this->*connected.uv.second * this->*connected.uv.second) / g_signalSpeed;
}

template <class Direction>
std::expected<Wire<typename Wire<Direction>::OppositeDirection>&, std::string_view> 
	Wire<Direction>::make_connect(const std::pair<float, float> newUv) 
{
	if(this->connected.has_value())
		return std::unexpected("Error: One-sided connection change not permitted.");
	return (this->connected.reset(std::make_unique<Wire<OppositeDirection>>(newUv)));
}
