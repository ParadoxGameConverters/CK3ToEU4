
using commonItems;

namespace CK3ToEU4.CK3.Characters;

class Characters
{
	public Characters() {}
	public void loadCharacters(BufferedReader reader)
	{
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}

	public const auto& getCharacters() const { return characters; }

	public void linkCultures(const Cultures& cultures)
	{
		auto counter = 0;
		const auto& cultureData = cultures.getCultures();
		for (const auto& character: characters)
		{
			if (!character.second->getCulture())
			{
				// We'll sideload this later.
				continue;
			}
			const auto& cultureDataItr = cultureData.find(character.second->getCulture()->first);
			if (cultureDataItr != cultureData.end())
			{
				character.second->loadCulture(*cultureDataItr);
				++counter;
			}
			else
			{
				throw std::runtime_error("Character " + std::to_string(character.first) + " has culture " + std::to_string(character.second->getCulture()->first) +
				                         " which has no definition!");
			}
		}
		Log(LogLevel::Info) << "<> " + counter + " characters updated.";
	}
	public void linkFaiths(const Faiths& faiths)
	{
		auto counter = 0;
		const auto& faithData = faiths.getFaiths();
		for (const auto& character: characters)
		{
			if (!character.second->getFaith())
			{
				// We'll sideload this later.
				continue;
			}
			const auto& faithDataItr = faithData.find(character.second->getFaith()->first);
			if (faithDataItr != faithData.end())
			{
				character.second->loadFaith(*faithDataItr);
				++counter;
			}
			else
			{
				throw std::runtime_error("Character " + std::to_string(character.first) + " has faith " + std::to_string(character.second->getFaith()->first) +
				                         " which has no definition!");
			}
		}
		Log(LogLevel::Info) << "<> " << counter << " characters updated.";
	}
	public void linkHouses(const Houses& houses)
	
	{
		auto counter = 0;
		auto missingFaith = 0;
		auto missingCulture = 0;
		const auto& houseData = houses.getHouses();
		for (const auto& character: characters)
		{
			const auto& houseDataItr = houseData.find(character.second->getHouse().first);
			if (houseDataItr != houseData.end())
			{
				character.second->loadHouse(*houseDataItr);
				++counter;
				if (!character.second->getCulture())
				{
					if (character.second->getHouse().second->getHouseHead() && character.second->getHouse().second->getHouseHead()->second->getCulture())
						character.second->loadCulture(*character.second->getHouse().second->getHouseHead()->second->getCulture());
					else
						missingCulture++;
				}
				if (!character.second->getFaith())
				{
					if (character.second->getHouse().second->getHouseHead() && character.second->getHouse().second->getHouseHead()->second->getFaith())
						character.second->loadFaith(*character.second->getHouse().second->getHouseHead()->second->getFaith());
					else
						missingFaith++;
				}
			}
			else
			{
				throw std::runtime_error(
					"Character " + std::to_string(character.first) + " has house " + std::to_string(character.second->getHouse().first) + " which has no definition!");
			}
		}
		Log(LogLevel::Info) << "<> " << counter << " characters updated, " << missingCulture << " are missing culture, " << missingFaith << " are missing faith.";
	}
	public void linkTitles(const Titles& titles)
	
	{
		auto capitalCounter = 0;
		auto titleCounter = 0;
		auto claimCounter = 0;
		const auto& titleData = titles.getTitles();
		// Since titles are locked behind name keys and we'll needs IDs, make a cache.
		std::map<long long, std::shared_ptr<Title>> IDCache;
		for (const auto& title: titleData)
		IDCache.insert(std::pair(title.second->getID(), title.second));

		for (const auto& character: characters)
		{
			const auto& domain = character.second->getCharacterDomain();
			if (domain && domain->getRealmCapital().first)
			{
				// load realm capital
				auto titleDataItr = IDCache.find(domain->getRealmCapital().first);
				if (titleDataItr != IDCache.end())
				{
					character.second->loadRealmCapital(*titleDataItr);
					++capitalCounter;
				}
				else
				{
					Log(LogLevel::Warning) << "Character " << character.first << " has capital " << domain->getRealmCapital().first << " which has no definition!";
				}
				// load owned including landless titles ("the domain").
				std::vector<std::pair<long long, std::shared_ptr<Title>>> replacementDomain;
				for (const auto& title: domain->getDomain())
				{
					titleDataItr = IDCache.find(title.first);
					if (titleDataItr != IDCache.end())
					{
						replacementDomain.emplace_back(*titleDataItr);
						++titleCounter;
					}
					else
					{
						throw std::runtime_error(
							"Character " + std::to_string(character.first) + " owns title " + std::to_string(title.first) + " which has no definition!");
					}
				}
				character.second->loadDomain(replacementDomain);
			}
			// Now claims.
			std::map<long long, std::shared_ptr<Title>> replacementClaims;
			for (const auto& claim: character.second->getClaims())
			{
				auto titleDataItr = IDCache.find(claim.first);
				if (titleDataItr != IDCache.end())
				{
					replacementClaims.insert(*titleDataItr);
					++claimCounter;
				}
				else
				{
					Log(LogLevel::Error) << "Character " << std::to_string(character.first) << " claims title " << std::to_string(claim.first)
						<< " which has no definition!";
				}
			}
			character.second->loadClaims(replacementClaims);
		}
		Log(LogLevel::Info) << "<> " << capitalCounter << " capitals, " << titleCounter << " titles and " << claimCounter << " claims updated.";
	}
	public void linkCharacters()
	
	{
		auto employerCounter = 0;
		auto spouseCounter = 0;

		for (const auto& character: characters)
		{
			if (character.second->getSpouse())
			{
				const auto& charItr = characters.find(character.second->getSpouse()->first);
				if (charItr != characters.end())
				{
					character.second->loadSpouse(*charItr);
					++spouseCounter;
				}
				else
				{
					// dead spouse.
					character.second->resetSpouse();
				}
			}
			if (character.second->getEmployer())
			{
				const auto& charItr = characters.find(character.second->getEmployer()->first);
				if (charItr != characters.end())
				{
					character.second->loadEmployer(*charItr);
					++employerCounter;
				}
				else
				{
					// sigh.
					character.second->resetEmployer();
				}
			}
		}
		Log(LogLevel::Info) << "<> " << spouseCounter << " spouses, " << employerCounter << " employers updated.";
	}
	public void linkTraits(TraitScraper traitScraper)
	{
		auto counter = 0;
		for (const auto& character: characters)
		{
			std::map<int, std::string> translatedTraits;
			for (const auto& trait: character.second->getTraits())
			{
				const auto& traitMatch = traitScraper.getTraitForID(trait.first);
				if (traitMatch)
					translatedTraits.insert(std::pair(trait.first, *traitMatch));
			}
			counter += static_cast<int>(translatedTraits.size());
			character.second->loadTraits(translatedTraits);
		}
		Log(LogLevel::Info) << "<> " << counter << " traits observed.";
	}


	private void registerKeys()
	{
		registerRegex(R"(\d+)", [this](const std::string& charID, std::istream& theStream) {
			auto newCharacter = std::make_shared<Character>(theStream, std::stoll(charID));
			characters.insert(std::pair(newCharacter->getID(), newCharacter));
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}

	private std::map<long long, std::shared_ptr<Character>> characters;
};