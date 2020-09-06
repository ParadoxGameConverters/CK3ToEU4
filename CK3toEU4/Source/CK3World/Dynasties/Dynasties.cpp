#include "Dynasties.h"
#include "Dynasty.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Dynasties::Dynasties(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Dynasties::registerKeys()
{
	registerKeyword("dynasty_house", [this](const std::string& unused, std::istream& theStream) {
		houses = Houses(theStream);
	});
	registerKeyword("dynasties", [this](const std::string& unused, std::istream& theStream) {
		dynasties = Dynasties(theStream).getDynasties();
	});
	registerRegex(R"(\d+)", [this](const std::string& gameID, std::istream& theStream) {
		const auto newDynasty = std::make_shared<Dynasty>(theStream, std::stoi(gameID));
		dynasties.insert(std::pair(newDynasty->getGameID(), newDynasty));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}