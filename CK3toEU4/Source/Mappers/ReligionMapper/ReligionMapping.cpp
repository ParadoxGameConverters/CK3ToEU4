#include "ReligionMapping.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ReligionMapping::ReligionMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapping::registerKeys()
{
	registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString religionString(theStream);
		eu4Religion = religionString.getString();
	});
	registerKeyword("ck3", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString religionString(theStream);
		ck3Religions.insert(religionString.getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
