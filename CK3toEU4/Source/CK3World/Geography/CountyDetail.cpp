#include "CountyDetail.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CountyDetail::CountyDetail(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CountyDetail::registerKeys()
{
	registerKeyword("development", [this](const std::string& unused, std::istream& theStream) {
		development = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		culture = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
		if (culture.first == 4294967295)
			culture.first = 0;
	});
	registerKeyword("faith", [this](const std::string& unused, std::istream& theStream) {
		faith = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
		if (faith.first == 4294967295)
			faith.first = 0;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
