#include "requirement.hpp"

bool sub::Requirement::operator<(const Requirement& other) const noexcept {
	return this->kind() < other.kind() || this->source < other.source;
}

bool sub::Requirement::operator==(const Requirement& other) const noexcept {
	return this->kind() == other.kind() && this->source == other.source;
}
