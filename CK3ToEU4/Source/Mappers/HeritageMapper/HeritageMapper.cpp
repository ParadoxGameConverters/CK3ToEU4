#include "HeritageMapper.h"
#include "CommonRegexes.h"
#include "HeritageMapping.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::HeritageMapper::loadHeritagesFromDisk()
{
	Log(LogLevel::Info) << "-> Parsing Heritages";
	registerKeys();
	parseFile(std::filesystem::path("configurables/heritage_map.txt"));
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << heritagesToCultures.size() << " relations.";
}

mappers::HeritageMapper::HeritageMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::HeritageMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const HeritageMapping theMapping(theStream);
		if (!theMapping.getEU4CultureGroup().empty() && !theMapping.getCK3Heritages().empty())
			for (const auto& heritage: theMapping.getCK3Heritages())
				heritagesToCultures.emplace(heritage, theMapping.getEU4CultureGroup());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::HeritageMapper::getCultureGroupForHeritage(const std::string& heritage) const
{
	if (heritagesToCultures.contains(heritage))
		return heritagesToCultures.at(heritage);

	Log(LogLevel::Warning) << "Unmapped heritage for culture group: " << heritage;
	return std::nullopt;
}
