#ifndef PROVINCE_MAPPING_H
#define PROVINCE_MAPPING_H

#include "Parser.h"

namespace mappers
{
class ProvinceMapping: commonItems::parser
{
  public:
	ProvinceMapping() = default;
	explicit ProvinceMapping(std::istream& theStream);

	[[nodiscard]] const auto& getEU4Provinces() const { return eu4Provinces; }
	[[nodiscard]] const auto& getCK3Provinces() const { return ck3Provinces; }

  private:
	void registerKeys();

	std::vector<int> eu4Provinces;
	std::vector<int> ck3Provinces;
};
} // namespace mappers

#endif // PROVINCE_MAPPING_H