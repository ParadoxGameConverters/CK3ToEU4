#include "Houses.h"
#include "House.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Dynasties.h"

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
