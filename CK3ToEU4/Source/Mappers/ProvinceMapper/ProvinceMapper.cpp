#include "ProvinceMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProvinceMapping.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
#include "../../CK3World/Titles/LandedTitles.h"
#include "../../CK3World/Titles/Title.h"
#include "CommonRegexes.h"

mappers::ProvinceMapper::ProvinceMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	createMappings();
}

mappers::ProvinceMapper::ProvinceMapper(const std::filesystem::path& fileName)
{
	Log(LogLevel::Info) << "-> Parsing province mappings";
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
	createMappings();
	Log(LogLevel::Info) << "<> " << mappings.size() << " mappings loaded.";
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
				Log(LogLevel::Error) << "Province mapper error - requested ck3 province ID " << std::to_string(ck3ID)
											<< " is invalid. We cannot find related county.";
			}
		}
	}
	else
	{
		// We must not show fear here. Parent is likely iterating over all EU4 provinces. Play dead, they will go away.
	}
	return toReturn;
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
	else if (ck3CountyName.contains("c_nf_"))
	{
		// Nothing, false alarm, junk dynamic title.
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
		if (title.second->getLevel() == CK3::LEVEL::BARONY)
		{
			if (!title.second->getClay() || !title.second->getClay()->getProvince())
			{
				Log(LogLevel::Error) << "Barony " + title.first + " has no province data and is unmappable!";
				continue;
			}
			// This is indeed a barony. Is it broken?
			if (!title.second->getDJLiege())
			{
				Log(LogLevel::Error) << "Barony " + title.first + " has no county and is unmappable!";
				continue;
			}
			auto baronyID = title.second->getClay()->getProvince()->first;
			auto countyTitle = std::pair(title.second->getDJLiege()->second->getName(), title.second->getDJLiege()->second);
			// Is the county sane?
			if (countyTitle.second->getLevel() != CK3::LEVEL::COUNTY || !countyTitle.second->getClay() || !countyTitle.second->getClay()->getCounty())
			{
				Log(LogLevel::Error) << "Barony " + title.first + " has an insane county and is unmappable!";
				continue;
			}
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
				Log(LogLevel::Error) << "Province mapping ck3 = " + std::to_string(ck3ID) + " is invalid. We cannot find related county.";
			}
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " mappings validated.";
}
