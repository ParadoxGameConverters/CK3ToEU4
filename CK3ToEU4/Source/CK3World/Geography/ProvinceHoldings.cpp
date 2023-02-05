#include "ProvinceHoldings.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProvinceHolding.h"

CK3::ProvinceHoldings::ProvinceHoldings(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::ProvinceHoldings::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& baronyID, std::istream& theStream) {
		auto newBarony = std::make_shared<ProvinceHolding>(theStream);
		provinceHoldings.insert(std::pair(std::stoi(baronyID), newBarony));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
