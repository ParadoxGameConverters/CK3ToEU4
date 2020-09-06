#include "Characters.h"
#include "Character.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Characters::Characters(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Characters::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& charID, std::istream& theStream) {
		auto newCharacter = std::make_shared<Character>(theStream, std::stoi(charID));
		characters.insert(std::pair(newCharacter->getID(), newCharacter));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
