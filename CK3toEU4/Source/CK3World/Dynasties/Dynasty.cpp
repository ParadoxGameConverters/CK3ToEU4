#include "Dynasty.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

CK3::Dynasty::Dynasty(std::istream& theStream, long long theGameID): gameID(theGameID)
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
		coa = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
