
using CK3;

namespace CK3ToEU4.CK3.Dynasties;

class Houses
{
	public Houses()
	{
	}

	public Houses(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public const auto& getHouses() const { return houses; }

	public void linkDynasties(const Dynasties& dynasties)
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
				Log(LogLevel::Error) << "House " << std::to_string(house.first) << " has dynasty " << std::to_string(house.second->getDynasty().first)
					<< " which has no definition!";
			}
		}
		Log(LogLevel::Info) << "<> " << counter << " houses updated.";
	}
	public void linkCharacters(const Characters& characters)
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
	public void importNames(const HouseNameScraper& houseNameScraper)
	{
		auto nameCounter = 0;
		auto prefixCounter = 0;
		for (const auto& house: houses | std::views::values)
		{
			if (house->getName().empty() && houseNameScraper.getNameForKey(house->getKey()) && !houseNameScraper.getNameForKey(house->getKey())->empty())
			{
				house->setName(*houseNameScraper.getNameForKey(house->getKey()));
				++nameCounter;
			}
			if (house->getPrefix().empty() && houseNameScraper.getPrefixForKey(house->getKey()) && !houseNameScraper.getPrefixForKey(house->getKey())->empty())
			{
				house->setPrefix(*houseNameScraper.getPrefixForKey(house->getKey()));
				++prefixCounter;
			}
		}
		Log(LogLevel::Info) << "<> " << nameCounter << " house names and " << prefixCounter << " house prefixes updated.";
	}
		
	private void registerKeys()
	{
		registerRegex(R"(\d+)", [this](const string& ID, std::istream& theStream) {
			const auto suspiciousItem = commonItems::stringOfItem(theStream).getString();
			if (suspiciousItem.find('{') != string::npos)
			{
				auto houseStream = stringstream(suspiciousItem);
				const auto newHouse = std::make_shared<House>(houseStream, std::stoll(ID));
				houses.insert(std::pair(newHouse->getID(), newHouse));
			}
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	private Dictionary<long, std::shared_ptr<House>> houses;
};