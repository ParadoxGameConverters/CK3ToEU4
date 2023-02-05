#include "IslamOverrideMapping.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::IslamOverrideMapping::IslamOverrideMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IslamOverrideMapping::registerKeys()
{
	registerSetter("province", replacementMapping.first);
	registerSetter("religion", replacementMapping.second);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
