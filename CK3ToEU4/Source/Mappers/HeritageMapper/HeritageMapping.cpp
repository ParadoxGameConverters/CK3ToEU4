#include "HeritageMapping.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::HeritageMapping::HeritageMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::HeritageMapping::registerKeys()
{
	registerKeyword("eu4", [this](std::istream& theStream) {
		eu4CultureGroup = commonItems::getString(theStream);
	});
	registerKeyword("ck3", [this](std::istream& theStream) {
		ck3Heritages.insert(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
