#ifndef EU4_SUPERREGION_H
#define EU4_SUPERREGION_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace mappers
{
class Region;
class SuperRegion
{
  public:
	explicit SuperRegion(const std::vector<std::string>& theRegions);

	[[nodiscard]] const auto& getRegions() const { return regions; }
	[[nodiscard]] bool superRegionContainsProvince(int province) const;

	void linkRegion(const std::pair<std::string, std::shared_ptr<Region>>& theRegion) { regions[theRegion.first] = theRegion.second; }

  private:
	std::map<std::string, std::shared_ptr<Region>> regions;
};
} // namespace mappers

#endif // EU4_SUPERREGION_H