#include "CK3ReligionScraping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::CK3ReligionScraping::CK3ReligionScraping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CK3ReligionScraping::registerKeys()
{
	registerKeyword("faiths", [this](std::istream& theStream) {
		faithParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	faithParser.registerRegex(commonItems::catchallRegex, [this](const std::string& name, std::istream& theStream) {
		commonItems::ignoreItem(name, theStream);
		faiths.insert(name);
	});
}
