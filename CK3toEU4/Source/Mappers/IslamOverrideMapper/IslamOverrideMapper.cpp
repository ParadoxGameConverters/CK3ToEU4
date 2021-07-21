#include "IslamOverrideMapper.h"
#include "CommonRegexes.h"
#include "IslamOverrideMapping.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::IslamOverrideMapper::IslamOverrideMapper()
{
	Log(LogLevel::Info) << "-> Parsing religious override mappings.";
	registerKeys();
	parseFile("configurables/islam_override.txt");
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << replacements.size() << " replacement links.";
}

mappers::IslamOverrideMapper::IslamOverrideMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IslamOverrideMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const IslamOverrideMapping newMapping(theStream);
		if (newMapping.getReplacementMapping().first && !newMapping.getReplacementMapping().second.empty())
			replacements.emplace(newMapping.getReplacementMapping());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::IslamOverrideMapper::getReplacementReligionForProvince(int provinceID) const
{
	const auto& religion = replacements.find(provinceID);
	if (religion == replacements.end())
		return std::nullopt;
	else
		return religion->second;
}
