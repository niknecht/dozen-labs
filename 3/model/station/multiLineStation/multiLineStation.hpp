#include "../stationCRTP.hpp"
#include "../../common/traits/stationTraits.hpp"

namespace sub {

/*! Station class that represents a station that can connect up to 3 lines (checked in the ctor)*/
class MultiLineStation :public StationCRTP<MultiLineStation>{
private:
	std::vector<std::string> lines_;
public:
	std::vector<std::unique_ptr<sub::Requirement>> req() const;
	std::vector<std::string_view> lines() const;

	MultiLineStation(const std::string_view name, const std::vector<std::string_view>& lines); //!< Could be vector of views
};
static_assert(is_Station<MultiLineStation>);

}
