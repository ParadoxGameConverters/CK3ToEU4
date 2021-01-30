#include "Houses.h"
#include "../Characters/Characters.h"
#include "Dynasties.h"
#include "House.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

CK3::Houses::Houses(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Houses::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& ID, std::istream& theStream) {
		const auto suspiciousItem = commonItems::stringOfItem(theStream).getString();
		if (suspiciousItem.find('{') != std::string::npos)
		{
			auto houseStream = std::stringstream(suspiciousItem);
			const auto newHouse = std::make_shared<House>(houseStream, std::stoll(ID));
			houses.insert(std::pair(newHouse->getID(), newHouse));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Houses::linkCharacters(const Characters& characters)
{
	auto counter = 0;
	const auto& characterData = characters.getCharacters();
	for (const auto& house: houses)
	{
		if (house.second->getHouseHead())
		{
			const auto& characterDataItr = characterData.find(house.second->getHouseHead()->first);
			if (characterDataItr != characterData.end())
			{
				house.second->loadHouseHead(*characterDataItr);
				++counter;
			}
			else
			{
				// Not much we can do about erroneous saves, but we can fix this internally.
				house.second->resetHouseHead();
			}
		}
		else
		{
			// :/ House with no head. Okay.
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " houses updated.";
}

void CK3::Houses::linkDynasties(const Dynasties& dynasties)
{
	auto counter = 0;
	const auto& dynastyData = dynasties.getDynasties();
	for (const auto& house: houses)
	{
		const auto& dynastyDataItr = dynastyData.find(house.second->getDynasty().first);
		if (dynastyDataItr != dynastyData.end())
		{
			house.second->loadDynasty(*dynastyDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error(
				 "House " + std::to_string(house.first) + " has dynasty " + std::to_string(house.second->getDynasty().first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " houses updated.";
}
