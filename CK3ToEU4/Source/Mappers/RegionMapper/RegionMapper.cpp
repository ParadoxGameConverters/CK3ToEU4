#include "RegionMapper.h"
#include "../../Configuration/Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <filesystem>
namespace fs = std::filesystem;
#include <fstream>

void mappers::RegionMapper::loadRegions(const Configuration& theConfiguration)
{
	auto areaFilename = theConfiguration.getEU4Path() / "map/area.txt";
	auto regionFilename = theConfiguration.getEU4Path() / "map/region.txt";
	auto superRegionFilename = theConfiguration.getEU4Path() / "map/superregion.txt";

	std::ifstream areaStream(areaFilename);
	if (!areaStream.is_open())
		throw std::runtime_error("Could not open map/area.txt!");
	registerAreaKeys();
	parseStream(areaStream);
	clearRegisteredKeywords();
	areaStream.close();

	std::ifstream superRegionStream(superRegionFilename);
	if (!superRegionStream.is_open())
		throw std::runtime_error("Could not open map/superregion.txt!");
	registerSuperRegionKeys();
	parseStream(superRegionStream);
	clearRegisteredKeywords();
	superRegionStream.close();

	std::ifstream regionStream(regionFilename);
	if (!regionStream.is_open())
		throw std::runtime_error("Could not open map/region.txt!");
	registerRegionKeys();
	parseStream(regionStream);
	clearRegisteredKeywords();
	regionStream.close();

	linkSuperRegions();
	linkRegions();
}

void mappers::RegionMapper::loadRegions(std::istream& areaStream, std::istream& regionStream, std::istream& superRegionStream)
{
	registerAreaKeys();
	parseStream(areaStream);
	clearRegisteredKeywords();

	registerSuperRegionKeys();
	parseStream(superRegionStream);
	clearRegisteredKeywords();

	registerRegionKeys();
	parseStream(regionStream);
	clearRegisteredKeywords();

	linkSuperRegions();
	linkRegions();
}

void mappers::RegionMapper::registerAreaKeys()
{
	registerRegex(R"([\w_]+)", [this](const std::string& areaName, std::istream& theStream) {
		auto newArea = std::make_shared<Area>(theStream);
		areas.insert(std::make_pair(areaName, newArea));
	});
}

void mappers::RegionMapper::registerSuperRegionKeys()
{
	registerRegex(R"([\w_]+)", [this](const std::string& sregionName, std::istream& theStream) {
		const commonItems::stringList theList(theStream);
		std::vector<std::string> tempRegions;
		for (const auto& region: theList.getStrings())
		{
			if (region == "restrict_charter")
				continue;
			tempRegions.emplace_back(region);
		}
		auto newSRegion = std::make_shared<SuperRegion>(tempRegions);
		superRegions.insert(std::make_pair(sregionName, newSRegion));
	});
}

void mappers::RegionMapper::registerRegionKeys()
{
	registerRegex(R"([\w_]+)", [this](const std::string& regionName, std::istream& theStream) {
		auto newRegion = std::make_shared<Region>(theStream);
		regions.insert(std::pair(regionName, newRegion));
	});
}


bool mappers::RegionMapper::provinceIsInRegion(int province, const std::string& regionName) const
{
	const auto& regionItr = regions.find(regionName);
	if (regionItr != regions.end())
		return regionItr->second->regionContainsProvince(province);

	// "Regions" are such a fluid term.
	const auto& superRegionItr = superRegions.find(regionName);
	if (superRegionItr != superRegions.end())
		return superRegionItr->second->superRegionContainsProvince(province);

	// And sometimes they don't mean what people think they mean at all.
	const auto& areaItr = areas.find(regionName);
	if (areaItr != areas.end())
		return areaItr->second->areaContainsProvince(province);

	return false;
}

std::optional<std::string> mappers::RegionMapper::getParentAreaName(const int provinceID) const
{
	for (const auto& area: areas)
	{
		if (area.second->areaContainsProvince(provinceID))
			return area.first;
	}
	Log(LogLevel::Warning) << "Province ID " << provinceID << " has no parent area name!";
	return std::nullopt;
}

std::optional<std::string> mappers::RegionMapper::getParentRegionName(const int provinceID) const
{
	for (const auto& region: regions)
	{
		if (region.second->regionContainsProvince(provinceID))
			return region.first;
	}
	Log(LogLevel::Warning) << "Province ID " << provinceID << " has no parent region name!";
	return std::nullopt;
}

std::optional<std::string> mappers::RegionMapper::getParentSuperRegionName(const int provinceID) const
{
	for (const auto& superRegion: superRegions)
	{
		if (superRegion.second->superRegionContainsProvince(provinceID))
			return superRegion.first;
	}
	Log(LogLevel::Warning) << "Province ID " << provinceID << " has no parent superregion name!";
	return std::nullopt;
}

bool mappers::RegionMapper::regionNameIsValid(const std::string& regionName) const
{
	const auto& regionItr = regions.find(regionName);
	if (regionItr != regions.end())
		return true;

	// Who knows what the mapper needs. All kinds of stuff.
	const auto& superRegionItr = superRegions.find(regionName);
	if (superRegionItr != superRegions.end())
		return true;

	// And more stuff, what's the worst that could happen?
	const auto& areaItr = areas.find(regionName);
	if (areaItr != areas.end())
		return true;

	return false;
}

void mappers::RegionMapper::linkSuperRegions()
{
	for (const auto& superRegion: superRegions)
	{
		const auto& srsRegions = superRegion.second->getRegions();
		for (const auto& requiredRegion: srsRegions)
		{
			const auto& regionItr = regions.find(requiredRegion.first);
			if (regionItr != regions.end())
			{
				superRegion.second->linkRegion(std::pair(regionItr->first, regionItr->second));
			}
			else
			{
				throw std::runtime_error("Superregion's " + superRegion.first + " region " + requiredRegion.first + " does not exist!");
			}
		}
	}
}

void mappers::RegionMapper::linkRegions()
{
	for (const auto& region: regions)
	{
		const auto& rareas = region.second->getAreas();
		for (const auto& requiredArea: rareas)
		{
			const auto& areaItr = areas.find(requiredArea.first);
			if (areaItr != areas.end())
			{
				region.second->linkArea(std::pair(areaItr->first, areaItr->second));
			}
			else
			{
				throw std::runtime_error("Region's " + region.first + " area " + requiredArea.first + " does not exist!");
			}
		}
	}
}

void mappers::RegionMapper::linkProvinces(const std::map<int, std::shared_ptr<EU4::Province>>& theProvinces)
{
	for (const auto& area: areas)
	{
		const auto& rprovinces = area.second->getProvinces();
		for (const auto& requiredProvince: rprovinces)
		{
			const auto& provinceItr = theProvinces.find(requiredProvince.first);
			if (provinceItr != theProvinces.end())
			{
				area.second->linkProvince(std::pair(provinceItr->first, provinceItr->second));
			}
			else
			{
				throw std::runtime_error("Area's " + area.first + " area " + std::to_string(requiredProvince.first) + " does not exist!");
			}
		}
	}
}
