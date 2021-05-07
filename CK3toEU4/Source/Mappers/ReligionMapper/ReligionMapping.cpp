#include "ReligionMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ReligionMapping::ReligionMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapping::registerKeys()
{
	registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
		eu4Religion = commonItems::getString(theStream);
	});
	registerKeyword("ck3", [this](const std::string& unused, std::istream& theStream) {
		ck3Religions.insert(commonItems::getString(theStream));
	});
	registerKeyword("school", [this](const std::string& unused, std::istream& theStream) {
		eu4School = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
