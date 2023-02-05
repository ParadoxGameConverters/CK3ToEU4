#include "ProvinceMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ProvinceMapping::ProvinceMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProvinceMapping::registerKeys()
{
	registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt provinceInt(theStream);
		eu4Provinces.push_back(provinceInt.getInt());
	});
	registerKeyword("ck3", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt provinceInt(theStream);
		ck3Provinces.push_back(provinceInt.getInt());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
