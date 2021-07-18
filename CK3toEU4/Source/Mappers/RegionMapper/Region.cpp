#include "Region.h"
#include "Area.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::Region::Region(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::Region::registerKeys()
{
	registerKeyword("areas", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList names(theStream);
		for (const auto& name: names.getStrings())
			areas.insert(std::pair(name, nullptr));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::Region::regionContainsProvince(int province) const
{
	for (const auto& area: areas)
		if (area.second->areaContainsProvince(province))
			return true;
	return false;
}
