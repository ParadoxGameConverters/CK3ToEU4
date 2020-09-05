#include "BaronyHoldings.h"
#include "BaronyHolding.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::BaronyHoldings::BaronyHoldings(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::BaronyHoldings::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& baronyID, std::istream& theStream) {
		auto newBarony = std::make_shared<BaronyHolding>(theStream);
		baronyHoldings.insert(std::pair(std::stoi(baronyID), newBarony));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
