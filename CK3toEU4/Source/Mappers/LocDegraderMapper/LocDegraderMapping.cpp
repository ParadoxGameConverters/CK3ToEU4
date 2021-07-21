#include "LocDegraderMapping.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::LocDegraderMapping::LocDegraderMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LocDegraderMapping::registerKeys()
{
	registerSetter("utf8", replacementMapping.first);
	registerSetter("win1252", replacementMapping.second);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
