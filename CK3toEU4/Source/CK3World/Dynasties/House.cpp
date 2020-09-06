#include "House.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::House::House(std::istream& theStream, int houseID): houseID(houseID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::House::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerKeyword("prefix", [this](const std::string& unused, std::istream& theStream) {
		prefix = commonItems::singleString(theStream).getString();
	});
	registerKeyword("dynasty", [this](const std::string& unused, std::istream& theStream) {
		dynasty = std::make_pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
