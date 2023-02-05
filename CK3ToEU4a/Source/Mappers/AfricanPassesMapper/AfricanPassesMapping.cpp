#include "AfricanPassesMapping.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::AfricanPassesMapping::AfricanPassesMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AfricanPassesMapping::registerKeys()
{
	registerSetter("name", name);
	registerKeyword("endA", [this](std::istream& theStream) {
		endA = commonItems::getInts(theStream);
	});
	registerKeyword("endB", [this](std::istream& theStream) {
		endB = commonItems::getInts(theStream);
	});
	registerKeyword("sterilize", [this](std::istream& theStream) {
		sterilizeProvinces = commonItems::getInts(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
