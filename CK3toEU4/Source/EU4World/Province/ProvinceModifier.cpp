#include "ProvinceModifier.h"
#include "ParserHelpers.h"

EU4::ProvinceModifier::ProvinceModifier(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ProvinceModifier::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString nameStr(theStream);
		name = nameStr.getString();
	});
	registerKeyword("duration", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt durInt(theStream);
		duration = durInt.getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
