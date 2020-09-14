#ifndef EU4_REGIONS_H
#define EU4_REGIONS_H

#include "Area.h"
#include "Parser.h"
#include "Region.h"
#include "SuperRegion.h"
#include <map>

class Configuration;
namespace mappers
{
class RegionMapper: commonItems::parser
{
  public:
	RegionMapper() = default;
	virtual ~RegionMapper() = default;
	RegionMapper(const RegionMapper&) = default;
	RegionMapper& operator=(const RegionMapper&) = default;
	RegionMapper(RegionMapper&&) = default;
	RegionMapper& operator=(RegionMapper&&) = default;

	void loadRegions(const Configuration& theConfiguration);
	void loadRegions(std::istream& areaStream, std::istream& regionStream, std::istream& superRegionStream); // for testing

	[[nodiscard]] bool provinceIsInRegion(int province, const std::string& regionName) const;
	[[nodiscard]] bool regionNameIsValid(const std::string& regionName) const;

	[[nodiscard]] std::optional<std::string> getParentAreaName(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getParentRegionName(int provinceID) const;
	[[nodiscard]] std::optional<std::string> getParentSuperRegionName(int provinceID) const;

	void linkProvinces(const std::map<int, std::shared_ptr<EU4::Province>>& theProvinces);

  private:
	void registerAreaKeys();
	void registerSuperRegionKeys();
	void registerRegionKeys();
	void linkSuperRegions();
	void linkRegions();

	std::map<std::string, std::shared_ptr<Region>> regions;
	std::map<std::string, std::shared_ptr<SuperRegion>> superRegions;
	std::map<std::string, std::shared_ptr<Area>> areas;
};
} // namespace mappers

#endif // EU4_REGIONS_H
