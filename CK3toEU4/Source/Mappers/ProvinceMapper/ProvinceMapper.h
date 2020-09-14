#ifndef PROVINCE_MAPPER_H
#define PROVINCE_MAPPER_H

#include "ProvinceMappingsVersion.h"
#include "Parser.h"
#include <map>
#include <set>

class Configuration;

namespace mappers
{
class ProvinceMapper: commonItems::parser
{
  public:
	ProvinceMapper();
	explicit ProvinceMapper(std::istream& theStream);

	[[nodiscard]] std::vector<int> getCK3ProvinceNumbers(int eu4ProvinceNumber) const;
	[[nodiscard]] std::vector<int> getEU4ProvinceNumbers(int ck3ProvinceNumber) const;

  private:
	void registerKeys();
	void createMappings();

	std::map<int, std::vector<int>> CK3ToEU4ProvinceMap;
	std::map<int, std::vector<int>> EU4ToCK3ProvinceMap;
	ProvinceMappingsVersion theMappings;
};
} // namespace mappers

#endif // PROVINCE_MAPPER_H