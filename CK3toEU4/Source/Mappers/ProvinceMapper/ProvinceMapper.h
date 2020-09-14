#ifndef PROVINCE_MAPPER_H
#define PROVINCE_MAPPER_H

#include "ProvinceMappingsVersion.h"
#include "Parser.h"
#include <map>


namespace CK3
{
class Title;
}

namespace mappers
{
class ProvinceMapper: commonItems::parser
{
  public:
	ProvinceMapper();
	explicit ProvinceMapper(std::istream& theStream);

	[[nodiscard]] const std::map<std::string, std::shared_ptr<CK3::Title>>& getCK3Titles(int eu4ProvinceNumber) const;
	[[nodiscard]] std::vector<int> getEU4ProvinceNumbers(const std::string& ck3CountyName) const;
	[[nodiscard]] std::vector<int> getCK3ProvinceNumbers(int eu4ProvinceNumber) const; // for load testing
	[[nodiscard]] std::vector<int> getEU4ProvinceNumbers(int ck3ProvinceNumber) const; // for load testing

	void transliterateMappings(std::map<std::string, std::shared_ptr<CK3::Title>> titles);

  private:
	void registerKeys();
	void createMappings();

	std::map<int, std::vector<int>> CK3ToEU4ProvinceMap;
	std::map<int, std::vector<int>> EU4ToCK3ProvinceMap;
	std::vector<std::shared_ptr<ProvinceMapping>> mappings;
	std::map<int, std::pair<std::string, std::shared_ptr<CK3::Title>>> baroniesToCounties;
	std::map<std::string, int> countiesToBaronies;
};
} // namespace mappers

#endif // PROVINCE_MAPPER_H