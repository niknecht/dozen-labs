#pragma once

namespace sub {
class StationConcept;

class Requirement;
}

#include <concepts>

namespace sub {

/*! The main concept defining what a requirement class is.*/
template<typename R>
concept is_Req = (requires(const R& t, const Requirement& b, StationConcept& s, const StationConcept& sc) {
	{t <=> b} -> std::convertible_to<bool>;
	{t <=> t} -> std::convertible_to<bool>;
	{b <=> t} -> std::convertible_to<bool>;
	//{t.dispatchTryFix(s)} -> std::convertible_to<bool>;
	//{t.dispatchVerify(sc)} -> std::convertible_to<bool>;
} && std::is_base_of_v<Requirement, R>);

}
