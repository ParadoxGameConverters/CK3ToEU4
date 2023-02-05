#ifndef AFRICAN_PASSES_MAPPING
#define AFRICAN_PASSES_MAPPING
#include "ConvenientParser.h"

namespace mappers
{
class AfricanPassesMapping: commonItems::convenientParser
{
  public:
	AfricanPassesMapping() = default;
	explicit AfricanPassesMapping(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getEndA() const { return endA; }
	[[nodiscard]] const auto& getEndB() const { return endB; }
	[[nodiscard]] const auto& getSterilizeProvinces() const { return sterilizeProvinces; }

  private:
	void registerKeys();

	std::string name;
	std::vector<int> endA;
	std::vector<int> endB;
	std::vector<int> sterilizeProvinces;
};
} // namespace mappers

#endif // AFRICAN_PASSES_MAPPING