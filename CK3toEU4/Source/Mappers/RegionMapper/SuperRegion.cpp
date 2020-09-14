#include "SuperRegion.h"
#include "Region.h"

mappers::SuperRegion::SuperRegion(const std::vector<std::string>& theRegions)
{
	for (const auto& region: theRegions)
	{
		regions.insert(std::pair(region, nullptr));
	}
}

bool mappers::SuperRegion::superRegionContainsProvince(const int province) const
{
	for (const auto& region: regions)
		if (region.second->regionContainsProvince(province))
			return true;
	return false;
}
