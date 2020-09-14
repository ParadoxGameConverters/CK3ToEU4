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

	[[nodiscard]] auto getMappings() { return std::move(mappings); }

  private:
	void registerKeys();
	
	std::vector<std::shared_ptr<ProvinceMapping>> mappings;
};
} // namespace mappers

#endif // PROVINCE_MAPPING_VERSION_H