#include "Houses.h"
#include "House.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Houses::Houses(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Houses::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& ID, std::istream& theStream) {
		const auto newHouse = std::make_shared<House>(theStream, std::stoi(ID));
		houses.insert(std::pair(newHouse->getID(), newHouse));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
