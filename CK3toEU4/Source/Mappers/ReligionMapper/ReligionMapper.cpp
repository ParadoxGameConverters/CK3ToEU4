#include "ReligionMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ReligionMapping.h"
#include "../../CK3World/Religions/Faiths.h"
#include "../../CK3World/Religions/Faith.h"
#include "../LocalizationMapper/LocalizationMapper.h"

mappers::ReligionMapper::ReligionMapper()
{
	LOG(LogLevel::Info) << "-> Parsing religion mappings.";
	registerKeys();
	parseFile("configurables/religion_map.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> Loaded " << CK3toEU4ReligionMap.size() << " religious links.";
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
			CK3toEU4ReligionMap.insert(std::make_pair(ck3Religion, theMapping.getEU4Religion()));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ReligionMapper::getEU4ReligionForCK3Religion(const std::string& ck3Religion) const
{
	const auto& mapping = CK3toEU4ReligionMap.find(ck3Religion);
	if (mapping != CK3toEU4ReligionMap.end())
		return mapping->second;
	return std::nullopt;
}

void mappers::ReligionMapper::importCK3Faiths(const CK3::Faiths& faiths)
{
	for (const auto& faith: faiths.getFaiths())
	{
		if (!getEU4ReligionForCK3Religion(faith.second->getName()))
		{
			// This is a new faith.
			importCK3Faith(*faith.second);
		}
	}
}

void mappers::ReligionMapper::importCK3Faith(const CK3::Faith& faith)
{
	// Hello, imported CK3 dynamic faith.
	const auto faithName = "converted_" + faith.getName(); // makes them easier to notice
	const auto displayName = faith.getCustomAdj(); // Catholic, not catholicism
	LocBlock locBlock;
	locBlock.english = displayName;
	locBlock.french = displayName;
	locBlock.german = displayName;
	locBlock.spanish = displayName; // Ck3 save only stores the one display name, so we have no choice but to copy it around.

	
	
}
