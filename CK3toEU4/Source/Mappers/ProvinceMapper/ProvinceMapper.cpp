#include "ProvinceMapper.h"
#include "../../Configuration/Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProvinceMapping.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

mappers::ProvinceMapper::ProvinceMapper()
{
	LOG(LogLevel::Info) << "-> Parsing province mappings";
	registerKeys();
	parseFile("configurables/province_mappings.txt");
	clearRegisteredKeywords();
	createMappings();
	LOG(LogLevel::Info) << "<> " << theMappings.getMappings().size() << " mappings loaded.";
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
		theMappings = ProvinceMappingsVersion(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::ProvinceMapper::createMappings()
{
	for (const auto& mapping: theMappings.getMappings())
	{
		// fix deliberate errors where we leave mappings without keys (asian wasteland comes to mind):
		if (mapping.getCK3Provinces().empty())
			continue;
		if (mapping.getEU4Provinces().empty())
			continue;

		for (const auto& ck3Number: mapping.getCK3Provinces())
		{
			if (ck3Number)
				CK3ToEU4ProvinceMap.insert(std::make_pair(ck3Number, mapping.getEU4Provinces()));
		}
		for (const auto& eu4Number: mapping.getEU4Provinces())
		{
			if (eu4Number)
				EU4ToCK3ProvinceMap.insert(std::make_pair(eu4Number, mapping.getCK3Provinces()));
		}
	}
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