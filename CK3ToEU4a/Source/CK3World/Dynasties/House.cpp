#include "House.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::House::House(std::istream& theStream, long long housID): houseID(housID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::House::registerKeys()
{
	registerKeyword("key", [this](std::istream& theStream) {
		key = commonItems::getString(theStream);
	});
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerKeyword("localized_name", [this](const std::string& unused, std::istream& theStream) {
		localizedName = commonItems::singleString(theStream).getString();
	});
	registerKeyword("prefix", [this](const std::string& unused, std::istream& theStream) {
		prefix = commonItems::singleString(theStream).getString();
	});
	registerKeyword("dynasty", [this](const std::string& unused, std::istream& theStream) {
		dynasty = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("head_of_house", [this](const std::string& unused, std::istream& theStream) {
		houseHead = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
