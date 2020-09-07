#include "CoatsOfArms.h"
#include "CoatOfArms.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CoatsOfArms::CoatsOfArms(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CoatsOfArms::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& coaID, std::istream& theStream) {
		auto newCoA = std::make_shared<CoatOfArms>(theStream, std::stoi(coaID));
		coats.insert(std::pair(newCoA->getID(), newCoA));
	});
	registerKeyword("coat_of_arms_manager_database", [this](const std::string& unused, std::istream& theStream) {
		coats = CoatsOfArms(theStream).getCoats();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
