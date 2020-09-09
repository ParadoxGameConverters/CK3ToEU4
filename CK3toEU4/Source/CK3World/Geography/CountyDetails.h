#ifndef CK3_COUNTYDETAILS_H
#define CK3_COUNTYDETAILS_H
#include "Parser.h"

namespace CK3
{
class CountyDetail;
class CountyDetails: commonItems::parser
{
  public:
	CountyDetails() = default;
	explicit CountyDetails(std::istream& theStream);

	[[nodiscard]] const auto& getCountyDetails() const { return countyDetails; }

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<CountyDetail>> countyDetails;
};
} // namespace CK3

#endif // CK3_COUNTYDETAILS_H
