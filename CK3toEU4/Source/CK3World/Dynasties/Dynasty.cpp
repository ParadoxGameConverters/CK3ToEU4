#include "Dynasty.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Dynasty::Dynasty(std::istream& theStream, int gameID): gameID(gameID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Dynasty::registerKeys()
{
	registerKeyword("key", [this](const std::string& unused, std::istream& theStream) {
		dynID = commonItems::singleString(theStream).getString();
	});
	registerKeyword("good_for_realm_name", [this](const std::string& unused, std::istream& theStream) {
		appropriateRealmName = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("coat_of_arms_id", [this](const std::string& unused, std::istream& theStream) {
		coa = std::make_pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
