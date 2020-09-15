#include "ReligionMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ReligionMapping.h"

mappers::ReligionMapper::ReligionMapper()
{
	LOG(LogLevel::Info) << "-> Parsing religion mappings.";
	registerKeys();
	parseFile("configurables/religion_map.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> Loaded " << ck3Toeu4ReligionMap.size() << " religious links.";
}

mappers::ReligionMapper::ReligionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const ReligionMapping theMapping(theStream);
		for (const auto& ck3Religion: theMapping.getCK3Religions())
		{
			ck3Toeu4ReligionMap.insert(std::make_pair(ck3Religion, theMapping.getEU4Religion()));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ReligionMapper::getEU4ReligionForCK3Religion(const std::string& ck3Religion) const
{
	const auto& mapping = ck3Toeu4ReligionMap.find(ck3Religion);
	if (mapping != ck3Toeu4ReligionMap.end())
		return mapping->second;
	return std::nullopt;
}