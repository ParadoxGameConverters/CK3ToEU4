#include "DynamicTemplate.h"
#include "ParserHelpers.h"

CK3::DynamicTemplate::DynamicTemplate(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::DynamicTemplate::registerKeys()
{
	registerKeyword("key", [this](const std::string& unused, std::istream& theStream) {
		dynamicKey = commonItems::singleString(theStream).getString();
	});
	registerKeyword("tier", [this](const std::string& unused, std::istream& theStream) {
		dynamicRank = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
