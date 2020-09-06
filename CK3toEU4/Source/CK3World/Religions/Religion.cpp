#include "Religion.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Religion::Religion(std::istream& theStream, int ID): ID(ID)
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
		for (auto faith: commonItems::intList(theStream).getInts())
			faiths.insert(std::pair(faith, nullptr));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
