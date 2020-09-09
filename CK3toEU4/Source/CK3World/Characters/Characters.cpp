#include "Characters.h"
#include "Character.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "../Cultures/Cultures.h"
#include "../Religions/Faiths.h"

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

void CK3::Characters::linkCultures(const Cultures& cultures)
{
	auto counter = 0;
	const auto& cultureData = cultures.getCultures();
	for (const auto& character: characters)
	{
		const auto& cultureDataItr = cultureData.find(character.second->getCulture().first);
		if (cultureDataItr != cultureData.end())
		{
			character.second->loadCulture(*cultureDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error(
				 "Character " + std::to_string(character.first) + " has culture " + std::to_string(character.second->getCulture().first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " characters updated.";
}

void CK3::Characters::linkFaiths(const Faiths& faiths)
{
	auto counter = 0;
	const auto& faithData = faiths.getFaiths();
	for (const auto& character: characters)
	{
		const auto& faithDataItr = faithData.find(character.second->getFaith().first);
		if (faithDataItr != faithData.end())
		{
			character.second->loadFaith(*faithDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error("Character " + std::to_string(character.first) + " has faith " + std::to_string(character.second->getFaith().first) +
											 " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " characters updated.";
}
