#include "Religion.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

CK3::Religion::Religion(std::istream& theStream, long long theID): ID(theID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Religion::registerKeys()
{
	registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
		tag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("family", [this](const std::string& unused, std::istream& theStream) {
		family = commonItems::singleString(theStream).getString();
	});
	registerKeyword("faiths", [this](const std::string& unused, std::istream& theStream) {
		for (auto faith: commonItems::llongList(theStream).getLlongs())
			faiths.insert(std::pair(faith, nullptr));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
