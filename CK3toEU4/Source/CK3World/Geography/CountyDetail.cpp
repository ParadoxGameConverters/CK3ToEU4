#include "CountyDetail.h"
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
	});
	registerKeyword("faith", [this](const std::string& unused, std::istream& theStream) {
		faith = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
