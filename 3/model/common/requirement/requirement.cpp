#include "requirement.hpp"

bool sub::Requirement::operator<(const Requirement& other) const noexcept {
	return this->priority() < other.priority() || this->source < other.source;
}

bool sub::Requirement::operator==(const Requirement& other) const noexcept {
	return this->priority() == other.priority() && this->source == other.source;
}

sub::Requirement::Requirement(std::string_view src) :source{src}
{}
