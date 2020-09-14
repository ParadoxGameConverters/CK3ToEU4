#include "ProvinceMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProvinceMapping.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
#include "../../CK3World/Geography/ProvinceHolding.h"
#include "../../CK3World/Titles/LandedTitles.h"
#include "../../CK3World/Titles/Title.h"

mappers::ProvinceMapper::ProvinceMapper()
{
	LOG(LogLevel::Info) << "-> Parsing province mappings";
	registerKeys();
	parseFile("configurables/province_mappings.txt");
	clearRegisteredKeywords();
	createMappings();
	LOG(LogLevel::Info) << "<> " << mappings.size() << " mappings loaded.";
}

mappers::ProvinceMapper::ProvinceMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	createMappings();
}

void mappers::ProvinceMapper::registerKeys()
{
	registerRegex(R"([0-9\.]+)", [this](const std::string& unused, std::istream& theStream) {
		// We support only a single, current version, so eu4-vic2 style multiple versions
		// have been cut. There should only be a single, 0.0.0.0={} block inside province_mappings.txt
		if (mappings.empty())
		{
			mappings = ProvinceMappingsVersion(theStream).getMappings();
		}
		else
		{
			commonItems::ignoreItem(unused, theStream);
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::ProvinceMapper::createMappings()
{
	for (const auto& mapping: mappings)
	{
		// fix deliberate errors where we leave mappings without keys (asian wasteland comes to mind):
		if (mapping->getCK3Provinces().empty())
			continue;
		if (mapping->getEU4Provinces().empty())
			continue;

		for (auto ck3Number: mapping->getCK3Provinces())
		{
			if (ck3Number)
				CK3ToEU4ProvinceMap.insert(std::make_pair(ck3Number, mapping->getEU4Provinces()));
		}
		for (const auto& eu4Number: mapping->getEU4Provinces())
		{
			if (eu4Number)
				EU4ToCK3ProvinceMap.insert(std::make_pair(eu4Number, mapping->getCK3Provinces()));
		}
	}
}

std::map<std::string, std::shared_ptr<CK3::Title>> mappers::ProvinceMapper::getCK3Titles(int eu4ProvinceNumber) const
{
	const auto& mapping = EU4ToCK3ProvinceMap.find(eu4ProvinceNumber);
	std::map<std::string, std::shared_ptr<CK3::Title>> toReturn;
	if (mapping != EU4ToCK3ProvinceMap.end())
	{
		for (auto ck3ID: mapping->second)
		{
			if (baroniesToCounties.count(ck3ID))
			{
				auto targetTitle = baroniesToCounties.find(ck3ID)->second;
				toReturn.insert(targetTitle);
			}
			else
			{
				throw std::runtime_error(
					 "Province mapper error - requested ck3 province ID " + std::to_string(ck3ID) + " is invalid. We cannot find related county. Farewell.");
			}
		}
		return toReturn;
	}
	else
	{
		throw std::runtime_error("Province mapper error - requested ck3 province ID does not exist in mappings.");
	}
}

std::vector<int> mappers::ProvinceMapper::getEU4ProvinceNumbers(const std::string& ck3CountyName) const
{
	const auto& mapping = countiesToBaronies.find(ck3CountyName);
	if (mapping != countiesToBaronies.end())
	{
		const auto& remapping = CK3ToEU4ProvinceMap.find(mapping->second);
		if (remapping != CK3ToEU4ProvinceMap.end())
		{
			return remapping->second;
		}
		else
		{
			Log(LogLevel::Warning) << "Province Mapper mismatch - " << ck3CountyName << " with baronyID " << mapping->second << " has no mappings!";
		}
	}
	else
	{
		Log(LogLevel::Warning) << "Province Mapper mismatch - " << ck3CountyName << " has no mappings!";
	}
	return std::vector<int>();
}

void mappers::ProvinceMapper::transliterateMappings(std::map<std::string, std::shared_ptr<CK3::Title>> titles)
{
	// CK3 baronies are worthless. We need to generate a translation matrix to counties.
	// We can also use this opportunity to do a thorough check on sanity of both mappings and target counties.

	std::map<int, std::pair<std::string, std::shared_ptr<CK3::Title>>> theMatrix; // baronyID, county

	for (const auto& title: titles)
	{
		if (title.second->getLevel() == CK3::LEVEL::BARONY && title.second->getClay() && title.second->getClay()->getProvince())
		{
			// This is indeed a barony. Is it broken?
			if (!title.second->getDJLiege())
				throw std::runtime_error("Barony " + title.first + " has no county and is unmappable!");
			auto baronyID = title.second->getClay()->getProvince()->first;
			auto countyTitle = std::pair(title.second->getDJLiege()->second->getName(), title.second->getDJLiege()->second);
			// Is the county sane?
			if (countyTitle.second->getLevel() != CK3::LEVEL::COUNTY || !countyTitle.second->getClay() || !countyTitle.second->getClay()->getCounty())
				throw std::runtime_error("Barony " + title.first + " has an insane county and is unmappable!");
			theMatrix.insert(std::pair(baronyID, countyTitle));
		}
	}

	auto counter = 0;
	// Now roll through our mappings and save what's needed.
	for (const auto& mapping: mappings)
	{
		for (auto ck3ID: mapping->getCK3Provinces())
		{
			if (theMatrix.count(ck3ID))
			{
				// This is a sane barony and we know of its troubles.
				baroniesToCounties.insert(std::pair(ck3ID, theMatrix[ck3ID]));
				countiesToBaronies.insert(std::pair(theMatrix[ck3ID].first, ck3ID));
				++counter;
			}
			else
			{
				// We're mapping to something unheard of. Let's not.
				throw std::runtime_error("Province mapping ck3 = " + std::to_string(ck3ID) + " is invalid. We cannot find related county. Farewell.");
			}
		}
	}
	LOG(LogLevel::Info) << "<> " << counter << " mappings validated.";
}

std::vector<int> mappers::ProvinceMapper::getCK3ProvinceNumbers(const int eu4ProvinceNumber) const
{
	const auto& mapping = EU4ToCK3ProvinceMap.find(eu4ProvinceNumber);
	if (mapping != EU4ToCK3ProvinceMap.end())
		return mapping->second;
	return std::vector<int>();
}

std::vector<int> mappers::ProvinceMapper::getEU4ProvinceNumbers(int ck3ProvinceNumber) const
{
	const auto& mapping = CK3ToEU4ProvinceMap.find(ck3ProvinceNumber);
	if (mapping != CK3ToEU4ProvinceMap.end())
		return mapping->second;
	return std::vector<int>();
}
