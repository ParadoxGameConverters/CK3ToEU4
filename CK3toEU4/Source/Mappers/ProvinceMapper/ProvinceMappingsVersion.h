#ifndef PROVINCE_MAPPING_VERSION_H
#define PROVINCE_MAPPING_VERSION_H

#include "ProvinceMapping.h"
#include "Parser.h"

namespace mappers
{
class ProvinceMappingsVersion: commonItems::parser
{
  public:
	ProvinceMappingsVersion() = default;
	explicit ProvinceMappingsVersion(std::istream& theStream);

	[[nodiscard]] const auto& getMappings() const { return mappings; }

  private:
	std::vector<ProvinceMapping> mappings;
};
} // namespace mappers

#endif // PROVINCE_MAPPING_VERSION_H