#include "requirement.hpp"

bool sub::Requirement::operator<(const Requirement& other) const noexcept {
	return this->priority() < other.priority() || this->source_ < other.source_;
}

bool sub::Requirement::operator==(const Requirement& other) const noexcept {
	return this->priority() == other.priority() && this->source_ == other.source_;
}

sub::Requirement::Requirement(std::string_view src) :source_{src}
{}

std::string_view sub::Requirement::source() const noexcept {
	return source_;
}
