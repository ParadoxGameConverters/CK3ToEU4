#include "Titles.h"
#include "../CoatsOfArms/CoatsOfArms.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Title.h"

CK3::Titles::Titles(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Titles::registerKeys()
{
	registerKeyword("landed_titles", [this](const std::string& unused, std::istream& theStream) {
		// A bit of recursion is good for the soul.
		const auto& tempTitles = Titles(theStream);
		titles = tempTitles.getTitles();
		titleCounter = tempTitles.getCounter();
	});
	registerRegex(R"(\d+)", [this](const std::string& ID, std::istream& theStream) {
		// Incoming titles may not be actual titles but half-deleted junk.
		const auto& titleBlob = commonItems::singleItem(ID, theStream);
		if (titleBlob.find('{') != std::string::npos)
		{
			std::stringstream tempStream(titleBlob);
			auto newTitle = std::make_shared<Title>(tempStream, std::stoi(ID));
			if (!newTitle->getName().empty())
				titles.insert(std::pair(newTitle->getName(), newTitle));
			if (newTitle->getName().find("b_") == 0)
				++titleCounter[0];
			else if (newTitle->getName().find("c_") == 0)
				++titleCounter[1];
			else if (newTitle->getName().find("d_") == 0)
				++titleCounter[2];
			else if (newTitle->getName().find("k_") == 0)
				++titleCounter[3];
			else if (newTitle->getName().find("e_") == 0)
				++titleCounter[4];
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Titles::linkCoats(const CoatsOfArms& coats)
{
	auto counter = 0;
	const auto& coatData = coats.getCoats();
	for (const auto& title: titles)
	{
		if (!title.second->getCoA())
			continue;
		const auto& coatDataItr = coatData.find(title.second->getCoA()->first);
		if (coatDataItr != coatData.end())
		{
			title.second->loadCoat(*coatDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error("Title " + title.first + " has CoA " + std::to_string(title.second->getCoA()->first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " titles updated.";
}

void CK3::Titles::linkTitles()
{
	auto DFLcounter = 0;
	auto DJLcounter = 0;
	auto DFVcounter = 0;
	auto DJVcounter = 0;
	// We'll be needing a cache.
	std::map<int, std::shared_ptr<Title>> IDCache;
	for (const auto& title: titles)
		IDCache.insert(std::pair(title.second->getID(), title.second));
	// We'll also be needing a defacto vassal registry for later.
	std::map<int, std::map<int, std::shared_ptr<Title>>> dfvRegistry; // <liegeTitleID, map<vassalTitleID, vassalTitle>>

	for (const auto& title: titles)
	{
		// DFLiege
		if (title.second->getDFLiege())
		{
			const auto& cacheItr = IDCache.find(title.second->getDFLiege()->first);
			if (cacheItr != IDCache.end())
			{
				title.second->loadDFLiege(*cacheItr);
				dfvRegistry[title.second->getDFLiege()->first].insert(std::pair(title.second->getID(), title.second));
				++DFLcounter;
			}
			else
			{
				throw std::runtime_error(
					 "Title " + title.first + " has defacto liege " + std::to_string(title.second->getDFLiege()->first) + " which has no definition!");
			}
		}
		// DJLiege
		if (title.second->getDJLiege())
		{
			const auto& cacheItr = IDCache.find(title.second->getDJLiege()->first);
			if (cacheItr != IDCache.end())
			{
				title.second->loadDJLiege(*cacheItr);
				++DJLcounter;
			}
			else
			{
				throw std::runtime_error(
					 "Title " + title.first + " has dejure liege " + std::to_string(title.second->getDJLiege()->first) + " which has no definition!");
			}
		}
		// DJVassals
		if (title.second->getDJVassals())
		{
			std::map<int, std::shared_ptr<Title>> replacementMap;
			for (const auto& dJVassal: *title.second->getDJVassals())
			{
				const auto& cacheItr = IDCache.find(dJVassal.first);
				if (cacheItr != IDCache.end())
				{
					replacementMap.insert(*cacheItr);
					++DJVcounter;
				}
				else
				{
					throw std::runtime_error("Title " + title.first + " has dejure vassal " + std::to_string(dJVassal.first) + " which has no definition!");
				}
			}
			title.second->loadDJVassals(replacementMap);
		}
	}
	// Now with fully generated dfvCache we can load it into all lieges.
	for (const auto& liege: dfvRegistry)
	{
		const auto& cacheItr = IDCache.find(liege.first);
		if (cacheItr != IDCache.end())
		{
			cacheItr->second->loadDFVassals(liege.second);
			DFVcounter += static_cast<int>(liege.second.size());
		}
		else
		{
			throw std::runtime_error("DJVCache corruption: " + std::to_string(liege.first) + " cannot be found!");
		}
	}

	Log(LogLevel::Info) << "<> " << DFLcounter << " defacto lieges, " << DJLcounter << " dejure lieges, " << DFVcounter << " defacto vassals, " << DJVcounter << " dejure vassals updated.";
}
