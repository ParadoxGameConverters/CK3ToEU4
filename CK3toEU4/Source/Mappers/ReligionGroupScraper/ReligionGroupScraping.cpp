#include "ReligionGroupScraping.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ReligionGroupScraping::ReligionGroupScraping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionGroupScraping::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& name, std::istream& theStream) {
		commonItems::ignoreItem(name, theStream);
		religions.insert(name);
	});
}
