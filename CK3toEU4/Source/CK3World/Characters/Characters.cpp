#include "Characters.h"
#include "../Cultures/Cultures.h"
#include "../Dynasties/Houses.h"
#include "../Religions/Faiths.h"
#include "../Titles/Title.h"
#include "../Titles/Titles.h"
#include "Character.h"
#include "CharacterDomain.h"
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
			throw std::runtime_error("Character " + std::to_string(character.first) + " has culture " + std::to_string(character.second->getCulture().first) +
											 " which has no definition!");
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
			throw std::runtime_error(
				 "Character " + std::to_string(character.first) + " has faith " + std::to_string(character.second->getFaith().first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " characters updated.";
}

void CK3::Characters::linkHouses(const Houses& houses)
{
	auto counter = 0;
	const auto& houseData = houses.getHouses();
	for (const auto& character: characters)
	{
		const auto& houseDataItr = houseData.find(character.second->getHouse().first);
		if (houseDataItr != houseData.end())
		{
			character.second->loadHouse(*houseDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error(
				 "Character " + std::to_string(character.first) + " has house " + std::to_string(character.second->getHouse().first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " characters updated.";
}

void CK3::Characters::linkTitles(const Titles& titles)
{
	auto capitalCounter = 0;
	auto titleCounter = 0;
	auto claimCounter = 0;
	const auto& titleData = titles.getTitles();
	// Since titles are locked behind name keys and we'll needs IDs, make a cache.
	std::map<int, std::shared_ptr<Title>> IDCache;
	for (const auto& title: titleData)
		IDCache.insert(std::pair(title.second->getID(), title.second));

	for (const auto& character: characters)
	{
		const auto& domain = character.second->getDomain();
		if (domain)
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
				throw std::runtime_error("Character " + std::to_string(character.first) + " has capital " + std::to_string(domain->getRealmCapital().first) +
												 " which has no definition!");
			}
			// load owned hopefully non-landless titles ("the domain").
			std::vector<std::pair<int, std::shared_ptr<Title>>> replacementDomain;
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
		std::map<int, std::shared_ptr<Title>> replacementClaims;
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
				throw std::runtime_error(
					 "Character " + std::to_string(character.first) + " claims title " + std::to_string(claim.first) + " which has no definition!");
			}
		}
		character.second->loadClaims(replacementClaims);
	}
	Log(LogLevel::Info) << "<> " << capitalCounter << " capitals, " << titleCounter << " titles and " << claimCounter << " claims updated.";
}
