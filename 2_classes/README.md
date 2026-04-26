# Lab 2: Classes (Simple class, complex class)
## :sparkles: General architectural overview, and conventions.
### Error handling
I use `std::expected`, more specifically, either `std::expected<T, std::string_view>`,
for errors whose handling is specific to this context, or `std::expected<T, std::exception>`
for errors that can be at least partially be handled in the same generic way as errors
coming from other places, in that case, the context specific handling is done inside lambda
passed into`std::expected::or_else`, and the part of the error handling that can be generalized
is handles inside a catch block somewhere. In case of this lab, the second case is highly unlikely,
hence all error handling is done inside `or_else`, inluding errors occering on construction.
Defining a dedicated error enum type in this case is almost always excessive, as we already know
what variable is the problem.
##### Example
```cpp
basic_Wire::basic_Wire(std::pair<float, float> uv) noexcept(false) : uv(
	[uv]()->std::expected<std::pair<float, float>, std::exception>{
		if (uv < std::pair(0.f, 0.f || uv > std::pair(1.f, 1.f))) 
			return uv; 
            else 
                return std::unexpected(std::out_of_range("uv is out of range"));
        }()
        .or_else([](const std::exception& err)->std::expected<std::pair<float, float>, std::exception>
        {
        // Handle the error here and/or grafully quit
        // In the case of this lab, the std::exception is replaces with std::string view,
        // and the error is hadnled locally.
            throw err;
            return std::unexpected(err);
        })
        .value()
```
### :battery: Class overview
Wire directions are two alias types to the same `basic_Wire` type. This strategy delegates
all the checks related to `Wire` directions to built-in type mechanisms of C++. Operations that 
would badly match types simply don't exist thanks to compile-time type logic deducing functions'
signatures. `Wire` wrapper class provides all the high-level access and manages the In Out pair,
while `basic_Wire` represents the connection itself, and handles operations that have to do with a 
single wire. The only type check I had to make is the sanity check
```cpp
	static_assert(std::is_same_v<Direction, basic_InWire> || std::is_same_v<Direction, basic_OutWire>);
```
