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
		try
		{
			auto newCharacter = std::make_shared<Character>(theStream, std::stoi(charID));
			characters.insert(std::pair(newCharacter->getID(), newCharacter));
		}
		catch (std::exception& e)
		{
			throw std::runtime_error("Character ID: " + charID + " is not a number? " + e.what());
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
