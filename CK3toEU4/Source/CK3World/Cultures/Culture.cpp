#include "Culture.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Culture::Culture(std::istream& theStream, long long theID): ID(theID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Culture::registerKeys()
{
	registerKeyword("culture_template", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
