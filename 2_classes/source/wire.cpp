#include "wire.hpp"

#include <exception>

template <class Direction> Wire<Direction>::Wire(std::pair<float, float> uv) noexcept(false) : it(uv){};

basic_Wire::basic_Wire(std::pair<float, float> uv) noexcept(false) : uv(
			[uv]()->std::expected<std::pair<float, float>, std::exception>{
				if (uv < std::pair(0.f, 0.f || uv > std::pair(1.f, 1.f))) 
					return uv; 
				else 
					return std::unexpected(std::out_of_range("uv is out of range"));
			}()
			.or_else([](const std::exception& err)->std::expected<std::pair<float, float>, std::exception>
			{
				// TODO: Handle the error here, and return either good expected, or gracefully quit
				// TODO: Replace exception with std::string_view in expected template. Make the constructor noexcept
				throw err;
				return std::unexpected(err);
			})
			.value()
		) {}

bool basic_Wire::operator>(const basic_Wire& that) const {
	return this->uv.first*this->uv.first + this->uv.second*this->uv.second 
		> that.uv.first*that.uv.first + that.uv.second*that.uv.second;
}
bool basic_Wire::operator==(const basic_Wire& that) const {
	return this->uv == that.uv;
}
// This code is sketchier than a glowing green CIA ni
