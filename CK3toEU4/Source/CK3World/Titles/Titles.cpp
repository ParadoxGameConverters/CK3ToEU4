#include "Titles.h"
#include "../Characters/Character.h"
#include "../Characters/Characters.h"
#include "../CoatsOfArms/CoatsOfArms.h"
#include "CommonRegexes.h"
#include "DynamicTemplate.h"
#include "LandedTitles.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Title.h"

CK3::Titles::Titles(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	// This bit assigns CK3::Level to dynamic titles that have a rank definition in the save. It should be all of them, but it's CK3, so who knows.
	if (!dynamicTitleRanks.empty())
		transcribeDynamicRanks();
}

void CK3::Titles::registerKeys()
{
	registerKeyword("dynamic_templates", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::blobList dynamicRanks(theStream);
		for (const auto& dynamicTitle: dynamicRanks.getBlobs())
		{
			std::stringstream blobStream(dynamicTitle);
			const DynamicTemplate dynamicTitleTemplate(blobStream);
			if (!dynamicTitleTemplate.getDynamicTitleKey().empty() && !dynamicTitleTemplate.getDynamicTitleRank().empty())
				dynamicTitleRanks.insert(std::pair(dynamicTitleTemplate.getDynamicTitleKey(), dynamicTitleTemplate.getDynamicTitleRank()));
		}
	});
	registerKeyword("landed_titles", [this](const std::string& unused, std::istream& theStream) {
		// A bit of recursion is good for the soul.
		const auto& tempTitles = Titles(theStream);
		titles = tempTitles.getTitles();
		titleCounter = tempTitles.getCounter();
	});
	registerRegex(R"(\d+)", [this](const std::string& ID, std::istream& theStream) {
		// Incoming titles may not be actual titles but half-deleted junk.
		const auto& titleBlob = commonItems::stringOfItem(theStream).getString();
		if (titleBlob.find('{') != std::string::npos)
		{
			std::stringstream tempStream(titleBlob);
			try
			{
				auto newTitle = std::make_shared<Title>(tempStream, std::stoll(ID));
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
				else
					++titleCounter[5]; // x_x_, x_mc_ and the rest.
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Cannot import title ID: " + ID + " (" + e.what() + ")");
			}
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Titles::transcribeDynamicRanks()
{
	Log(LogLevel::Info) << "-> Transcribing dynamic ranks.";
	auto counter = 0;
	for (const auto& [key, rank]: dynamicTitleRanks)
	{
		if (!titles.contains(key))
			continue; // Yay?
		if (!titles.at(key))
			continue; // Huh?

		// There will not be a dynamic county or barony.
		if (rank == "duchy")
			titles.at(key)->setDynamicLevel(LEVEL::DUCHY);
		else if (rank == "kingdom")
		{
			titles.at(key)->setDynamicLevel(LEVEL::KINGDOM);
			titles.at(key)->setCustomTitle();
		}
		else if (rank == "empire")
		{
			titles.at(key)->setDynamicLevel(LEVEL::EMPIRE);
			titles.at(key)->setCustomTitle();
		}
		counter++;
	}
	Log(LogLevel::Info) << "<> Transcribed " << counter << " dynamics.";
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
	std::map<long long, std::shared_ptr<Title>> IDCache;
	for (const auto& title: titles)
		IDCache.insert(std::pair(title.second->getID(), title.second));

	// We'll also be needing a defacto vassal registry for later.
	std::map<long long, std::map<long long, std::shared_ptr<Title>>> dfvRegistry; // map<liegeTitleID, map<vassalTitleID, vassalTitle>>

	// But before we begin, we need to clear out the mess that is the save. Titles will have defacto liege set to titles that have no holder.
	// To filter them, let's first run through all titles and make a cache of all without a holder.
	std::set<long long> holderlessTitles;
	for (const auto& title: titles)
		if (!title.second->getHolder())
			holderlessTitles.insert(title.second->getID());

	// Additional objective:
	// When a county and a duchy are held by same holder, under a king, both titles will have the kingdom set as defacto liege. We need to
	// untangle this mess and drop all vassals held by a person that also holds the vassal's dejure title as a vassal under same liege. :/

	for (const auto& title: titles)
	{
		// Self-loading - this is used for coalescing and congregating counties later.
		if (title.first.find("c_") == 0)
		{
			std::map<std::string, std::shared_ptr<Title>> self;
			self.insert(title);
			title.second->loadOwnedDFCounties(self); // every county defacto owns itself, by definition.
			title.second->loadOwnedDJCounties(self); // dejure as well.
		}

		// DFLiege
		if (title.second->getDFLiege())
		{
			// STOP. Is this erroneous ID?
			if (holderlessTitles.count(title.second->getDFLiege()->first))
			{
				// fix it immediately.
				Log(LogLevel::Warning) << "Title " + title.first + " has defacto liege " + std::to_string(title.second->getDFLiege()->first) +
														" which has no holder. Fixing error internally.";
				title.second->resetDFLiege();
			}
			else
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
					// Yet another savegame error type.
					Log(LogLevel::Warning) << "Title " + title.first + " has defacto liege " + std::to_string(title.second->getDFLiege()->first) +
															" which has no definition!";
					title.second->resetDFLiege();
				}
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
				// Sigh. Yes, this happens. DEJURE friggin LIEGE, completely undefined, missing. Well, well.
				Log(LogLevel::Warning) << "Title " << title.first << " has DEJURE >>LIEGE<< " << std::to_string(title.second->getDJLiege()->first)
											  << " which has no definition! This is not amusing. Rerouting.";
				if (title.second->getDFLiege())
				{
					if (const auto& dfLiegeItr = IDCache.find(title.second->getDFLiege()->first); dfLiegeItr != IDCache.end())
					{
						// We're rerouting title to be dejure under dfliege, whomever this is. The can cause baronies to be dejure under empires, but see if I care at
						// this point.
						title.second->loadDJLiege(*dfLiegeItr);
						Log(LogLevel::Warning) << "Rerouted " << title.first << " to dejure liege " << dfLiegeItr->second->getName();
					}
					else
					{
						Log(LogLevel::Warning) << "Title " << title.first << " also has a missing defacto liege. Your save is likely very broken. Rerouting to void.";
						title.second->resetDFLiege();
						title.second->resetDJLiege();
					}
				}
				else
				{
					Log(LogLevel::Warning) << "Rerouting failed. Rerouting to void.";
					title.second->resetDJLiege();
				}
			}
		}
		// DJVassals
		if (!title.second->getDJVassals().empty())
		{
			std::map<long long, std::shared_ptr<Title>> replacementMap;
			for (const auto& dJVassal: title.second->getDJVassals())
			{
				const auto& cacheItr = IDCache.find(dJVassal.first);
				if (cacheItr != IDCache.end())
				{
					replacementMap.insert(*cacheItr);
					++DJVcounter;
				}
				else
				{
					// Sigh. Yes, this happens. DEJURE vassals, not defacto! And void! Woe is PDX.
					Log(LogLevel::Warning) << "Title " << title.first << " has DEJURE vassal " << std::to_string(dJVassal.first)
												  << " which has no definition! Amusing.";
				}
			}
			title.second->loadDJVassals(replacementMap);
		}
	}

	// Now with fully generated dfvCache we can load it into all lieges.
	std::map<long long, std::map<long long, std::shared_ptr<Title>>> updateMap;
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
	// Finally update the dfvassal lists of all titles that gained new vassals.
	for (const auto& update: updateMap)
	{
		IDCache[update.first]->addDFVassals(update.second);
	}

	Log(LogLevel::Info) << "<> " << DFLcounter << " defacto lieges, " << DJLcounter << " dejure lieges, " << DFVcounter << " defacto vassals, " << DJVcounter
							  << " dejure vassals updated.";
}

void CK3::Titles::relinkDeFactoVassals()
{
	for (const auto& title: titles)
		title.second->relinkDeFactoVassals();
}

void CK3::Titles::linkCharacters(const Characters& characters)
{
	auto holderCounter = 0;
	auto claimantCounter = 0;
	auto heirCounter = 0;
	auto exCounter = 0;
	auto electorCounter = 0;

	const auto& characterData = characters.getCharacters();
	for (const auto& title: titles)
	{
		// Holder
		if (title.second->getHolder())
		{
			const auto& characterDataItr = characterData.find(title.second->getHolder()->first);
			if (characterDataItr != characterData.end())
			{
				title.second->loadHolder(*characterDataItr);
				++holderCounter;
			}
			else
			{
				Log(LogLevel::Error) << "For heaven's sake, title " + title.first + " has holder " + std::to_string(title.second->getHolder()->first) +
													 " who is dead or has no definition!";
				// Attempt recovery. Since titles are still unlinked we'll have to iterate to find DFliege's holder manually.
				if (title.second->getDFLiege())
				{
					long long replacementHolder = 0;
					for (const auto& [titleName, theTitle]: titles)
						if (theTitle->getID() == title.second->getDFLiege()->first)
						{
							if (theTitle->isHolderSet())
								replacementHolder = theTitle->getHolder()->first;
							break;
						}

					if (replacementHolder > 0)
						if (const auto& replacementHolderItr = characterData.find(replacementHolder); replacementHolderItr != characterData.end())
						{
							title.second->loadHolder(*replacementHolderItr);
							Log(LogLevel::Warning) << "We have recovered from this.";
							++holderCounter;
						}
				}
				else
				{
					title.second->brickTitle();
					Log(LogLevel::Error) << "Recovery failed, title " << title.second->getName() << " bricked. Yay. :/";
				}
			}
		}
		// claimants
		std::map<long long, std::shared_ptr<Character>> replacementMap;
		for (const auto& claimant: title.second->getClaimants())
		{
			const auto& characterDataItr = characterData.find(claimant.first);
			if (characterDataItr != characterData.end())
			{
				replacementMap.insert(*characterDataItr);
				++claimantCounter;
			}
			else
			{
				// Maybe deleted.
			}
		}
		title.second->loadClaimants(replacementMap);

		// heirs
		std::vector<std::pair<long long, std::shared_ptr<Character>>> replacementVector;
		for (const auto& heir: title.second->getHeirs())
		{
			const auto& characterDataItr = characterData.find(heir.first);
			if (characterDataItr != characterData.end())
			{
				replacementVector.emplace_back(*characterDataItr);
				++heirCounter;
			}
			else
			{
				// Dead person, save hadn't updated state yet.
			}
		}
		title.second->loadHeirs(replacementVector);

		// previous holders
		replacementVector.clear();
		for (const auto& exHolder: title.second->getPreviousHolders())
		{
			const auto& characterDataItr = characterData.find(exHolder.first);
			if (characterDataItr != characterData.end())
			{
				replacementVector.emplace_back(*characterDataItr);
				++exCounter;
			}
			else
			{
				// Maybe deleted.
			}
		}
		title.second->loadPreviousHolders(replacementVector);

		// electors
		replacementMap.clear();
		for (const auto& elector: title.second->getElectors())
		{
			const auto& characterDataItr = characterData.find(elector.first);
			if (characterDataItr != characterData.end())
			{
				replacementMap.insert(*characterDataItr);
				++electorCounter;
			}
			else
			{
				// Maybe deleted.
			}
		}
		title.second->loadElectors(replacementMap);
	}

	Log(LogLevel::Info) << "<> " << holderCounter << " holders, " << claimantCounter << " claimants, " << heirCounter << " heirs, " << exCounter
							  << " previous holders, " << electorCounter << " electors updated.";
}

void CK3::Titles::linkLandedTitles(const LandedTitles& landedTitles)
{
	// We're injecting clay towards titles instead the other way around.
	auto counter = 0;
	for (const auto& clay: landedTitles.getFoundTitles())
	{
		const auto& titleItr = titles.find(clay.first);
		if (titleItr != titles.end())
		{
			titleItr->second->loadClay(clay.second);
			++counter;
		}
		else
		{
			Log(LogLevel::Error) << "Clay for " << clay.first
										<< " has no title definition! THIS IS BAD! Are you converting an old save against a new CK3 version?";
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " titles updated.";
}
