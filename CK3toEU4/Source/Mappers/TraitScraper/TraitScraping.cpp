#include "TraitScraping.h"
#include "ParserHelpers.h"

mappers::TraitScraping::TraitScraping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TraitScraping::registerKeys()
{
	registerKeyword("index", [this](const std::string& personalityName, std::istream& theStream) {
		index = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
